/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2022
 * Generated linker script file for MIMXRT1176xxxxx
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.5.0 [Build 7232] [2022-01-11] on Jan 31, 2022, 12:40:36 PM
 */

MEMORY
{
  /* Define each memory region */
  BOARD_FLASH (rx) : ORIGIN = 0x30000000, LENGTH = 0x4000000 /* 64M bytes (alias Flash) */  
  BOARD_SDRAM (rwx) : ORIGIN = 0x80000000, LENGTH = 0x1000000 /* 16M bytes (alias RAM) */  
  SRAM_DTC_cm4 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x20000 /* 128K bytes (alias RAM2) */  
  SRAM_ITC_cm4 (rwx) : ORIGIN = 0x1ffe0000, LENGTH = 0x20000 /* 128K bytes (alias RAM3) */  
  rpmsg_sh_mem (rwx) : ORIGIN = 0x20360000, LENGTH = 0x20000 /* 128K bytes (alias RAM4) */  
  NCACHE_REGION (rwx) : ORIGIN = 0x81000000, LENGTH = 0x1000000 /* 16M bytes (alias RAM5) */  
}

  /* Define a symbol for the top of each memory region */
  __base_BOARD_FLASH = 0x30000000  ; /* BOARD_FLASH */  
  __base_Flash = 0x30000000 ; /* Flash */  
  __top_BOARD_FLASH = 0x30000000 + 0x4000000 ; /* 64M bytes */  
  __top_Flash = 0x30000000 + 0x4000000 ; /* 64M bytes */  
  __base_BOARD_SDRAM = 0x80000000  ; /* BOARD_SDRAM */  
  __base_RAM = 0x80000000 ; /* RAM */  
  __top_BOARD_SDRAM = 0x80000000 + 0x1000000 ; /* 16M bytes */  
  __top_RAM = 0x80000000 + 0x1000000 ; /* 16M bytes */  
  __base_SRAM_DTC_cm4 = 0x20000000  ; /* SRAM_DTC_cm4 */  
  __base_RAM2 = 0x20000000 ; /* RAM2 */  
  __top_SRAM_DTC_cm4 = 0x20000000 + 0x20000 ; /* 128K bytes */  
  __top_RAM2 = 0x20000000 + 0x20000 ; /* 128K bytes */  
  __base_SRAM_ITC_cm4 = 0x1ffe0000  ; /* SRAM_ITC_cm4 */  
  __base_RAM3 = 0x1ffe0000 ; /* RAM3 */  
  __top_SRAM_ITC_cm4 = 0x1ffe0000 + 0x20000 ; /* 128K bytes */  
  __top_RAM3 = 0x1ffe0000 + 0x20000 ; /* 128K bytes */  
  __base_rpmsg_sh_mem = 0x20360000  ; /* rpmsg_sh_mem */  
  __base_RAM4 = 0x20360000 ; /* RAM4 */  
  __top_rpmsg_sh_mem = 0x20360000 + 0x20000 ; /* 128K bytes */  
  __top_RAM4 = 0x20360000 + 0x20000 ; /* 128K bytes */  
  __base_NCACHE_REGION = 0x81000000  ; /* NCACHE_REGION */  
  __base_RAM5 = 0x81000000 ; /* RAM5 */  
  __top_NCACHE_REGION = 0x81000000 + 0x1000000 ; /* 16M bytes */  
  __top_RAM5 = 0x81000000 + 0x1000000 ; /* 16M bytes */  
