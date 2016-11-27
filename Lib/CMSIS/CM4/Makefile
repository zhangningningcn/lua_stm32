include Makefile.mk

STM32LIB_STARTUP_SRC:=startup_stm32f4xx.s


all:$(CM4_OBJA)
	echo $(CM4_OBJA:.o=.d)
    
$(CM4_OBJA):
	@echo Assembling ASM: $(STM32LIB_STARTUP_SRC)
	$(CC) -c $(ASFLAGS) $(STM32_INCLUDES) $(STM32LIB_STARTUP_SRC) -o $(OBJDIR)$(CM4_OBJA)

    
.PHONY: clean
clean:
	rm -f $(OBJDIR)$(CM4_OBJA)
	rm -f $(addprefix $(DEPDIR),$(CM4_OBJA:.o=.d))
