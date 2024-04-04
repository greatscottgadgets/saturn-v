// Copyright 2015 Tessel See the COPYRIGHT
// file at the top-level directory of this distribution.
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#include "boot.h"
#include "usb.h"
#include "samd/usb_samd.h"

// Return a string that contains a unique serial number for this board.
char* get_serial_number_string();

USB_ENDPOINTS(1);

#define BCD_ONES(x)     (x % 10)
#define BCD_TENS(x)     ((x / 10) % 10)
#define BCDDEVICE(x, y) (BCD_TENS(x)<<12 | BCD_ONES(x)<<8 | BCD_TENS(y)<<4 | BCD_ONES(y))

const USB_DeviceDescriptor device_descriptor = {
	.bLength = sizeof(USB_DeviceDescriptor),
	.bDescriptorType = USB_DTYPE_Device,

	.bcdUSB                 = 0x0200,
	.bDeviceClass           = 0,
	.bDeviceSubClass        = USB_CSCP_NoDeviceSubclass,
	.bDeviceProtocol        = USB_CSCP_NoDeviceProtocol,

	.bMaxPacketSize0        = 64,
	.idVendor               = 0x1d50,
	.idProduct              = 0x615c,
	.bcdDevice              = BCDDEVICE(VERSION_MAJOR, VERSION_MINOR),

	.iManufacturer          = 0x01,
	.iProduct               = 0x02,
	.iSerialNumber          = 0x03,

	.bNumConfigurations     = 1
};

typedef struct ConfigDesc {
	USB_ConfigurationDescriptor Config;
	USB_InterfaceDescriptor dfu_intf_flash;
	DFU_FunctionalDescriptor dfu_desc_flash;
} ConfigDesc;

const ConfigDesc configuration_descriptor = {
	.Config = {
		.bLength = sizeof(USB_ConfigurationDescriptor),
		.bDescriptorType = USB_DTYPE_Configuration,
		.wTotalLength  = sizeof(ConfigDesc),
		.bNumInterfaces = 1,
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		.bmAttributes = USB_CONFIG_ATTR_BUSPOWERED,
		.bMaxPower = USB_CONFIG_POWER_MA(500)
	},
	.dfu_intf_flash = {
		.bLength = sizeof(USB_InterfaceDescriptor),
		.bDescriptorType = USB_DTYPE_Interface,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 0,
		.bInterfaceClass = DFU_INTERFACE_CLASS,
		.bInterfaceSubClass = DFU_INTERFACE_SUBCLASS,
		.bInterfaceProtocol = DFU_INTERFACE_PROTOCOL,
		.iInterface = 0x02
	},
	.dfu_desc_flash = {
		.bLength = sizeof(DFU_FunctionalDescriptor),
		.bDescriptorType = DFU_DESCRIPTOR_TYPE,
		.bmAttributes = DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_WILL_DETACH,
		.wDetachTimeout = 0,
		.wTransferSize = DFU_TRANSFER_SIZE,
		.bcdDFUVersion = 0x0101,
	},
};

const USB_StringDescriptor language_string = {
	.bLength = USB_STRING_LEN(1),
	.bDescriptorType = USB_DTYPE_String,
	.bString = {USB_LANGUAGE_EN_US},
};

const USB_MicrosoftCompatibleDescriptor msft_compatible = {
	.dwLength = sizeof(USB_MicrosoftCompatibleDescriptor) + sizeof(USB_MicrosoftCompatibleDescriptor_Interface),
	.bcdVersion = 0x0100,
	.wIndex = 0x0004,
	.bCount = 1,
	.reserved = {0, 0, 0, 0, 0, 0, 0},
	.interfaces = {
		{
			.bFirstInterfaceNumber = 0,
			.reserved1 = 0,
			.compatibleID = "WINUSB\0\0",
			.subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
			.reserved2 = {0, 0, 0, 0, 0, 0},
		}
	}
};


void* str_to_descriptor(char* str, uint8_t len) {
	USB_StringDescriptor* desc = (((USB_StringDescriptor*)ep0_buf_in));
	desc->bLength = USB_STRING_LEN(len);
	desc->bDescriptorType = USB_DTYPE_String;
	for (int i=0; i<len; i++) {
		desc->bString[i] = str[i];
	}
	return desc;
}

