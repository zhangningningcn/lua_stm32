LUA_OBJ := lapi.o lauxlib.o lbaselib.o lbitlib.o lcode.o \
lcorolib.o lctype.o ldblib.o ldebug.o ldo.o ldump.o \
lfunc.o lgc.o linit.o liolib.o llex.o lmem.o \
loadlib.o lobject.o lopcodes.o loslib.o lparser.o lstate.o \
lstring.o lstrlib.o ltable.o ltablib.o ltm.o lua.o \
lundump.o lutf8lib.o lvm.o lzio.o

ifneq ($(LIBM_NOT_WORK), 1)
LUA_OBJ += lmathlib.o
endif
OBJ += $(LUA_OBJ)