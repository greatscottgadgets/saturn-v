# Saturn-V

## A DFU Bootloader
_for [Cynthion](https://greatscottgadgets.com/cynthion/) and similar SAMD11 and SAMD21 boards_

Based on [opendime/DAFU/](https://github.com/opendime/) and [t2-firmware/boot/](https://github.com/tessel/t2-firmware).

Compatible with [DFU Utils](http://dfu-util.sourceforge.net/) and [pyfwup](http://github.com/usb-tools/pyfwup).

## Background

Saturn-V is the Device Firmware Upgrade (DFU) bootloader for Cynthion. It is used to load [Apollo](https://github.com/greatscottgadgets/apollo) firmware over USB onto Cynthion's on-board debugger. Alternatively it may be used as a bootloader for other devices featuring a SAMD11 or SAMD21 microcontroller.

Saturn-V uses only 2 KiB of flash memory, leaving plenty of space for application firmware. Space optimization in Saturn-V was achieved using some of the tricks in [SAMDx1-USB-DFU-Bootloader](https://github.com/majbthrd/SAMDx1-USB-DFU-Bootloader) which is even smaller at 1 KiB but lacks features such as [Microsoft-compatible descriptors](https://github.com/pbatard/libwdi/wiki/WCID-Devices).

## Use

To invoke Saturn-V on Cynthion, hold down the PROGRAM button while connecting power or while pressing and releasing the RESET button. LED C will blink, indicating that Saturn-V is running. In this mode you can update Apollo with Apollo's `make dfu` command or use `dfu-util` or another DFU utility.

## Building

Normally Saturn-V needs to be installed only once on a newly assembled board.

To compile for the latest Cynthion hardware revision, type:

```
$ cd saturn-v
$ make
```

Alternatively, use variables to specify the board and/or hardware revision:

```
$ cd saturn-v
$ make BOARD=cynthion BOARD_REVISION_MAJOR=1 BOARD_REVISION_MINOR=3
```

If you're not using the `arm-none-eabi-` toolchain, you'll need to specify your compiler prefix using the `CROSS` variable.

Once the bootloader has been built, use an SWD programmer to load the `bootloader.elf` file, or program `bootloader.bin` to the start of flash (address `0x00000000`). If you're using the Black Magic Probe, this might look like:

```
$ arm-none-eabi-gdb -nx --batch \
    -ex 'target extended-remote /dev/ttyACM0' \
    -ex 'monitor unlock_bootprot' \
    -ex 'monitor swdp_scan' \
    -ex 'attach 1' \
    -ex 'load' \
    -ex 'monitor lock_bootprot 4' \
    -ex 'kill' \
    bootloader.elf
```
