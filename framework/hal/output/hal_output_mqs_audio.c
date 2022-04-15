/*
 * Copyright 2019-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief MQS (medium quality sound) audio HAL output device implementation. Used for playing audio clips over the
 * board's speakers.
 */

#include "board_define.h"
#ifdef ENABLE_OUTPUT_DEV_MqsAudio
#include "board.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "math.h"

#include "fsl_dmamux.h"
#include "fsl_sai_edma.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_cache.h"

#include "fwk_log.h"
#include "fwk_output_manager.h"
#include "fwk_platform.h"
#include "fwk_task.h"

#include "hal_event_descriptor_common.h"
#include "hal_event_descriptor_voice.h"
#include "hal_output_dev.h"
//#include "hal_smart_lock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* DMA */
#define MQS_DMA            DMA0
#define MQS_DMAMUX         DMAMUX0
#define MQS_SAI_EDMA_TX_CH 0
#define MQS_SAI_TX_SOURCE  kDmaRequestMuxSai3Tx

#define MQS_SAI_DMA_IRQ_PRIO (configMAX_SYSCALL_INTERRUPT_PRIORITY - 1)
/* MQS_AUDIO_SLOT_SIZE = 9600 => 100ms at 48KHz
 * MQS_AUDIO_SLOT_CNT should be 2 or more in order to be able to play audio without pauses */
#define MQS_AUDIO_SLOT_SIZE (9600)
#define MQS_AUDIO_SLOT_CNT  (2)
#define MQS_AUDIO_SLOT_MS   (MQS_AUDIO_SLOT_SIZE / (2 * 48))

#if !AMP_LOOPBACK_DISABLED
/* MQS_FEEDBACK_SLOT_SIZE should be (MQS_AUDIO_SLOT_SIZE / 3) because MQS is 48KHz and Mics are 16KHz
 * MQS_FEEDBACK_SLOT_CNT should be (MQS_AUDIO_SLOT_CNT + 1) in order to give AFE time to process all chunks without
 * overlapping */
#define MQS_FEEDBACK_SLOT_SIZE (MQS_AUDIO_SLOT_SIZE / 3)
#define MQS_FEEDBACK_SLOT_CNT  (MQS_AUDIO_SLOT_CNT + 1)
#endif /* !AMP_LOOPBACK_DISABLED */

#if defined(__cplusplus)
extern "C" {
#endif
void BOARD_InitMqsResource(void);
int HAL_OutputDev_MqsAudio_Register();
static hal_output_status_t HAL_OutputDev_MqsAudio_Init(output_dev_t *dev, output_dev_callback_t callback);
static hal_output_status_t HAL_OutputDev_MqsAudio_Start(const output_dev_t *dev);
static void _PlaySound(const uint8_t *buffer, int32_t size);
static status_t _GetVolume(char *valueToString);
#if defined(__cplusplus)
}
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

static volatile SemaphoreHandle_t s_MqsSemFreeSlots = NULL;
static AT_NONCACHEABLE_SECTION_ALIGN_OCRAM(uint8_t s_MqsStreamPool[MQS_AUDIO_SLOT_CNT][MQS_AUDIO_SLOT_SIZE], 4);
#if !AMP_LOOPBACK_DISABLED
static AT_NONCACHEABLE_SECTION_ALIGN_OCRAM(uint8_t s_MqsAfeFeedback[MQS_FEEDBACK_SLOT_CNT][MQS_FEEDBACK_SLOT_SIZE], 4);
#endif /* !AMP_LOOPBACK_DISABLED */

static AT_NONCACHEABLE_SECTION_ALIGN(sai_edma_handle_t s_SaiTxHandle, 4);
static AT_NONCACHEABLE_SECTION_ALIGN(edma_handle_t s_SaiDmaHandle, 4);

#define DEFER_PLAYBACK_TO_TASK 1
#if DEFER_PLAYBACK_TO_TASK
typedef struct
{
    fwk_task_data_t commonData;
    const output_dev_t *dev;
} mqs_task_data_t;

typedef struct
{
    fwk_task_t task;
    mqs_task_data_t data;
} mqs_task_t;

typedef enum _mqs_configs
{
    kMQSConfigs_Volume = 0,
} mqs_configs;

