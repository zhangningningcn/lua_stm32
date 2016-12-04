#include "stm32f4xx.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "modules.h"
#include "module_comm.h"
#include <ctype.h>

#define MODULE_IO_INPUT        2
#define MODULE_IO_OUTPUT       3


GPIO_TypeDef * LIB_GPIO_DEF[9] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOF,GPIOG,GPIOH,GPIOI};
static int GPIO_Interrupt_Ref[16];
static int GPIO_Interrupts[16];

void EXTI0_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    if(EXTI->PR & 1) {
        // Do the actual callback
        lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[0]);
        lua_call(L, 1, 0);
        EXTI->PR = 1;
    }
}
void EXTI1_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    if(EXTI->PR & 2) {
        // Do the actual callback
        lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[1]);
        lua_call(L, 1, 0);
        EXTI->PR = 2;
    }
}
void EXTI2_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    if(EXTI->PR & 4) {
        // Do the actual callback
        lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[2]);
        lua_call(L, 1, 0);
        EXTI->PR = 4;
    }
}
void EXTI3_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    if(EXTI->PR & 8) {
        // Do the actual callback
        lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[3]);
        lua_call(L, 1, 0);
        EXTI->PR = 8;
    }
}
void EXTI4_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    if(EXTI->PR & 16) {
        // Do the actual callback
        lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[4]);
        lua_call(L, 1, 0);
        EXTI->PR = 16;
    }
}
void EXTI9_5_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    uint32_t line = 1 << 5;
    uint32_t pr = EXTI->PR;
    for(int i=5;i<10;i++) {
        if(pr & line) {
            // Do the actual callback
            lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[i]);
            lua_call(L, 1, 0);
            EXTI->PR = line;
        }
        line <<= 1;
    }
}
void EXTI15_10_IRQHandler(void)
{
    lua_State *L = lua_getstate();
    uint32_t line = 1 << 10;
    uint32_t pr = EXTI->PR;
    for(int i=10;i<16;i++) {
        if(pr & line) {
            // Do the actual callback
            lua_rawgeti(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[i]);
            lua_call(L, 1, 0);
            EXTI->PR = line;
        }
        line <<= 1;
    }
}
// Lua: setInt(PA,1,r/f/rf,func) 
static int lgpio_setTrig( lua_State* L ) {
    unsigned pingroup = luaL_checkinteger( L, 1 );
    uint32_t pinnum   = luaL_checkinteger( L, 2 );
    uint32_t mode  = luaL_checkinteger( L, 3 );
    if(lua_type(L, 4) != LUA_TFUNCTION) {
        luaL_argcheck(L,  0, 3, "invalid callback type");
    }
    if(pinnum > 15) {
        return luaL_error( L, "pin number range:0~15." );
    }
    if(pingroup > 8) {
        return luaL_error( L, "pin group range:0~8" );
    }
    if(GPIO_Interrupts[pinnum] < 0) {
        GPIO_Interrupts[pinnum] = pingroup;
    }
    else if(GPIO_Interrupts[pinnum] != pingroup) {
        return luaL_error( L, "EXTI line %d is unsed by GPIO%c",pinnum,pingroup+'A');
    }
    pinnum = (uint32_t)1 << pinnum;
    if(PinIsUsed(pingroup,pinnum) == 0) {
        if(!PinGroupIsEnable(pingroup)) {
            uint32_t group = ((uint32_t)1 << pingroup);
            RCC_AHB1PeriphClockCmd(group, ENABLE);
        }
        PinUse(GPIO_PIN,pingroup,pinnum,PIN_STATE_USE);
        GPIO_InitTypeDef  GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = pinnum;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(LIB_GPIO_DEF[pingroup], &GPIO_InitStructure);
    }
    if ((RCC->APB2ENR & RCC_APB2Periph_SYSCFG) == 0) {
        RCC->APB2ENR |= RCC_APB2Periph_SYSCFG;
    }
    SYSCFG_EXTILineConfig(pingroup, pinnum);
    
    EXTI_InitTypeDef   EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = pinnum;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    lua_pushvalue(L, 4);
    GPIO_Interrupt_Ref[pinnum] = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}
