/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "guider_fonts.h"

typedef struct
{
    lv_obj_t *home;
    lv_obj_t *home_cont_background;
    lv_obj_t *home_img_cameraPreview;
    lv_obj_t *home_img_nxpFaceRec;
    lv_obj_t *home_img_nxpVoiceRec;
    lv_obj_t *home_imgbtn_start;
    lv_obj_t *home_imgbtn_start_label;
    lv_obj_t *home_cont_coffeeSize;
    lv_obj_t *home_img_sizeSlider;
    lv_obj_t *home_slider_coffeeSize;
    lv_obj_t *home_cont_coffeeStrength;
    lv_obj_t *home_img_strengthSlider;
    lv_obj_t *home_slider_coffeeStrength;
    lv_obj_t *home_cont_1;
    lv_obj_t *home_img_coffee;
    lv_obj_t *home_imgbtn_coffeeNext;
    lv_obj_t *home_imgbtn_coffeeNext_label;
    lv_obj_t *home_imgbtn_coffeePrev;
    lv_obj_t *home_imgbtn_coffeePrev_label;
    lv_obj_t *home_imgbtn_unregister;
    lv_obj_t *home_imgbtn_unregister_label;
    lv_obj_t *brewing;
    lv_obj_t *brewing_cont_background;
    lv_obj_t *brewing_imgbtn_cancel;
    lv_obj_t *brewing_imgbtn_cancel_label;
    lv_obj_t *brewing_cont_brewing;
    lv_obj_t *brewing_bar_fillStatus;
    lv_obj_t *brewing_animimg_brewing;
    lv_obj_t *finished;
    lv_obj_t *finished_cont_background;
    lv_obj_t *finished_img_coffee;
    lv_obj_t *finished_imgbtn_back;
    lv_obj_t *finished_imgbtn_back_label;
    lv_obj_t *standby;
    lv_obj_t *standby_cont_background;
    lv_obj_t *standby_cont_header;
    lv_obj_t *standby_label_coffeeMachine;
    lv_obj_t *standby_cont_coffee;
    lv_obj_t *standby_img_cafeLatte;
    lv_obj_t *standby_img_americano;
    lv_obj_t *standby_img_espresso;
    lv_obj_t *standby_img_cappuccino;
    lv_obj_t *standby_cont_voiceAndFaceRec;
    lv_obj_t *standby_img_nxpFaceRec;
    lv_obj_t *standby_img_nxpVoiceRec;
} lv_ui;

void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_home(lv_ui *ui);
void setup_scr_brewing(lv_ui *ui);

#include "lvgl/src/extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(brewing_animimg_brewingf01)
LV_IMG_DECLARE(brewing_animimg_brewingf02)
LV_IMG_DECLARE(brewing_animimg_brewingf03)
LV_IMG_DECLARE(brewing_animimg_brewingf04)
LV_IMG_DECLARE(brewing_animimg_brewingf05)
LV_IMG_DECLARE(brewing_animimg_brewingf06)
LV_IMG_DECLARE(brewing_animimg_brewingf07)
LV_IMG_DECLARE(brewing_animimg_brewingf08)
LV_IMG_DECLARE(brewing_animimg_brewingf09)
LV_IMG_DECLARE(brewing_animimg_brewingf10)
LV_IMG_DECLARE(brewing_animimg_brewingf11)
LV_IMG_DECLARE(brewing_animimg_brewingf12)
LV_IMG_DECLARE(brewing_animimg_brewingf13)
LV_IMG_DECLARE(brewing_animimg_brewingf14)
LV_IMG_DECLARE(brewing_animimg_brewingf15)
LV_IMG_DECLARE(brewing_animimg_brewingf16)
LV_IMG_DECLARE(brewing_animimg_brewingf17)
LV_IMG_DECLARE(brewing_animimg_brewingf18)
LV_IMG_DECLARE(brewing_animimg_brewingf19)
LV_IMG_DECLARE(brewing_animimg_brewingf20)
LV_IMG_DECLARE(brewing_animimg_brewingf21)
LV_IMG_DECLARE(brewing_animimg_brewingf22)
LV_IMG_DECLARE(brewing_animimg_brewingf23)
LV_IMG_DECLARE(brewing_animimg_brewingf24)
LV_IMG_DECLARE(brewing_animimg_brewingf25)
LV_IMG_DECLARE(brewing_animimg_brewingf26)
LV_IMG_DECLARE(brewing_animimg_brewingf27)
LV_IMG_DECLARE(brewing_animimg_brewingf28)
LV_IMG_DECLARE(brewing_animimg_brewingf29)
LV_IMG_DECLARE(brewing_animimg_brewingf31)
void setup_scr_finished(lv_ui *ui);
void setup_scr_standby(lv_ui *ui);
LV_IMG_DECLARE(_NxpVoiceRec_300x80);
LV_IMG_DECLARE(_Forward_alpha_100x100);
LV_IMG_DECLARE(_Unregister_alpha_75x75);
LV_IMG_DECLARE(_Cancel_alpha_100x100);
LV_IMG_DECLARE(_Start_alpha_200x125);
LV_IMG_DECLARE(_NxpFaceRec_300x80);
LV_IMG_DECLARE(_Americano_250x250);
LV_IMG_DECLARE(_Cappuccino_250x250);
LV_IMG_DECLARE(_Espresso_250x250);
LV_IMG_DECLARE(_Cafe_Latte_250x250);
LV_IMG_DECLARE(_NxpVoiceRec_400x150);
LV_IMG_DECLARE(_NxpFaceRec_400x150);
LV_IMG_DECLARE(_Size_400x200);
LV_IMG_DECLARE(_Strength_400x125);
LV_IMG_DECLARE(_Back_alpha_100x100);
void setup_imgs(unsigned char* base);
#ifdef __cplusplus
}
#endif
#endif