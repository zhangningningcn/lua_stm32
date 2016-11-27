#ifndef _PARAMS_H_
#define _PARAMS_H_

typedef  uint16_t  u16_t;
typedef  uint8_t   u8_t;
typedef  uint32_t  u32_t;
typedef  int16_t   s16_t;
typedef  int8_t    s8_t;
typedef  int32_t   s32_t;


// Turn off stats
#define SPIFFS_CACHE_STATS 	    0
#define SPIFFS_GC_STATS         0
#define SPIFFS_TEST_VISUALISATION 0
//#define SPIFFS_CACHE            0

#define SPIFFS_FILEHDL_OFFSET   4

#define SPIFFS_HAL_CALLBACK_EXTRA       1

#endif