static mqs_task_t s_MqsAudioTask;

#define MQS_TASK_NAME     "mqs_audio"
#define MQS_TASK_STACK    1024
#define MQS_TASK_PRIORITY 3
#if FWK_SUPPORT_STATIC_ALLOCATION
FWKDATA static StackType_t s_MqsAudioTaskStack[MQS_TASK_STACK];
FWKDATA static StaticTask_t s_MqsAudioTaskTcb;
static void *s_MqsAudioTaskTcbReference = (void *)&s_MqsAudioTaskTcb;
#else
static void *s_MqsAudioTaskStack        = NULL;
static void *s_MqsAudioTaskTcbReference = NULL;
#endif

typedef struct
{
    const uint8_t *buffer;
    int32_t size;
} sound_info_t;

const static output_dev_operator_t s_OutputDev_MqsAudioOps = {
    .init   = HAL_OutputDev_MqsAudio_Init,
    .deinit = NULL,
    .start  = HAL_OutputDev_MqsAudio_Start,
    .stop   = NULL,
};

static output_dev_t s_OutputDev_MqsAudio = {
    .name         = "sound",
    .attr.type    = kOutputDevType_Audio,
    .attr.reserve = NULL,
    .ops          = &s_OutputDev_MqsAudioOps,
    .configs =
        {
            [kMQSConfigs_Volume] = {.name          = "volume",
                                    .expectedValue = "<0-100>",
                                    .description   = "% volume of the speaker",
                                    .get           = _GetVolume},
        },
    .cap.callback = NULL,
};

static void _postSoundPlayRequest(const uint8_t *buffer, int32_t size)
{
    fwk_message_t *pMsg = (fwk_message_t *)FWK_MALLOC(sizeof(fwk_message_t));

    if (pMsg != NULL)
    {
        memset(pMsg, 0, sizeof(fwk_message_t));
        pMsg->freeAfterConsumed  = 1;
        pMsg->id                 = kFWKMessageID_Raw;
        sound_info_t *pSoundInfo = FWK_MALLOC(sizeof(sound_info_t));
        if (pSoundInfo != NULL)
        {
            pSoundInfo->buffer          = buffer;
            pSoundInfo->size            = size;
            pMsg->raw.data              = pSoundInfo;
            pMsg->raw.freeAfterConsumed = 1;
            FWK_Message_Put(MQS_AUDIO_TASK_ID, &pMsg);
        }
        else
        {
            LOGE("Failed to allocate memory for mqs message info.");
            FWK_FREE(pMsg);
        }
    }
    else
    {
        LOGE("Failed to allocate memory for mqs message.");
    }
}

static void HAL_OutputDev_MqsAudio_MsgHandle(fwk_message_t *pMsg, fwk_task_data_t *pTaskData)
{
    LOGI("HAL_OutputDev_MqsAudio_MsgHandle\r");
    if ((pMsg == NULL) || (pTaskData == NULL) || (pMsg->id != kFWKMessageID_Raw) || (pMsg->raw.data == NULL))
    {
        return;
    }

    sound_info_t *pSoundInfo = (sound_info_t *)pMsg->raw.data;
    _PlaySound(pSoundInfo->buffer, pSoundInfo->size);

    if (pMsg->raw.freeAfterConsumed)
    {
        FWK_FREE(pMsg->raw.data);
        pMsg->raw.data = NULL;
    }
}

#define MQS_SOUND_PLAY_FUNC(buffer, size) _postSoundPlayRequest((buffer), (size))
#else
#define MQS_SOUND_PLAY_FUNC(buffer, size) _PlaySound((buffer), (size))
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

__attribute__((weak)) int APP_OutputDev_MqsAudio_InferCompleteDecode(output_algo_source_t source,
                                                                     void *inferResult,
                                                                     void **audio,
                                                                     uint32_t *len)
{
    return 0;
}

__attribute__((weak)) int APP_OutputDev_MqsAudio_InputNotifyDecode(const output_dev_t *dev,
                                                                   void *inputData,
                                                                   void **audio,
                                                                   uint32_t *len)
{
    return 0;
}

