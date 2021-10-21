
LOCAL_PACKAGES = $(HOME)/Library/Arduino15/packages

# AVR_GCC_PATH = $(ARDUINO_ROOT)/hardware/tools/avr
# AVR_GCC_PATH = /usr/local/CrossPack-AVR-20131216
AVR_GCC_PATH = $(HOME)/projects/toolchain-avr/objdir

ifneq ("$(wildcard $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino17)", "")
AVRDUDE_PATH = $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino17
else
AVRDUDE_PATH = $(ARDUINO_ROOT)/hardware/tools/avr
endif

AVRDUDE = $(AVRDUDE_PATH)/bin/avrdude
AVRDUDE_CONF = $(AVRDUDE_PATH)/etc/avrdude.conf

upload-flip2: $(project).hex
	$(AVRDUDE) -v \
    -C $(AVRDUDE_CONF) \
    -c flip2 \
    -p $(MCU) \
    -P usb \
    -U application:w:$<:i \

upload-mk2: $(project).hex
	$(AVRDUDE) -v \
    -C $(AVRDUDE_CONF) \
    -c avrispmkII \
    -p $(MCU) \
    -P usb \
    -U application:w:$<:i \

upload-test:
	$(AVRDUDE) -v -n \
    -C $(AVRDUDE_CONF) \
    -c avrispmkII \
    -p $(MCU) \
    -P usb

DFU = $(PRJ_PATH)/bin/dfu-$(MCU)-$(BOARD).ihex

upload-dfu:
	$(AVRDUDE) -v \
    -C $(AVRDUDE_CONF) \
    -c avrispmkII \
    -p $(MCU) \
    -P usb \
    -U flash:w:$(DFU):i \
    -U fuse2:w:0xbf:m

