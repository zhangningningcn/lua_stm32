include Makefile.mk


all:$(STDLIB_OBJ)
	echo $(VPATH)

$(STDLIB_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(STM32_INCLUDES) $< -o $(OBJDIR)$@ 
	$(CC) -MM $(STM32_INCLUDES) $< -MF $(DEPDIR)$(*F).d

sinclude $(addprefix $(DEPDIR),$(STDLIB_OBJ:.o=.d))  

.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(STDLIB_OBJ))
	rm -f $(addprefix $(DEPDIR),$(STDLIB_OBJ:.o=.d))