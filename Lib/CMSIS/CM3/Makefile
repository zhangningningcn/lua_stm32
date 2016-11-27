include Makefile.mk


#STM32F10X_LD_VL  STM32 Low density Value line devices
#STM32F10X_LD     STM32 Low density devices
#STM32F10X_MD_VL  STM32 Medium density Value line devices
#STM32F10X_MD     STM32 Medium density devices
#STM32F10X_HD_VL  STM32 High density Value line devices
#STM32F10X_HD     STM32 High density device
#STM32F10X_XL     STM32 XL density devices
#STM32F10X_CL     STM32 Connectivity line devices  

ifeq ($(DEVICETYPE), STM32F10X_LD_VL)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_ld_vl.s
endif
ifeq ($(DEVICETYPE), STM32F10X_LD)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_ld.s
endif
ifeq ($(DEVICETYPE), STM32F10X_MD_VL)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_md_vl.s
endif
ifeq ($(DEVICETYPE), STM32F10X_MD)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_md.s
endif
ifeq ($(DEVICETYPE), STM32F10X_HD_VL)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_hd_vl.s
endif
ifeq ($(DEVICETYPE), STM32F10X_HD)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_hd.s
endif
ifeq ($(DEVICETYPE), STM32F10X_XL)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_xl.s
endif
ifeq ($(DEVICETYPE), STM32F10X_CL)
  STM32LIB_STARTUP_SRC:=startup_stm32f10x_cl.s
endif

ifndef STM32LIB_STARTUP_SRC
    $(error May be not defined the DEVICETYPE)
endif


all:objc obja

objc:$(CM3_OBJC)

$(CM3_OBJC):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(STM32_INCLUDES) $< -o $@ 
	$(CP) $@ $(OBJDIR)
	
obja:
	@echo Assembling ASM: $(STM32LIB_STARTUP_SRC)
	$(CC) -c $(ASFLAGS) $(STM32_INCLUDES) $(STM32LIB_STARTUP_SRC) -o $(OBJDIR)$(CM3_OBJA)

.PHONY: clean
clean:
	rm -f $(CM3_OBJC)
	rm -f $(CM3_OBJA)
