# List of available make goals:
#
# all                     Default goal: build the project
# clean                   Clean up the project
# rebuild                 Rebuild the project
#
# doc                     Build the documentation
# cleandoc                Clean up the documentation
# rebuilddoc              Rebuild the documentation
#
#
# Copyright (c) 2010 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Include the common Makefile, which will also include the project specific
# config.mk file.
MAKEFILE_PATH = ../../../../../../../../../../common/utils/make/Makefile.avr.in
include $(MAKEFILE_PATH)

LOCAL_PACKAGES = $(HOME)/Library/Arduino15/packages

# AVR_GCC_PATH = $(ARDUINO_ROOT)/hardware/tools/avr
# AVR_GCC_PATH = /usr/local/CrossPack-AVR-20131216
AVR_GCC_PATH = $(HOME)/projects/toolchain-avr/objdir

ifneq ("$(wildcard $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino17)", "")
# AVRDUDE_PATH = $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino14
AVRDUDE_PATH = $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino17
else
AVRDUDE_PATH = $(ARDUINO_ROOT)/hardware/tools/avr
endif

AVRDUDE = $(AVRDUDE_PATH)/bin/avrdude
AVRDUDE_CONF = $(AVRDUDE_PATH)/etc/avrdude.conf

upload: $(project).hex
	$(AVRDUDE) -v \
    -C $(AVRDUDE_CONF) \
    -c avrispmkII \
    -p $(MCU) \
    -P usb \
    -U application:w:$<:i \
    -e

test:
	$(AVRDUDE) -v -n \
    -C $(AVRDUDE_CONF) \
    -c avrispmkII \
    -p $(MCU) \
    -P usb
