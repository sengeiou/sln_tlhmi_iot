/*
 * Copyright 2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief Framework task declaration.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "board_define.h"
#include "board.h"

#ifndef SH_MEM_TOTAL_SIZE
#define SH_MEM_TOTAL_SIZE 0x20000
#endif

extern unsigned char rpmsg_sh_mem[];

#define APP_SH_MEM_BASE (uint32_t) & rpmsg_sh_mem

#define BOARD_SHMEM_WRITE (APP_SH_MEM_BASE)
#define BOARD_SHMEM_READ  (APP_SH_MEM_BASE + (SH_MEM_TOTAL_SIZE / 2))

#ifndef RES_SHMEM_TOTAL_SIZE
#define RES_SHMEM_TOTAL_SIZE 0x600000
#endif

extern unsigned char res_sh_mem[];
#define APP_RES_SHMEM_BASE (uint32_t) & res_sh_mem
#define APP_LVGL_IMGS_BASE (APP_RES_SHMEM_BASE)
#define APP_LVGL_IMGS_SIZE 0x0052cbc0

#endif /* APP_CONFIG_H */
