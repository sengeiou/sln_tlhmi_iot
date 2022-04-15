/*
 * Copyright 2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief Ui coffee machine output HAL device implementation.
 */

#include "board_define.h"
#ifdef ENABLE_OUTPUT_DEV_UiCoffeeMachine

#include "FreeRTOS.h"
#include "board.h"

#include "fwk_graphics.h"
#include "fwk_log.h"
#include "fwk_timer.h"
#include "fwk_output_manager.h"
#include "fwk_lpm_manager.h"
#include "hal_output_dev.h"
#include "hal_vision_algo.h"
#include "hal_voice_algo_asr_local.h"
#include "hal_event_descriptor_common.h"
#include "hal_event_descriptor_face_rec.h"
#include "hal_event_descriptor_voice.h"

#include "smart_tlhmi_event_descriptor.h"

#include "icons/process_bar_240x14.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
     |<------------640------------------>|
___  ____________________________________   ____
 |   |                                   |    |
 |   |-----------------------------------|  --|<--- UI_TOPINFO_Y
 |   |       Top Info                    |    |
 |   |-----------------------------------|  --|<--- UI_MAINWINDOW_Y = UI_TOPINFO_Y+UI_TOPINFO_H
 |   |                                   |    |
 |   |                                   |    |
480  |      Main Window                  |    |
 |   |                                   |    |
 |   |                                   |    |
 |   |                                   |    |
 |   |-----------------------------------|  --|<--- UI_BOTTOMINFO_H = UI_MAINWINDOW_Y+UI_MAINWINDOW_H
 |   |        Bottom Info                |    |
_|_  |___________________________________|  __|_

*/

#define UI_BUFFER_WIDTH  640
#define UI_BUFFER_HEIGHT 480
#define UI_BUFFER_BPP    2
#define UI_BUFFER_PITCH  (UI_BUFFER_WIDTH * UI_BUFFER_BPP)
#define UI_SCALE_W       (640 / UI_BUFFER_WIDTH)
#define UI_SCALE_H       (480 / UI_BUFFER_HEIGHT)

#define UI_GUIDE_RECT_W 360
#define UI_GUIDE_RECT_H 300

#define UI_TOPINFO_W    UI_BUFFER_WIDTH
#define UI_TOPINFO_H    30
#define UI_TOPINFO_X    0
#define UI_TOPINFO_Y    (4 / UI_SCALE_H)
#define UI_BOTTOMINFO_W UI_BUFFER_WIDTH
#define UI_BOTTOMINFO_H 20
#define UI_MAINWINDOW_W UI_BUFFER_WIDTH
#define UI_MAINWINDOW_H (UI_BUFFER_HEIGHT - UI_TOPINFO_H - UI_BOTTOMINFO_H - UI_TOPINFO_Y)
#define UI_MAINWINDOW_X 0
#define UI_MAINWINDOW_Y (UI_TOPINFO_Y + UI_TOPINFO_H)
#define UI_BOTTOMINFO_X 0
#define UI_BOTTOMINFO_Y (UI_MAINWINDOW_Y + UI_MAINWINDOW_H)

#define UI_MAINWINDOW_PROCESS_FG_X_OFFSET ((PROCESS_BAR_BG_W - PROCESS_BAR_FG_W) / 2)
#define UI_MAINWINDOW_PROCESS_FG_Y_OFFSET ((PROCESS_BAR_BG_H - PROCESS_BAR_FG_H) / 2)

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f
#define RGB565_BLACK    0x0001
#define RGB565_NXPGREEN 0xBEA6
#define RGB565_NXPRED   0xFD83
#define RGB565_NXPBLUE  0x6D5B

#if 0
char *cmd_iot_coffee_machine[] = {"Start", "Cancel", "No, thanks", "No, thank you", "Confirm",
                      "Go ahead",    "Espresso",        "Americano",          "Cappuccino",         "Cafe Latte",
                      "Small", "Medium", "Large", "Soft", "Mild", "Strong", "Deregister"};
#endif

