/*
 * Copyright 2021-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "board_define.h"
#ifdef ENABLE_VOICEALGO_Asr
#include "hal_voice_algo_asr_local.h"

#include "fwk_log.h"
#include "fwk_message.h"
#include "fwk_voice_algo_manager.h"
#include "hal_event_descriptor_voice.h"

/* local voice includes */
#include "IndexCommands.h"
#include "local_voice_model.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define NUM_SAMPLES_AFE_OUTPUT (480)

#define WAKE_WORD_MEMPOOL_SIZE    (60 * 1024)
#define ZH_WAKE_WORD_MEMPOOL_SIZE (90 * 1024)
#define COMMAND_MEMPOOL_SIZE      (100 * 1024)

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if CHINESE_MANDARIN_SUPPORTED
AT_NONCACHEABLE_SECTION_ALIGN_DTC(static uint8_t s_memPoolWLang1[ZH_WAKE_WORD_MEMPOOL_SIZE], 8);
#else
AT_NONCACHEABLE_SECTION_ALIGN_DTC(static uint8_t s_memPoolWLang1[WAKE_WORD_MEMPOOL_SIZE], 8);
#endif

AT_CACHEABLE_SECTION_ALIGN_OCRAM(static uint8_t s_memPoolCmd[COMMAND_MEMPOOL_SIZE], 8);

typedef struct _asr_voice_param
{
    struct asr_language_model langModel[NUM_LANGUAGES];
    struct asr_inference_engine infWW[NUM_INFERENCES_WW];
    struct asr_inference_engine infCMD;
    asr_voice_control_t voiceControl;
    asr_voice_config_t voiceConfig;
    asr_inference_result_t voiceResult;
} asr_voice_param_t;

static asr_voice_param_t s_AsrEngine;

static void voice_algo_asr_result_notify(asr_inference_result_t *result, uint32_t wwLength);

/* Used to trigger and handle command timeout */
static TimerHandle_t s_asrCmdTimeoutTimer = NULL;
static volatile bool s_asrCmdTimeout      = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Utility function to extract indices from bitwise variable. this is used for asr_inference_t.
 */
static unsigned int decode_bitshift(unsigned int x)
{
    unsigned int y = 1; // starting from index 1 (not 0)
    while (x >>= 1)
        y++;
    return y;
}

/*!
 * @brief Utility function to break D-Spotter model binary pack into multiple groups where each group will represent an
 * inference engine combined with base model.
 */
static signed int unpackBin(unsigned char lpbyBin[], unsigned char *lppbyModel[], int32_t nMaxNumModel)
{
    unsigned int *lpnBin     = (unsigned int *)lpbyBin;
    signed int nNumBin       = lpnBin[0];
    unsigned int *lpnBinSize = lpnBin + 1;
    signed int i;

    lppbyModel[0] = (unsigned char *)(lpnBinSize + nNumBin);
    for (i = 1; i < nNumBin; i++)
    {
        if (i >= nMaxNumModel)
            break;
        lppbyModel[i] = lppbyModel[i - 1] + lpnBinSize[i - 1];
    }

    return i;
}

/*!
 * @brief Language model installation.
 */
static int32_t install_language(asr_voice_control_t *pAsrCtrl,
                                struct asr_language_model *pLangModel,
                                asr_language_t lang,
                                unsigned char *pAddrBin,
                                uint8_t nGroups)
{
    sln_asr_local_states_t status = kAsrLocalSuccess;
    uint8_t nGroupsMapID          = nGroups - 2; // -1 cause there is no MapID group for base model,
                                        // -1 cause the nGroups contains 1 more section with the no of mapID binaries

    if (lang && pAddrBin && nGroups)
    {
        pLangModel->iWhoAmI = lang;
        pLangModel->addrBin = pAddrBin;
        pLangModel->nGroups = nGroups;
        pLangModel->next    = pAsrCtrl->langModel;
        pAsrCtrl->langModel = pLangModel;

        if ((status = unpackBin(pAddrBin, pLangModel->addrGroup, nGroups)) <
            nGroups) // unpack group addresses from model binary
        {
            LOGD("Invalid bin. Error Code: %d.\r\n", status);
        }
        else
        {
            if ((status = unpackBin(pLangModel->addrGroup[nGroups - 1], pLangModel->addrGroupMapID, nGroupsMapID)) <
                (nGroupsMapID)) // unpack group addresses from mapID binary
            {
                LOGD("Invalid bin. Error Code: %d.\r\n", status);
            }
        }
    }
    else
        status = 1;

    return status;
}

