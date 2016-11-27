//#include "stm32f10x.h"
#include "app_misc.h"
#include "app_usart.h"
#include "module_led.h"
#if ! defined( __CC_ARM )
#include <sys/stat.h>
#endif
#include <errno.h>
#include <stdlib.h>

static uint32_t DelayMsCounter = 0;
static uint32_t SysTickCounter = 0;
static TimeOutActionList *TimeOutActionHead=NULL;

uint32_t getSysTickValue(void) {
    return SysTick->VAL;
}

void DelayMs(uint32_t ms) {
    DelayMsCounter = ms;
    while(DelayMsCounter > 0);
}


void SysTick_Handler(void)
{
    SysTickCounter++;
    if(DelayMsCounter > 0) {
        DelayMsCounter--;
    }
    if(TimeOutActionHead != NULL) {
        if(TimeOutActionHead->actiontick == SysTickCounter) {
            TimeOutActionList *action,*nextaction;
            action = TimeOutActionHead;
            while(action->actiontick == SysTickCounter) {
                nextaction = action->next;
                addRunList(action);
                action = nextaction;
            }
            TimeOutActionHead = action;
        }
    }
}
uint32_t getTick(void) {
    return SysTickCounter;
}


int addTimeOutAction(int (*fun)(void *parameter),uint32_t ticks,void *parameter) {
    TimeOutActionList *action = malloc(sizeof(TimeOutActionList));
    action->fun = fun;
    action->parameter = parameter;
    action->actiontick = SysTickCounter + ticks;
    action->next = NULL;
    if(TimeOutActionHead == NULL) {
        TimeOutActionHead = action;
    }
    else {
        TimeOutActionList *action1,*action2;
        action2 = action1 = TimeOutActionHead;
        
        while(action1 != NULL) {
            if((int32_t)(action1->actiontick - action->actiontick) > 0) {
                break;
            }
            action2 = action1;
            action1 = action1->next;
        }
        if(TimeOutActionHead == action1) {
            TimeOutActionHead = action;
            action->next = action1;
        }
        else {
            action2->next = action;
            action->next = action1;
        }
    }
    //free(action);
    return 0;
}
void usbconnect(void) {
    if(LED_Func_Terminal & PIN_LED1) {
        PORT_LED1->BSRRH = PIN_LED1;
    }
    if(LED_Func_Terminal & PIN_LED2) {
        PORT_LED2->BSRRL = PIN_LED2;
    }
}
void usbdisconnect(void) {
    if(LED_Func_Terminal & PIN_LED1) {
        PORT_LED1->BSRRL = PIN_LED1;
    }
    if(LED_Func_Terminal & PIN_LED2) {
        PORT_LED2->BSRRH = PIN_LED2;
    }
}
#if ! defined( __CC_ARM )
char *itoa(int value, char *str, int base);
void _exit(int code) {
	char strcode[10];
	USART_Lua_SendS("Application Exit",'\0');
	itoa(code,strcode,10);
	USART_Lua_SendS(strcode,'\n');
	while(1);
}
caddr_t _sbrk(int incr) {
	extern char _end; /* Defined by the linker */
	extern char __HeapLimit;
	static char *heap_end;
	char *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
	if (heap_end + incr > &__HeapLimit) {
		//write (1, "Heap and stack collision\n", 25);
		USART_Lua_SendS("Memory out in stack",'\n');
		abort ();
	}
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}
#undef errno
extern int errno;
int _kill(int pid, int sig) {
	errno = EINVAL;
	return -1;
}
int _getpid(void) {
	return 1;
}
int _write(int file, char *ptr, int len) {
	char strcode[10];
	USART_Lua_SendS("f:",'\0');
	itoa(file,strcode,10);
	USART_Lua_SendS(strcode,'\n');
	USART_Lua_Send(ptr,len);
	return len;
}
int _close(int file) {
	return -1;
}
int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}
int _isatty(int file) {
	return 1;
}
int _lseek(int file, int ptr, int dir) {
	return 0;
}
int _read(int file, char *ptr, int len) {
	return 0;
}
#ifdef _LIBM_NOT_WORK
double pow(double x,double y) {
	return 0.f;
}
double floor(double x) {
	return 0.f;
}
double fmod(double x,double y) {
	return 0.f;
}
#endif

#endif
