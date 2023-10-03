// Copyright 2019-2023 Great Scott Gadgets <info@greatscottgadgets.com>
// Copyright 2019 Katherine J. Temkin <kate@ktemkin.com>
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
#if ((_BOARD_REVISION_MAJOR_ == 0) && (_BOARD_REVISION_MINOR_ < 3))
const static Pin PROGRAM_BUTTON = {.group = 1, .pin = 22, .mux = 0 };
#elif ((_BOARD_REVISION_MAJOR_ == 0) && (_BOARD_REVISION_MINOR_ < 6))
const static Pin PROGRAM_BUTTON = {.group = 0, .pin = 16, .mux = 0 };
#else
const static Pin PROGRAM_BUTTON = {.group = 0, .pin = 2, .mux = 0 };

// USB switch
const static Pin USB_SWITCH      = {.group = 0, .pin = 6, .mux = 0 };
#endif

// LEDs.
#if ((_BOARD_REVISION_MAJOR_ == 0) && (_BOARD_REVISION_MINOR_ < 3))
const static Pin LED_PIN         = {.group = 0, .pin = 18, .mux = 0 };
#else
const static Pin LED_PIN         = {.group = 0, .pin = 22, .mux = 0 };
#endif

// USB pins
const static Pin PIN_USB_DM      = {.group = 0, .pin = 24, .mux = MUX_PA24G_USB_DM };
const static Pin PIN_USB_DP      = {.group = 0, .pin = 25, .mux = MUX_PA25G_USB_DP };

#endif
