/*
 * Copyright 2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "board_define.h"
#ifdef ENABLE_AUDIO_PROCESSING_DEV_Afe
#include "stdint.h"
#include "fwk_audio_processing.h"
#include "fwk_log.h"
#include "fwk_platform.h"
#include "hal_audio_processing_dev.h"
#include "hal_audio_defs.h"
#include "hal_event_descriptor_voice.h"
#include "sln_afe.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AFE_PCM_SAMPLE_SIZE_BYTES (2) /* 16bit depth */
#define AFE_BLOCKS_TO_ACCUMULATE  (3)

#if !AMP_LOOPBACK_DISABLED
/* Defines used to sync microphones with speaker */
#define PCM_CYCLE_DURATION_US  10000
#define SPEAKER_CONST_DELAY_US 2150

/* Should be the same with MQS_FEEDBACK_SLOT_CNT */
#define SPEAKER_FEEDBACK_SLOTS_CNT 3

typedef enum _sln_speaker_feedback_state
{
    kSpeakerFeedbackIdle,
    kSpeakerFeedbackFirstPacket,
    kSpeakerFeedbackSyncing,
    kSpeakerFeedbackPlaying,
} sln_speaker_feedback_state_t;

typedef struct _sln_speaker_feedback
{
    int16_t *audio_stream;
    uint32_t audio_length;
    uint32_t audio_played;
    uint32_t start_time;
} sln_speaker_feedback_t;
#endif /* !AMP_LOOPBACK_DISABLED */

#ifdef ENABLE_OUTPUT_DEV_AudioDump
#define AUDIO_DUMP_SLOTS_CNT 10
#endif /* ENABLE_OUTPUT_DEV_AudioDump */

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t *s_afeMemPool = NULL;
static AT_CACHEABLE_SECTION_ALIGN_OCRAM(uint8_t s_afeExternalMemory[AFE_MEM_SIZE_2MICS], 8);

AT_CACHEABLE_SECTION_ALIGN_OCRAM(static int16_t s_afeMicIn[AUDIO_PCM_SAMPLE_COUNT], 4);
AT_CACHEABLE_SECTION_ALIGN_OCRAM(static int16_t s_afeAudioIn[AUDIO_PCM_SINGLE_CH_SMPL_COUNT], 4);
AT_CACHEABLE_SECTION_ALIGN_OCRAM(static int16_t s_afeAudioOut[AUDIO_PCM_SINGLE_CH_SMPL_COUNT], 4);
AT_CACHEABLE_SECTION_ALIGN_OCRAM(static int16_t s_audioOut[AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_BLOCKS_TO_ACCUMULATE], 4);

static volatile uint32_t s_wakeWordLength = 0;

#if !AMP_LOOPBACK_DISABLED
static sln_speaker_feedback_state_t s_speakerState = kSpeakerFeedbackIdle;
static uint8_t s_speakerFeedbackSlotWrite          = 0;
static uint8_t s_speakerFeedbackSlotRead           = 0;
static uint8_t s_speakerFeedbackSlotFull           = 0;
static sln_speaker_feedback_t s_speakerFeedback[SPEAKER_FEEDBACK_SLOTS_CNT] = {0};
#endif /* !AMP_LOOPBACK_DISABLED */

#ifdef ENABLE_OUTPUT_DEV_AudioDump
AT_CACHEABLE_SECTION_ALIGN_OCRAM(static uint8_t s_dumpOutPool[AUDIO_DUMP_SLOTS_CNT][4][AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES], 2);
#endif /* ENABLE_OUTPUT_DEV_AudioDump */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#if !AMP_LOOPBACK_DISABLED
static void _addSpeakerFeedback(int16_t *buffer, uint32_t length, uint32_t timeUs);
static uint32_t _consumeSpeakerFeedback(int16_t *dst, uint32_t length);
static int16_t* _getSpeakerFeedback(void);
#endif /* !AMP_LOOPBACK_DISABLED */

/*******************************************************************************
 * Code
 ******************************************************************************/

#if !AMP_LOOPBACK_DISABLED
/**
 * @brief  Add speaker's audio packet to a feedback circular buffer in order to be used by AFE for AEC.
 *
 * @param  buffer Pointer to the buffer containing speaker's audio packet.
 * @param  length Length of speaker's audio packet.
 * @param  timeUs Timestamp when speaker started to play current audio packet.
 */
