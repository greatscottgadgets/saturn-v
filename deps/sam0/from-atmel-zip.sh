#!/bin/bash
set -e

# Usage:
#  wget http://www.atmel.com/images/asf-standalone-archive-3.20.1.101.zip
#  ./from-atmel-zip.sh asf-standalone-archive-3.20.1.101.zip

DIR=xdk-asf-*

rm -rf extract
unzip "$1" "$DIR/sam0/*" "$DIR/thirdparty/CMSIS/Include/*" "$DIR/common/utils/*" \
      -d extract -x "*/unit_test*" "*/quick_start*" "*/docimg*" "*/iar/*"

rm -rf drivers
mv extract/$DIR/sam0/drivers .
mv extract/$DIR/common/utils/interrupt/interrupt_sam_nvic.{c,h} drivers/system/interrupt/

rm -rf cmsis
mv extract/$DIR/sam0/utils/cmsis .
mv extract/$DIR/thirdparty/CMSIS/Include/{core_cm0plus.h,core_cmFunc.h,core_cmInstr.h} cmsis

rm -rf include
mkdir include
mv extract/$DIR/sam0/utils/{preprocessor/*,header_files/io.h,compiler.h,status_codes.h} include
mv extract/$DIR/common/utils/{interrupt,parts}.h include

rm -rf linker_scripts
mv extract/$DIR/sam0/utils/linker_scripts .

rm -rf extract
find ./* -type f -exec dos2unix {} \;
git add drivers cmsis include linker_scripts
