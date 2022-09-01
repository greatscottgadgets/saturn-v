/*
 * Copyright (c) 2019 Great Scott Gadgets <info@greatscottgadgets.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include "common/util.h"
#include "common/hw.h"

#include <board.h>

// Memory Layout
// - first 4k reserved for DAFU Bootloader
// - remainder of flash for main firmware
//
#define FLASH_BOOT_START 	0
#ifndef FLASH_BOOT_SIZE
#define FLASH_BOOT_SIZE 	4096
#endif

// Calcuated at runtime, based on chip's report of it's size.
extern uint32_t 			total_flash_size;

#define FLASH_FW_START 		FLASH_BOOT_SIZE
#define FLASH_FW_SIZE 		(total_flash_size - FLASH_BOOT_SIZE)

#define FLASH_BOOT_ADDR 	FLASH_BOOT_START
#define FLASH_FW_ADDR 		FLASH_FW_START

#define BOOT_MAGIC 			0
