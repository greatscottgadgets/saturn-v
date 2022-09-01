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

#ifndef __BOARD_H__
#define __BOARD_H__

// Buttons.
const static Pin RECOVERY_BUTTON = {.group = 0, .pin = 00, .mux = 0 }; // unused on QT Py

// LEDs.
const static Pin LED_PIN         = {.group = 0, .pin = 01, .mux = 0 }; // unused on QT Py

// USB pins
const static Pin PIN_USB_DM      = {.group = 0, .pin = 24, .mux = MUX_PA24G_USB_DM };
const static Pin PIN_USB_DP      = {.group = 0, .pin = 25, .mux = MUX_PA25G_USB_DP };

// the size of the original uf2 bootlaoder is 8k
// thus firmware expects to start at 8k
#define FLASH_BOOT_SIZE 8192

#endif
