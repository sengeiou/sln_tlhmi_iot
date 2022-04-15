/*
 * Copyright 2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef HAL_VOICE_ALGO_ASR_LOCAL_H_
#define HAL_VOICE_ALGO_ASR_LOCAL_H_

#include <sln_asr.h>
#include <stdint.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

// languages
#define IMXRT105S     (0)
#define NUM_LANGUAGES (1)

// applications
#define ENABLE_COFFEE_MACHINE (1)
#define NUM_APPS              (ENABLE_COFFEE_MACHINE)

#define CHINESE_MANDARIN_SUPPORTED    (0)

/* Only one language active; Change here when wanting to use ASR_CHINESE, ASR_FRENCH etc. */
#if CHINESE_MANDARIN_SUPPORTED
#define ACTIVE_LANGUAGE ASR_CHINESE
#else
#define ACTIVE_LANGUAGE ASR_ENGLISH
#endif

// groups: base, ww, cmd_iot, cmd_elevator, and so on
#define NUM_GROUPS (NUM_APPS + 3)
#define MAX_GROUPS (NUM_GROUPS)

// WW in multiple languages (not supported for now in TLHMI)
#define NUM_INFERENCES_WW (NUM_LANGUAGES)

#define k_nMaxTime (300)

// the response waiting time in ASR session
#define TIMEOUT_TIME_IN_MS 60000

// Shell Commands Related
#define ASR_SHELL_COMMANDS_FILE_NAME "asr_shell_commands.dat"

#define AUDIO_EN_01_FILE "audio_en_01_begin.dat"
#define AUDIO_EN_02_FILE "audio_en_02_begin.dat"
#define AUDIO_EN_03_FILE "audio_en_03_begin.dat"
#define AUDIO_EN_04_FILE "audio_en_04_begin.dat"
#define AUDIO_EN_05_FILE "audio_en_05_begin.dat"
#define AUDIO_EN_06_FILE "audio_en_06_begin.dat"

#define AUDIO_ZH_01_FILE "audio_zh_01_begin.dat"
#define AUDIO_ZH_02_FILE "audio_zh_02_begin.dat"

#define AUDIO_DE_01_FILE "audio_de_01_begin.dat"
#define AUDIO_DE_02_FILE "audio_de_02_begin.dat"

#define AUDIO_FR_01_FILE "audio_fr_01_begin.dat"
#define AUDIO_FR_02_FILE "audio_fr_02_begin.dat"

// Out-of-box demo languages. Developers can add more language. Note that the runtime max number is up to four
// languages.
typedef enum _asr_languages
{
    UNDEFINED_LANGUAGE = 0,
    ASR_ENGLISH        = (1U << 0U),
    ASR_CHINESE        = (1U << 1U),
    ASR_GERMAN         = (1U << 2U),
    ASR_FRENCH         = (1U << 3U),
    ASR_KOREAN         = (1U << 4U),
    // DEVELOPERS: add more languages here with the form ASR_XXXXX
} asr_language_t;

// ASR events. Currently only ASR_SESSION_STARTED and ASR_SESSION_ENDED are used.
typedef enum _asr_events
{
    ASR_SESSION_STARTED,
    ASR_SESSION_ENDED,
    //    ASR_COMMAND_DETECTED,
    //    ASR_COMMAND_NOT_RECOGNIZED,
    ASR_SESSION_TIMEOUT,
    //    ASR_CANCELLED,
} asr_events_t;

// ASR inference engines that cover
// 1) wake word engine,
// 2) LED control (demo #1),
// 3) voice commands in multiple languages (demo #2),
// 4) dialogic commands (demo #3)
typedef enum _asr_inference
{
    UNDEFINED_INFERENCE    = 0,
    ASR_WW                 = (1U << 0U),
    ASR_CMD_COFFEE_MACHINE = (ENABLE_COFFEE_MACHINE << 1U),
} asr_inference_t;

struct asr_language_model;   // will be used to install the selected languages.
struct asr_inference_engine; // will be used to install and set the selected WW/CMD inference engines.