enum
{
    VOICE_CMD_START         = 0,
    VOICE_CMD_CANCEL        = 1,
    VOICE_CMD_NO_THANK_YOU1 = 2,
    VOICE_CMD_NO_THANK_YOU2 = 3,
    VOICE_CMD_NO_THANK_YOU3 = 4,
    VOICE_CMD_NO_THANK_YOU4 = 5,
    VOICE_CMD_NO_THANK_YOU5 = 6,
    VOICE_CMD_CONFIRM1      = 7,
    VOICE_CMD_CONFIRM2      = 8,
    VOICE_CMD_CONFIRM3      = 9,
    VOICE_CMD_CONFIRM4      = 10,
    VOICE_CMD_CONFIRM5      = 11,
    VOICE_CMD_CONFIRM6      = 12,
    VOICE_CMD_CONFIRM7      = 13,
    VOICE_CMD_CONFIRM8      = 14,
    VOICE_CMD_GO_AHEAD      = 15,
    VOICE_CMD_ESPRESSO      = 16,
    VOICE_CMD_AMERICANO     = 17,
    VOICE_CMD_CAPPUCCINO    = 18,
    VOICE_CMD_CAFE_LATTE1   = 19,
    VOICE_CMD_CAFE_LATTE2   = 20,
    VOICE_CMD_SMALL         = 21,
    VOICE_CMD_MEDIUM        = 22,
    VOICE_CMD_LARGE         = 23,
    VOICE_CMD_SOFT          = 24,
    VOICE_CMD_SOFT2         = 25,
    VOICE_CMD_MILD          = 26,
    VOICE_CMD_STRONG        = 27,
    VOICE_CMD_DEREGISTER    = 28,
    VOICE_CMD_INVALID
};

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

static char *s_PromptName[PROMPT_INVALID + 1] = {"Ok",
                                                 "Can I Help",
                                                 "Timeout",
                                                 "Another Expresso",
                                                 "Another Americano",
                                                 "Another Cappuccino",
                                                 "Another CafeLate",
                                                 "Register Selection",
                                                 "Recognition success",
                                                 "Invalid"};

static gfx_surface_t s_UiSurface;

SDK_ALIGN(static char s_AsBuffer[UI_BUFFER_WIDTH * UI_BUFFER_HEIGHT * UI_BUFFER_BPP], 32);

static float s_FaceRecProgress = 0.0f;
TimerHandle_t pFaceRecProgressTimer;
#define FACE_REC_UPDATE_INTERVAL (60000 / 100)

static TimerHandle_t s_CheckBrewingTimer = NULL;

static int s_IsWaitingAnotherSelection  = 0;
static int s_IsWaitingRegisterSelection = 0;
static int s_Recognized                 = 0;
static int s_UserId                     = -1;
static uint8_t s_UserCoffeeType         = kCoffeeType_NumTypes;
static uint8_t s_UserCoffeeSize         = kCoffeeSize_NumSizes;
static uint8_t s_UserCoffeeStrengh      = kCoffeeStrength_NumStrengths;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Init(output_dev_t *dev, output_dev_callback_t callback);
static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Deinit(const output_dev_t *dev);
static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Start(const output_dev_t *dev);
static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Stop(const output_dev_t *dev);

static void _StopVoiceCmd(void);

__attribute__((weak)) uint32_t APP_OutputDev_UiCoffeeMachine_InferCompleteDecode(output_algo_source_t source,
                                                                                 void *inferResult)
{
    return 0;
}
__attribute__((weak)) uint32_t APP_OutputDev_UiCoffeeMachine_InputNotifyDecode(event_base_t *inputData)
{
    return 0;
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_InferComplete(const output_dev_t *dev,
                                                                       output_algo_source_t source,
                                                                       void *inferResult);
static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_InputNotify(const output_dev_t *dev, void *data);

#if defined(__cplusplus)
}
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

const static output_dev_operator_t s_OutputDev_UiCoffeeMachineOps = {
    .init   = HAL_OutputDev_UiCoffeeMachine_Init,
    .deinit = HAL_OutputDev_UiCoffeeMachine_Deinit,
    .start  = HAL_OutputDev_UiCoffeeMachine_Start,
    .stop   = HAL_OutputDev_UiCoffeeMachine_Stop,
};

static output_dev_t s_OutputDev_UiCoffeeMachine = {
    .name          = "UiCoffeeMachine",
    .attr.type     = kOutputDevType_UI,
    .attr.pSurface = &s_UiSurface,
    .ops           = &s_OutputDev_UiCoffeeMachineOps,
};

