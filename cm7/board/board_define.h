/*
 * Copyright 2020-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief board-related macro definitions. Please place each required definition here before compiling.
 */

#ifndef BOARD_DEFINE_H_
#define BOARD_DEFINE_H_

/*
 *  Board define
 */
#define BOARD_1170EVK     0
#define BOARD_SMART_TLHMI 1

#define TARGET_BOARD BOARD_SMART_TLHMI

/*
 * Enablement of the HAL devices
 */
//#define ENABLE_VISIONALGO_DEV_Benchmark
#define ENABLE_INPUT_DEV_PdmMic
#define ENABLE_AUDIO_PROCESSING_DEV_Afe
#define ENABLE_VOICEALGO_Asr
#define ENABLE_OUTPUT_DEV_MqsAudio
#if defined(ENABLE_MASTER) && ENABLE_MASTER
#define ENABLE_MULTICORE_DEV_MessageBuffer
#endif /* defined(ENABLE_MASTER) && ENABLE_MASTER */

//#define ENABLE_CAMERA_DEV_2DSim
#define ENABLE_VISIONALGO_DEV_Oasis_CoffeeMachine

#define ENABLE_FLASH_DEV_Littlefs
#define ENABLE_FACEDB
/* Memory regions definitions */
#define AT_NONCACHEABLE_SECTION_ALIGN_DTC(var, alignbytes) \
    __attribute__((section(".bss.$SRAM_DTC_cm7,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_CACHEABLE_SECTION_ALIGN_OCRAM(var, alignbytes) \
    __attribute__((section(".bss.$SRAM_OCRAM_CACHED,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION_ALIGN_OCRAM(var, alignbytes) \
    __attribute__((section(".bss.$SRAM_OCRAM_NCACHED,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_SHMEM_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section(".bss.$rpmsg_sh_mem,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_FB_SHMEM_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section(".bss.$fb_sh_mem,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_RES_SHMEM_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section(".noinit.$res_sh_mem,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))

/* App task ID offset definition here. App task ID will start from kFWKTaskID_APPStart */
#define APP_TASK_ID(n)    (kFWKTaskID_APPStart + n)
#define MQS_AUDIO_TASK_ID (APP_TASK_ID(1))

#endif /* BOARD_DEFINE_H_ */
