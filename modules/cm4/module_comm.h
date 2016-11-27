#ifndef _MODULE_COMM_H_
#define _MODULE_COMM_H_
#include <stdint.h>

void InitPinGroupTable(void);
int PinGroupIsEnable(int group);
int PinUse(int type,int group,uint32_t pin,int state);
int PinIsUsed(int group,uint32_t pin);

extern uint32_t LIB_GPIO_USED_TABLE[9];
extern uint32_t LIB_ADC_USED_TABLE[9];
#define GPIO_PIN  1
#define ADC_PIN   2
#define SYS_PIN   3

#define PIN_STATE_USE    1
#define PIN_STATE_UNUSE  0


#define MODULE_GPIOA    0
#define MODULE_GPIOB    1
#define MODULE_GPIOC    2
#define MODULE_GPIOD    3
#define MODULE_GPIOE    4
#define MODULE_GPIOF    5
#define MODULE_GPIOG    6
#define MODULE_GPIOH    7
#define MODULE_GPIOI    8

#define MODULE_IO_ENABLE  1
#define MODULE_IO_DISABLE 0


#endif
