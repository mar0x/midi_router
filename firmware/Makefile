.PHONY: all clean

MAKEFILE_PATH = asf-3.50.0/common/utils/make/Makefile.avr.in

LOCAL_PACKAGES = $(HOME)/Library/Arduino15/packages
AVR_GCC_PATH = $(HOME)/projects/toolchain-avr/objdir

ifneq ("$(wildcard $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino17)", "")
AVRDUDE_PATH = $(LOCAL_PACKAGES)/arduino/tools/avrdude/6.3.0-arduino17
else
AVRDUDE_PATH = $(ARDUINO_ROOT)/hardware/tools/avr
endif

AVRDUDE_CONF = $(AVRDUDE_PATH)/etc/avrdude.conf
AVRDUDE = $(AVRDUDE_PATH)/bin/avrdude -v -C $(AVRDUDE_CONF) -P usb


define BOARD_MCU_template
.PHONY: $(1) $(1)-$(2)
all: $(1)
$(1): $(1)-$(2)

$(1)-$(2) $(1)/$(2)/$(1)-$(2).hex:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2)


.PHONY: $(1)-clean $(1)-$(2)-clean
clean: $(1)-clean

$(1)-clean: $(1)-$(2)-clean

$(1)-$(2)-clean:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) clean


.PHONY: $(1)-upload-flip2 $(1)-$(2)-upload-flip2
$(1)-upload-flip2: $(1)-$(2)-upload-flip2

$(1)-$(2)-upload-flip2: $(1)/$(2)/$(1)-$(2).hex
	$(AVRDUDE) -c flip2 -p $(2) -U application:w:$$<:i


.PHONY: $(1)-upload-mk2 $(1)-$(2)-upload-mk2
$(1)-upload-mk2: $(1)-$(2)-upload-mk2

$(1)-$(2)-upload-mk2: $(1)/$(2)/$(1)-$(2).hex
	$(AVRDUDE) -c avrispmkII -p $(2) -U application:w:$$<:i


.PHONY: $(1)-upload-dfu $(1)-$(2)-upload-dfu
$(1)-upload-dfu: $(1)-$(2)-upload-dfu

$(1)-$(2)-upload-dfu: bin/dfu-$(2)-$(1).ihex
	$(AVRDUDE) -c avrispmkII -p $(2) \
    -U flash:w:$$<:i \
    -U fuse2:w:0xbf:m

endef


$(eval $(call BOARD_MCU_template,midi-router-x2,atxmega128a1u))
$(eval $(call BOARD_MCU_template,midi-router-x4,atxmega32a4u))
#$(eval $(call BOARD_MCU_template,midi-router-x4,atxmega128a4u))
$(eval $(call BOARD_MCU_template,midi-router-x7,atxmega128a1u))
