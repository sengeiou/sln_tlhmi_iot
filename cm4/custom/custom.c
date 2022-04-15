// SPDX-License-Identifier: MIT
// Copyright 2022 NXP

/**
 * @file custom.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "custom.h"
#include "lvgl/lvgl.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void _set_bar_value(void *bar, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
static char *s_ScreenName[SCR_INVALID + 1] = {"Home", "Brewing", "Finished", "Standby", "Invalid"};

static int s_CoffeeStrength       = kCoffeeStrength_Weak;
static coffee_size_t s_CoffeeSize = kCoffeeSize_Small;
static coffee_type_t s_CoffeeType = kCoffeeType_Americano;

const lv_img_dsc_t *coffee_imgs[kCoffeeType_NumTypes] = {
    [kCoffeeType_Americano]  = &_Americano_250x250,
    [kCoffeeType_Cappuccino] = &_Cappuccino_250x250,
    [kCoffeeType_Espresso]   = &_Espresso_250x250,
    [kCoffeeType_Latte]      = &_Cafe_Latte_250x250,
};

/**
 * @brief enter screen callback
 */
void UI_EnterScreen_Callback(coffee_machine_screen_id_t screenId);

/**
 * @brief Set the type of the coffee to brew and set coffee image as appropriate
 *
 * @param type the type of coffee to brew
 */
void set_coffee_type(coffee_type_t type)
{
    // check that type is valid
    if (kCoffeeType_Invalid < type && type < kCoffeeType_NumTypes)
    {
        lv_img_set_src(guider_ui.home_img_coffee, coffee_imgs[type]);
        s_CoffeeType = type;
    }
}

/**
 * @brief Set the size of coffee to brew
 *
 * @param size the size of coffee to brew
 */
void set_coffee_size(coffee_size_t size)
{
    if (kCoffeeSize_Invalid < size && size < kCoffeeSize_NumSizes)
    {
        s_CoffeeSize = size;
        /* If set via shell command/voice command */
        if (lv_slider_get_value(guider_ui.home_slider_coffeeSize) != size)
        {
            lv_slider_set_value(guider_ui.home_slider_coffeeSize, size, LV_ANIM_OFF);
        }
    }
}

/**
 * @brief Set the strength of coffee to brew
 *
 * @param strength the current strength of coffee to brew
 */
void set_coffee_strength(coffee_strength_t strength)
{
    if (kCoffeeStrength_Invalid < strength && strength < kCoffeeStrength_NumStrengths)
    {
        s_CoffeeStrength = strength;
        /* If set via shell command/voice command */
        if (lv_slider_get_value(guider_ui.home_slider_coffeeStrength) != strength)
        {
            lv_slider_set_value(guider_ui.home_slider_coffeeStrength, strength, LV_ANIM_OFF);
        }
    }
}

/**
 * @brief Get the currently selected coffee size
 *
 * @return coffee_size_t the current size of coffee to brew
 */
coffee_size_t get_coffee_size(void)
{
    return s_CoffeeSize;
}

/**
 * @brief Get the currently selected coffee type
 *
 * @return coffee_type_t the currently type of coffee to brew
 */
coffee_type_t get_coffee_type(void)
{
    return s_CoffeeType;
}

/**
 * @brief Get the currently selected coffee strength
 *
 * @return coffee_strength_t the strength of coffee to brew
 */
coffee_strength_t get_coffee_strength(void)
{
    return s_CoffeeStrength;
}

/**
 * @brief Animate the coffee fill progress bar by setting its value
 *
 * @param bar the bar object to set the value of
 * @param v the value to set the bar to
 */
static void _set_bar_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}

/**
 * @brief Load the screen specified by `scr_id` after `delay` milliseconds
 *
 * @param scr_id the id of the screen to load
 * @param delay milliseconds to delay
 */
