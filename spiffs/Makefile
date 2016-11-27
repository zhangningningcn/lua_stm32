include Makefile.mk
INCLUDES = -I $(TOP_DIR)/spiffs $(STM32_INCLUDES)

all:$(SPIFFS_OBJ)

$(SPIFFS_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(INCLUDES) -D$(DEVICETYPE) $< -o $(OBJDIR)$@ 
	$(CC) -MM $(INCLUDES) -D$(DEVICETYPE) $< -MF $(DEPDIR)$(*F).d
	
sinclude $(addprefix $(DEPDIR),$(SPIFFS_OBJ:.o=.d))  

.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(SPIFFS_OBJ))
	rm -f $(addprefix $(DEPDIR),$(SPIFFS_OBJ:.o=.d))