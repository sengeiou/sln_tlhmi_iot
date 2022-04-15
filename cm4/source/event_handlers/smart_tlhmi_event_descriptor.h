/*
 * Copyright 2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief smart tlhmi-specific event descriptor declarations.
 */

#ifndef _SMART_TLHMI_EVENT_DESCRIPTOR_H_
#define _SMART_TLHMI_EVENT_DESCRIPTOR_H_

#include "hal_event_descriptor_common.h"
#include "custom.h"

typedef enum _event_smart_tlhmi_id
{
    kEventID_GetCoffeeType = kEventType_FaceRec,
    kEventID_SetCoffeeType,
    kEventID_SmartTLHMIID_COUNT
} event_smart_tlhmi_id_t;

typedef struct _coffee_type_event
{
    coffee_type_t type;
} coffee_type_event_t;

typedef struct _event_smart_tlhmi
{
    event_base_t eventBase;

    union
    {
        void *data;
        coffee_type_event_t coffeeType;
    };
} event_smart_tlhmi_t;

#endif /* _SMART_TLHMI_EVENT_DESCRIPTOR_H_ */