/*!
 * @brief Inference engine installation.
 */
static uint32_t install_inference_engine(asr_voice_control_t *pAsrCtrl,
                                         struct asr_inference_engine *pInfEngine,
                                         asr_language_t lang,
                                         asr_inference_t infType,
                                         char **idToString,
                                         unsigned char *addrMemPool,
                                         uint32_t sizeMemPool)
{
    sln_asr_local_states_t status = kAsrLocalSuccess;

    if (pAsrCtrl && pInfEngine && lang && infType && addrMemPool && sizeMemPool)
    {
        pInfEngine->iWhoAmI_inf  = infType;
        pInfEngine->iWhoAmI_lang = lang;
        pInfEngine->handler      = NULL;
        pInfEngine->nGroups      = 2;
        pInfEngine->idToKeyword  = idToString;
        pInfEngine->memPool      = addrMemPool;
        pInfEngine->memPoolSize  = sizeMemPool;
        if (infType == ASR_WW)
        {                                                  // linked list for WW engines
            pInfEngine->next      = pAsrCtrl->infEngineWW; // the end of pInfEngine->next should be NULL
            pAsrCtrl->infEngineWW = pInfEngine;
        }
        else
        { // linked list for CMD engines. Dialog demo needs a linked list of CMD engines.
            pInfEngine->next       = pAsrCtrl->infEngineCMD;
            pAsrCtrl->infEngineCMD = pInfEngine;
        }
    }
    else
        status = kAsrLocalInstallFailed;

    return status;
}

/*!
 * @brief Checks memory pool size for WW / CMD engines.
 */
static void verify_inference_handler(struct asr_inference_engine *p)
{
    sln_asr_local_states_t status = kAsrLocalSuccess;
    int32_t mem_usage;
    int32_t mem_size_ww;

    mem_usage = SLN_ASR_LOCAL_Verify(p->addrGroup[0], (unsigned char **)&p->addrGroup[1], 1, k_nMaxTime);

    if (p->iWhoAmI_lang == ASR_CHINESE)
    {
        mem_size_ww = ZH_WAKE_WORD_MEMPOOL_SIZE;
    }
    else
    {
        mem_size_ww = WAKE_WORD_MEMPOOL_SIZE;
    }

    if ((p->iWhoAmI_inf == ASR_WW) && (mem_usage > mem_size_ww))
    {
        LOGD("Memory size %d for WW exceeds the memory pool %d!\r\n", mem_usage, WAKE_WORD_MEMPOOL_SIZE);
        status = kAsrLocalOutOfMemory;
    }
    else if ((p->iWhoAmI_inf != ASR_WW) && (mem_usage > COMMAND_MEMPOOL_SIZE))
    {
        LOGD("Memory size %d for CMD exceeds the memory pool %d!\r\n", mem_usage, COMMAND_MEMPOOL_SIZE);
        status = kAsrLocalOutOfMemory;
    }

    if (status != kAsrLocalSuccess)
    {
        // TODO show an LED
    }
}

/*!
 * @brief Handler should be set with valid
 *  p->addrGroup[0] (D-Spotter base model address) and
 *  p->addrGroup[1] (D-Spotter application group such as WW, CMD for IoT, etc)
 */
static void set_inference_handler(struct asr_inference_engine *p)
{
    int status = kAsrLocalSuccess;

    p->handler = SLN_ASR_LOCAL_Init(p->addrGroup[0], (unsigned char **)&p->addrGroup[1], 1, k_nMaxTime, p->memPool,
                                    p->memPoolSize, (int32_t *)&status);

    if (status != kAsrLocalSuccess)
    {
        // TODO show an LED
        LOGD("Could not initialize ASR engine. Please check language settings or license limitations!\r\n");
    }

    if ((status = SLN_ASR_LOCAL_Set_CmdMapID(p->handler, &p->addrGroupMapID, 1)) != kAsrLocalSuccess)
    {
        LOGD("Fail to set map id! - %d\r\n", status);
    }
}

/*!
 * @brief Handler should be reset after WW or CMDs are detected.
 */
static void reset_inference_handler(struct asr_inference_engine *p)
{
    SLN_ASR_LOCAL_Reset(p->handler);
}

/*!
 * @brief Initialize WW inference engine from the installed language models.
 *  After, pInfEngine should be a linked list of the installed languages.
 */