const static output_dev_event_handler_t s_OutputDev_UiCoffeeMachineHandler = {
    .inferenceComplete = HAL_OutputDev_UiCoffeeMachine_InferComplete,
    .inputNotify       = HAL_OutputDev_UiCoffeeMachine_InputNotify,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static void _DrawGuideRect(int color)
{
    // guide rect
    int w = UI_GUIDE_RECT_W / UI_SCALE_W;
    int h = UI_GUIDE_RECT_H / UI_SCALE_H;
    int x = (UI_BUFFER_WIDTH - w) / 2;
    int y = (UI_BUFFER_HEIGHT - h) / 2;
    int l = 100 / UI_SCALE_W;
    int d = 4 / UI_SCALE_W;
    gfx_drawRect(&s_UiSurface, x, y, l, d, color);
    gfx_drawRect(&s_UiSurface, x, y, d, l, color);
    gfx_drawRect(&s_UiSurface, x + w - l, y, l, d, color);
    gfx_drawRect(&s_UiSurface, x + w, y, d, l, color);
    gfx_drawRect(&s_UiSurface, x, y + h, l, d, color);
    gfx_drawRect(&s_UiSurface, x, y + h - l, d, l, color);
    gfx_drawRect(&s_UiSurface, x + w - l, y + h, l, d, color);
    gfx_drawRect(&s_UiSurface, x + w, y + h - l, d, l, color);
}

static void _DrawProgressBar(float percent)
{
    /* process bar background */
    int x = (UI_BUFFER_WIDTH - PROCESS_BAR_BG_W / UI_SCALE_W) / 2;
    int y = (UI_BUFFER_HEIGHT + UI_GUIDE_RECT_H / UI_SCALE_H) / 2 + 36;
    gfx_drawPicture(&s_UiSurface, x, y, PROCESS_BAR_BG_W, PROCESS_BAR_BG_H, 0xFFFF, (char *)process_bar_240x14);

    /* process bar foreground */
    gfx_drawRect(&s_UiSurface, x + UI_MAINWINDOW_PROCESS_FG_X_OFFSET, y + UI_MAINWINDOW_PROCESS_FG_Y_OFFSET,
                 (int)(PROCESS_BAR_FG_W * percent), PROCESS_BAR_FG_H, RGB565_NXPBLUE);
}

static void _DrawOverlayUI(int drawGuider, int guiderColor, int drawProgressBar, float progressPercent)
{
    if (s_UiSurface.lock)
    {
        xSemaphoreTake(s_UiSurface.lock, portMAX_DELAY);
    }

    if (drawGuider)
    {
        _DrawGuideRect(guiderColor);
    }

    if (drawProgressBar)
    {
        _DrawProgressBar(progressPercent);
    }

    if (s_UiSurface.lock)
    {
        xSemaphoreGive(s_UiSurface.lock);
    }
}

static void _PlayPrompt(int id)
{
    static event_common_t s_PlayPromptEvent;
    LOGD("UI:play prompt \"%s\"", s_PromptName[id]);
    if (id >= PROMPT_OK && id < PROMPT_INVALID)
    {
        s_PlayPromptEvent.eventBase.eventId = kEventID_PlayPrompt;
        s_PlayPromptEvent.data              = (void *)id;

        uint8_t fromISR = __get_IPSR();
        s_OutputDev_UiCoffeeMachine.cap.callback(s_OutputDev_UiCoffeeMachine.id, kOutputEvent_OutputInputNotify,
                                                 &s_PlayPromptEvent, sizeof(s_PlayPromptEvent), fromISR);
    }
}

static void _CheckBrewingTimer_Stop()
{
    if (s_CheckBrewingTimer != NULL)
    {
        xTimerStop(s_CheckBrewingTimer, 0);
    }
}

static int _NeedToAskRegister(void)
{
    if (s_Recognized && s_UserId == -1)
    {
        // new user
        return 1;
    }

    return 0;
}

static void _CheckBrewingTimer_Callback(TimerHandle_t xTimer)
{
    coffee_machine_screen_id_t currentScreenId = get_current_screen();
    LOGD("UI:%s: \"CheckBrewingTimer\" callback", get_screen_name(currentScreenId));
    if (currentScreenId == SCR_FINISHED)
    {
        if (_NeedToAskRegister())
        {
            // register selection?
            if (s_IsWaitingRegisterSelection == 0)
            {
                _PlayPrompt(PROMPT_REGISTER_SELECTION);
                s_IsWaitingRegisterSelection = 1;
            }
        }
        else
        {
            LOGD("UI:stop \"CheckBrewingTimer\" timer");
            xTimerStop(xTimer, 0);
            set_standby_screen();
        }
    }
    else if (currentScreenId == SCR_HOME || currentScreenId == SCR_STANDBY)
    {
        // stop the timer
        LOGD("UI:stop \"CheckBrewingTimer\" timer");
        xTimerStop(xTimer, 0);
    }
}

static void _CheckBrewingTimer_Start()
{
    if (s_CheckBrewingTimer == NULL)
    {
        s_CheckBrewingTimer = xTimerCreate("BrewingTimer", (TickType_t)pdMS_TO_TICKS(3000), pdTRUE, NULL,
                                           (TimerCallbackFunction_t)_CheckBrewingTimer_Callback);
        if (s_CheckBrewingTimer == NULL)
        {
            LOGE("Failed to create \"CheckBrewingTimer\" timer.");
        }
    }

    if (s_CheckBrewingTimer != NULL)
    {
        if (xTimerStart(s_CheckBrewingTimer, 0) != pdPASS)
        {
            LOGE("Failed to start \"CheckBrewingTimer\" timer.");
        }
        LOGD("UI:start \"CheckBrewingTimer\" timer");
    }
}

static void _FaceRecProgressTimer_Start()
{
    if (pFaceRecProgressTimer != NULL)
    {
        if (xTimerStart(pFaceRecProgressTimer, 0) != pdPASS)
        {
            LOGE("Failed to start \"FaceRecProgress\" timer.");
        }
    }
}

static void _FaceRecProgressTimer_Stop()
{
    if (pFaceRecProgressTimer != NULL)
    {
        xTimerStop(pFaceRecProgressTimer, 0);
    }
}

static void _FaceRecProgressTimer_Callback(TimerHandle_t xTimer)
{
    int drawUpdate = 0;
    if (s_FaceRecProgress < 1.0f)
    {
        s_FaceRecProgress += 0.01f;
        drawUpdate = 1;
    }
    else
    {
        xTimerStop(xTimer, 0);
    }

    if (drawUpdate)
    {
        _DrawOverlayUI(0, 0, 1, s_FaceRecProgress);
    }
}

static void _StopVoiceCmd()
{
    LOGD("UI:voice cmd stop");
    static event_voice_t s_VoiceEvent;
    s_VoiceEvent.event_base.eventId = STOP_VOICE_CMD_SESSION;

    uint8_t fromISR = __get_IPSR();
    s_OutputDev_UiCoffeeMachine.cap.callback(s_OutputDev_UiCoffeeMachine.id, kOutputEvent_VoiceAlgoInputNotify,
                                             &s_VoiceEvent, sizeof(s_VoiceEvent), fromISR);
}

static void _StopFaceRec(int stop)
{
    static event_face_rec_t s_FaceRecEvent;
    LOGD("UI:face rec stop %d", stop);
    // notify the face rec to start
    s_FaceRecEvent.eventBase.eventId = kEventFaceRecID_OasisSetState;
    if (stop)
    {
        s_FaceRecEvent.oasisState.state = kOasisState_Stopped;
    }
    else
    {
        s_FaceRecEvent.oasisState.state = kOasisState_Running;
    }
    uint8_t fromISR = __get_IPSR();
    s_OutputDev_UiCoffeeMachine.cap.callback(s_OutputDev_UiCoffeeMachine.id, kOutputEvent_VisionAlgoInputNotify,
                                             &s_FaceRecEvent, sizeof(s_FaceRecEvent), fromISR);
}

static void _RegisterCoffeeSelection(coffee_type_t type, coffee_size_t size, coffee_strength_t strength)
{
    static event_face_rec_t s_FaceRecEvent;
    LOGD("UI:register coffee selection %d:%d:%d", type, size, strength);
    s_FaceRecEvent.eventBase.eventId                  = kEventFaceRecId_RegisterCoffeeSelection;
    s_FaceRecEvent.regCoffeeSelection.coffee_type     = type;
    s_FaceRecEvent.regCoffeeSelection.coffee_size     = size;
    s_FaceRecEvent.regCoffeeSelection.coffee_strength = strength;
    uint8_t fromISR                                   = __get_IPSR();
    s_OutputDev_UiCoffeeMachine.cap.callback(s_OutputDev_UiCoffeeMachine.id, kOutputEvent_VisionAlgoInputNotify,
                                             &s_FaceRecEvent, sizeof(s_FaceRecEvent), fromISR);
}

static void _DeregisterCoffeeSelection()
{
    static event_face_rec_t s_FaceRecEvent;
    LOGD("UI:Deregister coffee selection %d:%d", s_Recognized, s_UserId);
    if (s_Recognized && s_UserId >= 0)
    {
        s_FaceRecEvent.eventBase.eventId = kEventFaceRecID_DelUser;
        s_FaceRecEvent.delFace.hasID     = 0;
        s_FaceRecEvent.delFace.hasName   = 0;
        uint8_t fromISR                  = __get_IPSR();
        s_OutputDev_UiCoffeeMachine.cap.callback(s_OutputDev_UiCoffeeMachine.id, kOutputEvent_VisionAlgoInputNotify,
                                                 &s_FaceRecEvent, sizeof(s_FaceRecEvent), fromISR);
    }
}

void UI_EnterScreen_Callback(coffee_machine_screen_id_t screenId)
{
    LOGD("UI:enter screen [%s]", get_screen_name(screenId));
    switch (screenId)
    {
        case SCR_HOME:
        {
            // draw overlay UI
            s_FaceRecProgress           = 0.0f;
            s_IsWaitingAnotherSelection = 0;
            s_Recognized                = 0;
            s_UserId                    = -1;
            s_UserCoffeeType            = kCoffeeType_NumTypes;
            s_UserCoffeeSize            = kCoffeeSize_NumSizes;
            s_UserCoffeeStrengh         = kCoffeeStrength_NumStrengths;
            _DrawOverlayUI(1, RGB565_GREEN, 1, s_FaceRecProgress);
            _FaceRecProgressTimer_Start();
            // disable the lpm
            FWK_LpmManager_EnableSleepMode(0);
            // start the face rec
            _StopFaceRec(0);
        }
        break;
        case SCR_BREWING:
        {
            _CheckBrewingTimer_Start();
        }
        break;
        case SCR_FINISHED:
        {
        }
        break;
        case SCR_STANDBY:
        {
            _StopFaceRec(1);
            _StopVoiceCmd();
            FWK_LpmManager_EnableSleepMode(1);
        }
        break;
        default:
            break;
    }
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Init(output_dev_t *dev, output_dev_callback_t callback)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    LOGD("++HAL_OutputDev_UiCoffeeMachine_Init");

    dev->cap.callback = callback;

    /* Add initialization code here */
    s_UiSurface.left   = 0;
    s_UiSurface.top    = 0;
    s_UiSurface.right  = UI_BUFFER_WIDTH - 1;
    s_UiSurface.bottom = UI_BUFFER_HEIGHT - 1;
    s_UiSurface.height = UI_BUFFER_HEIGHT;
    s_UiSurface.width  = UI_BUFFER_WIDTH;
    s_UiSurface.pitch  = UI_BUFFER_WIDTH * 2;
    s_UiSurface.format = kPixelFormat_RGB565;
    s_UiSurface.buf    = s_AsBuffer;
    s_UiSurface.lock   = xSemaphoreCreateMutex();

    pFaceRecProgressTimer =
        xTimerCreate("FaceRecProgress", (TickType_t)pdMS_TO_TICKS(FACE_REC_UPDATE_INTERVAL), pdTRUE,
                     (void *)&s_FaceRecProgress, (TimerCallbackFunction_t)_FaceRecProgressTimer_Callback);
    if (pFaceRecProgressTimer == NULL)
    {
        LOGE("Failed to start \"FaceRecProgress\" timer.");
    }

    LOGD("--HAL_OutputDev_UiCoffeeMachine_Init");
    return error;
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Deinit(const output_dev_t *dev)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    LOGD("++HAL_OutputDev_UiLvgl_Deinit");

    /* Add de-initialization code here */

    LOGD("--HAL_OutputDev_UiLvgl_Deinit");
    return error;
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Start(const output_dev_t *dev)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    LOGD("++HAL_OutputDev_UiCoffeeMachine_Start");

    /* Add start code here */
    if (FWK_OutputManager_RegisterEventHandler(dev, &s_OutputDev_UiCoffeeMachineHandler) != 0)
    {
        error = kStatus_HAL_OutputError;
    }

    LOGD("--HAL_OutputDev_UiCoffeeMachine_Start");
    return error;
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_Stop(const output_dev_t *dev)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    LOGD("++HAL_OutputDev_UiCoffeeMachine_Stop");

    /* Add stop code here */

    LOGD("--HAL_OutputDev_UiCoffeeMachine_Stop");
    return error;
}

static hal_output_status_t _InferComplete_Vision(const output_dev_t *dev,
                                                 void *inferResult,
                                                 coffee_machine_screen_id_t currentScreenId)
{
    hal_output_status_t error              = kStatus_HAL_OutputSuccess;
    vision_algo_result_t *visionAlgoResult = (vision_algo_result_t *)inferResult;
    oasis_lite_result_t *pResult           = NULL;

    if (visionAlgoResult != NULL)
    {
        if (visionAlgoResult->id == kVisionAlgoID_OasisLite)
        {
            pResult = (oasis_lite_result_t *)&(visionAlgoResult->oasisLite);
        }
    }

    if (pResult != NULL)
    {
        LOGD("UI:%s:face recognized:%d face_id:%d", get_screen_name(currentScreenId), pResult->face_recognized,
             pResult->face_id);
        if (currentScreenId == SCR_HOME)
        {
            int drawUpdate = 0;
            if (!pResult->face_recognized && pResult->face_id < 0)
            {
                // start the recognition
                s_FaceRecProgress = 0.0f;
                drawUpdate        = 1;
                _FaceRecProgressTimer_Start();
            }
            else if (pResult->face_recognized && pResult->face_id >= 0)
            {
                // known user
                s_IsWaitingAnotherSelection = 1;
                _PlayPrompt(PROMPT_RECOGNITION_SUCCESS);
                s_FaceRecProgress = 1.0f;
                drawUpdate        = 1;
                _FaceRecProgressTimer_Stop();

                // store the user's selection
                s_Recognized        = 1;
                s_UserId            = pResult->face_id;
                s_UserCoffeeType    = pResult->coffee_type;
                s_UserCoffeeSize    = pResult->coffee_size;
                s_UserCoffeeStrengh = pResult->coffee_strength;

                // ask another selection
                int promptId = PROMPT_INVALID;
                switch (pResult->coffee_type)
                {
                    case kCoffeeType_Americano:
                        promptId = PROMPT_ANOTHER_AMERICANO;
                        break;
                    case kCoffeeType_Cappuccino:
                        promptId = PROMPT_ANOTHER_CAPPUCCINO;
                        break;
                    case kCoffeeType_Espresso:
                        promptId = PROMPT_ANOTHER_ESPRESSO;
                        break;
                    case kCoffeeType_Latte:
                        promptId = PROMPT_ANOTHER_CAFE_LATTE;
                        break;
                    default:
                        break;
                }
                if (promptId != PROMPT_INVALID)
                {
                    _PlayPrompt(promptId);
                }
            }
            else if (pResult->face_recognized && pResult->face_id < 0)
            {
                // new user
                _PlayPrompt(PROMPT_RECOGNITION_SUCCESS);
                s_Recognized      = 1;
                s_UserId          = -1;
                s_FaceRecProgress = 1.0f;
                drawUpdate        = 1;
                _FaceRecProgressTimer_Stop();
            }

            if (drawUpdate)
            {
                _DrawOverlayUI(1, RGB565_GREEN, 1, s_FaceRecProgress);
            }
        }
    }

    return error;
}

static hal_output_status_t _InferComplete_Voice(const output_dev_t *dev,
                                                void *inferResult,
                                                coffee_machine_screen_id_t currentScreenId)
{
    hal_output_status_t error               = kStatus_HAL_OutputSuccess;
    asr_inference_result_t *voiceAlgoResult = (asr_inference_result_t *)inferResult;
    LOGD("UI:%s:voice status:%d  cmd:%d %d:%d", get_screen_name(currentScreenId), voiceAlgoResult->status,
         voiceAlgoResult->keywordID, s_IsWaitingAnotherSelection, s_IsWaitingRegisterSelection);

    if (currentScreenId == SCR_STANDBY)
    {
        if (voiceAlgoResult->status == ASR_WW_DETECT)
        {
            // wake up word detected
            _PlayPrompt(PROMPT_CAN_I_HELP);
            // go to home screen
            set_home_screen();
        }
    }
    else if (currentScreenId == SCR_HOME)
    {
        if (voiceAlgoResult->status == ASR_CMD_DETECT && voiceAlgoResult->keywordID > -1)
        {
            // voice command detected
            switch (voiceAlgoResult->keywordID)
            {
                case (VOICE_CMD_START):
                {
                    _PlayPrompt(PROMPT_OK);
                    brew_coffee(get_coffee_type());
                }
                break;
                case (VOICE_CMD_CONFIRM1):
                case (VOICE_CMD_CONFIRM2):
                case (VOICE_CMD_CONFIRM3):
                case (VOICE_CMD_CONFIRM4):
                case (VOICE_CMD_CONFIRM5):
                case (VOICE_CMD_CONFIRM6):
                case (VOICE_CMD_CONFIRM7):
                case (VOICE_CMD_CONFIRM8):
                case (VOICE_CMD_GO_AHEAD):
                {
                    if (s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        s_IsWaitingAnotherSelection = 0;
                        brew_coffee(get_coffee_type());
                    }
                }
                break;
                case (VOICE_CMD_CANCEL):
                case (VOICE_CMD_NO_THANK_YOU1):
                case (VOICE_CMD_NO_THANK_YOU2):
                case (VOICE_CMD_NO_THANK_YOU3):
                case (VOICE_CMD_NO_THANK_YOU4):
                case (VOICE_CMD_NO_THANK_YOU5):
                {
                    if (s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        s_IsWaitingAnotherSelection = 0;
                    }
                }
                break;
                case (VOICE_CMD_ESPRESSO):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_type_t curType = get_coffee_type();
                        coffee_type_t newType = kCoffeeType_Espresso;
                        if (curType != newType)
                        {
                            LOGD("CurType:%d newType:%d", curType, newType);
                            set_coffee_type(newType);
                        }
                    }
                }
                break;
                case (VOICE_CMD_AMERICANO):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_type_t curType = get_coffee_type();
                        coffee_type_t newType = kCoffeeType_Americano;
                        if (curType != newType)
                        {
                            LOGD("UI:CurType:%d newType:%d", curType, newType);
                            set_coffee_type(newType);
                        }
                    }
                }
                break;
                case (VOICE_CMD_CAPPUCCINO):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_type_t curType = get_coffee_type();
                        coffee_type_t newType = kCoffeeType_Cappuccino;
                        if (curType != newType)
                        {
                            LOGD("UI:CurType:%d newType:%d", curType, newType);
                            set_coffee_type(newType);
                        }
                    }
                }
                break;
                case (VOICE_CMD_CAFE_LATTE1):
                case (VOICE_CMD_CAFE_LATTE2):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_type_t curType = get_coffee_type();
                        coffee_type_t newType = kCoffeeType_Latte;
                        if (curType != newType)
                        {
                            LOGD("UI:CurType:%d newType:%d", curType, newType);
                            set_coffee_type(newType);
                        }
                    }
                }
                break;
                case (VOICE_CMD_SMALL):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_size_t curSize = get_coffee_size();
                        coffee_size_t newSize = kCoffeeSize_Small;
                        if (curSize != newSize)
                        {
                            LOGD("UI:CurSize:%d newSize:%d", curSize, newSize);
                            set_coffee_size(newSize);
                        }
                    }
                }
                break;
                case (VOICE_CMD_MEDIUM):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_size_t curSize = get_coffee_size();
                        coffee_size_t newSize = kCoffeeSize_Medium;
                        if (curSize != newSize)
                        {
                            LOGD("UI:CurSize:%d newSize:%d", curSize, newSize);
                            set_coffee_size(newSize);
                        }
                    }
                }
                break;
                case (VOICE_CMD_LARGE):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_size_t curSize = get_coffee_size();
                        coffee_size_t newSize = kCoffeeSize_Large;
                        if (curSize != newSize)
                        {
                            LOGD("UI:CurSize:%d newSize:%d", curSize, newSize);
                            set_coffee_size(newSize);
                        }
                    }
                }
                break;
                case (VOICE_CMD_SOFT):
                case (VOICE_CMD_SOFT2):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_strength_t curStrength = get_coffee_strength();
                        coffee_strength_t newStrength = kCoffeeStrength_Weak;
                        LOGD("UI:CurStrength:%d newStrength:%d", curStrength, newStrength);
                        set_coffee_strength(newStrength);
                    }
                }
                break;
                case (VOICE_CMD_MILD):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_strength_t curStrength = get_coffee_strength();
                        coffee_strength_t newStrength = kCoffeeStrength_Medium;
                        LOGD("CurStrength:%d newStrength:%d", curStrength, newStrength);
                        set_coffee_strength(newStrength);
                    }
                }
                break;
                case (VOICE_CMD_STRONG):
                {
                    if (!s_IsWaitingAnotherSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        coffee_strength_t curStrength = get_coffee_strength();
                        coffee_strength_t newStrength = kCoffeeStrength_Strong;
                        LOGD("UI:CurStrength:%d newStrength:%d", curStrength, newStrength);
                        set_coffee_strength(newStrength);
                    }
                }
                break;
                case (VOICE_CMD_DEREGISTER):
                {
                    _PlayPrompt(PROMPT_OK);
                    _DeregisterCoffeeSelection();
                }
                break;
                default:
                    break;
            }
        }
        else if (voiceAlgoResult->status == ASR_TIMEOUT)
        {
            _PlayPrompt(PROMPT_TONE_TIMEOUT);
            // voice command session timeout
            set_standby_screen();
        }

        if (s_IsWaitingAnotherSelection)
        {
            // cancel the user's confirmation
            s_IsWaitingAnotherSelection = 0;
        }
    }
    else if (currentScreenId == SCR_FINISHED)
    {
        if (voiceAlgoResult->status == ASR_CMD_DETECT && voiceAlgoResult->keywordID > -1)
        {
            switch (voiceAlgoResult->keywordID)
            {
                case (VOICE_CMD_CONFIRM1):
                case (VOICE_CMD_CONFIRM2):
                case (VOICE_CMD_CONFIRM3):
                case (VOICE_CMD_CONFIRM4):
                case (VOICE_CMD_CONFIRM5):
                case (VOICE_CMD_CONFIRM6):
                case (VOICE_CMD_CONFIRM7):
                case (VOICE_CMD_CONFIRM8):
                case (VOICE_CMD_GO_AHEAD):
                {
                    if (s_IsWaitingRegisterSelection)
                    {
                        _PlayPrompt(PROMPT_OK);
                        s_Recognized = 0;
                        s_UserId     = -1;
                        _FaceRecProgressTimer_Stop();
                        s_IsWaitingRegisterSelection = 0;

                        coffee_type_t curType         = get_coffee_type();
                        coffee_size_t curSize         = get_coffee_size();
                        coffee_strength_t curStrength = get_coffee_strength();
                        _RegisterCoffeeSelection(curType, curSize, curStrength);
                    }
                }
                break;
                case (VOICE_CMD_CANCEL):
                case (VOICE_CMD_NO_THANK_YOU1):
                case (VOICE_CMD_NO_THANK_YOU2):
                case (VOICE_CMD_NO_THANK_YOU3):
                case (VOICE_CMD_NO_THANK_YOU4):
                case (VOICE_CMD_NO_THANK_YOU5):
                {
                    _PlayPrompt(PROMPT_OK);
                    if (s_IsWaitingRegisterSelection)
                    {
                        s_Recognized = 0;
                        s_UserId     = -1;
                        _FaceRecProgressTimer_Stop();
                        s_IsWaitingRegisterSelection = 0;
                    }
                    else
                    {
                        set_home_screen();
                    }
                }
                break;
                default:
                    break;
            }
        }
        else if (voiceAlgoResult->status == ASR_TIMEOUT)
        {
            _PlayPrompt(PROMPT_TONE_TIMEOUT);
            // voice command session timeout
            set_standby_screen();
            if (s_IsWaitingRegisterSelection)
            {
                s_IsWaitingRegisterSelection = 0;
            }
        }
    }
    return error;
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_InferComplete(const output_dev_t *dev,
                                                                       output_algo_source_t source,
                                                                       void *inferResult)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;

    if (inferResult == NULL)
    {
        return error;
    }

    coffee_machine_screen_id_t currentScreenId = get_current_screen();

    if (currentScreenId == SCR_INVALID)
    {
        return error;
    }

    if (source == kOutputAlgoSource_Vision)
    {
        _InferComplete_Vision(dev, inferResult, currentScreenId);
    }
    else if (source == kOutputAlgoSource_Voice)
    {
        _InferComplete_Voice(dev, inferResult, currentScreenId);
    }

    return error;
}

static hal_output_status_t HAL_OutputDev_UiCoffeeMachine_InputNotify(const output_dev_t *dev, void *data)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    event_base_t *eventBase   = (event_base_t *)data;

    /* Add 'inputNotify' event handler code here */
    APP_OutputDev_UiCoffeeMachine_InputNotifyDecode(eventBase);

    return error;
}

int HAL_OutputDev_UiCoffeeMachine_Register()
{
    int error = 0;
    LOGD("++HAL_OutputDev_UiCoffeeMachine_Register");

    error = FWK_OutputManager_DeviceRegister(&s_OutputDev_UiCoffeeMachine);

    LOGD("--HAL_OutputDev_UiCoffeeMachine_Register");
    return error;
}

#endif /* ENABLE_OUTPUT_DEV_UiCoffeeMachine */
