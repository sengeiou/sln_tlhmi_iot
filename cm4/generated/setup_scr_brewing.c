/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

static const lv_img_dsc_t *brewing_animimg_brewing_imgs[30] = {
    &brewing_animimg_brewingf01, &brewing_animimg_brewingf02, &brewing_animimg_brewingf03, &brewing_animimg_brewingf04,
    &brewing_animimg_brewingf05, &brewing_animimg_brewingf06, &brewing_animimg_brewingf07, &brewing_animimg_brewingf08,
    &brewing_animimg_brewingf09, &brewing_animimg_brewingf10, &brewing_animimg_brewingf11, &brewing_animimg_brewingf12,
    &brewing_animimg_brewingf13, &brewing_animimg_brewingf14, &brewing_animimg_brewingf15, &brewing_animimg_brewingf16,
    &brewing_animimg_brewingf17, &brewing_animimg_brewingf18, &brewing_animimg_brewingf19, &brewing_animimg_brewingf20,
    &brewing_animimg_brewingf21, &brewing_animimg_brewingf22, &brewing_animimg_brewingf23, &brewing_animimg_brewingf24,
    &brewing_animimg_brewingf25, &brewing_animimg_brewingf26, &brewing_animimg_brewingf27, &brewing_animimg_brewingf28,
    &brewing_animimg_brewingf29, &brewing_animimg_brewingf31};

