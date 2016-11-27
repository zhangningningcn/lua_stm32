#include "stm32f4xx.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "modules.h"
#include "module_comm.h"
#include "user_define.h"
#include <ctype.h>



static int ADC_PORT_TABLE[][4] = {
    {MODULE_GPIOF, 3,3, 9},    // PF3 ADC3_IN9
    {MODULE_GPIOF, 4,3,14},   // PF4 ADC3_IN14
    {MODULE_GPIOF, 5,3,15},   // PF4 ADC3_IN15
    {MODULE_GPIOF, 6,3, 4},
    {MODULE_GPIOF, 7,3, 5},
    {MODULE_GPIOF, 8,3, 6},
    {MODULE_GPIOF, 9,3, 7},
    {MODULE_GPIOF,10,3, 8},
    {MODULE_GPIOC, 0,1,10},//ADC123_IN10
    {MODULE_GPIOC, 1,1,11},
    {MODULE_GPIOC, 2,1,12},
    {MODULE_GPIOC, 3,1,13},
    {MODULE_GPIOA, 0,1, 0}, //ADC123_IN0
    {MODULE_GPIOA, 1,1, 1}, 
    {MODULE_GPIOA, 2,1, 2}, 
    {MODULE_GPIOA, 3,1, 3}, 
    {MODULE_GPIOA, 4,1, 4},  //ADC12_IN4
    {MODULE_GPIOA, 5,1, 5}, 
    {MODULE_GPIOA, 6,1, 6}, 
    {MODULE_GPIOA, 7,1, 7}, 
    {MODULE_GPIOC, 4,1,14}, //ADC12_IN14
    {MODULE_GPIOC, 5,1,15},
    {MODULE_GPIOB, 0,1, 8}, //ADC12_IN8
    {MODULE_GPIOB, 1,1, 9},
};
static float ADC_Vref = DEFAULT_ADC_VREF_VALUE;

static int getADCNumFromPort(int port,int num,int *adc,int *adport) {
    *adc = 0;
    *adport = 0;
    for(int i=0;i<sizeof(ADC_PORT_TABLE)/(sizeof(int)*4);i++) {
        if(ADC_PORT_TABLE[i][0] == port && ADC_PORT_TABLE[i][1] == num) {
            *adc = ADC_PORT_TABLE[i][2];
            *adport = ADC_PORT_TABLE[i][3];
            return 1;
        }
    }
    return 0;
}

uint32_t Totle_Used_ADC = 0;
uint32_t Totle_Used_ADC1 = 0;
uint32_t Totle_Used_ADC3 = 0;
//Lua: setup(PA,3,ENABLE)
int ladc_setup( lua_State* L ) {

	int pingroup = luaL_checkinteger( L, 1 );
	uint32_t  pinnum   = luaL_checkinteger( L, 2 );
	int  enable   = luaL_optinteger( L, 3, MODULE_IO_ENABLE );
    int adcnum,adcchannel;
    getADCNumFromPort(pingroup,pinnum,&adcnum,&adcchannel);
    if(adcnum == 0) {
        return luaL_error( L, "PA0~7,PB0~1,PC0~5,PF3~10 is ADC Pin" );
    }
    pinnum = (uint32_t)1 << pinnum;
    if(enable){
        ADC_InitTypeDef       ADC_InitStructure;
        int used = PinIsUsed(pingroup,pinnum);
        if(used == ADC_PIN) {
            return 0;
        }
        else if(used != 0) {
            return luaL_error( L, "This Pin is Used for GPIO" );
        }
        if(Totle_Used_ADC == 0) {
            ADC_CommonInitTypeDef ADC_CommonInitStructure;
            /* ADC Common Init **********************************************************/
            ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
            ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
            ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
            ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
            ADC_CommonInit(&ADC_CommonInitStructure);
        }
        Totle_Used_ADC++;
        switch(adcnum) {
            
        case 1:
            if(Totle_Used_ADC1 == 0) {
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
                /* ADC1 Init ****************************************************************/
                ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
                ADC_InitStructure.ADC_ScanConvMode = DISABLE;
                ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
                ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
                ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
                ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
                ADC_InitStructure.ADC_NbrOfConversion = 1;
                ADC_Init(ADC1, &ADC_InitStructure);
                ADC_Cmd(ADC1, ENABLE);
            }
            Totle_Used_ADC1++;
            ADC_RegularChannelConfig(ADC1, adcchannel, 1, ADC_SampleTime_15Cycles);
        break;
        case 3:
            if(Totle_Used_ADC3 == 0) {
                /* ADC1 Init ****************************************************************/
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
                ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
                ADC_InitStructure.ADC_ScanConvMode = DISABLE;
                ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
                ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
                ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
                ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
                ADC_InitStructure.ADC_NbrOfConversion = 1;
                ADC_Init(ADC3, &ADC_InitStructure);
                ADC_Cmd(ADC3, ENABLE);
            }
            Totle_Used_ADC3++;
            ADC_RegularChannelConfig(ADC1, adcchannel, 1, ADC_SampleTime_15Cycles);
        break;
        }
        if(!PinGroupIsEnable(pingroup)) {
            uint32_t group = ((uint32_t)1 << pingroup);
            RCC_AHB1PeriphClockCmd(group, enable);
        }
        PinUse(ADC_PIN,pingroup,pinnum,PIN_STATE_USE);
    }
    else{
        if(Totle_Used_ADC > 0) {
            Totle_Used_ADC--;
        }
        if(Totle_Used_ADC == 0) {
            
        }
        switch(adcnum) {
            case 1:
                if(Totle_Used_ADC1 > 0) {
                    Totle_Used_ADC1--;
                }
                if(Totle_Used_ADC1 == 0) {
                    ADC_Cmd(ADC1, DISABLE);
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
                }
            break;
            case 3:
                if(Totle_Used_ADC3 > 0) {
                    Totle_Used_ADC3--;
                }
                if(Totle_Used_ADC3 == 0) {
                    ADC_Cmd(ADC3, DISABLE);
                    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, DISABLE);
                }
            break;
        }
        PinUse(ADC_PIN,pingroup,pinnum,PIN_STATE_UNUSE);
        if(!PinGroupIsEnable(pingroup)) {
            uint32_t group = ((uint32_t)1 << pingroup);
            RCC_AHB1PeriphClockCmd(group, DISABLE);
        }
    }
    return 0;
}

