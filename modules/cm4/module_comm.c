#include "module_comm.h"

uint32_t LIB_GPIO_USED_TABLE[9];
uint32_t LIB_ADC_USED_TABLE[9];
uint32_t LIB_SYS_USED_TABLE[9];
void InitPinGroupTable(void) {
    for(int i=0;i<9;i++) {
        LIB_GPIO_USED_TABLE[i] = 0;
        LIB_ADC_USED_TABLE[i] = 0;
        LIB_SYS_USED_TABLE[i] = 0;
    }
}
int PinGroupIsEnable(int group) {
    if(LIB_GPIO_USED_TABLE[group]) return 1;
    if(LIB_ADC_USED_TABLE[group])  return 1;
    if(LIB_SYS_USED_TABLE[group])  return 1;
    return 0;
}
int PinIsUsed(int group,uint32_t pin) {
    if(LIB_GPIO_USED_TABLE[group] & pin) return GPIO_PIN;
    if(LIB_ADC_USED_TABLE[group] & pin)  return ADC_PIN;
    if(LIB_SYS_USED_TABLE[group] & pin)  return SYS_PIN;
    return 0;
}
int PinUse(int type,int group,uint32_t pin,int state) {
    switch(type) {
        case GPIO_PIN:
            if(state != PIN_STATE_UNUSE) {
                LIB_GPIO_USED_TABLE[group] |= pin;
            }
            else {
                LIB_GPIO_USED_TABLE[group] &= ~pin;
            }
        break;
        case ADC_PIN:
            if(state != PIN_STATE_UNUSE) {
                LIB_ADC_USED_TABLE[group] |= pin;
            }
            else {
                LIB_ADC_USED_TABLE[group] &= ~pin;
            }
        break;
        case SYS_PIN:
            if(state != PIN_STATE_UNUSE) {
                LIB_SYS_USED_TABLE[group] |= pin;
            }
            else {
                LIB_SYS_USED_TABLE[group] &= ~pin;
            }
        break;
        default:
        return -1;
    }
    return 0;
}
