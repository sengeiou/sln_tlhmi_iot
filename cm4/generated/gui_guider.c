/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"

lv_img_dsc_t brewing_animimg_brewingf01 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf02 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf03 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf04 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf05 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf06 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf07 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf08 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf09 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf10 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf11 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf12 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf13 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf14 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf15 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf16 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf17 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf18 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf19 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf20 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf21 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf22 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf23 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf24 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf25 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf26 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf27 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf28 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf29 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t brewing_animimg_brewingf31 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 200,
    .data_size          = 40000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _NxpVoiceRec_300x80 = {
    .header.always_zero = 0,
    .header.w           = 300,
    .header.h           = 80,
    .data_size          = 24000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Forward_alpha_100x100 = {
    .header.always_zero = 0,
    .header.w           = 100,
    .header.h           = 100,
    .data_size          = 10000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Unregister_alpha_75x75 = {
    .header.always_zero = 0,
    .header.w           = 75,
    .header.h           = 75,
    .data_size          = 5625 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Cancel_alpha_100x100 = {
    .header.always_zero = 0,
    .header.w           = 100,
    .header.h           = 100,
    .data_size          = 10000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Start_alpha_200x125 = {
    .header.always_zero = 0,
    .header.w           = 200,
    .header.h           = 125,
    .data_size          = 25000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _NxpFaceRec_300x80 = {
    .header.always_zero = 0,
    .header.w           = 300,
    .header.h           = 80,
    .data_size          = 24000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Americano_250x250 = {
    .header.always_zero = 0,
    .header.w           = 250,
    .header.h           = 250,
    .data_size          = 62500 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Cappuccino_250x250 = {
    .header.always_zero = 0,
    .header.w           = 250,
    .header.h           = 250,
    .data_size          = 62500 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Espresso_250x250 = {
    .header.always_zero = 0,
    .header.w           = 250,
    .header.h           = 250,
    .data_size          = 62500 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Cafe_Latte_250x250 = {
    .header.always_zero = 0,
    .header.w           = 250,
    .header.h           = 250,
    .data_size          = 62500 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _NxpVoiceRec_400x150 = {
    .header.always_zero = 0,
    .header.w           = 400,
    .header.h           = 150,
    .data_size          = 60000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _NxpFaceRec_400x150 = {
    .header.always_zero = 0,
    .header.w           = 400,
    .header.h           = 150,
    .data_size          = 60000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Size_400x200 = {
    .header.always_zero = 0,
    .header.w           = 400,
    .header.h           = 200,
    .data_size          = 80000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Strength_400x125 = {
    .header.always_zero = 0,
    .header.w           = 400,
    .header.h           = 125,
    .data_size          = 50000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

lv_img_dsc_t _Back_alpha_100x100 = {
    .header.always_zero = 0,
    .header.w           = 100,
    .header.h           = 100,
    .data_size          = 10000 * LV_COLOR_SIZE / 8,
    .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
};

void setup_imgs(unsigned char *base)
{
    brewing_animimg_brewingf01.data = (base + 0);
    brewing_animimg_brewingf02.data = (base + 120000);
    brewing_animimg_brewingf03.data = (base + 240000);
    brewing_animimg_brewingf04.data = (base + 360000);
    brewing_animimg_brewingf05.data = (base + 480000);
    brewing_animimg_brewingf06.data = (base + 600000);
    brewing_animimg_brewingf07.data = (base + 720000);
    brewing_animimg_brewingf08.data = (base + 840000);
    brewing_animimg_brewingf09.data = (base + 960000);
    brewing_animimg_brewingf10.data = (base + 1080000);
    brewing_animimg_brewingf11.data = (base + 1200000);
    brewing_animimg_brewingf12.data = (base + 1320000);
    brewing_animimg_brewingf13.data = (base + 1440000);
    brewing_animimg_brewingf14.data = (base + 1560000);
    brewing_animimg_brewingf15.data = (base + 1680000);
    brewing_animimg_brewingf16.data = (base + 1800000);
    brewing_animimg_brewingf17.data = (base + 1920000);
    brewing_animimg_brewingf18.data = (base + 2040000);
    brewing_animimg_brewingf19.data = (base + 2160000);
    brewing_animimg_brewingf20.data = (base + 2280000);
    brewing_animimg_brewingf21.data = (base + 2400000);
    brewing_animimg_brewingf22.data = (base + 2520000);
    brewing_animimg_brewingf23.data = (base + 2640000);
    brewing_animimg_brewingf24.data = (base + 2760000);
    brewing_animimg_brewingf25.data = (base + 2880000);
    brewing_animimg_brewingf26.data = (base + 3000000);
    brewing_animimg_brewingf27.data = (base + 3120000);
    brewing_animimg_brewingf28.data = (base + 3240000);
    brewing_animimg_brewingf29.data = (base + 3360000);
    brewing_animimg_brewingf31.data = (base + 3480000);
    _NxpVoiceRec_300x80.data        = (base + 3600000);
    _Forward_alpha_100x100.data     = (base + 3672000);
    _Unregister_alpha_75x75.data    = (base + 3702016);
    _Cancel_alpha_100x100.data      = (base + 3718912);
    _Start_alpha_200x125.data       = (base + 3748928);
    _NxpFaceRec_300x80.data         = (base + 3823936);
    _Americano_250x250.data         = (base + 3895936);
    _Cappuccino_250x250.data        = (base + 4083456);
    _Espresso_250x250.data          = (base + 4270976);
    _Cafe_Latte_250x250.data        = (base + 4458496);
    _NxpVoiceRec_400x150.data       = (base + 4646016);
    _NxpFaceRec_400x150.data        = (base + 4826048);
    _Size_400x200.data              = (base + 5006080);
    _Strength_400x125.data          = (base + 5246080);
    _Back_alpha_100x100.data        = (base + 5396096);
}

void setup_ui(lv_ui *ui)
{
    setup_scr_standby(ui);
    lv_scr_load(ui->standby);
}
