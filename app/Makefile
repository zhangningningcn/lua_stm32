include Makefile.mk
INCLUDES = -I $(TOP_DIR)/spiffs -I $(TOP_DIR)/Lib/VCP/inc $(STM32_INCLUDES) 

all:$(APP_OBJ)

$(APP_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(INCLUDES) -D$(DEVICETYPE) $< -o $(OBJDIR)$@ 
	$(CC) -MM $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -MF $(DEPDIR)$(*F).d
	
sinclude $(addprefix $(DEPDIR),$(APP_OBJ:.o=.d))  

.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(APP_OBJ))
	rm -f $(addprefix $(DEPDIR),$(APP_OBJ:.o=.d))