static int _ladc_getValue( lua_State* L ,int *value) {
	int pingroup = luaL_checkinteger( L, 1 );
	uint32_t  pinnum   = luaL_checkinteger( L, 2 );
    int adcnum,adcchannel;
    getADCNumFromPort(pingroup,pinnum,&adcnum,&adcchannel);
    if(adcnum == 0) {
        return luaL_error( L, "PA0~7,PB0~1,PC0~5,PF3~10 is ADC Pin" );
    }
    int used = PinIsUsed(pingroup,(uint32_t)1 << pinnum);
    if(used != ADC_PIN) {
        return luaL_error( L, "Please setup this pin before using" );
    }
    ADC_TypeDef *adc;
    switch(adcnum) {
        case 1:
            adc = ADC1;
            break;
        case 3:
            adc = ADC3;
            break;
        default:
            return luaL_error( L, "Unknow error" );
    }
    ADC_RegularChannelConfig(adc, adcchannel, 1, ADC_SampleTime_15Cycles);
    ADC_SoftwareStartConv(adc);
    int delay_counter = 0;
    while(!ADC_GetITStatus(adc,ADC_IT_EOC)) {
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        __asm("NOP");
        delay_counter++;
        if(delay_counter > 1000) {
            return luaL_error( L, "Convert time out" );
        }
    }
    *value = ADC_GetConversionValue(adc);
    return 1;
}

//Lua: getValue(PA,3)
static int ladc_getValue( lua_State* L ) {
    int value;
    int ret = _ladc_getValue(L,&value );
    if(ret == 1) {
        lua_pushinteger( L,  value );
    }
    return ret;
}
//Lua: getValue(PA,3)
static int ladc_getVoltage( lua_State* L ) {
    int value;
    LUA_NUMBER voltage;
    int ret = _ladc_getValue(L,&value );
    if(ret == 1) {
        voltage = value*ADC_Vref/4096;
        lua_pushnumber( L,  voltage );
    }
    return ret;
}

//Lua: setVref(3.3)
static int ladc_setVref( lua_State* L ) {
	LUA_NUMBER vref = luaL_optnumber( L,1, DEFAULT_ADC_VREF_VALUE );
    if(vref < 0.1) {
        return luaL_error( L, "ADC Vref out of range" );
    }
    ADC_Vref = vref;
    return 0;
}

// Module function map
static const luaL_Reg adc_map[] = {
  {"setup"      ,ladc_setup      },
  {"getValue"   ,ladc_getValue   },
  {"getVoltage" ,ladc_getVoltage },
  {"setVref"    ,ladc_setVref    },
  
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
  
  { NULL, NULL }
};


LUAMOD_API int luaopen_adc (lua_State *L) {
    luaL_newlib(L, adc_map);
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
    return 1;
}