void gui_load_screen(int scr_id, int delay)
{
    lv_obj_t **scr_to_load = NULL;

    switch (scr_id)
    {
        case SCR_HOME:
            // clear any pending animations
            // TODO: Remove lv_anim_del_all once lv_scr_load_anim auto_del LVGL bug is fixed
            lv_anim_del_all();
            scr_to_load = &guider_ui.home;
            if (!lv_obj_is_valid(*scr_to_load))
            {
                setup_scr_home(&guider_ui);
            }
            break;

        case SCR_BREWING:
            scr_to_load = &guider_ui.brewing;
            if (!lv_obj_is_valid(*scr_to_load))
            {
                setup_scr_brewing(&guider_ui);
            }

            // TODO: Remove everything before 'break' once lv_scr_load_anim auto_del LVGL bug is fixed
            // enable bar animation
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, guider_ui.brewing_bar_fillStatus);
            lv_anim_set_values(&a, 0, 100);
            lv_anim_set_exec_cb(&a, _set_bar_value);
            lv_anim_set_time(&a, BREW_TIME);
            lv_anim_start(&a);

            // set duration for fill animation
            lv_animimg_set_duration(guider_ui.brewing_animimg_brewing, BREW_TIME);
            lv_animimg_start(guider_ui.brewing_animimg_brewing);
            break;

        case SCR_FINISHED:
            scr_to_load = &guider_ui.finished;
            if (!lv_obj_is_valid(*scr_to_load))
            {
                setup_scr_finished(&guider_ui);
            }
            // TODO: Remove everything before 'break' once lv_scr_load_anim auto_del LVGL bug is fixed
            // set coffee image to display based on current coffee type
            lv_img_set_src(guider_ui.finished_img_coffee, coffee_imgs[get_coffee_type()]);
            break;

        case SCR_STANDBY:
            lv_anim_del_all();
            scr_to_load = &guider_ui.standby;
            if (!lv_obj_is_valid(*scr_to_load))
            {
                setup_scr_standby(&guider_ui);
            }
            break;
    }
    lv_scr_load_anim(*scr_to_load, LV_SCR_LOAD_ANIM_NONE, 0, delay, false);
}

/**
 * @brief start the coffee brewing animation
 *
 * @param type the type of coffee to brew
 */
void brew_coffee(coffee_type_t type)
{
    UI_EnterScreen_Callback(SCR_BREWING);

    // if no coffee has been selected, do nothing
    if (type == kCoffeeType_Invalid)
    {
        return;
    }

    gui_load_screen(SCR_BREWING, NO_DELAY);

    // delay for BREW_TIME seconds
    gui_load_screen(SCR_FINISHED, BREW_TIME);
}

/**
 * @brief set to home screen
 *
 */
void set_home_screen(void)
{
    UI_EnterScreen_Callback(SCR_HOME);
    gui_load_screen(SCR_HOME, NO_DELAY);
}

/**
 * @brief set to standby screen
 *
 */
void set_standby_screen(void)
{
    UI_EnterScreen_Callback(SCR_STANDBY);
    gui_load_screen(SCR_STANDBY, NO_DELAY);
}

/**
 * @brief get current screen
 *
 */
coffee_machine_screen_id_t get_current_screen(void)
{
    lv_obj_t *pCurrentScreen                   = lv_scr_act();
    coffee_machine_screen_id_t currentScreenId = SCR_INVALID;

    if (pCurrentScreen == guider_ui.home)
    {
        currentScreenId = SCR_HOME;
    }
    else if (pCurrentScreen == guider_ui.brewing)
    {
        currentScreenId = SCR_BREWING;
    }
    else if (pCurrentScreen == guider_ui.finished)
    {
        currentScreenId = SCR_FINISHED;
    }
    else if (pCurrentScreen == guider_ui.standby)
    {
        currentScreenId = SCR_STANDBY;
    }

    return currentScreenId;
}

/**
 * @brief get screen name
 *
 */
const char *get_screen_name(coffee_machine_screen_id_t screenId)
{
    return s_ScreenName[screenId];
}

/**
 * Initializes demo application
 */

void custom_init(lv_ui *ui)
{ /* Add your code here */
}
