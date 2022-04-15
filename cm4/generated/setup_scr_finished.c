/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

void setup_scr_finished(lv_ui *ui)
{
    // Write codes finished
    ui->finished = lv_obj_create(NULL);

    // Write style state: LV_STATE_DEFAULT for style_finished_main_main_default
    static lv_style_t style_finished_main_main_default;
    lv_style_reset(&style_finished_main_main_default);
    lv_style_set_bg_color(&style_finished_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_opa(&style_finished_main_main_default, 0);
    lv_obj_add_style(ui->finished, &style_finished_main_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes finished_cont_background
    ui->finished_cont_background = lv_obj_create(ui->finished);
    lv_obj_set_pos(ui->finished_cont_background, 0, 0);
    lv_obj_set_size(ui->finished_cont_background, 1280, 720);

    // Write style state: LV_STATE_DEFAULT for style_finished_cont_background_main_main_default
    static lv_style_t style_finished_cont_background_main_main_default;
    lv_style_reset(&style_finished_cont_background_main_main_default);
    lv_style_set_radius(&style_finished_cont_background_main_main_default, 0);
    lv_style_set_bg_color(&style_finished_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_finished_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_dir(&style_finished_cont_background_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_finished_cont_background_main_main_default, 255);
    lv_style_set_border_color(&style_finished_cont_background_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_finished_cont_background_main_main_default, 0);
    lv_style_set_border_opa(&style_finished_cont_background_main_main_default, 0);
    lv_style_set_pad_left(&style_finished_cont_background_main_main_default, 0);
    lv_style_set_pad_right(&style_finished_cont_background_main_main_default, 0);
    lv_style_set_pad_top(&style_finished_cont_background_main_main_default, 0);
    lv_style_set_pad_bottom(&style_finished_cont_background_main_main_default, 0);
    lv_obj_add_style(ui->finished_cont_background, &style_finished_cont_background_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes finished_img_coffee
    ui->finished_img_coffee = lv_img_create(ui->finished_cont_background);
    lv_obj_set_pos(ui->finished_img_coffee, 515, 160);
    lv_obj_set_size(ui->finished_img_coffee, 250, 250);

    // Write style state: LV_STATE_DEFAULT for style_finished_img_coffee_main_main_default
    static lv_style_t style_finished_img_coffee_main_main_default;
    lv_style_reset(&style_finished_img_coffee_main_main_default);
    lv_style_set_img_recolor(&style_finished_img_coffee_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_finished_img_coffee_main_main_default, 0);
    lv_style_set_img_opa(&style_finished_img_coffee_main_main_default, 255);
    lv_obj_add_style(ui->finished_img_coffee, &style_finished_img_coffee_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->finished_img_coffee, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->finished_img_coffee, coffee_imgs[get_coffee_type()]);
    lv_img_set_pivot(ui->finished_img_coffee, 0, 0);
    lv_img_set_angle(ui->finished_img_coffee, 0);

    // Write codes finished_imgbtn_back
    ui->finished_imgbtn_back = lv_imgbtn_create(ui->finished);
    lv_obj_set_pos(ui->finished_imgbtn_back, 0, 0);
    lv_obj_set_size(ui->finished_imgbtn_back, 100, 100);

    // Write style state: LV_STATE_DEFAULT for style_finished_imgbtn_back_main_main_default
    static lv_style_t style_finished_imgbtn_back_main_main_default;
    lv_style_reset(&style_finished_imgbtn_back_main_main_default);
    lv_style_set_text_color(&style_finished_imgbtn_back_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor(&style_finished_imgbtn_back_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_finished_imgbtn_back_main_main_default, 0);
    lv_style_set_img_opa(&style_finished_imgbtn_back_main_main_default, 255);
    lv_obj_add_style(ui->finished_imgbtn_back, &style_finished_imgbtn_back_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_PRESSED for style_finished_imgbtn_back_main_main_pressed
    static lv_style_t style_finished_imgbtn_back_main_main_pressed;
    lv_style_reset(&style_finished_imgbtn_back_main_main_pressed);
    lv_style_set_text_color(&style_finished_imgbtn_back_main_main_pressed, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_finished_imgbtn_back_main_main_pressed, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_finished_imgbtn_back_main_main_pressed, 91);
    lv_obj_add_style(ui->finished_imgbtn_back, &style_finished_imgbtn_back_main_main_pressed,
                     LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style state: LV_STATE_CHECKED for style_finished_imgbtn_back_main_main_checked
    static lv_style_t style_finished_imgbtn_back_main_main_checked;
    lv_style_reset(&style_finished_imgbtn_back_main_main_checked);
    lv_style_set_text_color(&style_finished_imgbtn_back_main_main_checked, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_finished_imgbtn_back_main_main_checked, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_finished_imgbtn_back_main_main_checked, 0);
    lv_obj_add_style(ui->finished_imgbtn_back, &style_finished_imgbtn_back_main_main_checked,
                     LV_PART_MAIN | LV_STATE_CHECKED);
    lv_imgbtn_set_src(ui->finished_imgbtn_back, LV_IMGBTN_STATE_RELEASED, NULL, &_Back_alpha_100x100, NULL);
    lv_obj_add_flag(ui->finished_imgbtn_back, LV_OBJ_FLAG_CHECKABLE);

    // Init events for screen
    events_init_finished(ui);
}