/*
 * Copyright 2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef INDEXCOMMANDS_H_
#define INDEXCOMMANDS_H_

#include "IndexToCommand_en.h"
#include "IndexToCommand_zh.h"
#include "IndexToCommand_de.h"
#include "IndexToCommand_fr.h"

/* These defines are currently used only for displaying in the commands shell */
#define NUMBER_OF_COFFEE_MACHINE_CMDS_EN 29

char **get_cmd_strings(asr_language_t asrLang, asr_inference_t infCMDType)
{
    char **retString = NULL;

    switch (asrLang)
    {
        case ASR_CHINESE:
            if (infCMDType == ASR_CMD_COFFEE_MACHINE)
            {
                retString = cmd_coffee_machine_zh;
            }
            break;
        case ASR_FRENCH:
            // TODO
            break;
        case ASR_ENGLISH:
            if (infCMDType == ASR_CMD_COFFEE_MACHINE)
            {
                retString = cmd_coffee_machine_en;
            }
            break;
        case ASR_GERMAN:
            // TODO
            break;
        default:
            retString = cmd_coffee_machine_en;
            break;
    }

    return retString;
}

unsigned int get_cmd_number(asr_language_t asrLang, asr_inference_t infCMDType)
{
    unsigned int cmd_number = 0;

    switch (asrLang)
    {
        case ASR_CHINESE:
            // TODO
            break;
        case ASR_FRENCH:
            // TODO
            break;
        case ASR_ENGLISH:
            if (infCMDType == ASR_CMD_COFFEE_MACHINE)
            {
                cmd_number = NUMBER_OF_COFFEE_MACHINE_CMDS_EN;
            }
            break;
        case ASR_GERMAN:
            // TODO
            break;
        default:
            cmd_number = NUMBER_OF_COFFEE_MACHINE_CMDS_EN;
            break;
    }

    return cmd_number;
}

#endif /* INDEXCOMMANDS_H_ */
