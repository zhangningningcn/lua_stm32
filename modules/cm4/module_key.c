#include "stm32f4xx.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "modules.h"
#include "module_comm.h"
#include <ctype.h>

static void lkey_init(void) {
     
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    PinUse(GPIO_PIN,MODULE_GPIOE,3,PIN_STATE_USE);
    PinUse(GPIO_PIN,MODULE_GPIOE,4,PIN_STATE_USE);
    PinUse(GPIO_PIN,MODULE_GPIOA,0,PIN_STATE_USE);
    
}

static int lled_read( lua_State* L ) {
    uint32_t pinnum   = luaL_checkinteger( L, 2 );
    switch(pinnum) {
        case 0:
            if(GPIOE->IDR & GPIO_Pin_4) {
                lua_pushinteger( L, 1 );
            }
            else {
                lua_pushinteger( L, 0 );
            }
        break;
        case 1:
            if(GPIOE->IDR & GPIO_Pin_6) {
                lua_pushinteger( L, 1 );
            }
            else {
                lua_pushinteger( L, 0 );
            }
        break;
        case 2:
            if(GPIOA->IDR & GPIO_Pin_0) {
                lua_pushinteger( L, 1 );
            }
            else {
                lua_pushinteger( L, 0 );
            }
        break;
        default:
            return luaL_error( L, "The key number is only one of 0,1 or 2." );
        //break;
    }
    return 1;
}


static const luaL_Reg key_map[] = {
  {"read"  , lled_read  },
  { NULL, NULL }
};


LUAMOD_API int luaopen_key (lua_State *L) {
    lkey_init();
    luaL_newlib(L, key_map);
    return 1;
}
