
INCLUDES := -I $(TOP_DIR)/lib/VCP/inc $(STM32_INCLUDES)
all:
	$(MAKE) -C src
	$(MAKE) -C USBDevLib
	$(MAKE) -C USBOTGD
.PHONY: clean
clean:
	$(MAKE) -C src clean
	$(MAKE) -C USBDevLib clean
	$(MAKE) -C USBOTGD clean
export