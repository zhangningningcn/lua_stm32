#ifndef _MODULES_H_
#define _MODULES_H_

#define LUA_GPIOLIBNAME	"gpio"
LUAMOD_API int (luaopen_gpio) (lua_State *L);
#define LUA_ADCLIBNAME	"adc"
LUAMOD_API int (luaopen_adc) (lua_State *L);
#define LUA_FLASHLIBNAME	"flash"
LUAMOD_API int (luaopen_flash) (lua_State *L);
#define LUA_LEDLIBNAME	"led"
LUAMOD_API int (luaopen_led) (lua_State *L);
#define LUA_KEYLIBNAME	"key"
LUAMOD_API int (luaopen_key) (lua_State *L);

#endif