static void init_WW_engine(asr_voice_control_t *pAsrCtrl, asr_inference_t infType)
{
    struct asr_inference_engine *pInfEngine;
    struct asr_language_model *pLang;
    int idx       = decode_bitshift(ASR_WW); // decode the bitwise ASR_WW which is 1.
    int idx_mapID = idx - 1;                 // the index for mapIDs starts from 0 instead of 1

    pInfEngine = pAsrCtrl->infEngineWW;
    for (pLang = pAsrCtrl->langModel; pLang != NULL; pLang = pLang->next)
    {
        pInfEngine->addrGroup[0]   = pLang->addrGroup[0];              // language model's base
        pInfEngine->addrGroup[1]   = pLang->addrGroup[idx];            // language model's wake word group
        pInfEngine->addrGroupMapID = pLang->addrGroupMapID[idx_mapID]; // language model's wake word mapID group

        verify_inference_handler(pInfEngine); // verify inference handler, checking mem pool size
        set_inference_handler(pInfEngine);    // set inf engine to ww mode for each language.

        pInfEngine = pInfEngine->next; // the end of pInfEngine->next should be NULL.
    }
}

/*!
 * @brief Initialize CMD inference engine from the installed language models.
 *  After, pInfEngine does not need to be a linked list for Demo #1 and #2 but does for Demo #3 (dialog).
 */
static void init_CMD_engine(asr_voice_control_t *pAsrCtrl, asr_inference_t infType)
{
    struct asr_inference_engine *pInfEngine;
    struct asr_language_model *pLang;
    int idx       = decode_bitshift(infType); // decode the bitwise infType variable.
    int idx_mapID = idx - 1;                  // the index for mapIDs starts from 0 instead of 1

    pInfEngine = pAsrCtrl->infEngineCMD;

    pLang                      = pAsrCtrl->langModel;   // langModel for CMD inf engine is selected when WW is detected.
    pInfEngine->addrGroup[0]   = pLang->addrGroup[0];   // the selected language model's base
    pInfEngine->addrGroup[1]   = pLang->addrGroup[idx]; // the selected language model's infType group
    pInfEngine->addrGroupMapID = pLang->addrGroupMapID[idx_mapID]; // the selected language model's mapID group

    verify_inference_handler(pInfEngine); // verify inference handler, checking mem pool size
    set_inference_handler(pInfEngine);    // set inf engine to ww mode for each language.
}

/*!
 * @brief Set language WW recognition engines.
 */
static void set_WW_engine(asr_voice_control_t *pAsrCtrl)
{
    struct asr_inference_engine *pInf = pAsrCtrl->infEngineWW;

    for (pInf = pAsrCtrl->infEngineWW; pInf != NULL; pInf = pInf->next)
    {
        set_inference_handler(pInf);
    }
}

/*!
 * @brief Reset language WW recognition engines.
 */
static void reset_WW_engine(asr_voice_control_t *pAsrCtrl)
{
    struct asr_inference_engine *pInf = pAsrCtrl->infEngineWW;

    for (pInf = pAsrCtrl->infEngineWW; pInf != NULL; pInf = pInf->next)
    {
        reset_inference_handler(pInf);
    }
}

/*!
 * @brief Set specific language CMD recognition engine, post WW detection.
 */
static void set_CMD_engine(asr_voice_control_t *pAsrCtrl,
                           asr_language_t langType,
                           asr_inference_t infCMDType,
                           char **cmdString)
{
    struct asr_language_model *pLang;
    struct asr_inference_engine *pInf = pAsrCtrl->infEngineCMD;
    int idx                           = decode_bitshift(infCMDType); // decode the bitwise infType variable
    int idx_mapID                     = idx - 1;                     // the index for mapIDs starts from 0 instead of 1

    for (pLang = pAsrCtrl->langModel; pLang != NULL; pLang = pLang->next)
    {
        if (pLang->iWhoAmI == langType)
        {
            pInf->iWhoAmI_inf  = infCMDType;
            pInf->iWhoAmI_lang = langType;
            pInf->addrGroup[0] = pLang->addrGroup[0]; // base model. should be same with WW's base
            if (pLang->addrGroup[idx] != NULL)
            {
                pInf->addrGroup[1]   = pLang->addrGroup[idx];
                pInf->addrGroupMapID = pLang->addrGroupMapID[idx_mapID];
            }
            set_inference_handler(pInf);
            pInf->idToKeyword = cmdString;
            break; // exit for loop, once pInf is set with the intended language
        }
    }
}

