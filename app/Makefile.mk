APP_OBJ:=io.o main.o rtc.o app_usart.o app_misc.o

ifeq ($(DEVICETYPE1), STM32F10X)
 APP_OBJ += stm32f10x_it.o system_stm32f10x.o
else
 APP_OBJ += stm32f4xx_it.o system_stm32f4xx.o
endif

OBJ += $(APP_OBJ)