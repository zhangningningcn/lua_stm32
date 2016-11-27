
CCFLAGS += -D$(DEVICETYPE)

ifeq ($(DEVICETYPE1), STM32F10X)
  DIR_PHLIB_SRC := ./STM32F10x/src
  DIR_CMSIS_SRC := ./CMSIS/CM3
else
  DIR_PHLIB_SRC := ./STM32F4xx/src
  DIR_CMSIS_SRC := ./CMSIS/CM4
endif
all:
	$(MAKE) -C $(DIR_PHLIB_SRC)
	$(MAKE) -C $(DIR_CMSIS_SRC)
	$(MAKE) -C VCP
.PHONY: clean
clean:
	$(MAKE) -C $(DIR_PHLIB_SRC) clean
	$(MAKE) -C $(DIR_CMSIS_SRC) clean
	$(MAKE) -C VCP clean
export