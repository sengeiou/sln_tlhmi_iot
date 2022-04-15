/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"


void setup_scr_standby(lv_ui *ui){

	//Write codes standby
	ui->standby = lv_obj_create(NULL);

	//Write style state: LV_STATE_DEFAULT for style_standby_main_main_default
	static lv_style_t style_standby_main_main_default;
	lv_style_reset(&style_standby_main_main_default);
	lv_style_set_bg_color(&style_standby_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_opa(&style_standby_main_main_default, 0);
	lv_obj_add_style(ui->standby, &style_standby_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes standby_cont_background
	ui->standby_cont_background = lv_obj_create(ui->standby);
	lv_obj_set_pos(ui->standby_cont_background, 0, 0);
	lv_obj_set_size(ui->standby_cont_background, 1280, 720);

	//Write style state: LV_STATE_DEFAULT for style_standby_cont_background_main_main_default
	static lv_style_t style_standby_cont_background_main_main_default;
	lv_style_reset(&style_standby_cont_background_main_main_default);
	lv_style_set_radius(&style_standby_cont_background_main_main_default, 0);
	lv_style_set_bg_color(&style_standby_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_color(&style_standby_cont_background_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_standby_cont_background_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_standby_cont_background_main_main_default, 255);
	lv_style_set_border_color(&style_standby_cont_background_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_standby_cont_background_main_main_default, 0);
	lv_style_set_border_opa(&style_standby_cont_background_main_main_default, 255);
	lv_style_set_pad_left(&style_standby_cont_background_main_main_default, 0);
	lv_style_set_pad_right(&style_standby_cont_background_main_main_default, 0);
	lv_style_set_pad_top(&style_standby_cont_background_main_main_default, 0);
	lv_style_set_pad_bottom(&style_standby_cont_background_main_main_default, 0);
	lv_obj_add_style(ui->standby_cont_background, &style_standby_cont_background_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes standby_cont_header
	ui->standby_cont_header = lv_obj_create(ui->standby);
	lv_obj_set_pos(ui->standby_cont_header, 0, 0);
	lv_obj_set_size(ui->standby_cont_header, 1280, 100);

	//Write style state: LV_STATE_DEFAULT for style_standby_cont_header_main_main_default
	static lv_style_t style_standby_cont_header_main_main_default;
	lv_style_reset(&style_standby_cont_header_main_main_default);
	lv_style_set_radius(&style_standby_cont_header_main_main_default, 0);
	lv_style_set_bg_color(&style_standby_cont_header_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_standby_cont_header_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_standby_cont_header_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_standby_cont_header_main_main_default, 255);
	lv_style_set_border_color(&style_standby_cont_header_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_standby_cont_header_main_main_default, 0);
	lv_style_set_border_opa(&style_standby_cont_header_main_main_default, 255);
	lv_style_set_pad_left(&style_standby_cont_header_main_main_default, 0);
	lv_style_set_pad_right(&style_standby_cont_header_main_main_default, 0);
	lv_style_set_pad_top(&style_standby_cont_header_main_main_default, 0);
	lv_style_set_pad_bottom(&style_standby_cont_header_main_main_default, 0);
	lv_obj_add_style(ui->standby_cont_header, &style_standby_cont_header_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes standby_label_coffeeMachine
	ui->standby_label_coffeeMachine = lv_label_create(ui->standby_cont_header);
	lv_obj_set_pos(ui->standby_label_coffeeMachine, 0, 0);
	lv_obj_set_size(ui->standby_label_coffeeMachine, 1280, 100);
	lv_label_set_text(ui->standby_label_coffeeMachine, "COFFEE MACHINE");
	lv_label_set_long_mode(ui->standby_label_coffeeMachine, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->standby_label_coffeeMachine, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_standby_label_coffeemachine_main_main_default
	static lv_style_t style_standby_label_coffeemachine_main_main_default;
	lv_style_reset(&style_standby_label_coffeemachine_main_main_default);
	lv_style_set_radius(&style_standby_label_coffeemachine_main_main_default, 0);
	lv_style_set_bg_color(&style_standby_label_coffeemachine_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_standby_label_coffeemachine_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_standby_label_coffeemachine_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_standby_label_coffeemachine_main_main_default, 0);
	lv_style_set_text_color(&style_standby_label_coffeemachine_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_standby_label_coffeemachine_main_main_default, &lv_font_montserratMedium_100);
	lv_style_set_text_letter_space(&style_standby_label_coffeemachine_main_main_default, 2);
	lv_style_set_pad_left(&style_standby_label_coffeemachine_main_main_default, 0);
	lv_style_set_pad_right(&style_standby_label_coffeemachine_main_main_default, 0);
	lv_style_set_pad_top(&style_standby_label_coffeemachine_main_main_default, 0);
	lv_style_set_pad_bottom(&style_standby_label_coffeemachine_main_main_default, 0);
	lv_obj_add_style(ui->standby_label_coffeeMachine, &style_standby_label_coffeemachine_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes standby_cont_coffee
	ui->standby_cont_coffee = lv_obj_create(ui->standby);
	lv_obj_set_pos(ui->standby_cont_coffee, 75, 161);
	lv_obj_set_size(ui->standby_cont_coffee, 600, 500);

	//Write style state: LV_STATE_DEFAULT for style_standby_cont_coffee_main_main_default
	static lv_style_t style_standby_cont_coffee_main_main_default;
	lv_style_reset(&style_standby_cont_coffee_main_main_default);
	lv_style_set_radius(&style_standby_cont_coffee_main_main_default, 0);
	lv_style_set_bg_color(&style_standby_cont_coffee_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_standby_cont_coffee_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_standby_cont_coffee_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_standby_cont_coffee_main_main_default, 0);
	lv_style_set_border_color(&style_standby_cont_coffee_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_standby_cont_coffee_main_main_default, 0);
	lv_style_set_border_opa(&style_standby_cont_coffee_main_main_default, 255);
	lv_style_set_pad_left(&style_standby_cont_coffee_main_main_default, 0);
	lv_style_set_pad_right(&style_standby_cont_coffee_main_main_default, 0);
	lv_style_set_pad_top(&style_standby_cont_coffee_main_main_default, 0);
	lv_style_set_pad_bottom(&style_standby_cont_coffee_main_main_default, 0);
	lv_obj_add_style(ui->standby_cont_coffee, &style_standby_cont_coffee_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes standby_img_cafeLatte
	ui->standby_img_cafeLatte = lv_img_create(ui->standby_cont_coffee);
	lv_obj_set_pos(ui->standby_img_cafeLatte, 350, 250);
	lv_obj_set_size(ui->standby_img_cafeLatte, 250, 250);

	//Write style state: LV_STATE_DEFAULT for style_standby_img_cafelatte_main_main_default
	static lv_style_t style_standby_img_cafelatte_main_main_default;
	lv_style_reset(&style_standby_img_cafelatte_main_main_default);
	lv_style_set_img_recolor(&style_standby_img_cafelatte_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_standby_img_cafelatte_main_main_default, 0);
	lv_style_set_img_opa(&style_standby_img_cafelatte_main_main_default, 255);
	lv_obj_add_style(ui->standby_img_cafeLatte, &style_standby_img_cafelatte_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->standby_img_cafeLatte, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->standby_img_cafeLatte,&_Cafe_Latte_250x250);
	lv_img_set_pivot(ui->standby_img_cafeLatte, 0,0);
	lv_img_set_angle(ui->standby_img_cafeLatte, 0);

	//Write codes standby_img_americano
	ui->standby_img_americano = lv_img_create(ui->standby_cont_coffee);
	lv_obj_set_pos(ui->standby_img_americano, 0, 0);
	lv_obj_set_size(ui->standby_img_americano, 250, 250);

	//Write style state: LV_STATE_DEFAULT for style_standby_img_americano_main_main_default
	static lv_style_t style_standby_img_americano_main_main_default;
	lv_style_reset(&style_standby_img_americano_main_main_default);
	lv_style_set_img_recolor(&style_standby_img_americano_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_standby_img_americano_main_main_default, 0);
	lv_style_set_img_opa(&style_standby_img_americano_main_main_default, 255);
	lv_obj_add_style(ui->standby_img_americano, &style_standby_img_americano_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->standby_img_americano, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->standby_img_americano,&_Americano_250x250);
	lv_img_set_pivot(ui->standby_img_americano, 0,0);
	lv_img_set_angle(ui->standby_img_americano, 0);

	//Write codes standby_img_espresso
	ui->standby_img_espresso = lv_img_create(ui->standby_cont_coffee);
	lv_obj_set_pos(ui->standby_img_espresso, 0, 250);
	lv_obj_set_size(ui->standby_img_espresso, 250, 250);

	//Write style state: LV_STATE_DEFAULT for style_standby_img_espresso_main_main_default
	static lv_style_t style_standby_img_espresso_main_main_default;
	lv_style_reset(&style_standby_img_espresso_main_main_default);
	lv_style_set_img_recolor(&style_standby_img_espresso_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_standby_img_espresso_main_main_default, 0);
	lv_style_set_img_opa(&style_standby_img_espresso_main_main_default, 255);
	lv_obj_add_style(ui->standby_img_espresso, &style_standby_img_espresso_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->standby_img_espresso, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->standby_img_espresso,&_Espresso_250x250);
	lv_img_set_pivot(ui->standby_img_espresso, 0,0);
	lv_img_set_angle(ui->standby_img_espresso, 0);

	//Write codes standby_img_cappuccino
	ui->standby_img_cappuccino = lv_img_create(ui->standby_cont_coffee);
	lv_obj_set_pos(ui->standby_img_cappuccino, 350, 0);
	lv_obj_set_size(ui->standby_img_cappuccino, 250, 250);

	//Write style state: LV_STATE_DEFAULT for style_standby_img_cappuccino_main_main_default
	static lv_style_t style_standby_img_cappuccino_main_main_default;
	lv_style_reset(&style_standby_img_cappuccino_main_main_default);
	lv_style_set_img_recolor(&style_standby_img_cappuccino_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_standby_img_cappuccino_main_main_default, 0);
	lv_style_set_img_opa(&style_standby_img_cappuccino_main_main_default, 255);
	lv_obj_add_style(ui->standby_img_cappuccino, &style_standby_img_cappuccino_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->standby_img_cappuccino, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->standby_img_cappuccino,&_Cappuccino_250x250);
	lv_img_set_pivot(ui->standby_img_cappuccino, 0,0);
	lv_img_set_angle(ui->standby_img_cappuccino, 0);

	//Write codes standby_cont_voiceAndFaceRec
	ui->standby_cont_voiceAndFaceRec = lv_obj_create(ui->standby);
	lv_obj_set_pos(ui->standby_cont_voiceAndFaceRec, 760, 210);
	lv_obj_set_size(ui->standby_cont_voiceAndFaceRec, 400, 400);

	//Write style state: LV_STATE_DEFAULT for style_standby_cont_voiceandfacerec_main_main_default
	static lv_style_t style_standby_cont_voiceandfacerec_main_main_default;
	lv_style_reset(&style_standby_cont_voiceandfacerec_main_main_default);
	lv_style_set_radius(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_bg_color(&style_standby_cont_voiceandfacerec_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_standby_cont_voiceandfacerec_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_standby_cont_voiceandfacerec_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_border_color(&style_standby_cont_voiceandfacerec_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_border_opa(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_pad_left(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_pad_right(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_pad_top(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_style_set_pad_bottom(&style_standby_cont_voiceandfacerec_main_main_default, 0);
	lv_obj_add_style(ui->standby_cont_voiceAndFaceRec, &style_standby_cont_voiceandfacerec_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes standby_img_nxpFaceRec
	ui->standby_img_nxpFaceRec = lv_img_create(ui->standby_cont_voiceAndFaceRec);
	lv_obj_set_pos(ui->standby_img_nxpFaceRec, 20, 250);
	lv_obj_set_size(ui->standby_img_nxpFaceRec, 400, 150);

	//Write style state: LV_STATE_DEFAULT for style_standby_img_nxpfacerec_main_main_default
	static lv_style_t style_standby_img_nxpfacerec_main_main_default;
	lv_style_reset(&style_standby_img_nxpfacerec_main_main_default);
	lv_style_set_img_recolor(&style_standby_img_nxpfacerec_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_standby_img_nxpfacerec_main_main_default, 0);
	lv_style_set_img_opa(&style_standby_img_nxpfacerec_main_main_default, 255);
	lv_obj_add_style(ui->standby_img_nxpFaceRec, &style_standby_img_nxpfacerec_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->standby_img_nxpFaceRec, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->standby_img_nxpFaceRec,&_NxpFaceRec_400x150);
	lv_img_set_pivot(ui->standby_img_nxpFaceRec, 0,0);
	lv_img_set_angle(ui->standby_img_nxpFaceRec, 0);

	//Write codes standby_img_nxpVoiceRec
	ui->standby_img_nxpVoiceRec = lv_img_create(ui->standby_cont_voiceAndFaceRec);
	lv_obj_set_pos(ui->standby_img_nxpVoiceRec, 20, 0);
	lv_obj_set_size(ui->standby_img_nxpVoiceRec, 400, 150);

	//Write style state: LV_STATE_DEFAULT for style_standby_img_nxpvoicerec_main_main_default
	static lv_style_t style_standby_img_nxpvoicerec_main_main_default;
	lv_style_reset(&style_standby_img_nxpvoicerec_main_main_default);
	lv_style_set_img_recolor(&style_standby_img_nxpvoicerec_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_standby_img_nxpvoicerec_main_main_default, 0);
	lv_style_set_img_opa(&style_standby_img_nxpvoicerec_main_main_default, 255);
	lv_obj_add_style(ui->standby_img_nxpVoiceRec, &style_standby_img_nxpvoicerec_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->standby_img_nxpVoiceRec, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->standby_img_nxpVoiceRec,&_NxpVoiceRec_400x150);
	lv_img_set_pivot(ui->standby_img_nxpVoiceRec, 0,0);
	lv_img_set_angle(ui->standby_img_nxpVoiceRec, 0);
}