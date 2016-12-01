

TOP_DIR:=$(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)

MAIN:=lua_stm32
OBJDIR:=$(TOP_DIR)/gcc/obj/
DEPDIR:=$(TOP_DIR)/gcc/dep/
VPATH:=$(OBJDIR)
OPTIMIZATION:= -g -O0
AR = arm-none-eabi-ar
CC = arm-none-eabi-gcc
NM = arm-none-eabi-nm
CPP = arm-none-eabi-cpp
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
CP = cp

#include cm3.mk
include cm4.mk

include ../lua/Makefile.mk
sinclude ../spiffs/Makefile.mk
include ../app/Makefile.mk
include ../Lib/VCP/src/Makefile.mk
include ../Lib/VCP/USBDevLib/Makefile.mk
include ../Lib/VCP/USBOTGD/Makefile.mk

OBJLIST = $(addprefix obj/,$(OBJ))

all: $(MAIN).hex
	$(SIZE) $(MAIN).elf
    

tmpdirs:
	mkdir -p obj
	mkdir -p dep
objects: tmpdirs
	$(MAKE) -C ../lua
	$(MAKE) -C ../Lib
	$(MAKE) -C ../spiffs
	$(MAKE) -C ../app
	$(MAKE) -C ../modules/cm4
	
$(MAIN).elf: objects
	$(CC) $(LINKFLAGS) $(OBJLIST) -o $@ 
    
$(MAIN).hex: $(MAIN).elf
	$(OBJCOPY) -O ihex $(MAIN).elf $(MAIN).hex
 
.PHONY: objects clean

clean:
	$(MAKE) -C ../lua clean
	$(MAKE) -C ../Lib clean
	$(MAKE) -C ../spiffs clean
	$(MAKE) -C ../app clean
	$(MAKE) -C ../modules/cm4 clean
	rm -f $(MAIN).map
	rm -f $(MAIN).elf
	rm -f $(MAIN).hex

export