static void _addSpeakerFeedback(int16_t *buffer, uint32_t length, uint32_t timeUs)
{
    if (s_speakerFeedbackSlotFull < SPEAKER_FEEDBACK_SLOTS_CNT)
    {
        s_speakerFeedback[s_speakerFeedbackSlotWrite].audio_stream = buffer;
        s_speakerFeedback[s_speakerFeedbackSlotWrite].audio_length = length;
        s_speakerFeedback[s_speakerFeedbackSlotWrite].audio_played = 0;
        s_speakerFeedback[s_speakerFeedbackSlotWrite].start_time   = timeUs;

        if (s_speakerState == kSpeakerFeedbackIdle)
        {
            s_speakerState = kSpeakerFeedbackFirstPacket;
        }

        s_speakerFeedbackSlotWrite = (s_speakerFeedbackSlotWrite + 1) % SPEAKER_FEEDBACK_SLOTS_CNT;
        s_speakerFeedbackSlotFull++;
    }
    else
    {
        //LOGE("[AFE ERROR] s_speakerFeedback buffer is full, discarding feedback packet");
    }
}

/**
 * @brief  Extract speaker's audio chunk from feedback buffer in order to be used by AFE for AEC.
 *
 * @param  dst Pointer to the buffer where to read speaker's audio chunk. Pass NULL to discard data.
 * @param  length Length of speaker's audio chunk that should be read (or discarded).
 *
 * @return Number of extracted samples.
 */
static uint32_t _consumeSpeakerFeedback(int16_t *dst, uint32_t length)
{
    uint32_t dataPlayed = 0;

    while ((dataPlayed < length) && (s_speakerFeedbackSlotFull > 0))
    {
        sln_speaker_feedback_t *speakerSlot = NULL;
        int16_t *data                       = NULL;
        uint32_t dataAvailable              = 0;

        speakerSlot   = &s_speakerFeedback[s_speakerFeedbackSlotRead];
        data          = &speakerSlot->audio_stream[speakerSlot->audio_played];
        dataAvailable = speakerSlot->audio_length - speakerSlot->audio_played;

        if (dataAvailable >= (length - dataPlayed))
        {
            dataAvailable = (length - dataPlayed);
        }

        if (dst != NULL)
        {
            memcpy((uint8_t *)(&dst[dataPlayed]), data, dataAvailable * sizeof(uint16_t));
        }

        dataPlayed                += dataAvailable;
        speakerSlot->audio_played += dataAvailable;
        if (speakerSlot->audio_played >= speakerSlot->audio_length)
        {
            s_speakerFeedbackSlotRead = (s_speakerFeedbackSlotRead + 1) % SPEAKER_FEEDBACK_SLOTS_CNT;
            s_speakerFeedbackSlotFull--;
        }
    }

    if (dst != NULL)
    {
        /* Fill with zeroes dst buffer in case there is no enough data in the feedback buffer */
        memset(&dst[dataPlayed], 0, (length - dataPlayed) * sizeof(int16_t));
    }

    return dataPlayed;
}

/**
 * @brief  Get speaker's audio chunk from feedback buffer in order to be used by AFE for AEC.
 *
 * @return Pointer to the buffer containing feedback data from Speaker.
 */
