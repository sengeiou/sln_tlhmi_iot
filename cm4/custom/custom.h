// SPDX-License-Identifier: MIT
// Copyright 2022 NXP

/*
 * custom.h
 *
 *  Created on: July 29, 2020
 *      Author: nxf53801
 */

#ifndef __CUSTOM_H_
#define __CUSTOM_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*********************
 *      INCLUDES
 *********************/
#include "gui_guider.h"

/*********************
 *      DEFINES
 *********************/
#define BREW_TIME (2000 * (1 + get_coffee_size()))
#define NO_DELAY  0

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    SCR_HOME = 0,
    SCR_BREWING,
    SCR_FINISHED,
    SCR_STANDBY,
    SCR_INVALID
} coffee_machine_screen_id_t;

typedef enum
{
    kCoffeeType_Invalid = -1,
    kCoffeeType_Americano,
    kCoffeeType_Cappuccino,
    kCoffeeType_Espresso,
    kCoffeeType_Latte,
    kCoffeeType_NumTypes
} coffee_type_t;

typedef enum
{
    kCoffeeSize_Invalid = -1,
    kCoffeeSize_Small,
    kCoffeeSize_Medium,
    kCoffeeSize_Large,
    kCoffeeSize_NumSizes
} coffee_size_t;

typedef enum
{
    kCoffeeStrength_Invalid = -1,
    kCoffeeStrength_Weak,
    kCoffeeStrength_Medium,
    kCoffeeStrength_Strong,
    kCoffeeStrength_NumStrengths
} coffee_strength_t;

/**********************
 *
 **********************/
extern const lv_img_dsc_t *coffee_imgs[kCoffeeType_NumTypes];

void set_coffee_size(coffee_size_t size);
void set_coffee_type(coffee_type_t type);
void set_coffee_strength(coffee_strength_t strength);

coffee_size_t get_coffee_size(void);
coffee_type_t get_coffee_type(void);
coffee_strength_t get_coffee_strength(void);

void gui_load_screen(int scr_id, int delay);
void brew_coffee(coffee_type_t type);
void set_home_screen(void);
void set_standby_screen(void);

coffee_machine_screen_id_t get_current_screen(void);
const char *get_screen_name(coffee_machine_screen_id_t screenId);
void enter_screen_callback(coffee_machine_screen_id_t screenId);

void custom_init(lv_ui *ui);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* EVENT_CB_H_ */