// Lua: setInt(PA,1,r/f/rf,func) 
static int lgpio_unsetTrig( lua_State* L ) {
    
    unsigned pingroup = luaL_checkinteger( L, 1 );
    uint32_t pinnum   = luaL_checkinteger( L, 2 );
    if(pinnum > 15) {
        return luaL_error( L, "pin number range:0~15." );
    }
    if(pingroup > 8) {
        return luaL_error( L, "pin group range:0~8" );
    }
    if(GPIO_Interrupts[pinnum] < 0) {
        return 0;
    }
    if(GPIO_Interrupt_Ref[pinnum] != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, GPIO_Interrupt_Ref[pinnum]);
    }
    GPIO_Interrupts[pinnum] = -1;
    EXTI_InitTypeDef   EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = pinnum;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    PinUse(GPIO_PIN,pingroup,pinnum,PIN_STATE_UNUSE);
    if(!PinGroupIsEnable(pingroup)) {
        uint32_t group = ((uint32_t)1 << pingroup);
        RCC_AHB1PeriphClockCmd(group, DISABLE);
    }
    return 0;
}
// Lua: mode(PA,1,OUT,PULL)
static int lgpio_mode( lua_State* L ) {
    uint32_t gpiomode = GPIO_Mode_IN;
    unsigned pingroup = luaL_checkinteger( L, 1 );
    uint32_t pinnum   = luaL_checkinteger( L, 2 );
    uint32_t pinmode  = luaL_checkinteger( L, 3 );
    uint32_t pullup   = luaL_optinteger( L, 4, GPIO_PuPd_NOPULL );
    if(pinnum > 15) {
        return luaL_error( L, "pin number range:0~15." );
    }
    if(pingroup > 8) {
        return luaL_error( L, "pin group range:0~8" );
    }
    pinnum = (uint32_t)1 << pinnum;
    switch(pinmode) {
        case MODULE_IO_DISABLE:
        case MODULE_IO_INPUT:
        gpiomode = GPIO_Mode_IN;
        break;
        case MODULE_IO_OUTPUT:
        gpiomode = GPIO_Mode_OUT;
        break;
        default:
        return luaL_error( L, "pin mode mast be OUTPUT,PULLUP or DISABLE." );
        //break;
    }
    switch(pullup) {
        case GPIO_PuPd_NOPULL:
        //pullup = GPIO_Mode_IN;
        //break;
        case GPIO_PuPd_UP:
        //pullup = GPIO_Mode_OUT;
        //break;
        case GPIO_PuPd_DOWN:
        //pullup = GPIO_Mode_OUT;
        break;
        default:
        return luaL_error( L, "pull mast be NOPULL,PULLUP or PULLDOWN." );
        //break;
    }
    if(pinmode == MODULE_IO_DISABLE) {
        if(PinIsUsed(pingroup,pinnum) == 0) {
            return 0;
        }
    }
    else{
        if(!PinGroupIsEnable(pingroup)) {
            uint32_t group = ((uint32_t)1 << pingroup);
            RCC_AHB1PeriphClockCmd(group, ENABLE);
        }
        PinUse(GPIO_PIN,pingroup,pinnum,PIN_STATE_USE);
    }
    
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = pinnum;
    GPIO_InitStructure.GPIO_Mode = gpiomode;
    GPIO_InitStructure.GPIO_PuPd = pullup;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(LIB_GPIO_DEF[pingroup], &GPIO_InitStructure);

    if(pinmode == MODULE_IO_DISABLE) {
        PinUse(GPIO_PIN,pingroup,pinnum,PIN_STATE_UNUSE);
        if(!PinGroupIsEnable(pingroup)) {
            uint32_t group = ((uint32_t)1 << pingroup);
            RCC_AHB1PeriphClockCmd(group, DISABLE);
        }
    }
  return 0;
}
// Lua: write(PA,1,HIGH)
static int lgpio_write( lua_State* L ) {
  unsigned pingroup = luaL_checkinteger( L, 1 );
  unsigned pinnum   = luaL_checkinteger( L, 2 );
  unsigned pinstate  = luaL_checkinteger( L, 3 );
  if(pinnum > 15) {
    return luaL_error( L, "pin number range:0~15." );
  }
  if(pingroup > 8) {
    return luaL_error( L, "pin group range:0~8" );
  }
  else {
      if(pinstate) {
        LIB_GPIO_DEF[pingroup]->BSRRL = (uint32_t)1 << pinnum;
          
      }
      else {
        LIB_GPIO_DEF[pingroup]->BSRRH = (uint32_t)1 << pinnum;
      }
  }
  return 0;
}