static int16_t* _getSpeakerFeedback(void)
{
    static int32_t speakerTotalDelayUs = 0;
    static uint32_t speakerDelayedUs   = 0;

    int16_t *ampStream = s_afeAudioIn;

    if (s_speakerState == kSpeakerFeedbackFirstPacket)
    {
        uint32_t currentTime = FWK_CurrentTimeUs();
        /* Once received first chunk of an audio from the amplifier, it is required to calculate and add a "pre" delay
         * in order to synchronize speaker and mics streams. There is a chance that the first chunk is received too late,
         * in this case instead of adding a "pre" delay, it is required to skip some chunks from the beginning of the audio. */
        speakerTotalDelayUs = (int32_t)(currentTime - s_speakerFeedback[s_speakerFeedbackSlotRead].start_time);
        if (speakerTotalDelayUs <= 0)
        {
            speakerTotalDelayUs = 0;
        }

        speakerTotalDelayUs = PCM_CYCLE_DURATION_US - speakerTotalDelayUs + SPEAKER_CONST_DELAY_US;
        if (speakerTotalDelayUs == 0)
        {
            s_speakerState = kSpeakerFeedbackPlaying;
            LOGD("[AFE] No need for sync");
        }
        else if (speakerTotalDelayUs > 0)
        {
            s_speakerState = kSpeakerFeedbackSyncing;
            LOGD("[AFE] Sync required: Pre-Delay = %d [us]", speakerTotalDelayUs);
        }
        else
        {
            uint32_t samplesToSkip = (-1) * ((speakerTotalDelayUs * 16) / 1000);
            _consumeSpeakerFeedback(NULL, samplesToSkip);
            LOGD("[AFE] Sync required: Post-Skip = %d [us]   Samples skipped = %d", ((-1) * speakerTotalDelayUs), samplesToSkip);

            s_speakerState = kSpeakerFeedbackPlaying;
        }
    }

    if (s_speakerState == kSpeakerFeedbackSyncing)
    {
        uint16_t currentDelayUs      = 0;
        uint16_t currentDelaySamples = 0;

        /* Max delayUs per cycle is 10000us */
        currentDelayUs = speakerTotalDelayUs - speakerDelayedUs;
        if (currentDelayUs > PCM_CYCLE_DURATION_US)
        {
            currentDelayUs = PCM_CYCLE_DURATION_US;
        }

        currentDelaySamples = (currentDelayUs * 16) / 1000;

        memset((uint8_t *)ampStream, 0, currentDelaySamples * sizeof(int16_t));
        speakerDelayedUs += currentDelayUs;

        /* If the "pre" delay was consumed, fill the remaining bytes in the amplifier buffer with
         * the beginning of the audio packet and move to the second phase where audio packet is consumed
         * chunk by chunk for Audio Echo Cancellation. */
        if (speakerDelayedUs >= speakerTotalDelayUs)
        {
            _consumeSpeakerFeedback(&ampStream[currentDelaySamples], (AUDIO_PCM_SINGLE_CH_SMPL_COUNT - currentDelaySamples));
            s_speakerState = kSpeakerFeedbackPlaying;

            LOGD("[AFE] Synced, Delayed = %d [us]  Consumed = %d [samples]", speakerDelayedUs, (AUDIO_PCM_SINGLE_CH_SMPL_COUNT - currentDelaySamples));
        }
    }
    else if (s_speakerState == kSpeakerFeedbackPlaying)
    {
        uint32_t extractedFeedbackSize = 0;

        extractedFeedbackSize = _consumeSpeakerFeedback(ampStream, AUDIO_PCM_SINGLE_CH_SMPL_COUNT);
        if (extractedFeedbackSize == 0)
        {
            LOGD("[AFE] Feedback buffer is empty");

            memset(ampStream, 0, AUDIO_PCM_SINGLE_CH_SMPL_COUNT * sizeof(int16_t));

            s_speakerState             = kSpeakerFeedbackIdle;
            s_speakerFeedbackSlotWrite = 0;
            s_speakerFeedbackSlotRead  = 0;
            s_speakerFeedbackSlotFull  = 0;
            memset(s_speakerFeedback, 0, sizeof(s_speakerFeedback));

            speakerTotalDelayUs = 0;
            speakerDelayedUs    = 0;

            ampStream = NULL;
        }
    }
    else
    {
        ampStream = NULL;
    }

#ifndef SLN_AFE_NXP
    /* Non NXP AFE can't accept NULL as ampStream. */
    ampStream = s_afeAudioIn;
#endif /* SLN_AFE_NXP */

    return ampStream;
}
#endif /* !AMP_LOOPBACK_DISABLED */

