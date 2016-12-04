#include "stm32f4xx.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "modules.h"
#include "module_comm.h"
#include "app_misc.h"

#define IICACK  (0x01)
#define IICNAK  (0x00)


#define SHT1x_SCK_Hi()    do{Port_Sck->BSRRL = Num_sck;DelayMs(2);}while(0)
#define SHT1x_SCK_Low()   do{Port_Sck->BSRRH = Num_sck;DelayMs(2);}while(0)
#define SHT1x_DATA_Hi()   do{Port_Data->BSRRL = Num_Data;DelayMs(2);}while(0)
#define SHT1x_DATA_Low()  do{Port_Data->BSRRH = Num_Data;DelayMs(2);}while(0)
#define SHT1x_DATA_ISHI()  (Port_Data->IDR & Num_Data)


#define READTEMPERATURE  (0x03)
#define READHUMIDITY     (0x05)

GPIO_TypeDef *Port_Sck = NULL, *Port_Data = NULL;
uint32_t Num_sck = 0,Num_Data = 0;

//static uint16_t Temperature;
//static uint16_t Humidity;

// Lua: sht.init(PORT_SCK,NUM_SCK,POTR_DATA,NUM_DATA) 
// Lua: sht.init(gpio.PA,1,gpio.PA,2) 
static int lsht_init( lua_State* L ) {
    uint32_t pg_sck   = luaL_checkinteger( L, 1 );
    uint32_t num_sck  = luaL_checkinteger( L, 2 );
    uint32_t pg_data  = luaL_checkinteger( L, 3 );
    uint32_t num_data = luaL_checkinteger( L, 4 );
    if(pg_sck > 15 || pg_data > 15) {
        return luaL_error( L, "pin number range:0~15." );
    }
    if(num_sck > 8 || num_data > 8) {
        return luaL_error( L, "pin group range:0~8" );
    }
	Port_Sck  = LIB_GPIO_DEF[pg_sck];
	Port_Data = LIB_GPIO_DEF[pg_data];
	Num_sck   = (uint32_t)1 <<  num_sck;
	Num_Data  = (uint32_t)1 <<  num_data;
	
	if(PinIsUsed(pg_sck,num_sck) || PinIsUsed(pg_data,num_data)) {
        return luaL_error( L, "PIN is Used." );
    }

    GPIO_InitTypeDef  GPIO_InitStructure;
    if(!PinGroupIsEnable(pg_sck)) {
        uint32_t group = ((uint32_t)1 << pg_sck);
        RCC_AHB1PeriphClockCmd(group, ENABLE);
    }
    PinUse(GPIO_PIN,pg_sck,num_sck,PIN_STATE_USE);
    GPIO_InitStructure.GPIO_Pin = num_sck;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(Port_Sck, &GPIO_InitStructure);
    
    if(!PinGroupIsEnable(pg_data)) {
        uint32_t group = ((uint32_t)1 << pg_data);
        RCC_AHB1PeriphClockCmd(group, ENABLE);
    }
    PinUse(GPIO_PIN,pg_data,num_data,PIN_STATE_USE);
    GPIO_InitStructure.GPIO_Pin = num_data;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Port_Data, &GPIO_InitStructure);
	
    return 0;
}

static void Sht1xIICStart(void)
{
  SHT1x_SCK_Hi();
  SHT1x_DATA_Low();
  SHT1x_SCK_Low();
  SHT1x_SCK_Hi();
  SHT1x_DATA_Hi();
  SHT1x_SCK_Low();
}


static void Sht1xResetSequence(void)
{
  uint8_t i;
  SHT1x_DATA_Hi();
  for(i=0;i<9;i++){
      SHT1x_SCK_Hi();
      SHT1x_SCK_Low();
  }
  Sht1xIICStart();
}
uint8_t Sht1xIICSendData(uint8_t data)
{
	uint8_t i;
	for(i=0;i<8;i++){
		if(data & 0x80){
			SHT1x_DATA_Hi();
		}
		else{
			SHT1x_DATA_Low();
		}
		SHT1x_SCK_Hi();
		data <<= 1;
		SHT1x_SCK_Low();
	}
	SHT1x_SCK_Hi();
	SHT1x_DATA_Hi();
	if(!SHT1x_DATA_ISHI()){
		i = IICACK;
	}
	else{
		i = IICNAK;
	}
	SHT1x_SCK_Low();
	return i;
}

uint16_t Sht1xReadData(uint8_t ack)
{
	uint8_t i;
	uint16_t data;
	data = 0;
	for(i=0;i<8;i++){
		data <<= 1;
		SHT1x_SCK_Hi();
		if(SHT1x_DATA_ISHI()){
			data |= 0x0001;
		}
		SHT1x_SCK_Low();
	}
	SHT1x_DATA_Low();
	SHT1x_SCK_Hi();
	SHT1x_SCK_Low();
	SHT1x_DATA_Hi();
	for(i=0;i<8;i++){
		data <<= 1;
		SHT1x_SCK_Hi();
		if(SHT1x_DATA_ISHI()){
			data |= 0x0001;
		}
		SHT1x_SCK_Low();
	}
	if(ack){
		SHT1x_DATA_Low();
	}
	SHT1x_SCK_Hi();
	SHT1x_SCK_Low();
	SHT1x_DATA_Hi();
    return data;
}
//Sht_Temperature
//sht.temp()
int16_t Sht_Temp( lua_State* L )
{
	int16_t temperature;
	Sht1xIICStart();
	Sht1xIICSendData(READTEMPERATURE);
	DelayMs(500);//µÈ´ý 0.5S
	if(!SHT1x_DATA_ISHI()){
		temperature = Sht1xReadData(IICNAK);
	}
	temperature += 5;
	temperature /= 10;
	temperature -= 396;
    lua_pushinteger( L, temperature );
	return 1;
}
//sht.hum()
uint8_t Sht_Hum( lua_State* L )
{
	float humidity;
	Sht1xIICStart();
	Sht1xIICSendData(READHUMIDITY);
	DelayMs(500);//µÈ´ý 0.5S
	if(!SHT1x_DATA_ISHI()){
		humidity = Sht1xReadData(IICNAK);
	}
	humidity = -2.0648 + 0.0367*humidity - 1.5955E-6*humidity*humidity;
    lua_pushnumber( L, humidity );
	return 1;
}

// Module function map
static const luaL_Reg sht_map[] = {
  {"init"      , lsht_init       },
  {"temp"      , Sht_Temp        },
  {"hum"       , Sht_Hum         },
};

LUAMOD_API int luaopen_sht (lua_State *L) {
    luaL_newlib(L, sht_map);
}

