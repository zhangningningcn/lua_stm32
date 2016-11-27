
#STM32F10X_LD_VL  STM32 Low density Value line devices
#STM32F10X_LD     STM32 Low density devices
#STM32F10X_MD_VL  STM32 Medium density Value line devices
#STM32F10X_MD     STM32 Medium density devices
#STM32F10X_HD_VL  STM32 High density Value line devices
#STM32F10X_HD     STM32 High density device
#STM32F10X_XL     STM32 XL density devices
#STM32F10X_CL     STM32 Connectivity line devices  


# - Low density Value line devices are STM32F100xx microcontrollers where the Flash  
#   memory density ranges between 16 and 32 Kbytes. 
#
# - Low density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers 
#   where the Flash memory density ranges between 16 and 32 Kbytes. 
#
# - Medium density Value line devices are STM32F100xx microcontrollers where the Flash  
#   memory density ranges between 64 and 128 Kbytes. 
#
# - Medium density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers 
#   where the Flash memory density ranges between 64 and 128 Kbytes. 
#
# - High density Value line devices are STM32F100xx microcontrollers wherethe Flash memory 
#   density ranges between 256 and 512 Kbytes. 
# - High density devices are STM32F101xx and STM32F103xx microcontrollers where 
#   the Flash memory density ranges between 256 and 512 Kbytes. 
# - XL-density devices are STM32F101xx and STM32F103xx microcontrollers where 
#   the Flash memory density ranges between 512 and 1024 Kbytes. 
# - Connectivity line devices are STM32F105xx and STM32F107xx microcontrollers. 


DEVICETYPE1:=STM32F10X
DEVICETYPE2:=_CL
DEVICETYPE:=$(DEVICETYPE1)$(DEVICETYPE2)

CCFLAGS:= -mthumb -mcpu=cortex-m3 --specs=nano.specs -ffunction-sections \
-std=c99 -fdata-sections -Wall -Os

CCFLAGS += -DUSE_STDPERIPH_DRIVER

ASFLAGS:= -mthumb -mcpu=cortex-m3 -Wall

LINKFLAGS := -Wl,-Map=$(MAIN).map -Wl,-rpath,$(OBJDIR) -T $(TOP_DIR)/ld/stm32_flash.ld 

STM32_INCLUDES := -I $(TOP_DIR)/Lib/STM32F10x/inc \
-I $(TOP_DIR)/Lib/CMSIS/CM3 -I $(TOP_DIR)/Lib/spiffs \
-I $(TOP_DIR)/app -I $(TOP_DIR)/spiffs -I $(TOP_DIR)/Lua

sinclude ../Lib/STM32F10x/src/Makefile.mk
sinclude ../Lib/CMSIS/CM3/Makefile.mk