hal_audio_processing_status_t audio_processing_afe_init(audio_processing_dev_t *dev,
                                                        audio_processing_dev_callback_t callback)
{
    hal_audio_processing_status_t error      = kStatus_HAL_AudioProcessingSuccess;
    sln_afe_states_t afeStatus               = kAfeSuccess;
    sln_afe_configuration_params_t afeConfig = {0};

    dev->cap.callback = callback;

    afeConfig.numberOfMics    = AUDIO_PDM_MIC_COUNT;
    afeConfig.afeMemBlock     = s_afeExternalMemory;
    afeConfig.afeMemBlockSize = sizeof(s_afeExternalMemory);

#ifdef SLN_AFE_NXP
    afeConfig.postProcessedGain = 3;
    afeConfig.wakeWordMaxLength = WAKE_WORD_MAX_LENGTH_MS;

#if !AMP_LOOPBACK_DISABLED
    afeConfig.aecEnabled      = 1;
    afeConfig.aecFilterLength = AEC_FILTER_LENGTH;
#else
    afeConfig.aecEnabled      = 0;
    afeConfig.aecFilterLength = 0;
#endif /* !AMP_LOOPBACK_DISABLED */

#ifdef VAD_ENABLED
    afeConfig.vadEnabled = 1;
#else
    afeConfig.vadEnabled = 0;
#endif /* VAD_ENABLED */

    afeConfig.micsPosition[0][0] = -30;
    afeConfig.micsPosition[0][1] = 0;
    afeConfig.micsPosition[0][2] = 0;
    afeConfig.micsPosition[1][0] = 30;
    afeConfig.micsPosition[1][1] = 0;
    afeConfig.micsPosition[1][2] = 0;

#else
    afeConfig.postProcessedGain = 0x0600;
    afeConfig.boostAecSeconds   = 10;
#endif /* SLN_AFE_NXP */

    afeStatus = SLN_AFE_Init(&s_afeMemPool, FWK_MALLOC, FWK_FREE, &afeConfig);
    if (afeStatus != kAfeSuccess)
    {
        LOGE("[AFE] SLN_AFE_Init failed %d", afeStatus);
        error = kStatus_HAL_AudioProcessingError;
    }

    return error;
}

hal_audio_processing_status_t audio_processing_afe_deinit(const audio_processing_dev_t *dev)
{
    hal_audio_processing_status_t error = kStatus_HAL_AudioProcessingSuccess;
    return error;
}

hal_audio_processing_status_t audio_processing_afe_start(const audio_processing_dev_t *dev)
{
    hal_audio_processing_status_t error = kStatus_HAL_AudioProcessingSuccess;
    return error;
}

hal_audio_processing_status_t audio_processing_afe_stop(const audio_processing_dev_t *dev)
{
    hal_audio_processing_status_t error = kStatus_HAL_AudioProcessingSuccess;
    return error;
}

hal_audio_processing_status_t audio_processing_afe_notify(const audio_processing_dev_t *dev, void *param)
{
    hal_audio_processing_status_t error = kStatus_HAL_AudioProcessingSuccess;
    event_voice_t event = *(event_voice_t *)param;

    switch (event.event_base.eventId)
    {
    case ASR_TO_AFE_FEEDBACK:
        s_wakeWordLength = event.ww_len.ww_len;
        break;

#if !AMP_LOOPBACK_DISABLED
    case SPEAKER_TO_AFE_FEEDBACK:
        _addSpeakerFeedback(event.speaker_audio.audio_stream,
                            event.speaker_audio.audio_length,
                            event.speaker_audio.start_time);
        break;
#endif /* !AMP_LOOPBACK_DISABLED */

    default:
        break;
    }

    return error;
}

