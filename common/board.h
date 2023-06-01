/*
 * Copyright (c) 2019 Great Scott Gadgets <info@greatscottgadgets.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include "common/util.h"
#include "common/hw.h"

// Memory Layout
// - first 4k reserved for DAFU Bootloader
// - remainder of flash for main firmware
//
#define FLASH_BOOT_START 	0
#define FLASH_BOOT_SIZE 	4096

#define FLASH_FW_START 		FLASH_BOOT_SIZE
#define FLASH_FW_SIZE 		(FLASH_SIZE - FLASH_BOOT_SIZE)

#define FLASH_BOOT_ADDR 	FLASH_BOOT_START
#define FLASH_FW_ADDR 		FLASH_FW_START

#define BOOT_MAGIC 			0
