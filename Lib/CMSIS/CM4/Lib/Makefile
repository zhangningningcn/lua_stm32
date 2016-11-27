
CCFLAGS:= -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 \
-ffunction-sections -std=c99 -fdata-sections -Wall -Os -DARM_MATH_CM4 -D__FPU_PRESENT=1
DEVICETYPE:= STM32F407

AR = arm-none-eabi-ar
CC = arm-none-eabi-gcc
NM = arm-none-eabi-nm
CPP = arm-none-eabi-cpp
OBJCOPY = arm-none-eabi-objcopy


include BasicMathFunctions/makefile.mk
include CommonTables/makefile.mk
include ComplexMathFunctions/makefile.mk
include ControllerFunctions/makefile.mk
include FastMathFunctions/makefile.mk
include FilteringFunctions/makefile.mk
include MatrixFunctions/makefile.mk
include StatisticsFunctions/makefile.mk
include SupportFunctions/makefile.mk
include TransformFunctions/makefile.mk

LIBOBJ = $(addprefix obj/,$(OBJ))


all:objects
	$(AR) rc libsoftfp.a $(LIBOBJ)

objects:
	$(MAKE) -C BasicMathFunctions
	$(MAKE) -C CommonTables
	$(MAKE) -C ComplexMathFunctions
	$(MAKE) -C ControllerFunctions
	$(MAKE) -C FastMathFunctions
	$(MAKE) -C FilteringFunctions
	$(MAKE) -C MatrixFunctions
	$(MAKE) -C StatisticsFunctions
	$(MAKE) -C SupportFunctions
	$(MAKE) -C TransformFunctions

.PHONY: clean objects
clean:
	rm -f obj/*.o
	
export