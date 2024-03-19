.PHONY: all clean

MAKEFILE_PATH = Makefile.avr.in

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

$(1)-$(2)-upload-flip2:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) upload-flip2


.PHONY: $(1)-upload-ice $(1)-$(2)-upload-ice
$(1)-upload-ice: $(1)-$(2)-upload-ice

$(1)-$(2)-upload-ice:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) upload-ice


.PHONY: $(1)-upload-mk2 $(1)-$(2)-upload-mk2
$(1)-upload-mk2: $(1)-$(2)-upload-mk2

$(1)-$(2)-upload-mk2:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) upload-mk2


.PHONY: $(1)-upload-dfu $(1)-$(2)-upload-dfu
$(1)-upload-dfu: $(1)-$(2)-upload-dfu

$(1)-$(2)-upload-dfu:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) upload-dfu


.PHONY: $(1)-upload-hw $(1)-$(2)-upload-hw
$(1)-upload-hw: $(1)-$(2)-upload-hw

$(1)-$(2)-upload-hw:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) upload-hw


.PHONY: $(1)-upload $(1)-$(2)-upload
$(1)-upload: $(1)-$(2)-upload

$(1)-$(2)-upload:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) upload


.PHONY: $(1)-download $(1)-$(2)-download
$(1)-download: $(1)-$(2)-download

$(1)-$(2)-download:
	$(MAKE) -f $(MAKEFILE_PATH) BOARD=$(1) MCU=$(2) download


.PHONY: $(1)-bin $(1)-$(2)-bin
$(1)-bin: $(1)-$(2)-bin

$(1)-$(2)-bin: $(1)/$(2)/$(1)-$(2).hex
	cp $$< bin/$(1)-$(2).ihex

endef


$(eval $(call BOARD_MCU_template,midi-router-x2,atxmega128a1u))
$(eval $(call BOARD_MCU_template,midi-router-x4,atxmega32a4u))
$(eval $(call BOARD_MCU_template,midi-router-x4-flat,atxmega32a4u))
$(eval $(call BOARD_MCU_template,midi-router-x4,atxmega128a4u))
$(eval $(call BOARD_MCU_template,midi-router-x4-flat,atxmega128a4u))
$(eval $(call BOARD_MCU_template,midi-router-x7,atxmega128a1u))
$(eval $(call BOARD_MCU_template,midi-router-x7-rack,atxmega128a1u))
