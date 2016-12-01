include Makefile.mk

#OBJ += $(LUA_OBJ)
INCLUDES += -I .

all:$(MODULES_OBJ)



$(MODULES_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS)  $(OPTIMIZATION) $(INCLUDES) $(STM32_INCLUDES) -D$(DEVICETYPE)  $< -o $(OBJDIR)$@ 
	$(CC) -MM $(CCFLAGS) $(STM32_INCLUDES) $< -MF $(DEPDIR)$(*F).d

sinclude $(addprefix $(DEPDIR),$(MODULES_OBJ:.o=.d))  


.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(MODULES_OBJ))
	rm -f $(addprefix $(DEPDIR),$(MODULES_OBJ:.o=.d))

# sinclude(..\Makefile)
