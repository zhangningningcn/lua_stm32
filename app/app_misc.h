#ifndef _APP_MISC_H
#define _APP_MISC_H
#ifdef STM32F4XX
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif

typedef struct TimeOutActionList_{
    int (*fun)(void *parameter);
    uint32_t actiontick;
    void *parameter;
    struct TimeOutActionList_ *next;
}TimeOutActionList;

uint32_t getSysTickValue(void);
void DelayMs(uint32_t ms);
uint32_t getTick(void);
void addRunList(TimeOutActionList *action);

void usbconnect(void);
void usbdisconnect(void);


#endif
