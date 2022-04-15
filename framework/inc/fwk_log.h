/*
 * Copyright 2019-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief log wrapper.
 */

#ifndef _FWK_LOG_H_
#define _FWK_LOG_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#ifdef LOG_ENABLE

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#ifdef RT_PLATFORM
#include "fsl_debug_console.h"
#include "fwk_common.h"
#include "fwk_config.h"
#else
#define PRINTF printf
#endif

#include "fwk_platform.h"
#include "iot_logging_task.h"

#ifndef DEBUG_CONSOLE_LOCK
#define DEBUG_CONSOLE_LOCK()
#endif

#ifndef DEBUG_CONSOLE_UNLOCK
#define DEBUG_CONSOLE_UNLOCK()
#endif

#ifndef LOGV
#define LOGV(fmt, args...)                                \
    {                                                     \
        if (FWK_ConfigGetLogLevel() >= kLOGLevel_Verbose) \
        {                                                 \
            {vLoggingPrintfInfo(fmt, ##args);}            \
        }                                                 \
    }
#endif

#ifndef LOGD
#define LOGD(fmt, args...)                              \
    {                                                   \
        if (FWK_ConfigGetLogLevel() >= kLOGLevel_Debug) \
        {                                               \
            {vLoggingPrintfDebug(fmt, ##args);}         \
        }                                               \
    }
#endif

#ifndef LOGI
#define LOGI(fmt, args...)                             \
    {                                                  \
        if (FWK_ConfigGetLogLevel() >= kLOGLevel_Info) \
        {                                              \
            {vLoggingPrintfInfo(fmt, ##args);}         \
        }                                              \
    }
#endif

#ifndef LOGE
#define LOGE(fmt, args...)                                                                     \
    {                                                                                          \
        if (FWK_ConfigGetLogLevel() >= kLOGLevel_Error)                                        \
        {                                                                                      \
            {vLoggingPrintfError("%s:%d: In function \"%s\":", __FILE__, __LINE__, __func__);} \
            {vLoggingPrintfError(fmt, ##args);}                                                \
        }                                                                                      \
    }
#endif

#else

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGE(...)

#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /*_FWK_LOG_H_*/
