#include "stm32f4xx.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "modules.h"
#include "module_led.h"
#include "module_comm.h"
#include <ctype.h>

uint32_t LED_Func_Terminal;

#define LED_FUNC_USER      0
#define LED_FUNC_TERMINAL  1


static void lled_init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN_LED1 | PIN_LED2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(PORT_LED1, &GPIO_InitStructure);
    
    PinUse(GPIO_PIN,MODULE_GPIOA,6,PIN_STATE_USE);
    PinUse(GPIO_PIN,MODULE_GPIOA,7,PIN_STATE_USE);
    
    LED_Func_Terminal = PIN_LED1 | PIN_LED2;
}

static int lled_on( lua_State* L ) {
    uint32_t pinnum   = luaL_checkinteger( L, 1 );
    if(pinnum == 2) {
        LED_Func_Terminal &= ~PIN_LED1;
        PORT_LED1->BSRRH = PIN_LED1;
    }
    else if(pinnum == 3) {
        LED_Func_Terminal &= ~PIN_LED2;
        PORT_LED2->BSRRH = PIN_LED2;
    }
    else {
        return luaL_error( L, "LED number range is 2~3." );
    }
    return 0;
}

static int lled_off( lua_State* L ) {
    uint32_t pinnum   = luaL_checkinteger( L, 1 );
    if(pinnum == 2) {
        LED_Func_Terminal &= ~PIN_LED1;
        PORT_LED1->BSRRL = PIN_LED1;
    }
    else if(pinnum == 3) {
        LED_Func_Terminal &= ~PIN_LED2;
        PORT_LED2->BSRRL = PIN_LED2;
    }
    else {
        return luaL_error( L, "LED number range is 2~3." );
    }
    return 0;
}
static int lled_setLedFunc(lua_State* L) {
    uint32_t pinnum   = luaL_checkinteger( L, 1 );
    uint32_t state   = luaL_checkinteger( L, 2 );
    if(pinnum == 2) {
        if(state == LED_FUNC_USER) {
            LED_Func_Terminal &= ~PIN_LED1;
        }
        else {
            PORT_LED1->BSRRL |= PIN_LED1;
        }
    }
    else if(pinnum == 3) {
        if(state == LED_FUNC_USER) {
            LED_Func_Terminal &= ~PIN_LED2;
        }
        else {
            PORT_LED2->BSRRL |= PIN_LED2;
        }
    }
    else {
        return luaL_error( L, "LED number range is 2~3." );
    }
    return 0;
}


static const luaL_Reg led_map[] = {
  {"on"  ,     lled_on },
  {"off"  ,    lled_off},
  {"setLedFunc",lled_setLedFunc},
  {"user"  ,   NULL    },
  {"terminal", NULL    },
  { NULL, NULL }
};


LUAMOD_API int luaopen_led (lua_State *L) {
    lled_init();
    luaL_newlib(L, led_map);
    lua_pushinteger(L, LED_FUNC_USER);
    lua_setfield(L, -2, "user");
    lua_pushinteger(L, LED_FUNC_TERMINAL);
    lua_setfield(L, -2, "terminal");
    return 1;
}
