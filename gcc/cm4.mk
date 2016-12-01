
DEVICETYPE1:=STM32F4XX
DEVICETYPE:=$(DEVICETYPE1)
LIBM_NOT_WORK:=1

CCFLAGS:= -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
-ffunction-sections -std=c99 -fdata-sections -Wall
#CCFLAGS:= -mthumb -mcpu=cortex-m4 \
#-ffunction-sections -std=c99 -fdata-sections -Wall -O0

CCFLAGS += -DUSE_STDPERIPH_DRIVER

ifeq ($(LIBM_NOT_WORK), 1)
CCFLAGS += -D_LIBM_NOT_WORK
endif

ASFLAGS:= -mthumb -mcpu=cortex-m4 -Wall

LINKFLAGS := -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -Wl,-Map=$(MAIN).map \
-Wl,-rpath,$(OBJDIR) -T $(TOP_DIR)/ld/stm32f407_flash.ld -L$(TOP_DIR)/Lib/CMSIS/CM4/ -lm
#LINKFLAGS := -mthumb -mcpu=cortex-m4 -Wl,--gc-sections -Wl,-Map=$(MAIN).map \
#-Wl,-rpath,$(OBJDIR) -T $(TOP_DIR)/ld/stm32f407_flash.ld -lm

STM32_INCLUDES := -I $(TOP_DIR)/Lib/STM32F4xx/inc \
-I $(TOP_DIR)/Lib/CMSIS/CM4 -I $(TOP_DIR)/Lib/spiffs \
-I $(TOP_DIR)/app -I $(TOP_DIR)/spiffs -I $(TOP_DIR)/lua \
-I $(TOP_DIR)/modules/cm4

sinclude ../Lib/STM32F4xx/src/Makefile.mk
sinclude ../Lib/CMSIS/CM4/Makefile.mk
sinclude ../modules/cm4/Makefile.mk

