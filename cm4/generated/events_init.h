/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#ifndef EVENTS_INIT_H_
#define EVENTS_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

void events_init(lv_ui *ui);
void events_init_home(lv_ui *ui);
void events_init_brewing(lv_ui *ui);
void events_init_finished(lv_ui *ui);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */