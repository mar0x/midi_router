
# Path to top level ASF directory relative to this project directory.
ASF = asf-3.50.0
PRJ_PATH = ../..

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = $(BOARD)-$(MCU).elf

# List of C source files.
CSRCS = \
       $(ASF)/common/services/clock/xmega/sysclk.c               \
       $(ASF)/common/services/sleepmgr/xmega/sleepmgr.c          \
       $(ASF)/common/services/usb/class/cdc/device/udi_cdc.c     \
       $(ASF)/common/services/usb/class/composite/device/udi_composite_desc.c \
       $(ASF)/common/services/usb/udc/udc.c                      \
       $(ASF)/xmega/drivers/nvm/nvm.c                            \
       $(ASF)/xmega/drivers/usb/usb_device.c                     \
       $(BOARD)/ui.cpp                                           \
       $(BOARD)/midi.cpp                                         \
       src/main.cpp                                              \
       src/timer.cpp                                             \

# List of assembler source files.
ASSRCS = \
       $(ASF)/xmega/drivers/cpu/ccp.s                            \
       $(ASF)/xmega/drivers/nvm/nvm_asm.s

# List of include paths.
INC_PATH = \
       $(ASF)/common/services/clock                              \
       $(ASF)/common/services/sleepmgr                           \
       $(ASF)/common/services/usb                                \
       $(ASF)/common/services/usb/class/cdc                      \
       $(ASF)/common/services/usb/class/cdc/device               \
       $(ASF)/common/services/usb/class/composite/device         \
       $(ASF)/common/services/usb/udc                            \
       $(ASF)/common/utils                                       \
       $(ASF)/xmega/drivers/cpu                                  \
       $(ASF)/xmega/drivers/nvm                                  \
       $(ASF)/xmega/drivers/pmic                                 \
       $(ASF)/xmega/drivers/sleep                                \
       $(ASF)/xmega/drivers/usb                                  \
       $(ASF)/xmega/utils                                        \
       $(ASF)/xmega/utils/preprocessor                           \
       $(BOARD)                                                  \
       src/usb                                                   \
       src

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
#LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3b/0256/gcc/link_uc3b0256.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -Os

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS = 

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
        -D F_CPU=24000000L                              \
        -D IOPORT_XMEGA_COMPAT

CXXFLAGS = \
        -std=gnu++11 \
        -fpermissive \
        -fno-exceptions \
        -fno-threadsafe-statics \

# Extra flags to use when linking
LDFLAGS = \
        -Wl,--section-start=.BOOT=0x8000

#       -nostartfiles -Wl,-e,_trampoline

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 
