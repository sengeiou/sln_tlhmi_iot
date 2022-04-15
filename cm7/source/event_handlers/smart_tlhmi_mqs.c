/*
 * Copyright 2020-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "board_define.h"
#ifdef ENABLE_OUTPUT_DEV_MqsAudio
#include "stdint.h"
#include "hal_voice_algo_asr_local.h"
#include "hal_event_descriptor_common.h"
#include "hal_output_dev.h"
#include "hal_vision_algo.h"
#include "fwk_log.h"

enum
{
    PROMPT_OK = 0,
    PROMPT_CAN_I_HELP,
    PROMPT_TONE_TIMEOUT,
    PROMPT_ANOTHER_ESPRESSO,
    PROMPT_ANOTHER_AMERICANO,
    PROMPT_ANOTHER_CAPPUCCINO,
    PROMPT_ANOTHER_CAFE_LATTE,
    PROMPT_REGISTER_SELECTION,

    PROMPT_RECOGNITION_SUCCESS,
    PROMPT_INVALID
};

typedef struct _audio_prompt_info
{
    void *data;
    uint32_t len;
} audio_prompt_info_t;

static audio_prompt_info_t s_Prompts[PROMPT_INVALID];

void LoadAudioPrompts(void *base)
{
    s_Prompts[PROMPT_OK].data = (base + 0);
    s_Prompts[PROMPT_OK].len = 46220;
    s_Prompts[PROMPT_CAN_I_HELP].data = (base + 46272);
    s_Prompts[PROMPT_CAN_I_HELP].len = 68780;
    s_Prompts[PROMPT_TONE_TIMEOUT].data = (base + 115072);
    s_Prompts[PROMPT_TONE_TIMEOUT].len = 21319;
    s_Prompts[PROMPT_ANOTHER_ESPRESSO].data = (base + 136448);
    s_Prompts[PROMPT_ANOTHER_ESPRESSO].len = 258298;
    s_Prompts[PROMPT_ANOTHER_AMERICANO].data = (base + 394752);
    s_Prompts[PROMPT_ANOTHER_AMERICANO].len = 278360;
    s_Prompts[PROMPT_ANOTHER_CAPPUCCINO].data = (base + 673152);
    s_Prompts[PROMPT_ANOTHER_CAPPUCCINO].len = 278360;
    s_Prompts[PROMPT_ANOTHER_CAFE_LATTE].data = (base + 951552);
    s_Prompts[PROMPT_ANOTHER_CAFE_LATTE].len = 278360;
    s_Prompts[PROMPT_REGISTER_SELECTION].data = (base + 1229952);
    s_Prompts[PROMPT_REGISTER_SELECTION].len = 323500;
    s_Prompts[PROMPT_RECOGNITION_SUCCESS].data = (base + 1553472);
    s_Prompts[PROMPT_RECOGNITION_SUCCESS].len = 140434;
    //Prompts Total: 0x0019d900  1693952
}

int APP_OutputDev_MqsAudio_InputNotifyDecode(const output_dev_t *dev, void *inputData, void **audio, uint32_t *len)
{
    event_base_t *pEventBase = (event_base_t *)inputData;

    if (pEventBase->eventId == kEventID_PlayPrompt)
    {
        int promptId = (int)((event_common_t *)pEventBase)->data;
        LOGD("MQS:prompt id %d", promptId);
        if (promptId >= PROMPT_OK && promptId < PROMPT_INVALID)
        {
            *audio = s_Prompts[promptId].data;
            *len   = s_Prompts[promptId].len;
        }
    }
    return 0;
}
#endif /* ENABLE_OUTPUT_DEV_MqsAudio */
