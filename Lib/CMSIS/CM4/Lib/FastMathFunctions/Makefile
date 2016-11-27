include makefile.mk

INCLUDES:= -I ../Include
OBJDIR:=../obj
VPATH=$(OBJDIR)

all:$(FastMathFunctionsOBJ)

$(FastMathFunctionsOBJ):%.o:%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o ../obj/$@

.PHONY: clean
clean:
	rm -f $(FastMathFunctionsOBJ)