static void _SaiCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
{
    BaseType_t xHigherPriorityTaskWoken, result;

    result = xSemaphoreGiveFromISR(s_MqsSemFreeSlots, &xHigherPriorityTaskWoken);
    if (result != pdFAIL)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void _ConfigMqs(void)
{
    CLOCK_EnableClock(kCLOCK_Mqs); /* Enable MQS hmclk. */

    IOMUXC_MQSEnterSoftwareReset(IOMUXC_GPR, true);  /* Reset MQS. */
    IOMUXC_MQSEnterSoftwareReset(IOMUXC_GPR, false); /* Release reset MQS. */
    IOMUXC_MQSEnable(IOMUXC_GPR, true);              /* Enable MQS. */
    //    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate64, 0u);           /* 78.6432MHz/64/(0+1) = 1.2288MHz
    //                                                                                 Higher frequency PWM involves
    //                                                                                 less low frequency harmonic.*/
    IOMUXC_MQSConfig(IOMUXC_GPR, kIOMUXC_MqsPwmOverSampleRate32,
                     0u); /* 78.6432MHz/32/(0+1) = 2.4576MHz
                             Higher frequency PWM involves less low frequency harmonic.*/
}

/*!
 * brief set audio volume for this amp.
 *
 * param volume volume value, support 0 ~ 100, only in multiples of 10, 0 is mute, 100 is the maximum volume value.
 */
static float _AdaptVolume(uint32_t volume)
{
    assert(volume >= 0 && volume <= 100);

    volume /= 10;

    /*
     * This is the function used for generating a nice polynomial curve for the volume levels.
     *
     *                  y = -0.0018 * x ^ 3 + 0.028 * x ^ 2
     *
     * In this case it's more like a linear function with the lower and upper ends slightly curved.
     *
     * The levels go from 0 to 1, making sure that level 1 stays low at 0.0262
     * while still being able to reach the value 1 at level 10.
     *
     * This function is called once for every volume change, so these operations shouldn't be
     * that much of a burden
     */
    return (-0.0018 * pow(volume, 3) + 0.028 * pow(volume, 2));
}

static void _VolDiffControl(void *data, uint32_t len)
{
    uint32_t i;
    int16_t *data16 = (int16_t *)data;
    float volume    = _AdaptVolume(s_OutputDev_MqsAudio.configs[kMQSConfigs_Volume].value);
    uint32_t len16  = len / sizeof(int16_t);

    for (i = 0; i < len16 - 1; i += 2)
    {
        // the volume is decreased by multiplying the samples with values between 0 and 1
        data16[i]     = (int16_t)(data16[i] * volume);
        data16[i + 1] = -data16[i];
    }
}

#if !AMP_LOOPBACK_DISABLED
/* TODO: Move downsampling stuff to dedicated folder!!! */
#define F3_NUM_TAPS 31U
static int16_t dsBuffer[32];
static const int16_t kDefaultF3Coeffs[F3_NUM_TAPS] = {
    0,    57,   83, 0,     190,   -279, 0,   554, 757, 0,    -1388, -1910, 0,  4327, 8922, 10898,
    8922, 4327, 0,  -1910, -1388, 0,    757, 554, 0,   -279, -190,  0,     83, 57,   0,
};

static inline int32_t mult_acc(int16_t coeff, int16_t op1, int16_t op2, int32_t acc)
{
    acc += ((int32_t)coeff * (int32_t)op1);
    acc += ((int32_t)coeff * (int32_t)op2);

    return acc;
}

static int16_t fir_lp_one_sixth(const int16_t *coeffs, int16_t *buffer, int16_t data)
{
    int32_t acc = 0;

    /* Pull new data into buffer */
    buffer[0] = data;

    /* Unrolled loop, skipping zero coefficients, leveraging dual multiply accumulates */
    acc = mult_acc(coeffs[1], buffer[1], buffer[29], acc);
    acc = mult_acc(coeffs[2], buffer[2], buffer[28], acc);

    acc = mult_acc(coeffs[4], buffer[4], buffer[26], acc);
    acc = mult_acc(coeffs[5], buffer[5], buffer[25], acc);

    acc = mult_acc(coeffs[7], buffer[7], buffer[23], acc);
    acc = mult_acc(coeffs[8], buffer[8], buffer[22], acc);

    acc = mult_acc(coeffs[10], buffer[10], buffer[20], acc);
    acc = mult_acc(coeffs[11], buffer[11], buffer[19], acc);

    acc = mult_acc(coeffs[13], buffer[13], buffer[17], acc);
    acc = mult_acc(coeffs[14], buffer[14], buffer[16], acc);

    acc = mult_acc(coeffs[15], buffer[15], 0, acc);

    /* Shift values up in buffer */
    for (uint32_t idx = F3_NUM_TAPS; idx > 0; idx--)
    {
        buffer[idx] = buffer[idx - 1];
    }

    /* Return quantized accumulator  */
    return (int16_t)(acc >> 15);
}

void SLN_DSP_downsample_by_3(int16_t *in, uint32_t inSampleCount, int16_t *out)
{
    int16_t *pOut   = out;
    int16_t filt    = 0;
    uint8_t counter = 0;

    for (uint32_t idx = 0; idx < inSampleCount; idx++)
    {
        filt = fir_lp_one_sixth(kDefaultF3Coeffs, dsBuffer, in[idx]);

        /* Down sample 3-1 */
        if (2 == counter++)
        {
            *pOut = filt;
            pOut++;
            counter = 0;
        }
    }
}

void _SpeakerToAfeNotify(int16_t *buffer, uint32_t length)
{
    event_voice_t feedbackEvent = {0};

    if (s_OutputDev_MqsAudio.cap.callback != NULL)
    {
        feedbackEvent.event_base.eventId         = SPEAKER_TO_AFE_FEEDBACK;
        feedbackEvent.speaker_audio.start_time   = FWK_CurrentTimeUs();
        feedbackEvent.speaker_audio.audio_stream = buffer;
        feedbackEvent.speaker_audio.audio_length = length;

        s_OutputDev_MqsAudio.cap.callback(s_OutputDev_MqsAudio.id, kOutputEvent_SpeakerToAfeFeedback, &feedbackEvent,
                                          sizeof(event_voice_t), 0);
    }
}
#endif /* !AMP_LOOPBACK_DISABLED */

// TODO: workaround for the self-wakeup
volatile int g_MQSPlaying = 0;

/*!
 * @brief play audio clip
 *
 * @param buffer pointer to audio clip
 * @param size size of audio buffer
 */
static void _PlaySound(const uint8_t *buffer, int32_t size)
{
    sai_transfer_t xfer         = {0};
    status_t tansferStatus      = kStatus_Success;
    uint32_t audioSize          = 0;
    uint32_t audioPlayedSize    = 0;
    uint32_t audioChunkSize     = 0;
    uint8_t mqsAudioPoolSlotIdx = 0;
    bool statusOk               = true;

#if !AMP_LOOPBACK_DISABLED
    uint8_t mqsFeedbackPoolSlotIdx = 0;
#endif /* !AMP_LOOPBACK_DISABLED */

    // TODO: workaround for the self-wakeup
    g_MQSPlaying = 1;

    /* Enable output of Audio amplifier */
    GPIO_PinWrite(BOARD_MQS_OE_GPIO_PORT, BOARD_MQS_OE_GPIO_PIN, 1);

    audioSize = size - (size % 96);

    LOGD("[MQS] Playing Audio of %d samples (%d ms)", audioSize, (audioSize / 96));

    while (audioPlayedSize < audioSize)
    {
        /* A new slot should be available in one MQS_AUDIO_SLOT_MS.
         * In case there is no empty slot in two MQS_AUDIO_SLOT_MS, something is wrong. */
        if (xSemaphoreTake(s_MqsSemFreeSlots, (TickType_t)(2 * MQS_AUDIO_SLOT_MS)) != pdTRUE)
        {
            LOGE("[MQS] Playing Failed, played %d samples (%d ms)", audioPlayedSize, (audioPlayedSize / 96));
            statusOk = false;
            break;
        }

        if ((audioSize - audioPlayedSize) >= MQS_AUDIO_SLOT_SIZE)
        {
            audioChunkSize = MQS_AUDIO_SLOT_SIZE;
        }
        else
        {
            audioChunkSize = audioSize - audioPlayedSize;
        }

        memcpy(s_MqsStreamPool[mqsAudioPoolSlotIdx], &buffer[audioPlayedSize], audioChunkSize);
        if (audioChunkSize < MQS_AUDIO_SLOT_SIZE)
        {
            memset(&s_MqsStreamPool[mqsAudioPoolSlotIdx][audioChunkSize], 0, MQS_AUDIO_SLOT_SIZE - audioChunkSize);
            audioChunkSize = MQS_AUDIO_SLOT_SIZE;
        }

        xfer.data     = s_MqsStreamPool[mqsAudioPoolSlotIdx];
        xfer.dataSize = audioChunkSize;

        /* Differentiate and adjust volume */
        _VolDiffControl(xfer.data, xfer.dataSize);

#if !AMP_LOOPBACK_DISABLED
        /* Downsample audio data (from 48KHz to 16KHz) for AFE */
        SLN_DSP_downsample_by_3((int16_t *)xfer.data, xfer.dataSize / 2,
                                (int16_t *)s_MqsAfeFeedback[mqsFeedbackPoolSlotIdx]);
#endif /* !AMP_LOOPBACK_DISABLED */

        /* Play this chunk */
        tansferStatus = SAI_TransferSendEDMA(MQS_SAI, &s_SaiTxHandle, &xfer);
        if (tansferStatus != kStatus_Success)
        {
            LOGE("[MQS] SAI_TransferSendEDMA failed %d for %d samples", tansferStatus, xfer.dataSize);
            statusOk = false;
            break;
        }

#if !AMP_LOOPBACK_DISABLED
        /* Notify AFE in order to perform AEC */
        _SpeakerToAfeNotify((int16_t *)s_MqsAfeFeedback[mqsFeedbackPoolSlotIdx], xfer.dataSize / 6);
        mqsFeedbackPoolSlotIdx = (mqsFeedbackPoolSlotIdx + 1) % MQS_FEEDBACK_SLOT_CNT;
#endif /* !AMP_LOOPBACK_DISABLED */

        mqsAudioPoolSlotIdx = (mqsAudioPoolSlotIdx + 1) % MQS_AUDIO_SLOT_CNT;
        audioPlayedSize += audioChunkSize;
    }

    if (statusOk)
    {
        /* s_MqsSemFreeSlots will not be free at least MQS_AUDIO_SLOT_CNT slots
         * Give one extra slot timeout to let AFE process everything. */
        vTaskDelay(MQS_AUDIO_SLOT_MS * (MQS_AUDIO_SLOT_CNT + 1));

        if (uxSemaphoreGetCount(s_MqsSemFreeSlots) != MQS_AUDIO_SLOT_CNT)
        {
            LOGE("[MQS] Playing Failed, not all slots are free %d", uxSemaphoreGetCount(s_MqsSemFreeSlots));
            statusOk = false;
        }
    }

    if (statusOk)
    {
        LOGD("[MQS] Playing Done");
    }
    else
    {
        SAI_TransferTerminateSendEDMA(MQS_SAI, &s_SaiTxHandle);

        for (uint8_t i = 0; i < MQS_AUDIO_SLOT_CNT; i++)
        {
            if (uxSemaphoreGetCount(s_MqsSemFreeSlots) == MQS_AUDIO_SLOT_CNT)
            {
                break;
            }
            else
            {
                xSemaphoreGive(s_MqsSemFreeSlots);
            }
        }
        LOGE("[MQS] Playing Failed, MQS stopped, semaphore value is %d", uxSemaphoreGetCount(s_MqsSemFreeSlots));
    }

    /* Disable output of Audio amplifier */
    GPIO_PinWrite(BOARD_MQS_OE_GPIO_PORT, BOARD_MQS_OE_GPIO_PIN, 0);
    // TODO: workaround for the self-wakeup
    g_MQSPlaying = 0;
}

static hal_output_status_t HAL_OutputDev_MqsAudio_InferComplete(const output_dev_t *dev,
                                                                output_algo_source_t source,
                                                                void *inferResult)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    uint8_t *audioBuffer      = NULL;
    uint32_t audioLen         = 0;

    APP_OutputDev_MqsAudio_InferCompleteDecode(source, inferResult, (void *)&audioBuffer, &audioLen);
    if (audioBuffer != NULL && audioLen != 0)
    {
        MQS_SOUND_PLAY_FUNC(audioBuffer, audioLen);
    }

    return error;
}

