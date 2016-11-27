include src.mk

all:$(STDLIB_OBJ)


$(STDLIB_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(STM32_INCLUDES) $< -o $(OBJDIR)$@ 
	
clean:
	rm -f $(STDLIB_OBJ)