// Lua: read(PA,1)
static int lgpio_read( lua_State* L ) {
  unsigned pingroup = luaL_checkinteger( L, 1 );
  unsigned pinnum   = luaL_checkinteger( L, 2 );
  if(pinnum > 15) {
    return luaL_error( L, "pin number range:0~15." );
  }
  if(pingroup > 8) {
    return luaL_error( L, "pin group range:0~8" );
  }
  else {
      if(LIB_GPIO_DEF[pingroup]->IDR & (uint32_t)1 << pinnum) {
            lua_pushinteger( L, 1 );
      }
      else {
            lua_pushinteger( L, 0 );
      }
  }
  return 1;
}

// Module function map
static const luaL_Reg gpio_map[] = {
  {"mode"      , lgpio_mode      },
  {"read"      , lgpio_read      },
  {"write"     , lgpio_write     },
  {"setTrig"   , lgpio_setTrig   },
  {"unsetTrig" , lgpio_unsetTrig },
  
  {"PA",NULL},
  {"PB",NULL},
  {"PC",NULL},
  {"PD",NULL},
  {"PE",NULL},
  {"PF",NULL},
  {"PG",NULL},
  {"PH",NULL},
  {"PI",NULL},
  
  {"DISABLE",NULL},
  {"ENABLE" ,NULL},
  
  {"OUTPUT" , NULL},
  {"INPUT"  , NULL},
  {"HIGH"   , NULL},
  {"LOW"    , NULL},
  
  {"NOPULL" , NULL},
  {"PULLUP" , NULL},
  {"PULLDOWN" , NULL},
  
  {"r" ,  NULL},
  {"f" ,  NULL},
  {"rf" , NULL},
  { NULL, NULL }
};

LUAMOD_API int luaopen_gpio (lua_State *L) {
    for(int i=0;i<16;i++) {
        GPIO_Interrupts[i] = -1;
        GPIO_Interrupt_Ref[i] = LUA_NOREF;
    }
    //InitPinGroupTable();
    luaL_newlib(L, gpio_map);
    lua_pushinteger(L, MODULE_GPIOA);
    lua_setfield(L, -2, "PA");
    lua_pushinteger(L, MODULE_GPIOB);
    lua_setfield(L, -2, "PB");
    lua_pushinteger(L, MODULE_GPIOC);
    lua_setfield(L, -2, "PC");
    lua_pushinteger(L, MODULE_GPIOD);
    lua_setfield(L, -2, "PD");
    lua_pushinteger(L, MODULE_GPIOE);
    lua_setfield(L, -2, "PE");
    lua_pushinteger(L, MODULE_GPIOF);
    lua_setfield(L, -2, "PF");
    lua_pushinteger(L, MODULE_GPIOH);
    lua_setfield(L, -2, "PH");
    lua_pushinteger(L, MODULE_GPIOI);
    lua_setfield(L, -2, "PI");
    lua_pushinteger(L, MODULE_IO_DISABLE);
    lua_setfield(L, -2, "DISABLE");
    lua_pushinteger(L, MODULE_IO_ENABLE);
    lua_setfield(L, -2, "ENABLE");
    lua_pushinteger(L, MODULE_IO_OUTPUT);
    lua_setfield(L, -2, "OUTPUT");
    lua_pushinteger(L, MODULE_IO_INPUT);
    lua_setfield(L, -2, "INPUT");
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "HIGH");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "LOW");
    lua_pushinteger(L, GPIO_PuPd_NOPULL);
    lua_setfield(L, -2, "NOPULL");
    lua_pushinteger(L, GPIO_PuPd_UP);
    lua_setfield(L, -2, "PULLUP");
    lua_pushinteger(L, GPIO_PuPd_DOWN);
    lua_setfield(L, -2, "PULLDOWN");
    lua_pushinteger(L, EXTI_Trigger_Rising);
    lua_setfield(L, -2, "r");
    lua_pushinteger(L, EXTI_Trigger_Falling);
    lua_setfield(L, -2, "f");
    lua_pushinteger(L, EXTI_Trigger_Rising_Falling);
    lua_setfield(L, -2, "rf");
    return 1;
}