static hal_output_status_t HAL_OutputDev_MqsAudio_Input_notify(const output_dev_t *dev, void *data)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    uint8_t *audioBuffer      = NULL;
    uint32_t audioLen         = 0;
    event_base_t eventBase    = *(event_base_t *)data;

    LOGD("MQS:Input Notify Event %d", eventBase.eventId);
    if (eventBase.eventId == kEventID_GetSpeakerVolume)
    {
        speaker_volume_event_t speaker;
        speaker.volume = 100 /*HAL_OutputDev_SmartLockConfig_GetSpeakerVolume()*/;
        LOGD("Current volume is: %d", speaker.volume);
        if (eventBase.respond != NULL)
        {
            eventBase.respond(kEventID_GetSpeakerVolume, &speaker, kEventStatus_Ok, true);
        }
    }
    else if (eventBase.eventId == kEventID_SetSpeakerVolume)
    {
        event_common_t event               = *(event_common_t *)data;
        event_status_t eventResponseStatus = kEventStatus_Ok;
        if (kSLNConfigStatus_Success /*!= HAL_OutputDev_SmartLockConfig_SetSpeakerVolume(event.speakerVolume.volume)*/)
        {
            error               = kStatus_HAL_OutputError;
            eventResponseStatus = kEventStatus_Error;
            LOGE("Failed to save speaker volume to flash");
        }
        else
        {
            s_OutputDev_MqsAudio.configs[kMQSConfigs_Volume].value = event.speakerVolume.volume;
        }
        if (eventBase.respond != NULL)
        {
            eventBase.respond(kEventID_SetSpeakerVolume, &event.speakerVolume, eventResponseStatus, true);
        }
    }
    else
    {
        APP_OutputDev_MqsAudio_InputNotifyDecode(dev, data, (void *)&audioBuffer, &audioLen);
        if (audioBuffer != NULL && audioLen != 0)
        {
            MQS_SOUND_PLAY_FUNC(audioBuffer, audioLen);
        }
    }
    return error;
}