/*!
 * @brief Reset specific language CMD recognition engine.
 */
static void reset_CMD_engine(asr_voice_control_t *pAsrCtrl)
{
    struct asr_inference_engine *pInf = pAsrCtrl->infEngineCMD;

    reset_inference_handler(pInf);
}

/*!
 * @brief Process audio stream to detect wake words or commands.
 */
static int asr_process_audio_buffer(void *handler, int16_t *audBuff, uint16_t bufSize, asr_inference_t infType)
{
    int status = 0;
    // reset values
    s_AsrEngine.voiceControl.result.keywordID[0] = 0xFFFF;
    s_AsrEngine.voiceControl.result.keywordID[1] = 0xFFFF;
    s_AsrEngine.voiceControl.result.cmdMapID     = 0xFF;

    status = SLN_ASR_LOCAL_Process(handler, audBuff, bufSize, &s_AsrEngine.voiceControl.result);

    return status;
}

static char *asr_get_string_by_id(struct asr_inference_engine *pInfEngine, int32_t id)
{
    return pInfEngine->idToKeyword[id];
}

static void initialize_asr(void)
{
    asr_language_t lang[NUM_LANGUAGES] = {UNDEFINED_LANGUAGE};
    asr_language_t langShell           = 0;

    s_AsrEngine.voiceConfig.multilingual = ACTIVE_LANGUAGE;

    langShell = s_AsrEngine.voiceConfig.multilingual;

    lang[0] = langShell & ACTIVE_LANGUAGE;

    // NULL to ensure the end of linked list.
    s_AsrEngine.voiceControl.langModel    = NULL;
    s_AsrEngine.voiceControl.infEngineWW  = NULL;
    s_AsrEngine.voiceControl.infEngineCMD = NULL;

    // install multilingual
#if CHINESE_MANDARIN_SUPPORTED
    install_language(&s_AsrEngine.voiceControl, &s_AsrEngine.langModel[0], lang[0], (unsigned char *)&oob_demo_zh_begin,
                     NUM_GROUPS);

    install_inference_engine(&s_AsrEngine.voiceControl, &s_AsrEngine.infWW[0], lang[0], ASR_WW, ww_zh,
                             &s_memPoolWLang1[0],
                             ZH_WAKE_WORD_MEMPOOL_SIZE); // ww language ZH

    // CMD inference engine will be reset with detected language after WW is detected
    install_inference_engine(&s_AsrEngine.voiceControl, &s_AsrEngine.infCMD, lang[0], s_AsrEngine.voiceConfig.demo,
                             cmd_coffee_machine_zh, &s_memPoolCmd[0],
                             COMMAND_MEMPOOL_SIZE); // commands, setting up with defaults
#else
    install_language(&s_AsrEngine.voiceControl, &s_AsrEngine.langModel[0], lang[0], (unsigned char *)&oob_demo_en_begin,
                     NUM_GROUPS);

    install_inference_engine(&s_AsrEngine.voiceControl, &s_AsrEngine.infWW[0], lang[0], ASR_WW, ww_en,
                             &s_memPoolWLang1[0],
                             WAKE_WORD_MEMPOOL_SIZE); // ww language1

    // CMD inference engine will be reset with detected language after WW is detected
    install_inference_engine(&s_AsrEngine.voiceControl, &s_AsrEngine.infCMD, ACTIVE_LANGUAGE,
                             s_AsrEngine.voiceConfig.demo, cmd_coffee_machine_en, &s_memPoolCmd[0],
                             COMMAND_MEMPOOL_SIZE); // commands, setting up with defaults
#endif

    // init
    init_WW_engine(&s_AsrEngine.voiceControl, s_AsrEngine.voiceConfig.demo);
    init_CMD_engine(&s_AsrEngine.voiceControl, s_AsrEngine.voiceConfig.demo);
}

static void print_asr_session(int status)
{
    switch (status)
    {
        case ASR_SESSION_STARTED:
            LOGD("[ASR] Session started");
            break;
        case ASR_SESSION_ENDED:
            LOGD("[ASR] Session ended\r\n");
            break;
        case ASR_SESSION_TIMEOUT:
            LOGD("[ASR] Timed out waiting for response");
            break;
    }
}

