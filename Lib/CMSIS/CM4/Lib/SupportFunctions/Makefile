include makefile.mk

INCLUDES:= -I ../Include
OBJDIR:=../obj
VPATH=$(OBJDIR)

all:$(SupportFunctionsOBJ)

$(SupportFunctionsOBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o ../obj/$@

.PHONY: clean
clean:
	rm -f $(SupportFunctionsOBJ)