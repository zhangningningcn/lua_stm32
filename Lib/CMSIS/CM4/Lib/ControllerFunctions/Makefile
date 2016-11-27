include makefile.mk

INCLUDES:= -I ../Include
OBJDIR:=../obj
VPATH=$(OBJDIR)

all:$(ControllerFunctionsOBJ)

$(ControllerFunctionsOBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o ../obj/$@

.PHONY: clean
clean:
	rm -f $(ControllerFunctionsOBJ)