const static output_dev_event_handler_t s_MqsAudioHandler = {
    .inferenceComplete = HAL_OutputDev_MqsAudio_InferComplete,
    .inputNotify       = HAL_OutputDev_MqsAudio_Input_notify,
};

static hal_output_status_t HAL_OutputDev_MqsAudio_Start(const output_dev_t *dev)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;

    if (FWK_OutputManager_RegisterEventHandler(dev, &s_MqsAudioHandler) != 0)
    {
        LOGE("HAL_OutputDev_MqsAudio_Start failed - FWK_OutputManager_RegisterEventHandler");
        error = kStatus_HAL_OutputError;
    }

    if (error == kStatus_HAL_OutputSuccess)
    {
        _ConfigMqs();

        s_MqsSemFreeSlots = xSemaphoreCreateCounting(MQS_AUDIO_SLOT_CNT, MQS_AUDIO_SLOT_CNT);
        if (s_MqsSemFreeSlots == NULL)
        {
            LOGE("HAL_OutputDev_MqsAudio_Start failed - xSemaphoreCreateCounting");
            error = kStatus_HAL_OutputError;
        }
    }

    if (error == kStatus_HAL_OutputSuccess)
    {
#if DEFER_PLAYBACK_TO_TASK
        s_MqsAudioTask.task.msgHandle  = HAL_OutputDev_MqsAudio_MsgHandle;
        s_MqsAudioTask.task.taskInit   = NULL;
        s_MqsAudioTask.task.data       = (fwk_task_data_t *)&(s_MqsAudioTask.data);
        s_MqsAudioTask.task.taskId     = MQS_AUDIO_TASK_ID;
        s_MqsAudioTask.task.delayMs    = 1;
        s_MqsAudioTask.task.taskStack  = s_MqsAudioTaskStack;
        s_MqsAudioTask.task.taskBuffer = s_MqsAudioTaskTcbReference;
        s_MqsAudioTask.data.dev        = dev;
        FWK_Task_Start((fwk_task_t *)&s_MqsAudioTask, MQS_TASK_NAME, MQS_TASK_STACK, MQS_TASK_PRIORITY);
#endif

        /* TODO: Update 'HAL_OutputDev_SmartLockConfig_Get...' function to be one getter/setter */
        s_OutputDev_MqsAudio.configs[kMQSConfigs_Volume].value =
            100 /*HAL_OutputDev_SmartLockConfig_GetSpeakerVolume()*/;
    }

    return error;
}

