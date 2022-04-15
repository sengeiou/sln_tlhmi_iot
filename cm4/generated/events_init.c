/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl/lvgl.h"
#include "custom.h"

void events_init(lv_ui *ui)
{
}

static void home_imgbtn_start_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_RELEASED:
        {
            brew_coffee(get_coffee_type());
        }
        break;
        default:
            break;
    }
}

static void home_slider_coffeeSize_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_VALUE_CHANGED:
        {
            set_coffee_size(lv_slider_get_value(guider_ui.home_slider_coffeeSize));
        }
        break;
        default:
            break;
    }
}

static void home_slider_coffeeStrength_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_VALUE_CHANGED:
        {
            set_coffee_strength(lv_slider_get_value(guider_ui.home_slider_coffeeStrength));
        }
        break;
        default:
            break;
    }
}

static inline coffee_type_t _prev_coffee_type()
{
    coffee_type_t cur = get_coffee_type();

    if ((cur - 1) <= kCoffeeType_Invalid)
    {
        // wrap around to top of types list
        return kCoffeeType_NumTypes - 1;
    }
    return cur - 1;
}

static void home_imgbtn_coffeePrev_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_PRESSED:
        {
            set_coffee_type(_prev_coffee_type());
        }
        break;
        default:
            break;
    }
}

static inline coffee_type_t _next_coffee_type()
{
    return ((get_coffee_type() + 1) % kCoffeeType_NumTypes);
}

static void home_imgbtn_coffeeNext_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_PRESSED:
        {
            set_coffee_type(_next_coffee_type());
        }
        break;
        default:
            break;
    }
}

static void home_imgbtn_unregister_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_RELEASED:
        {
			// TODO: Deregister the current face being recognized
        }
        break;
        default:
            break;
    }
}

void events_init_home(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->home_imgbtn_start, home_imgbtn_start_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->home_slider_coffeeSize, home_slider_coffeeSize_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->home_slider_coffeeStrength, home_slider_coffeeStrength_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->home_imgbtn_coffeeNext, home_imgbtn_coffeeNext_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->home_imgbtn_coffeePrev, home_imgbtn_coffeePrev_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->home_imgbtn_unregister, home_imgbtn_unregister_event_handler, LV_EVENT_ALL, NULL);
}

static void brewing_imgbtn_cancel_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_RELEASED:
        {
            gui_load_screen(SCR_HOME, NO_DELAY);
        }
        break;
        default:
            break;
    }
}

static void brewing_animimg_brewing_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_RELEASED:
        {
            gui_load_screen(SCR_HOME, NO_DELAY);
        }
        break;
        default:
            break;
    }
}

void events_init_brewing(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->brewing_imgbtn_cancel, brewing_imgbtn_cancel_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui->brewing_animimg_brewing, brewing_animimg_brewing_event_handler, LV_EVENT_ALL, NULL);
}

static void finished_imgbtn_back_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
        case LV_EVENT_RELEASED:
        {
            gui_load_screen(SCR_HOME, NO_DELAY);
        }
        break;
        default:
            break;
    }
}

void events_init_finished(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->finished_imgbtn_back, finished_imgbtn_back_event_handler, LV_EVENT_ALL, NULL);
}