static void asr_cmd_timeout_callback(TimerHandle_t xTimer)
{
    print_asr_session(ASR_SESSION_TIMEOUT);
    print_asr_session(ASR_SESSION_ENDED);

    s_AsrEngine.voiceResult.status    = ASR_TIMEOUT;
    s_AsrEngine.voiceResult.keywordID = ASR_KEYWORDID_INVALID;
    voice_algo_asr_result_notify(&s_AsrEngine.voiceResult, 0);

    s_asrCmdTimeout = true;
}

hal_valgo_status_t voice_algo_dev_asr_init(voice_algo_dev_t *dev, valgo_dev_callback_t callback, void *param)
{
    hal_valgo_status_t ret = kStatus_HAL_ValgoSuccess;
    uint32_t timerId       = 0;

    dev->cap.callback = callback;

    memset(&s_AsrEngine.langModel, 0, sizeof(s_AsrEngine.langModel));
    memset(&s_AsrEngine.infWW, 0, sizeof(s_AsrEngine.infWW));
    memset(&s_AsrEngine.infCMD, 0, sizeof(s_AsrEngine.infCMD));
    memset(&s_AsrEngine.voiceControl, 0, sizeof(asr_voice_control_t));
    s_AsrEngine.voiceResult.status    = ASR_NONE;
    s_AsrEngine.voiceResult.keywordID = ASR_KEYWORDID_INVALID;

    /* TODO: Replace all HAL_OutputDev_SmartLockConfig */
    /*s_AsrEngine.voiceConfig = HAL_OutputDev_SmartLockConfig_GetAsrConfig();*/
    s_AsrEngine.voiceConfig.status = WRITE_FAIL;
    if (s_AsrEngine.voiceConfig.status != WRITE_SUCCESS)
    {
        s_AsrEngine.voiceConfig.demo         = ASR_CMD_COFFEE_MACHINE;
        s_AsrEngine.voiceConfig.followup     = ASR_FOLLOWUP_OFF;
        s_AsrEngine.voiceConfig.multilingual = ACTIVE_LANGUAGE;
        s_AsrEngine.voiceConfig.mute         = ASR_MUTE_OFF;
        s_AsrEngine.voiceConfig.ptt          = ASR_PTT_OFF;
        s_AsrEngine.voiceConfig.timeout      = TIMEOUT_TIME_IN_MS;
        s_AsrEngine.voiceConfig.status       = WRITE_SUCCESS;
        s_AsrEngine.voiceConfig.asrCfg       = ASR_CFG_DEMO_NO_CHANGE;
        /*HAL_OutputDev_SmartLockConfig_SetAsrConfig(s_AsrEngine.voiceConfig);*/
    }

    /* Initialize the ASR engine */
    initialize_asr();

    s_asrCmdTimeoutTimer =
        xTimerCreate("AsrCmdTimeoutTimer", (TickType_t)pdMS_TO_TICKS(s_AsrEngine.voiceConfig.timeout), pdFALSE,
                     &timerId, asr_cmd_timeout_callback);
    if (s_asrCmdTimeoutTimer == NULL)
    {
        LOGE("[ASR] s_asrCmdTimeoutTimer failed to initialize");
        ret = kStatus_HAL_ValgoMallocError;
    }

    return ret;
}

/*!
 * @brief ASR main task
 */