#define STRLEN(x) ((sizeof(x)/sizeof(x[0])) - 1)

uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** ptr) {
	const void* address = NULL;
	uint16_t size    = 0;

	switch (type) {
		case USB_DTYPE_Device:
			address = &device_descriptor;
			size    = sizeof(USB_DeviceDescriptor);
			break;
		case USB_DTYPE_Configuration:
			address = &configuration_descriptor;
			size    = sizeof(ConfigDesc);
			break;
		case USB_DTYPE_String: {
			char *string = NULL;
			uint8_t string_len;
			switch (index) {
				case 0x00:
					address = &language_string;
					size = language_string.bLength;
					goto _exit;
				case 0x01:
					string = USB_MANUFACTURER_STR;
					string_len = STRLEN(USB_MANUFACTURER_STR);
					break;
				case 0x02:
					string = USB_PRODUCT_STR;
					string_len = STRLEN(USB_PRODUCT_STR);
					break;
				case 0x03:
					string = get_serial_number_string();
					string_len = 26;
					break;
				case 0xee:
					string = "MSFT100\xee";
					string_len = 8;
					break;
				default:
					goto _exit;
			}
			address = str_to_descriptor(string, string_len);
			size = (((USB_StringDescriptor*)address))->bLength;
			break;
		}
	}

	_exit:
	*ptr = address;
	return size;
}

void usb_cb_reset(void) {
}

bool usb_cb_set_configuration(uint8_t config) {
	if (config <= 1) {
		return true;
	}
	return false;
}

void usb_cb_control_setup(void) {
	uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;
	if (recipient == USB_RECIPIENT_DEVICE) {
		if (usb_setup.bRequest == 0xee) {
			return usb_handle_msft_compatible(&msft_compatible);
		}
	} else if (recipient == USB_RECIPIENT_INTERFACE) {
		if (usb_setup.wIndex == DFU_INTF) {
			return dfu_control_setup();
		}
	}
	return usb_ep0_stall();
}

void usb_cb_control_in_completion(void) {
	uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;
	if (recipient == USB_RECIPIENT_INTERFACE) {
		if (usb_setup.wIndex == DFU_INTF) {
			dfu_control_in_completion();
		}
	}
}

void usb_cb_control_out_completion(void) {
	uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;
	if (recipient == USB_RECIPIENT_INTERFACE) {
		if (usb_setup.wIndex == DFU_INTF) {
			dfu_control_out_completion();
		}
	}
}

void usb_cb_completion(void) {

}

bool usb_cb_set_interface(uint16_t interface, uint16_t altsetting) {
	if (interface == DFU_INTF) {
		if (altsetting == 0) {
			dfu_reset();
			return true;
		}
	}
	return false;
}

/**
 * Returns a string that describes this device's unique ID.
 */
char *get_serial_number_string(void)
{
	// Documented in section 9.3.3 of D21 datasheet, page 32 (rev G), but no header file,
	// these are not contiguous addresses.
	const uint32_t	*ser[4] = {
		(uint32_t *)0x0080A00C,
		(uint32_t *)0x0080A040,
		(uint32_t *)0x0080A044,
		(uint32_t *)0x0080A048
	};

	uint32_t copy[5];
	copy[0] = *ser[0];
	copy[1] = *ser[1];
	copy[2] = *ser[2];
	copy[3] = *ser[3];
	copy[4] = 0;

	uint8_t *tmp = (uint8_t *)copy;

    int next = 1;
	int buffer = tmp[0];
    int bitsLeft = 8;

	static char buf[27] = {0};

	for (int count = 0; count < 26; ++count) {
		if (bitsLeft < 5) {
			buffer <<= 8;
			buffer |= tmp[next++] & 0xff;
			bitsLeft += 8;
		}
		bitsLeft -= 5;
		int index = (buffer >> bitsLeft) & 0x1f;
		buf[count] = index + (index < 26 ? 'A' : '2');  // Base32
	}

	return buf;

}
