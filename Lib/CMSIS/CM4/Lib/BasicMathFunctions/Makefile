include makefile.mk

INCLUDES:= -I ../Include
OBJDIR:=../obj
VPATH=$(OBJDIR)

all:$(BasicMathFunctionsOBJ)

$(BasicMathFunctionsOBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o ../obj/$@

.PHONY: clean
clean:
	rm -f $(BasicMathFunctionsOBJ)