struct asr_language_model
{
    asr_language_t iWhoAmI; // language types for d-spotter language model. A model is language specific.
    uint8_t
        nGroups; // base, group1 (ww), group2 (commands set 1), group3 (commands set 2), ..., groupN (commands set N)
    unsigned char *addrBin;                        // d-spotter model binary address
    unsigned char *addrGroup[MAX_GROUPS];          // addresses for base, group1, group2, ...
    unsigned char *addrGroupMapID[MAX_GROUPS - 1]; // addresses for mapIDs for group1, group2, ...
    struct asr_language_model *next;               // pointer to next language model in this linked list
};

struct asr_inference_engine
{
    asr_inference_t iWhoAmI_inf;   // inference types for WW engine or CMD engine
    asr_language_t iWhoAmI_lang;   // language for inference engine
    void *handler;                 // d-spotter handler
    uint8_t nGroups;               // the number of groups for an inference engine. Default is 2 and it's enough.
    unsigned char *addrGroup[2];   // base + keyword group. default nGroups is 2
    unsigned char *addrGroupMapID; // mapID group. default nGroups is 1
    char **idToKeyword;            // the string list
    unsigned char *memPool;        // memory pool in ram for inference engine
    uint32_t memPoolSize;          // memory pool size
    struct asr_inference_engine
        *next; // pointer to next inference engine, if this is linked list. The end of "next" should be NULL.
};

typedef struct _asr_voice_control
{
    struct asr_language_model *langModel;      // linked list
    struct asr_inference_engine *infEngineWW;  // linked list
    struct asr_inference_engine *infEngineCMD; // not linked list
    uint32_t sampleCount;                      // to measure the waiting response time
    asr_result_t result;                       // results of the command processing
} asr_voice_control_t;

typedef enum _app_flash_status
{
    READ_SUCCESS  = (1U << 0U),
    READ_FAIL     = (1U << 1U),
    READ_READY    = (1U << 2U),
    WRITE_SUCCESS = (1U << 3U),
    WRITE_FAIL    = (1U << 4U),
    WRITE_READY   = (1U << 5U),
} app_flash_status_t;

typedef enum _asr_mute
{
    ASR_MUTE_OFF = 0,
    ASR_MUTE_ON,
} asr_mute_t;

typedef enum _asr_followup
{
    ASR_FOLLOWUP_OFF = 0,
    ASR_FOLLOWUP_ON,
} asr_followup_t;

typedef enum _asr_ptt
{
    ASR_PTT_OFF = 0,
    ASR_PTT_ON,
} asr_ptt_t;

typedef enum _asr_cmd_res
{
    ASR_CMD_RES_OFF = 0,
    ASR_CMD_RES_ON,
} asr_cmd_res_t;

typedef enum _asr_cfg_demo
{
    ASR_CFG_DEMO_NO_CHANGE = (1U << 0U), // OOB demo type or languages unchanged
    ASR_CFG_CMD_INFERENCE_ENGINE_CHANGED =
        (1U << 1U),                             // OOB demo type (iot, elevator, audio, wash, led, dialog) changed
    ASR_CFG_DEMO_LANGUAGE_CHANGED = (1U << 2U), // OOB language type changed
} asr_cfg_demo_t;

typedef struct _asr_voice_config
{
    app_flash_status_t status;
    asr_cfg_demo_t asrCfg;
    asr_mute_t mute;
    uint32_t timeout; // in millisecond
    asr_followup_t followup;
    asr_inference_t demo;        // demo types: LED (demo #1) / iot, elevator, audio, wash (demo #2) / dialog (demo #3)
    asr_language_t multilingual; // runtime language types (demo #2 and #3)
    asr_ptt_t ptt;
    asr_cmd_res_t cmdresults;
} asr_voice_config_t;

typedef enum _asr_voice_detect_status
{
    ASR_WW_DETECT = 0,
    ASR_CMD_DETECT,
    ASR_TIMEOUT,
    ASR_NONE
} asr_voice_detect_status_t;

#define ASR_KEYWORDID_INVALID (-1)

typedef struct _asr_inference_result
{
    asr_voice_detect_status_t status;
    int32_t keywordID;
} asr_inference_result_t;

#if defined(__cplusplus)
}
#endif

#endif /* HAL_VOICE_ALGO_ASR_LOCAL_H_ */
