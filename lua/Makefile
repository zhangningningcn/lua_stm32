include Makefile.mk

#OBJ += $(LUA_OBJ)
INCLUDES = -I $(TOP_DIR)/app -I $(TOP_DIR)/spiffs $(STM32_INCLUDES)

all:$(LUA_OBJ)



$(LUA_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(INCLUDES) -D$(DEVICETYPE) $< -o $(OBJDIR)$@ 
	$(CC) -MM $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -MF $(DEPDIR)$(*F).d
    
    
sinclude $(addprefix $(DEPDIR),$(LUA_OBJ:.o=.d))  
	
.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(LUA_OBJ))
	rm -f $(addprefix $(DEPDIR),$(LUA_OBJ:.o=.d))

# sinclude(..\Makefile)
