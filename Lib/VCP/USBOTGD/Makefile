include Makefile.mk

all:$(USB_OTG_DRIVER_OBJ)

$(USB_OTG_DRIVER_OBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(OPTIMIZATION) $(INCLUDES) -D$(DEVICETYPE) $< -o $(OBJDIR)$@ 
	$(CC) -MM $(INCLUDES) -D$(DEVICETYPE) $< -MF $(DEPDIR)$(*F).d
	
sinclude $(addprefix $(DEPDIR),$(USB_OTG_DRIVER_OBJ:.o=.d))  

.PHONY: clean
clean:
	rm -f $(addprefix $(OBJDIR),$(USB_OTG_DRIVER_OBJ))
	rm -f $(addprefix $(DEPDIR),$(USB_OTG_DRIVER_OBJ:.o=.d))