hal_audio_processing_status_t audio_processing_afe_run(const audio_processing_dev_t *dev, void *param)
{
    hal_audio_processing_status_t error = kStatus_HAL_AudioProcessingSuccess;
    event_voice_t event                 = *(event_voice_t *)param;

    if (event.event_base.eventId == AUDIO_IN)
    {
        static int16_t *s_audioOutPtr = s_audioOut;
        static uint8_t s_accSamples   = 0;

        sln_afe_states_t afeStatus = kAfeSuccess;

        /* Process microphone streams */
        int32_t *pcmIn32Bit = event.audio_in.audio_stream;
        int16_t *ampStream  = s_afeAudioIn;

        /* Convert PCM stream buffer to LLRR format for AFE/ASR */
        int32_t leftIdx  = 0;
        int32_t rightIdx = AUDIO_PCM_SINGLE_CH_SMPL_COUNT;
        for (int32_t i = 0; i < AUDIO_PCM_SAMPLE_COUNT; i += 2)
        {
            s_afeMicIn[leftIdx++]  = (int16_t)(pcmIn32Bit[i] >> 16);
            s_afeMicIn[rightIdx++] = (int16_t)(pcmIn32Bit[i + 1] >> 16);
        }

#if !AMP_LOOPBACK_DISABLED
        ampStream = _getSpeakerFeedback();
#endif /* !AMP_LOOPBACK_DISABLED */

        /* Run mic streams through the AFE */
#ifdef SLN_AFE_NXP
        afeStatus = SLN_AFE_Process_Audio(s_afeMemPool, s_afeMicIn, ampStream, s_afeAudioOut);
#else
        afeStatus = SLN_AFE_Process_Audio(s_afeMemPool, s_afeMicIn, ampStream, s_afeAudioOut);
#endif /* SLN_AFE_NXP */

        if (afeStatus != kAfeSuccess)
        {
            LOGE("[AFE] SLN_AFE_Process_Audio failed %d", afeStatus);
            error = kStatus_HAL_AudioProcessingError;
        }

#ifdef ENABLE_OUTPUT_DEV_AudioDump
        static uint8_t s_dumpOutPoolIdx = 0;

        memcpy(s_dumpOutPool[s_dumpOutPoolIdx][0], s_afeMicIn, AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES);
        memcpy(s_dumpOutPool[s_dumpOutPoolIdx][1], s_afeMicIn + AUDIO_PCM_SINGLE_CH_SMPL_COUNT, AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES);
        memcpy(s_dumpOutPool[s_dumpOutPoolIdx][2], s_afeAudioIn, AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES);
        memcpy(s_dumpOutPool[s_dumpOutPoolIdx][3], s_afeAudioOut, AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES);

        dev->cap.callback(dev, kAudioProcessingEvent_Dump, s_dumpOutPool[s_dumpOutPoolIdx], 4 * AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES, 0);

        s_dumpOutPoolIdx = (s_dumpOutPoolIdx + 1) % AUDIO_DUMP_SLOTS_CNT;
#endif /* ENABLE_OUTPUT_DEV_AudioDump */

        /* Pass output of AFE to wake word */
        memcpy((uint8_t *)s_audioOutPtr, s_afeAudioOut, AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_PCM_SAMPLE_SIZE_BYTES);
        s_audioOutPtr += AUDIO_PCM_SINGLE_CH_SMPL_COUNT;
        s_accSamples++;

        /* If we've accumulated enough audio, send it to ASR */
        if (s_accSamples == AFE_BLOCKS_TO_ACCUMULATE)
        {
            if (dev->cap.callback != NULL)
            {
                dev->cap.callback(dev, kAudioProcessingEvent_Done, s_audioOut,
                                  AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_BLOCKS_TO_ACCUMULATE, 0);
            }

            s_audioOutPtr = s_audioOut;
            s_accSamples  = 0;
        }

        if (s_wakeWordLength != 0)
        {
            uint32_t wakeWordLengthSamples = s_wakeWordLength + (s_accSamples * AUDIO_PCM_SINGLE_CH_SMPL_COUNT);
            uint32_t wakeWordLengthMs      = wakeWordLengthSamples / (AUDIO_PCM_SINGLE_CH_SMPL_COUNT / 10);

            LOGD("[AFE] Wake Word Length = %d [ms], %d(%d) [samples]", wakeWordLengthMs, wakeWordLengthSamples, s_accSamples);

#ifdef SLN_AFE_NXP
            afeStatus = SLN_AFE_Trigger_Found(s_afeMemPool, wakeWordLengthSamples);
#else
            afeStatus = SLN_AFE_Trigger_Found(s_afeMemPool);
#endif /* SLN_AFE_NXP */

            if (afeStatus != kAfeSuccess)
            {
                LOGE("[AFE] SLN_AFE_Trigger_Found failed %d", afeStatus);
                error = kStatus_HAL_AudioProcessingError;
            }
            s_wakeWordLength = 0;
        }
    }

    return error;
}

const static audio_processing_dev_operator_t audio_processing_afe_ops = {
    .init        = audio_processing_afe_init,
    .deinit      = audio_processing_afe_deinit,
    .start       = audio_processing_afe_start,
    .stop        = audio_processing_afe_stop,
    .run         = audio_processing_afe_run,
    .inputNotify = audio_processing_afe_notify,
};

static audio_processing_dev_t audio_processing_afe = {
    .id = 1, .name = "AFE", .ops = &audio_processing_afe_ops, .cap = {.callback = NULL}};

int HAL_AudioProcessingDev_Afe_Register()
{
    int error = 0;
    LOGD("HAL_AudioProcessingDev_Afe_Register");
    error = FWK_AudioProcessing_DeviceRegister(&audio_processing_afe);
    return error;
}
#endif /* ENABLE_AUDIO_PROCESSING_DEV_Afe */
