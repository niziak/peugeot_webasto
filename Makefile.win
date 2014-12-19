#
# !!!  must ends with backslash and NO SPACE AND END !!!
#
#GCC_BIN_DIR_PREFIX := n:/tools/WinAVR/bin/
#POSIX_UTILS_DIR_PREFIX : = n:/tools/WinAVR/utils/bin/

#GCC_BIN_DIR_PREFIX := n:/tools/avrgcc/bin/
POSIX_UTILS_DIR_PREFIX := n:/tools/WinAVR/utils/bin/

#GCC_BIN_DIR_PREFIX := n:/Arduino/hardware/tools/avr/bin/
#POSIX_UTILS_DIR_PREFIX : = n:/Arduino/hardware/tools/avr/utils/bin/

GCC_BIN_DIR_PREFIX := n:/tools/avr-gcc-4.8_2013-03-06_mingw32/bin/
#POSIX_UTILS_DIR_PREFIX : = n:/tools/avr-gcc-4.8_2013-03-06_mingw32/utils/bin/

#POSIX_UTILS_DIR_PREFIX := n:/tools/msys/bin/

GCC_AVR_SIZE_DIR_PREFIX := tools_win32/

export PATH:=$(patsubst %\,%,$(POSIX_UTILS_DIR_PREFIX));$(PATH)

JOBS ?= $(NUMBER_OF_PROCESSORS) 

AVRDUDE_PORT = COM9
AVRDUDE = tools_win32/avrdude.exe