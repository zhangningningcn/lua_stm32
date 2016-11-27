#ifndef _APP_USART_H_
#define _APP_USART_H_
#ifdef STM32F4XX
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif
#include "user_define.h"

void USART_Lua_Init(void);


int USART_Lua_Recive(char * buf,int len);
int USART_Lua_Send(const char * buf,int len);
int USART_Lua_ReciveS(char * buf,int len);
int USART_Lua_SendS(const char * buf,char endchar);
int USART_Lua_Getchar(void);
int USART_Lua_UnGetchar(const char c);
int USART_Lua_RecBufferEOF(void);
int USART_Lua_RecTell(void);
int readline(char *indata,int maxlen,const char *msg);

typedef enum e_USART_Lua_STATE_{
    USART_Lua_STATE,
    USART_Lua_STATE_NORMAL,
    USART_Lua_STATE_TRANSFILE,
    USB_Lua_STATE,
    USB_Lua_STATE_NORMAL,
    USB_Lua_STATE_TRANSFILE,
    
}e_USART_Lua_STATE;

typedef enum e_USART_Lua_Error_{
    USART_Lua_No_Error = 0,USART_Lua_TimeOut_Error
    
}e_USART_Lua_Error;

void USART_Lua_SetState(e_USART_Lua_STATE state);

uint16_t VCP_DataRx (uint32_t Len);

#endif