hal_valgo_status_t voice_algo_dev_asr_run(const voice_algo_dev_t *dev, void *data)
{
    hal_valgo_status_t status    = kStatus_HAL_ValgoSuccess;
    static asr_events_t asrEvent = ASR_SESSION_ENDED;
    struct asr_inference_engine *pInfWW;
    struct asr_inference_engine *pInfCMD;
    char **cmdString;
    int16_t *pi16Sample;

    audio_msg_payload_t *audioIn = (audio_msg_payload_t *)data;
    if ((audioIn->data == NULL) || (audioIn->size != NUM_SAMPLES_AFE_OUTPUT))
    {
        status = kStatus_HAL_ValgoError;
        LOGE("[ASR] Received invalid audio packet: addr=%d, size = %d", (uint32_t)audioIn->data, audioIn->size);
    }
    else
    {
        pi16Sample = audioIn->data;

        /* Reset ASR if command timeout was triggered */
        if (s_asrCmdTimeout)
        {
            s_asrCmdTimeout = false;

            reset_CMD_engine(&s_AsrEngine.voiceControl);
            set_WW_engine(&s_AsrEngine.voiceControl);
            asrEvent = ASR_SESSION_ENDED;

            LOGI("[ASR] Session reset");
        }

        if (asrEvent == ASR_SESSION_ENDED && s_AsrEngine.voiceConfig.ptt == ASR_PTT_OFF)
        {
            /* Wake Word detection. Check all enabled languages, but stop on first match. */
            for (pInfWW = s_AsrEngine.voiceControl.infEngineWW; pInfWW != NULL; pInfWW = pInfWW->next)
            {
                if (asr_process_audio_buffer(pInfWW->handler, pi16Sample, NUM_SAMPLES_AFE_OUTPUT,
                                             pInfWW->iWhoAmI_inf) == kAsrLocalDetected)
                {
                    uint32_t wwLength = 0;

                    s_AsrEngine.voiceResult.status = ASR_WW_DETECT;

                    /* Send Feedback to AFE regarding detected Wake Word.
                     * Based on this feedback, AFE calibrates itself for a better performance */
                    wwLength = SLN_ASR_LOCAL_GetDetectedCommandDuration(pInfWW->handler);

                    if (asr_get_string_by_id(pInfWW, s_AsrEngine.voiceControl.result.keywordID[0]) != NULL)
                    {
                        asrEvent = ASR_SESSION_STARTED;
                        print_asr_session(asrEvent);
                        LOGD("[ASR] Wake Word: %s(%d) - MapID(%d)",
                             asr_get_string_by_id(pInfWW, s_AsrEngine.voiceControl.result.keywordID[0]),
                             s_AsrEngine.voiceControl.result.keywordID[0], s_AsrEngine.voiceControl.result.cmdMapID);

                        voice_algo_asr_result_notify(&s_AsrEngine.voiceResult, wwLength);

                        if (s_AsrEngine.voiceConfig.cmdresults == ASR_CMD_RES_ON)
                        {
                            LOGI("      Trust: %d, SGDiff: %d\r\n", s_AsrEngine.voiceControl.result.trustScore,
                                 s_AsrEngine.voiceControl.result.SGDiffScore);
                        }

                        /* (Coffee Machine) Demo: CMD - detected WW's language. */
                        cmdString = get_cmd_strings(pInfWW->iWhoAmI_lang, s_AsrEngine.voiceConfig.demo);
                        set_CMD_engine(&s_AsrEngine.voiceControl, pInfWW->iWhoAmI_lang, s_AsrEngine.voiceConfig.demo,
                                       cmdString);

                        reset_WW_engine(&s_AsrEngine.voiceControl);

                        /* Start Timeout Timer for incoming command
                         * xTimerChangePeriod configures the timeout period and also starts the timer */
                        xTimerChangePeriod(s_asrCmdTimeoutTimer,
                                           (TickType_t)pdMS_TO_TICKS(s_AsrEngine.voiceConfig.timeout), 0);

                        break;
                    }
                }
            }
        }
        else if (asrEvent == ASR_SESSION_STARTED)
        {
            /* Command detection. Command's language is determined in the Wake Word section. */
            pInfCMD = s_AsrEngine.voiceControl.infEngineCMD;

            if (asr_process_audio_buffer(pInfCMD->handler, pi16Sample, NUM_SAMPLES_AFE_OUTPUT, pInfCMD->iWhoAmI_inf) ==
                kAsrLocalDetected)
            {
                if (s_asrCmdTimeout == false)
                {
                    if (asr_get_string_by_id(pInfCMD, s_AsrEngine.voiceControl.result.keywordID[1]) != NULL)
                    {
                        LOGD("[ASR] Command: %s(%d) - MapID(%d)",
                             asr_get_string_by_id(pInfCMD, s_AsrEngine.voiceControl.result.keywordID[1]),
                             s_AsrEngine.voiceControl.result.keywordID[1], s_AsrEngine.voiceControl.result.cmdMapID);

                        s_AsrEngine.voiceResult.status    = ASR_CMD_DETECT;
                        s_AsrEngine.voiceResult.keywordID = s_AsrEngine.voiceControl.result.keywordID[1];

                        voice_algo_asr_result_notify(&s_AsrEngine.voiceResult, 0);

                        if (s_AsrEngine.voiceConfig.cmdresults == ASR_CMD_RES_ON)
                        {
                            LOGI("      Trust: %d, SGDiff: %d\r\n", s_AsrEngine.voiceControl.result.trustScore,
                                 s_AsrEngine.voiceControl.result.SGDiffScore);
                        }

                        if (asrEvent == ASR_SESSION_ENDED)
                        {
                            print_asr_session(asrEvent);
                        }

                        reset_CMD_engine(&s_AsrEngine.voiceControl);

                        // TODO currently going back to ww listening mode only when timeout is triggered
                        //                        set_WW_engine(&s_AsrEngine.voiceControl);
                    }
                }
            }
        }
    }

    return status;
}

