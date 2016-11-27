include makefile.mk

INCLUDES:= -I ../Include
OBJDIR:=../obj
VPATH=$(OBJDIR)

all:$(ComplexMathFunctionsOBJ)

$(ComplexMathFunctionsOBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o ../obj/$@

.PHONY: clean
clean:
	rm -f $(ComplexMathFunctionsOBJ)