void BOARD_InitMqsResource(void)
{
    clock_root_config_t mqsRootClock = {
        .clockOff = false, .mux = MQS_SAI_CLOCK_SOURCE_SELECT, .div = MQS_SAI_CLOCK_SOURCE_DIVIDER};

    const clock_audio_pll_config_t mqsAudioPllConfig = {
        .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
        .postDivider = 1,   /* Divider after the PLL, should only be 0, 1, 2, 3, 4, 5 */
        .numerator   = 77,  /* 30 bit numerator of fractional loop divider. */
        .denominator = 100, /* 30 bit denominator of fractional loop divider */
    };

    CLOCK_InitAudioPll(&mqsAudioPllConfig);
    CLOCK_SetRootClock(MQS_SAI_CLOCK, &mqsRootClock);
}

static hal_output_status_t HAL_OutputDev_MqsAudio_Init(output_dev_t *dev, output_dev_callback_t callback)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;

    sai_config_t saiConfig                               = {0};
    edma_channel_Preemption_config_t dmaPreemptionConfig = {0};
    sai_transceiver_t edmaTxConfig                       = {0};

    dev->cap.callback = callback;

    BOARD_InitMqsResource();

    BOARD_InitEDMA(MQS_DMA);
    EDMA_CreateHandle(&s_SaiDmaHandle, MQS_DMA, MQS_SAI_EDMA_TX_CH);

    dmaPreemptionConfig.enableChannelPreemption = true;
    dmaPreemptionConfig.enablePreemptAbility    = false;
    dmaPreemptionConfig.channelPriority         = 0;
    EDMA_SetChannelPreemptionConfig(MQS_DMA, MQS_SAI_EDMA_TX_CH, &dmaPreemptionConfig);

    DMAMUX_Init(MQS_DMAMUX);
    DMAMUX_SetSource(MQS_DMAMUX, MQS_SAI_EDMA_TX_CH, MQS_SAI_TX_SOURCE);
    DMAMUX_EnableChannel(MQS_DMAMUX, MQS_SAI_EDMA_TX_CH);

    NVIC_SetPriority(DMA0_DMA16_IRQn, MQS_SAI_DMA_IRQ_PRIO);

    /* Initialize SAI TX */
    SAI_TxGetDefaultConfig(&saiConfig);
    saiConfig.protocol = kSAI_BusLeftJustified;
    SAI_TxInit(MQS_SAI, &saiConfig);

    SAI_GetLeftJustifiedConfig(&edmaTxConfig, kSAI_WordWidth16bits, kSAI_Stereo, kSAI_Channel0Mask);
    SAI_TransferTxCreateHandleEDMA(MQS_SAI, &s_SaiTxHandle, _SaiCallback, NULL, &s_SaiDmaHandle);
    SAI_TransferTxSetConfigEDMA(MQS_SAI, &s_SaiTxHandle, &edmaTxConfig);

    /* Force bit clock to override standard enablement */
    SAI_TxSetBitClockRate(MQS_SAI, MQS_SAI_CLK_FREQ, kSAI_SampleRate48KHz, kSAI_WordWidth16bits,
                          edmaTxConfig.channelNums);

    return error;
}

static status_t _GetVolume(char *valueToString)
{
    itoa(s_OutputDev_MqsAudio.configs[kMQSConfigs_Volume].value, valueToString, 10);
    strcat(valueToString, "%");
    return kStatus_Success;
}

int HAL_OutputDev_MqsAudio_Register()
{
    int error = 0;
    LOGD("HAL_OutputDev_MqsAudio_Register");
    error = FWK_OutputManager_DeviceRegister(&s_OutputDev_MqsAudio);
    return error;
}
#endif /* ENABLE_OUTPUT_DEV_MqsAudio */
