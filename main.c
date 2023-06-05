// Copyright 2019 Katherine J. Temkin <kate@ktemkin.com>
// Copyright 2019 Great Scott Gadgets <ktemkin@greatscottgadgets.com>
// Copyright 2014 Technical Machine, Inc. See the COPYRIGHT
// file at the top-level directory of this distribution.
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#include "common/util.h"
#include "samd/usb_samd.h"

#include <string.h>
#include <stdbool.h>

#include "boot.h"
#include "common/nvm.h"

#include <board.h>

volatile bool exit_and_jump = 0;

void delay_ms(unsigned ms) {
	/* Approximate ms delay using dummy clock cycles of 48 MHz clock */
	for (unsigned i = 0; i < ms * (48000000 / 1000 / 5); ++i) {
		__NOP();
	}
}

/*** USB / DFU ***/

void dfu_cb_dnload_block(uint16_t block_num, uint16_t len) {
	if (usb_setup.wLength > DFU_TRANSFER_SIZE) {
		dfu_error(DFU_STATUS_errUNKNOWN);
		return;
	}

	if (block_num * DFU_TRANSFER_SIZE > FLASH_FW_SIZE) {
		dfu_error(DFU_STATUS_errADDRESS);
		return;
	}

	nvm_erase_row(FLASH_FW_START + block_num * DFU_TRANSFER_SIZE);
}

void dfu_cb_dnload_packet_completed(uint16_t block_num, uint16_t offset, uint8_t* data, uint16_t length) {
	unsigned addr = FLASH_FW_START + block_num * DFU_TRANSFER_SIZE + offset;
	nvm_write_page(addr, data, length);
}

unsigned dfu_cb_dnload_block_completed(uint16_t block_num, uint16_t length) {
	return 0;
}

void dfu_cb_manifest(void) {
	exit_and_jump = 1;
}

void noopFunction(void)
{
	// Placeholder function for code that isn't needed. Keep empty!
}

void bootloader_main(void)
{
	// Set up the LED that indicates we're in bootloader mode.
	pin_out(LED_PIN);

	// Set up the main clocks.
	clock_init_usb(GCLK_SYSTEM);
	nvm_init();

	__enable_irq();

	pin_mux(PIN_USB_DM);
	pin_mux(PIN_USB_DP);
	usb_init();
	usb_attach();

	// Blink while we're in DFU mode.
	while(!exit_and_jump) {
		pin_toggle(LED_PIN);
		delay_ms(300);
	}

	usb_detach();
	nvm_invalidate_cache();

	// Hook: undo any special setup that board_setup_late might be needed to
	// undo the setup the bootloader code has done.
	NVIC_SystemReset();
}

bool flash_valid() {
	unsigned sp = ((unsigned *)FLASH_FW_ADDR)[0];
	unsigned ip = ((unsigned *)FLASH_FW_ADDR)[1];

	return     sp > 0x20000000
			&& ip >= 0x00001000
			&& ip <  0x00400000;
}

bool bootloader_sw_triggered(void)
{
	// Was reset caused by watchdog timer (WDT)?
	return PM->RCAUSE.reg & PM_RCAUSE_WDT;
}


bool button_pressed(void)
{
	pin_pull_up(RECOVERY_BUTTON);


	// Drop into Recovery Mode if the recovery button is presssed.
	if (pin_read(RECOVERY_BUTTON) == 0) {
		return true;
	}

	return false;
}




void main_bl(void) {
	if (!flash_valid() || button_pressed() || bootloader_sw_triggered()) {
		bootloader_main();
	}

	jump_to_flash(FLASH_FW_ADDR, 0);
}
