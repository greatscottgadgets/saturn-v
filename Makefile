#
# Saturn-V Makefile
#
# Targets:
#	all - make everything, look for bootloader.elf in particular
#	clean - delete intermediates
#	clobber - delete all build products
#
#
#

BOARD ?= cynthion

# These should default to the latest hardware revision but can be set on the command line.
BOARD_REVISION_MAJOR ?= 1
BOARD_REVISION_MINOR ?= 3

VERSION_MAJOR = 1
VERSION_MINOR = 0

CROSS=arm-none-eabi-

# Toolchain
CC      = $(CROSS)gcc
OBJDUMP = $(CROSS)objdump
OBJCOPY = $(CROSS)objcopy
NM      = $(CROSS)nm
SIZE    = $(CROSS)size

# Basename of targets
TARGET_NAME = bootloader

# Per-board configuration.
include boards/$(BOARD)/board.mk

PRODUCT_STRING ?= "Bootloader"

# Compiler flags.
CFLAGS = -Wall --std=gnu99 -Os -g3 -nostartfiles -fno-builtin \
			-flto -fdata-sections -ffunction-sections -funsigned-char -funsigned-bitfields \
			-mcpu=cortex-m0plus -mthumb -D __$(PART)__ -I . -Iboards/$(BOARD)

# USB PID/VID and other branding values
CFLAGS += \
			-D USB_PRODUCT_ID=0x615c \
			-D USB_VENDOR_ID=0x1d50 \
			-D USB_MANUFACTURER_STR='"Saturn-V Project"' \
			-D USB_PRODUCT_STR='$(PRODUCT_STRING)' \
			-D_BOARD_REVISION_MAJOR_=$(BOARD_REVISION_MAJOR) \
			-D_BOARD_REVISION_MINOR_=$(BOARD_REVISION_MINOR) \
			-D VERSION_MAJOR=$(VERSION_MAJOR) \
			-D VERSION_MINOR=$(VERSION_MINOR)

# Header file search path
PRJ_PATH = deps
INC_PATHS = \
			usb \
			sam0/cmsis \
			sam0/include \
			sam0/cmsis/samd21/include \
			sam0/cmsis/samd21/source \

CFLAGS += $(foreach INC,$(addprefix $(PRJ_PATH)/,$(INC_PATHS)),-I$(INC))

# Specialized linker-script here. Not the standard one!
#
LINKER_SCRIPT = link-script.ld

LDFLAGS += -flto -Wl,--gc-sections --specs=nano.specs -Wl,-T$(LINKER_SCRIPT)

C_SRCS = \
			common/startup_samd21.c \
			main.c \
			usb.c \
			common/clock.c \
			deps/usb/class/dfu/dfu.c \
			deps/usb/samd/usb_samd.c \
			deps/usb/usb_requests.c

OBJS = $(addsuffix .o, $(basename $(C_SRCS) $(ASM_SRCS)))

TARGET_ELF = $(TARGET_NAME).elf
TARGETS = $(TARGET_NAME).hex $(TARGET_NAME).lss $(TARGET_NAME).bin $(TARGET_NAME).sym $(TARGET_NAME).o

all: $(TARGETS)

# recompile on any change, because with a small project like this...
$(OBJS): Makefile $(C_SRCS) $(ASM_SRCS)

$(TARGETS): $(TARGET_ELF) Makefile

# link step
$(TARGET_ELF): $(OBJS) $(LINKER_SCRIPT) Makefile
	$(CC) $(CFLAGS) -o $(TARGET_ELF) $(LDFLAGS) $(OBJS)
	$(SIZE) -Ax $@

# detailed listing, very handy
%.lss: $(TARGET_ELF)
	$(OBJDUMP) -h -S $< > $@

# symbol dump, meh
%.sym: $(TARGET_ELF)
	$(NM) -n $< > $@

# intel HEX format
%.hex: $(TARGET_ELF)
	$(OBJCOPY) -O ihex  $< $@

# raw binary
%.bin: $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

# a binary of just the ROM area, ready to be linked into another program.
$(TARGET_NAME).o: $(TARGET_ELF)
	$(OBJCOPY) --rename-section .text=.vectors.bootloader \
				--prefix-symbols=dafu_ \
				--pad-to 0x1000 --gap-fill 0xff \
				$(TARGET_ELF) $@
	$(OBJDUMP) -h $@
	$(SIZE) -Ax $@

clean:
	$(RM) $(OBJS)

clobber: clean
	$(RM) $(TARGETS)

debug:
	@echo CFLAGS = $(CFLAGS)
	@echo
	@echo C_SRCS = $(C_SRCS)
	@echo
	@echo OBJS = $(OBJS)

tags:
	ctags -f .tags *.[ch] -R deps/sam0 deps/usb common