hal_valgo_status_t voice_algo_dev_input_notify(const voice_algo_dev_t *dev, void *data)
{
    hal_valgo_status_t error = kStatus_HAL_ValgoSuccess;
    event_voice_t event      = *(event_voice_t *)data;

    switch (event.event_base.eventId)
    {
        case (GET_TIMEOUT_DURATION):
            /*PRINTF("\r\nCurrent timeout duration is: %d\r\n",
             * HAL_OutputDev_SmartLockConfig_GetAsrTimeoutDuration());*/
            break;

        case (SET_TIMEOUT_DURATION):

            if (0/*kSLNConfigStatus_Success ==
                HAL_OutputDev_SmartLockConfig_SetAsrTimeoutDuration(event.set_timeout_duration.timeout)*/)
            {
                s_AsrEngine.voiceConfig.timeout = event.set_timeout_duration.timeout;
            }
            else
            {
                error = kStatus_HAL_ValgoError;
                LOGE("[ERROR] Failed to save Timeout config to flash.");
            }
            break;

        case (GET_FOLLOWUP_STATUS):
            /*PRINTF("\r\nFollowup is currently: %s\r\n",
                   HAL_OutputDev_SmartLockConfig_GetAsrFollowupStatus() == ASR_FOLLOWUP_ON ? "enabled" : "disabled");*/
            break;

        case (SET_FOLLOWUP_STATUS):
            if (0/*kSLNConfigStatus_Success ==
                HAL_OutputDev_SmartLockConfig_SetAsrFollowupStatus(event.set_followup_status.followup)*/)
            {
                s_AsrEngine.voiceConfig.followup = event.set_followup_status.followup;
            }
            else
            {
                error = kStatus_HAL_ValgoError;
                LOGE("[ERROR] Failed to save Followup config to flash.");
            }
            break;

        case (GET_MULTILINGUAL_CONFIG):
            if (s_AsrEngine.voiceConfig.multilingual > 0)
            {
                PRINTF("\r\nRecognition of the following lanaguages is enabled:\r\n");
                if (s_AsrEngine.voiceConfig.multilingual & ASR_ENGLISH)
                {
                    PRINTF("\tEnglish (en)\r\n");
                }
                if (s_AsrEngine.voiceConfig.multilingual & ASR_CHINESE)
                {
                    PRINTF("\tChinese (zh)\r\n");
                }
                if (s_AsrEngine.voiceConfig.multilingual & ASR_GERMAN)
                {
                    PRINTF("\tGerman (de)\r\n");
                }
                if (s_AsrEngine.voiceConfig.multilingual & ASR_FRENCH)
                {
                    PRINTF("\tFrench (fr)\r\n");
                }
            }
            else
            {
                PRINTF("\r\nVoice recognition is not currently enabled for any languages.\r\n");
            }

            break;

        case (SET_MULTILINGUAL_CONFIG):
            if (0/*kStatus_HAL_ValgoSuccess ==
                HAL_OutputDev_SmartLockConfig_SetAsrMultilingualConfig(event.set_multilingual_config.languages)*/)
            {
                // TODO Deinit voice engine to enable multi-language w/o reset
                // s_AsrEngine.voiceConfig.multilingual = event.set_multilingual_status.languages;
                // voice_algo_dev_asr_deinit();
                // voice_algo_dev_asr_init();
                NVIC_SystemReset();
            }
            else
            {
                error = kStatus_HAL_ValgoError;
                LOGE("[ERROR] Failed to save Multilingual config to flash.");
            }
            break;

        case (GET_VOICE_DEMO):

            PRINTF("********************************\r\n");
            PRINTF("\r\nCurrent voice demo set to: ");
            switch (s_AsrEngine.voiceConfig.demo)
            {
                case ASR_CMD_COFFEE_MACHINE:
                    PRINTF("\"IoT\"\r\n");
                    break;
                default:
                    break;
            }
            for (int i = 0; i < NUM_LANGUAGES; i++)
            {
                if (s_AsrEngine.voiceConfig.multilingual & (1 << i))
                {
                    PRINTF("********************************\r\n");
                    switch (1 << i)
                    {
                        case ASR_ENGLISH:
                            PRINTF("English\r\n");
                            PRINTF("********************************\r\n");
                            PRINTF("Wake Word:\r\n\t\"%s\"\r\n", ww_en[0]);
                            break;
                        case ASR_CHINESE:
                            PRINTF("Chinese\r\n");
                            PRINTF("********************************\r\n");
                            PRINTF("Wake Word:\r\n\t\"%s\"\r\n", ww_zh[0]);
                            break;
                        case ASR_GERMAN:
                            PRINTF("German\r\n");
                            PRINTF("********************************\r\n");
                            PRINTF("Wake Word:\r\n\t\"%s\"\r\n", ww_de[0]);
                            break;
                        case ASR_FRENCH:
                            PRINTF("French\r\n");
                            PRINTF("********************************\r\n");
                            PRINTF("Wake Word:\r\n\t\"%s\"\r\n", ww_fr[0]);
                            break;
                        default:
                            break;
                    }
                    PRINTF("Commands:\r\n");

                    int num_commands = get_cmd_number((1 << i), s_AsrEngine.voiceConfig.demo);
                    char **commands  = get_cmd_strings((1 << i), s_AsrEngine.voiceConfig.demo);
                    for (int j = 0; j < num_commands; j++)
                    {
                        PRINTF("\t\"%s\"\r\n", commands[j]);
                    }
                }
            }

            break;

        case (SET_VOICE_DEMO):
            if (0 /*kSLNConfigStatus_Success == HAL_OutputDev_SmartLockConfig_SetAsrDemo(event.set_voice_demo.demo)*/)
            {
                // TODO Deinit voice engine to enable multi-language w/o reset
                // s_AsrEngine.voiceConfig.demo = event.set_voice_demo.demo;
                // voice_algo_dev_asr_deinit();
                // voice_algo_dev_asr_init();
                NVIC_SystemReset();
            }
            else
            {
                error = kStatus_HAL_ValgoError;
                LOGE("[ERROR] Failed to save selected voice demo to flash.");
            }
            break;
        case (STOP_VOICE_CMD_SESSION):
        {
            LOGD("ASR:stop voice cmd session");
            asr_cmd_timeout_callback(NULL);
        }
        break;
        default:
            break;
    }

    return error;
}