void setup_scr_brewing(lv_ui *ui)
{
    // Write codes brewing
    ui->brewing = lv_obj_create(NULL);

    // Write style state: LV_STATE_DEFAULT for style_brewing_main_main_default
    static lv_style_t style_brewing_main_main_default;
    lv_style_reset(&style_brewing_main_main_default);
    lv_style_set_bg_color(&style_brewing_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_opa(&style_brewing_main_main_default, 0);
    lv_obj_add_style(ui->brewing, &style_brewing_main_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes brewing_cont_background
    ui->brewing_cont_background = lv_obj_create(ui->brewing);
    lv_obj_set_pos(ui->brewing_cont_background, 0, 0);
    lv_obj_set_size(ui->brewing_cont_background, 1280, 720);

    // Write style state: LV_STATE_DEFAULT for style_brewing_cont_background_main_main_default
    static lv_style_t style_brewing_cont_background_main_main_default;
    lv_style_reset(&style_brewing_cont_background_main_main_default);
    lv_style_set_radius(&style_brewing_cont_background_main_main_default, 0);
    lv_style_set_bg_color(&style_brewing_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_brewing_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_dir(&style_brewing_cont_background_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_brewing_cont_background_main_main_default, 255);
    lv_style_set_border_color(&style_brewing_cont_background_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_brewing_cont_background_main_main_default, 0);
    lv_style_set_border_opa(&style_brewing_cont_background_main_main_default, 255);
    lv_style_set_pad_left(&style_brewing_cont_background_main_main_default, 0);
    lv_style_set_pad_right(&style_brewing_cont_background_main_main_default, 0);
    lv_style_set_pad_top(&style_brewing_cont_background_main_main_default, 0);
    lv_style_set_pad_bottom(&style_brewing_cont_background_main_main_default, 0);
    lv_obj_add_style(ui->brewing_cont_background, &style_brewing_cont_background_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes brewing_imgbtn_cancel
    ui->brewing_imgbtn_cancel = lv_imgbtn_create(ui->brewing_cont_background);
    lv_obj_set_pos(ui->brewing_imgbtn_cancel, 0, 0);
    lv_obj_set_size(ui->brewing_imgbtn_cancel, 100, 100);

    // Write style state: LV_STATE_DEFAULT for style_brewing_imgbtn_cancel_main_main_default
    static lv_style_t style_brewing_imgbtn_cancel_main_main_default;
    lv_style_reset(&style_brewing_imgbtn_cancel_main_main_default);
    lv_style_set_text_color(&style_brewing_imgbtn_cancel_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor(&style_brewing_imgbtn_cancel_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_brewing_imgbtn_cancel_main_main_default, 0);
    lv_style_set_img_opa(&style_brewing_imgbtn_cancel_main_main_default, 255);
    lv_obj_add_style(ui->brewing_imgbtn_cancel, &style_brewing_imgbtn_cancel_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_PRESSED for style_brewing_imgbtn_cancel_main_main_pressed
    static lv_style_t style_brewing_imgbtn_cancel_main_main_pressed;
    lv_style_reset(&style_brewing_imgbtn_cancel_main_main_pressed);
    lv_style_set_text_color(&style_brewing_imgbtn_cancel_main_main_pressed, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_brewing_imgbtn_cancel_main_main_pressed, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_brewing_imgbtn_cancel_main_main_pressed, 83);
    lv_obj_add_style(ui->brewing_imgbtn_cancel, &style_brewing_imgbtn_cancel_main_main_pressed,
                     LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style state: LV_STATE_CHECKED for style_brewing_imgbtn_cancel_main_main_checked
    static lv_style_t style_brewing_imgbtn_cancel_main_main_checked;
    lv_style_reset(&style_brewing_imgbtn_cancel_main_main_checked);
    lv_style_set_text_color(&style_brewing_imgbtn_cancel_main_main_checked, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_brewing_imgbtn_cancel_main_main_checked, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_brewing_imgbtn_cancel_main_main_checked, 0);
    lv_obj_add_style(ui->brewing_imgbtn_cancel, &style_brewing_imgbtn_cancel_main_main_checked,
                     LV_PART_MAIN | LV_STATE_CHECKED);
    lv_imgbtn_set_src(ui->brewing_imgbtn_cancel, LV_IMGBTN_STATE_RELEASED, NULL, &_Cancel_alpha_100x100, NULL);
    lv_obj_add_flag(ui->brewing_imgbtn_cancel, LV_OBJ_FLAG_CHECKABLE);

    // Write codes brewing_cont_brewing
    ui->brewing_cont_brewing = lv_obj_create(ui->brewing);
    lv_obj_set_pos(ui->brewing_cont_brewing, 439, 160);
    lv_obj_set_size(ui->brewing_cont_brewing, 400, 335);

    // Write style state: LV_STATE_DEFAULT for style_brewing_cont_brewing_main_main_default
    static lv_style_t style_brewing_cont_brewing_main_main_default;
    lv_style_reset(&style_brewing_cont_brewing_main_main_default);
    lv_style_set_radius(&style_brewing_cont_brewing_main_main_default, 0);
    lv_style_set_bg_color(&style_brewing_cont_brewing_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_brewing_cont_brewing_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_brewing_cont_brewing_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_brewing_cont_brewing_main_main_default, 0);
    lv_style_set_border_color(&style_brewing_cont_brewing_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_brewing_cont_brewing_main_main_default, 0);
    lv_style_set_border_opa(&style_brewing_cont_brewing_main_main_default, 255);
    lv_style_set_pad_left(&style_brewing_cont_brewing_main_main_default, 0);
    lv_style_set_pad_right(&style_brewing_cont_brewing_main_main_default, 0);
    lv_style_set_pad_top(&style_brewing_cont_brewing_main_main_default, 0);
    lv_style_set_pad_bottom(&style_brewing_cont_brewing_main_main_default, 0);
    lv_obj_add_style(ui->brewing_cont_brewing, &style_brewing_cont_brewing_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes brewing_bar_fillStatus
    ui->brewing_bar_fillStatus = lv_bar_create(ui->brewing_cont_brewing);
    lv_obj_set_pos(ui->brewing_bar_fillStatus, 49, 310);
    lv_obj_set_size(ui->brewing_bar_fillStatus, 300, 20);

    // Write style state: LV_STATE_DEFAULT for style_brewing_bar_fillstatus_main_main_default
    static lv_style_t style_brewing_bar_fillstatus_main_main_default;
    lv_style_reset(&style_brewing_bar_fillstatus_main_main_default);
    lv_style_set_radius(&style_brewing_bar_fillstatus_main_main_default, 10);
    lv_style_set_bg_color(&style_brewing_bar_fillstatus_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_brewing_bar_fillstatus_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_brewing_bar_fillstatus_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_brewing_bar_fillstatus_main_main_default, 60);
    lv_style_set_pad_left(&style_brewing_bar_fillstatus_main_main_default, 0);
    lv_style_set_pad_right(&style_brewing_bar_fillstatus_main_main_default, 0);
    lv_style_set_pad_top(&style_brewing_bar_fillstatus_main_main_default, 0);
    lv_style_set_pad_bottom(&style_brewing_bar_fillstatus_main_main_default, 0);
    lv_obj_add_style(ui->brewing_bar_fillStatus, &style_brewing_bar_fillstatus_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for style_brewing_bar_fillstatus_main_indicator_default
    static lv_style_t style_brewing_bar_fillstatus_main_indicator_default;
    lv_style_reset(&style_brewing_bar_fillstatus_main_indicator_default);
    lv_style_set_radius(&style_brewing_bar_fillstatus_main_indicator_default, 10);
    lv_style_set_bg_color(&style_brewing_bar_fillstatus_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_brewing_bar_fillstatus_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_brewing_bar_fillstatus_main_indicator_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_brewing_bar_fillstatus_main_indicator_default, 255);
    lv_obj_add_style(ui->brewing_bar_fillStatus, &style_brewing_bar_fillstatus_main_indicator_default,
                     LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_anim_time(ui->brewing_bar_fillStatus, 1000, 0);
    lv_bar_set_mode(ui->brewing_bar_fillStatus, LV_BAR_MODE_NORMAL);
    lv_bar_set_value(ui->brewing_bar_fillStatus, 50, LV_ANIM_OFF);

    // Write codes brewing_animimg_brewing
    ui->brewing_animimg_brewing = lv_animimg_create(ui->brewing_cont_brewing);
    lv_obj_set_pos(ui->brewing_animimg_brewing, 101, 50);
    lv_obj_set_size(ui->brewing_animimg_brewing, 200, 200);
    lv_animimg_set_src(ui->brewing_animimg_brewing, (lv_img_dsc_t **)brewing_animimg_brewing_imgs, 30);
    lv_animimg_set_duration(ui->brewing_animimg_brewing, 2250);
    lv_animimg_set_repeat_count(ui->brewing_animimg_brewing, 1);
    lv_animimg_start(ui->brewing_animimg_brewing);

    // Init events for screen
    events_init_brewing(ui);
}