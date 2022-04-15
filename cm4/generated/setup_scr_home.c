/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

void setup_scr_home(lv_ui *ui)
{
    // Write codes home
    ui->home = lv_obj_create(NULL);

    // Write style state: LV_STATE_DEFAULT for style_home_main_main_default
    static lv_style_t style_home_main_main_default;
    lv_style_reset(&style_home_main_main_default);
    lv_style_set_bg_color(&style_home_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_opa(&style_home_main_main_default, 0);
    lv_obj_add_style(ui->home, &style_home_main_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes home_cont_background
    ui->home_cont_background = lv_obj_create(ui->home);
    lv_obj_set_pos(ui->home_cont_background, 0, 0);
    lv_obj_set_size(ui->home_cont_background, 1280, 720);

    // Write style state: LV_STATE_DEFAULT for style_home_cont_background_main_main_default
    static lv_style_t style_home_cont_background_main_main_default;
    lv_style_reset(&style_home_cont_background_main_main_default);
    lv_style_set_radius(&style_home_cont_background_main_main_default, 0);
    lv_style_set_bg_color(&style_home_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_home_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_dir(&style_home_cont_background_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_cont_background_main_main_default, 255);
    lv_style_set_border_color(&style_home_cont_background_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_home_cont_background_main_main_default, 0);
    lv_style_set_border_opa(&style_home_cont_background_main_main_default, 0);
    lv_style_set_pad_left(&style_home_cont_background_main_main_default, 0);
    lv_style_set_pad_right(&style_home_cont_background_main_main_default, 0);
    lv_style_set_pad_top(&style_home_cont_background_main_main_default, 0);
    lv_style_set_pad_bottom(&style_home_cont_background_main_main_default, 0);
    lv_obj_add_style(ui->home_cont_background, &style_home_cont_background_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes home_img_cameraPreview
    ui->home_img_cameraPreview = lv_img_create(ui->home_cont_background);
    lv_obj_set_pos(ui->home_img_cameraPreview, 50, 80);
    lv_obj_set_size(ui->home_img_cameraPreview, 640, 480);

    // Write style state: LV_STATE_DEFAULT for style_home_img_camerapreview_main_main_default
    static lv_style_t style_home_img_camerapreview_main_main_default;
    lv_style_reset(&style_home_img_camerapreview_main_main_default);
    lv_style_set_img_recolor(&style_home_img_camerapreview_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_img_camerapreview_main_main_default, 0);
    lv_style_set_img_opa(&style_home_img_camerapreview_main_main_default, 255);
    lv_obj_add_style(ui->home_img_cameraPreview, &style_home_img_camerapreview_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes home_img_nxpFaceRec
    ui->home_img_nxpFaceRec = lv_img_create(ui->home_cont_background);
    lv_obj_set_pos(ui->home_img_nxpFaceRec, 200, 0);
    lv_obj_set_size(ui->home_img_nxpFaceRec, 300, 80);

    // Write style state: LV_STATE_DEFAULT for style_home_img_nxpfacerec_main_main_default
    static lv_style_t style_home_img_nxpfacerec_main_main_default;
    lv_style_reset(&style_home_img_nxpfacerec_main_main_default);
    lv_style_set_img_recolor(&style_home_img_nxpfacerec_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_img_nxpfacerec_main_main_default, 0);
    lv_style_set_img_opa(&style_home_img_nxpfacerec_main_main_default, 255);
    lv_obj_add_style(ui->home_img_nxpFaceRec, &style_home_img_nxpfacerec_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->home_img_nxpFaceRec, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_nxpFaceRec, &_NxpFaceRec_300x80);
    lv_img_set_pivot(ui->home_img_nxpFaceRec, 0, 0);
    lv_img_set_angle(ui->home_img_nxpFaceRec, 0);

    // Write codes home_img_nxpVoiceRec
    ui->home_img_nxpVoiceRec = lv_img_create(ui->home_cont_background);
    lv_obj_set_pos(ui->home_img_nxpVoiceRec, 850, 0);
    lv_obj_set_size(ui->home_img_nxpVoiceRec, 300, 80);

    // Write style state: LV_STATE_DEFAULT for style_home_img_nxpvoicerec_main_main_default
    static lv_style_t style_home_img_nxpvoicerec_main_main_default;
    lv_style_reset(&style_home_img_nxpvoicerec_main_main_default);
    lv_style_set_img_recolor(&style_home_img_nxpvoicerec_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_img_nxpvoicerec_main_main_default, 0);
    lv_style_set_img_opa(&style_home_img_nxpvoicerec_main_main_default, 255);
    lv_obj_add_style(ui->home_img_nxpVoiceRec, &style_home_img_nxpvoicerec_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->home_img_nxpVoiceRec, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_nxpVoiceRec, &_NxpVoiceRec_300x80);
    lv_img_set_pivot(ui->home_img_nxpVoiceRec, 0, 0);
    lv_img_set_angle(ui->home_img_nxpVoiceRec, 0);

    // Write codes home_imgbtn_start
    ui->home_imgbtn_start = lv_imgbtn_create(ui->home);
    lv_obj_set_pos(ui->home_imgbtn_start, 270, 580);
    lv_obj_set_size(ui->home_imgbtn_start, 200, 125);

    // Write style state: LV_STATE_DEFAULT for style_home_imgbtn_start_main_main_default
    static lv_style_t style_home_imgbtn_start_main_main_default;
    lv_style_reset(&style_home_imgbtn_start_main_main_default);
    lv_style_set_text_color(&style_home_imgbtn_start_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor(&style_home_imgbtn_start_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_start_main_main_default, 0);
    lv_style_set_img_opa(&style_home_imgbtn_start_main_main_default, 255);
    lv_obj_add_style(ui->home_imgbtn_start, &style_home_imgbtn_start_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_PRESSED for style_home_imgbtn_start_main_main_pressed
    static lv_style_t style_home_imgbtn_start_main_main_pressed;
    lv_style_reset(&style_home_imgbtn_start_main_main_pressed);
    lv_style_set_text_color(&style_home_imgbtn_start_main_main_pressed, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_start_main_main_pressed, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_start_main_main_pressed, 90);
    lv_obj_add_style(ui->home_imgbtn_start, &style_home_imgbtn_start_main_main_pressed,
                     LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style state: LV_STATE_CHECKED for style_home_imgbtn_start_main_main_checked
    static lv_style_t style_home_imgbtn_start_main_main_checked;
    lv_style_reset(&style_home_imgbtn_start_main_main_checked);
    lv_style_set_text_color(&style_home_imgbtn_start_main_main_checked, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_start_main_main_checked, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_start_main_main_checked, 0);
    lv_obj_add_style(ui->home_imgbtn_start, &style_home_imgbtn_start_main_main_checked,
                     LV_PART_MAIN | LV_STATE_CHECKED);
    lv_imgbtn_set_src(ui->home_imgbtn_start, LV_IMGBTN_STATE_RELEASED, NULL, &_Start_alpha_200x125, NULL);
    lv_obj_add_flag(ui->home_imgbtn_start, LV_OBJ_FLAG_CHECKABLE);

    // Write codes home_cont_coffeeSize
    ui->home_cont_coffeeSize = lv_obj_create(ui->home);
    lv_obj_set_pos(ui->home_cont_coffeeSize, 795, 367);
    lv_obj_set_size(ui->home_cont_coffeeSize, 400, 200);

    // Write style state: LV_STATE_DEFAULT for style_home_cont_coffeesize_main_main_default
    static lv_style_t style_home_cont_coffeesize_main_main_default;
    lv_style_reset(&style_home_cont_coffeesize_main_main_default);
    lv_style_set_radius(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_bg_color(&style_home_cont_coffeesize_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_home_cont_coffeesize_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_home_cont_coffeesize_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_border_color(&style_home_cont_coffeesize_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_border_opa(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_pad_left(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_pad_right(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_pad_top(&style_home_cont_coffeesize_main_main_default, 0);
    lv_style_set_pad_bottom(&style_home_cont_coffeesize_main_main_default, 0);
    lv_obj_add_style(ui->home_cont_coffeeSize, &style_home_cont_coffeesize_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes home_img_sizeSlider
    ui->home_img_sizeSlider = lv_img_create(ui->home_cont_coffeeSize);
    lv_obj_set_pos(ui->home_img_sizeSlider, 0, 0);
    lv_obj_set_size(ui->home_img_sizeSlider, 400, 200);

    // Write style state: LV_STATE_DEFAULT for style_home_img_sizeslider_main_main_default
    static lv_style_t style_home_img_sizeslider_main_main_default;
    lv_style_reset(&style_home_img_sizeslider_main_main_default);
    lv_style_set_img_recolor(&style_home_img_sizeslider_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_img_sizeslider_main_main_default, 0);
    lv_style_set_img_opa(&style_home_img_sizeslider_main_main_default, 255);
    lv_obj_add_style(ui->home_img_sizeSlider, &style_home_img_sizeslider_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->home_img_sizeSlider, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_sizeSlider, &_Size_400x200);
    lv_img_set_pivot(ui->home_img_sizeSlider, 0, 0);
    lv_img_set_angle(ui->home_img_sizeSlider, 0);

    // Write codes home_slider_coffeeSize
    ui->home_slider_coffeeSize = lv_slider_create(ui->home_cont_coffeeSize);
    lv_obj_set_pos(ui->home_slider_coffeeSize, 27, 123);
    lv_obj_set_size(ui->home_slider_coffeeSize, 329, 34);

    // Write style state: LV_STATE_DEFAULT for style_home_slider_coffeesize_main_main_default
    static lv_style_t style_home_slider_coffeesize_main_main_default;
    lv_style_reset(&style_home_slider_coffeesize_main_main_default);
    lv_style_set_radius(&style_home_slider_coffeesize_main_main_default, 50);
    lv_style_set_bg_color(&style_home_slider_coffeesize_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_home_slider_coffeesize_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_home_slider_coffeesize_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_slider_coffeesize_main_main_default, 0);
    lv_style_set_outline_color(&style_home_slider_coffeesize_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_outline_width(&style_home_slider_coffeesize_main_main_default, 0);
    lv_style_set_outline_opa(&style_home_slider_coffeesize_main_main_default, 0);
    lv_style_set_pad_left(&style_home_slider_coffeesize_main_main_default, 0);
    lv_style_set_pad_right(&style_home_slider_coffeesize_main_main_default, 0);
    lv_style_set_pad_top(&style_home_slider_coffeesize_main_main_default, 0);
    lv_style_set_pad_bottom(&style_home_slider_coffeesize_main_main_default, 0);
    lv_obj_add_style(ui->home_slider_coffeeSize, &style_home_slider_coffeesize_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for style_home_slider_coffeesize_main_indicator_default
    static lv_style_t style_home_slider_coffeesize_main_indicator_default;
    lv_style_reset(&style_home_slider_coffeesize_main_indicator_default);
    lv_style_set_radius(&style_home_slider_coffeesize_main_indicator_default, 50);
    lv_style_set_bg_color(&style_home_slider_coffeesize_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_home_slider_coffeesize_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_home_slider_coffeesize_main_indicator_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_slider_coffeesize_main_indicator_default, 0);
    lv_obj_add_style(ui->home_slider_coffeeSize, &style_home_slider_coffeesize_main_indicator_default,
                     LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for style_home_slider_coffeesize_main_knob_default
    static lv_style_t style_home_slider_coffeesize_main_knob_default;
    lv_style_reset(&style_home_slider_coffeesize_main_knob_default);
    lv_style_set_radius(&style_home_slider_coffeesize_main_knob_default, 50);
    lv_style_set_bg_color(&style_home_slider_coffeesize_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_home_slider_coffeesize_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_home_slider_coffeesize_main_knob_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_slider_coffeesize_main_knob_default, 255);
    lv_obj_add_style(ui->home_slider_coffeeSize, &style_home_slider_coffeesize_main_knob_default,
                     LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_slider_set_range(ui->home_slider_coffeeSize, 0, 2);
    lv_slider_set_value(ui->home_slider_coffeeSize, 0, false);

    // Write codes home_cont_coffeeStrength
    ui->home_cont_coffeeStrength = lv_obj_create(ui->home);
    lv_obj_set_pos(ui->home_cont_coffeeStrength, 765, 589);
    lv_obj_set_size(ui->home_cont_coffeeStrength, 450, 125);

    // Write style state: LV_STATE_DEFAULT for style_home_cont_coffeestrength_main_main_default
    static lv_style_t style_home_cont_coffeestrength_main_main_default;
    lv_style_reset(&style_home_cont_coffeestrength_main_main_default);
    lv_style_set_radius(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_style_set_bg_color(&style_home_cont_coffeestrength_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_home_cont_coffeestrength_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_home_cont_coffeestrength_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_style_set_border_color(&style_home_cont_coffeestrength_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_style_set_border_opa(&style_home_cont_coffeestrength_main_main_default, 255);
    lv_style_set_pad_left(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_style_set_pad_right(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_style_set_pad_top(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_style_set_pad_bottom(&style_home_cont_coffeestrength_main_main_default, 0);
    lv_obj_add_style(ui->home_cont_coffeeStrength, &style_home_cont_coffeestrength_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes home_img_strengthSlider
    ui->home_img_strengthSlider = lv_img_create(ui->home_cont_coffeeStrength);
    lv_obj_set_pos(ui->home_img_strengthSlider, 25, -3);
    lv_obj_set_size(ui->home_img_strengthSlider, 400, 125);

    // Write style state: LV_STATE_DEFAULT for style_home_img_strengthslider_main_main_default
    static lv_style_t style_home_img_strengthslider_main_main_default;
    lv_style_reset(&style_home_img_strengthslider_main_main_default);
    lv_style_set_img_recolor(&style_home_img_strengthslider_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_img_strengthslider_main_main_default, 0);
    lv_style_set_img_opa(&style_home_img_strengthslider_main_main_default, 255);
    lv_obj_add_style(ui->home_img_strengthSlider, &style_home_img_strengthslider_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->home_img_strengthSlider, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_strengthSlider, &_Strength_400x125);
    lv_img_set_pivot(ui->home_img_strengthSlider, 0, 0);
    lv_img_set_angle(ui->home_img_strengthSlider, 0);

    // Write codes home_slider_coffeeStrength
    ui->home_slider_coffeeStrength = lv_slider_create(ui->home_cont_coffeeStrength);
    lv_obj_set_pos(ui->home_slider_coffeeStrength, 25, 13);
    lv_obj_set_size(ui->home_slider_coffeeStrength, 400, 35);

    // Write style state: LV_STATE_DEFAULT for style_home_slider_coffeestrength_main_main_default
    static lv_style_t style_home_slider_coffeestrength_main_main_default;
    lv_style_reset(&style_home_slider_coffeestrength_main_main_default);
    lv_style_set_radius(&style_home_slider_coffeestrength_main_main_default, 50);
    lv_style_set_bg_color(&style_home_slider_coffeestrength_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_home_slider_coffeestrength_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_home_slider_coffeestrength_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_slider_coffeestrength_main_main_default, 0);
    lv_style_set_outline_color(&style_home_slider_coffeestrength_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_outline_width(&style_home_slider_coffeestrength_main_main_default, 0);
    lv_style_set_outline_opa(&style_home_slider_coffeestrength_main_main_default, 255);
    lv_style_set_pad_left(&style_home_slider_coffeestrength_main_main_default, 0);
    lv_style_set_pad_right(&style_home_slider_coffeestrength_main_main_default, 0);
    lv_style_set_pad_top(&style_home_slider_coffeestrength_main_main_default, 0);
    lv_style_set_pad_bottom(&style_home_slider_coffeestrength_main_main_default, 0);
    lv_obj_add_style(ui->home_slider_coffeeStrength, &style_home_slider_coffeestrength_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for style_home_slider_coffeestrength_main_indicator_default
    static lv_style_t style_home_slider_coffeestrength_main_indicator_default;
    lv_style_reset(&style_home_slider_coffeestrength_main_indicator_default);
    lv_style_set_radius(&style_home_slider_coffeestrength_main_indicator_default, 50);
    lv_style_set_bg_color(&style_home_slider_coffeestrength_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_color(&style_home_slider_coffeestrength_main_indicator_default,
                               lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_bg_grad_dir(&style_home_slider_coffeestrength_main_indicator_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_slider_coffeestrength_main_indicator_default, 0);
    lv_obj_add_style(ui->home_slider_coffeeStrength, &style_home_slider_coffeestrength_main_indicator_default,
                     LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_DEFAULT for style_home_slider_coffeestrength_main_knob_default
    static lv_style_t style_home_slider_coffeestrength_main_knob_default;
    lv_style_reset(&style_home_slider_coffeestrength_main_knob_default);
    lv_style_set_radius(&style_home_slider_coffeestrength_main_knob_default, 50);
    lv_style_set_bg_color(&style_home_slider_coffeestrength_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_home_slider_coffeestrength_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_home_slider_coffeestrength_main_knob_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_slider_coffeestrength_main_knob_default, 255);
    lv_obj_add_style(ui->home_slider_coffeeStrength, &style_home_slider_coffeestrength_main_knob_default,
                     LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_slider_set_range(ui->home_slider_coffeeStrength, 0, 2);
    lv_slider_set_value(ui->home_slider_coffeeStrength, 0, false);

    // Write codes home_cont_1
    ui->home_cont_1 = lv_obj_create(ui->home);
    lv_obj_set_pos(ui->home_cont_1, 749, 91);
    lv_obj_set_size(ui->home_cont_1, 500, 299);

    // Write style state: LV_STATE_DEFAULT for style_home_cont_1_main_main_default
    static lv_style_t style_home_cont_1_main_main_default;
    lv_style_reset(&style_home_cont_1_main_main_default);
    lv_style_set_radius(&style_home_cont_1_main_main_default, 0);
    lv_style_set_bg_color(&style_home_cont_1_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_home_cont_1_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_dir(&style_home_cont_1_main_main_default, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_home_cont_1_main_main_default, 0);
    lv_style_set_border_color(&style_home_cont_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
    lv_style_set_border_width(&style_home_cont_1_main_main_default, 0);
    lv_style_set_border_opa(&style_home_cont_1_main_main_default, 255);
    lv_style_set_pad_left(&style_home_cont_1_main_main_default, 0);
    lv_style_set_pad_right(&style_home_cont_1_main_main_default, 0);
    lv_style_set_pad_top(&style_home_cont_1_main_main_default, 0);
    lv_style_set_pad_bottom(&style_home_cont_1_main_main_default, 0);
    lv_obj_add_style(ui->home_cont_1, &style_home_cont_1_main_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes home_img_coffee
    ui->home_img_coffee = lv_img_create(ui->home_cont_1);
    lv_obj_set_pos(ui->home_img_coffee, 126, 12);
    lv_obj_set_size(ui->home_img_coffee, 250, 250);

    // Write style state: LV_STATE_DEFAULT for style_home_img_coffee_main_main_default
    static lv_style_t style_home_img_coffee_main_main_default;
    lv_style_reset(&style_home_img_coffee_main_main_default);
    lv_style_set_img_recolor(&style_home_img_coffee_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_img_coffee_main_main_default, 0);
    lv_style_set_img_opa(&style_home_img_coffee_main_main_default, 255);
    lv_obj_add_style(ui->home_img_coffee, &style_home_img_coffee_main_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui->home_img_coffee, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_coffee, coffee_imgs[get_coffee_type()]);
    lv_img_set_pivot(ui->home_img_coffee, 0, 0);
    lv_img_set_angle(ui->home_img_coffee, 0);

    // Write codes home_imgbtn_coffeeNext
    ui->home_imgbtn_coffeeNext = lv_imgbtn_create(ui->home_cont_1);
    lv_obj_set_pos(ui->home_imgbtn_coffeeNext, 400, 84);
    lv_obj_set_size(ui->home_imgbtn_coffeeNext, 100, 100);

    // Write style state: LV_STATE_DEFAULT for style_home_imgbtn_coffeenext_main_main_default
    static lv_style_t style_home_imgbtn_coffeenext_main_main_default;
    lv_style_reset(&style_home_imgbtn_coffeenext_main_main_default);
    lv_style_set_text_color(&style_home_imgbtn_coffeenext_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor(&style_home_imgbtn_coffeenext_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_coffeenext_main_main_default, 0);
    lv_style_set_img_opa(&style_home_imgbtn_coffeenext_main_main_default, 255);
    lv_obj_add_style(ui->home_imgbtn_coffeeNext, &style_home_imgbtn_coffeenext_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_PRESSED for style_home_imgbtn_coffeenext_main_main_pressed
    static lv_style_t style_home_imgbtn_coffeenext_main_main_pressed;
    lv_style_reset(&style_home_imgbtn_coffeenext_main_main_pressed);
    lv_style_set_text_color(&style_home_imgbtn_coffeenext_main_main_pressed, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_coffeenext_main_main_pressed, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_coffeenext_main_main_pressed, 158);
    lv_obj_add_style(ui->home_imgbtn_coffeeNext, &style_home_imgbtn_coffeenext_main_main_pressed,
                     LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style state: LV_STATE_CHECKED for style_home_imgbtn_coffeenext_main_main_checked
    static lv_style_t style_home_imgbtn_coffeenext_main_main_checked;
    lv_style_reset(&style_home_imgbtn_coffeenext_main_main_checked);
    lv_style_set_text_color(&style_home_imgbtn_coffeenext_main_main_checked, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_coffeenext_main_main_checked, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_coffeenext_main_main_checked, 0);
    lv_obj_add_style(ui->home_imgbtn_coffeeNext, &style_home_imgbtn_coffeenext_main_main_checked,
                     LV_PART_MAIN | LV_STATE_CHECKED);
    lv_imgbtn_set_src(ui->home_imgbtn_coffeeNext, LV_IMGBTN_STATE_RELEASED, NULL, &_Forward_alpha_100x100, NULL);
    lv_obj_add_flag(ui->home_imgbtn_coffeeNext, LV_OBJ_FLAG_CHECKABLE);

    // Write codes home_imgbtn_coffeePrev
    ui->home_imgbtn_coffeePrev = lv_imgbtn_create(ui->home_cont_1);
    lv_obj_set_pos(ui->home_imgbtn_coffeePrev, 0, 84);
    lv_obj_set_size(ui->home_imgbtn_coffeePrev, 100, 100);

    // Write style state: LV_STATE_DEFAULT for style_home_imgbtn_coffeeprev_main_main_default
    static lv_style_t style_home_imgbtn_coffeeprev_main_main_default;
    lv_style_reset(&style_home_imgbtn_coffeeprev_main_main_default);
    lv_style_set_text_color(&style_home_imgbtn_coffeeprev_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor(&style_home_imgbtn_coffeeprev_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_coffeeprev_main_main_default, 0);
    lv_style_set_img_opa(&style_home_imgbtn_coffeeprev_main_main_default, 255);
    lv_obj_add_style(ui->home_imgbtn_coffeePrev, &style_home_imgbtn_coffeeprev_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_PRESSED for style_home_imgbtn_coffeeprev_main_main_pressed
    static lv_style_t style_home_imgbtn_coffeeprev_main_main_pressed;
    lv_style_reset(&style_home_imgbtn_coffeeprev_main_main_pressed);
    lv_style_set_text_color(&style_home_imgbtn_coffeeprev_main_main_pressed, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_coffeeprev_main_main_pressed, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_coffeeprev_main_main_pressed, 158);
    lv_obj_add_style(ui->home_imgbtn_coffeePrev, &style_home_imgbtn_coffeeprev_main_main_pressed,
                     LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style state: LV_STATE_CHECKED for style_home_imgbtn_coffeeprev_main_main_checked
    static lv_style_t style_home_imgbtn_coffeeprev_main_main_checked;
    lv_style_reset(&style_home_imgbtn_coffeeprev_main_main_checked);
    lv_style_set_text_color(&style_home_imgbtn_coffeeprev_main_main_checked, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_coffeeprev_main_main_checked, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_coffeeprev_main_main_checked, 0);
    lv_obj_add_style(ui->home_imgbtn_coffeePrev, &style_home_imgbtn_coffeeprev_main_main_checked,
                     LV_PART_MAIN | LV_STATE_CHECKED);
    lv_imgbtn_set_src(ui->home_imgbtn_coffeePrev, LV_IMGBTN_STATE_RELEASED, NULL, &_Back_alpha_100x100, NULL);
    lv_obj_add_flag(ui->home_imgbtn_coffeePrev, LV_OBJ_FLAG_CHECKABLE);

    // Write codes home_imgbtn_unregister
    ui->home_imgbtn_unregister = lv_imgbtn_create(ui->home);
    lv_obj_set_pos(ui->home_imgbtn_unregister, 45, 0);
    lv_obj_set_size(ui->home_imgbtn_unregister, 75, 75);

    // Write style state: LV_STATE_DEFAULT for style_home_imgbtn_unregister_main_main_default
    static lv_style_t style_home_imgbtn_unregister_main_main_default;
    lv_style_reset(&style_home_imgbtn_unregister_main_main_default);
    lv_style_set_text_color(&style_home_imgbtn_unregister_main_main_default, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor(&style_home_imgbtn_unregister_main_main_default, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_unregister_main_main_default, 0);
    lv_style_set_img_opa(&style_home_imgbtn_unregister_main_main_default, 255);
    lv_obj_add_style(ui->home_imgbtn_unregister, &style_home_imgbtn_unregister_main_main_default,
                     LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style state: LV_STATE_PRESSED for style_home_imgbtn_unregister_main_main_pressed
    static lv_style_t style_home_imgbtn_unregister_main_main_pressed;
    lv_style_reset(&style_home_imgbtn_unregister_main_main_pressed);
    lv_style_set_text_color(&style_home_imgbtn_unregister_main_main_pressed, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_unregister_main_main_pressed, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_unregister_main_main_pressed, 138);
    lv_obj_add_style(ui->home_imgbtn_unregister, &style_home_imgbtn_unregister_main_main_pressed,
                     LV_PART_MAIN | LV_STATE_PRESSED);

    // Write style state: LV_STATE_CHECKED for style_home_imgbtn_unregister_main_main_checked
    static lv_style_t style_home_imgbtn_unregister_main_main_checked;
    lv_style_reset(&style_home_imgbtn_unregister_main_main_checked);
    lv_style_set_text_color(&style_home_imgbtn_unregister_main_main_checked, lv_color_make(0xFF, 0x33, 0xFF));
    lv_style_set_img_recolor(&style_home_imgbtn_unregister_main_main_checked, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_img_recolor_opa(&style_home_imgbtn_unregister_main_main_checked, 0);
    lv_obj_add_style(ui->home_imgbtn_unregister, &style_home_imgbtn_unregister_main_main_checked,
                     LV_PART_MAIN | LV_STATE_CHECKED);
    lv_imgbtn_set_src(ui->home_imgbtn_unregister, LV_IMGBTN_STATE_RELEASED, NULL, &_Unregister_alpha_75x75, NULL);
    lv_obj_add_flag(ui->home_imgbtn_unregister, LV_OBJ_FLAG_CHECKABLE);

    // Init events for screen
    events_init_home(ui);
}