const static voice_algo_dev_operator_t voice_algo_dev_asr_ops = {.init        = voice_algo_dev_asr_init,
                                                                 .deinit      = NULL,
                                                                 .run         = voice_algo_dev_asr_run,
                                                                 .inputNotify = voice_algo_dev_input_notify};

static voice_algo_dev_t voice_algo_dev_asr = {
    .id  = 0,
    .ops = (voice_algo_dev_operator_t *)&voice_algo_dev_asr_ops,
    .cap = {.param = NULL},
};

void voice_algo_asr_result_notify(asr_inference_result_t *result, uint32_t wwLength)
{
    if (voice_algo_dev_asr.cap.callback != NULL)
    {
        valgo_event_t valgo_event;
        valgo_event.eventId = kVAlgoEvent_VoiceResultUpdate;
        valgo_event.data    = result;
        valgo_event.size    = sizeof(asr_inference_result_t);
        valgo_event.copy    = 0;
        voice_algo_dev_asr.cap.callback(voice_algo_dev_asr.id, valgo_event, 0);
        LOGD("[ASR_RESULT]: status:%d command:%d wwLength:%d\r\n", result->status, result->keywordID, wwLength);
        if ((result->status == ASR_WW_DETECT) && (wwLength != 0))
        {
            event_voice_t feedbackEvent      = {0};
            feedbackEvent.event_base.eventId = ASR_TO_AFE_FEEDBACK;
            feedbackEvent.ww_len.ww_len      = wwLength;

            /* Build Valgo event */
            valgo_event.eventId = kVAlgoEvent_AsrToAfeFeedback;
            valgo_event.data    = &feedbackEvent;
            valgo_event.size    = sizeof(event_voice_t);
            valgo_event.copy    = 1;
            voice_algo_dev_asr.cap.callback(voice_algo_dev_asr.id, valgo_event, 0);
        }
    }
}

int HAL_VoiceAlgoDev_Asr_Register()
{
    int error = 0;
    LOGD("HAL_VoiceAlgoDev_Asr_Register");
    error = FWK_VoiceAlgoManager_DeviceRegister(&voice_algo_dev_asr);
    return error;
}
#endif /* ENABLE_VOICEALGO_Asr */
