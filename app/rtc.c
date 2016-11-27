#ifdef STM32F4XX
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif
#include "rtc.h"
#include <string.h>

const int MonthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
//static uint32_t AsynchPrediv = 0, SynchPrediv = 0;
static RTC_TimeTypeDef  RTC_TimeStruct;
static RTC_DateTypeDef  RTC_DateStruct;
static void stm32time2tm(RTC_TimeTypeDef *rtctm,RTC_DateTypeDef *rtcdt,struct tm * time);

int TimeZone;
/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
// static void NVIC_Configuration(void)
// {
	// NVIC_InitTypeDef NVIC_InitStructure;

	// /* Configure one bit for preemption priority */
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	// /* Enable the RTC Interrupt */
	// NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
// }
/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
static void RTC_Configuration(void)
{
	#ifdef STM32F4XX
	
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  #else
	
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	#endif
}

void RTCInit(void) {
	
	
	#ifdef STM32F4XX
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
  {  
	RTC_InitTypeDef RTC_InitStructure;
    /* RTC configuration  */
    RTC_Configuration();

    //SynchPrediv = 0xFF;
    //AsynchPrediv = 0x7F;
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      //printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
    }

    RTC_TimeStruct.RTC_H12     = 0;
    RTC_TimeStruct.RTC_Hours   = 0x00;
    RTC_TimeStruct.RTC_Minutes = 0x00;
    RTC_TimeStruct.RTC_Seconds = 0x00;  
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);
	
    RTC_DateStruct.RTC_WeekDay = 7;
    RTC_DateStruct.RTC_Month   = 0x01;
    RTC_DateStruct.RTC_Date    = 0x01;
    RTC_DateStruct.RTC_Year    = 0x17;  
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);
    /* Configure the time register */
    //RTC_TimeRegulate(); 
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
	RTC_WriteBackupRegister(RTC_BKP_DR1, 480);
	TimeZone = 480;
  }
  else
  {
    //printf("\n\r No need to configure RTC....\n\r");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Clear the RTC Alarm Flag */
    //RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
    //EXTI_ClearITPendingBit(EXTI_Line17);

  }
  #else
	
	
	//NVIC_Configuration();
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		/* Backup data register value is not correct or not yet programmed (when
		the first time the program is executed) */

		/* RTC Configuration */
		RTC_Configuration();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		BKP_WriteBackupRegister(BKP_DR2, 480);
		TimeZone = 480;
	}
	else
	{
		// No need to configure RTC
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		TimeZone = BKP_ReadBackupRegister(BKP_DR2);
	}


	/* Clear reset flags */
	RCC_ClearFlag();
	#endif
}
#ifdef STM32F4XX
static void stm32time2tm(RTC_TimeTypeDef *rtctm,RTC_DateTypeDef *rtcdt,struct tm * time) {
	
	time->tm_year = rtcdt->RTC_Year+2000;
	time->tm_mon  = rtcdt->RTC_Month;
	time->tm_mday = rtcdt->RTC_Date;
	time->tm_hour = rtctm->RTC_Hours;
	time->tm_min  = rtctm->RTC_Minutes;
	time->tm_sec  = rtctm->RTC_Seconds;
}
#endif
time_t rtc_time(time_t *t) {
	time_t cseconds;
	#ifdef STM32F4XX
	struct tm time1;
	//RTC_GetTime() and RTC_GetDate()
	//uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
	stm32time2tm(&RTC_TimeStruct,&RTC_DateStruct,&time1);
	cseconds = rtc_mktime(&time1);
	#else
	RTC_WaitForLastTask();
	cseconds = RTC_GetCounter();
	#endif
	if(t != NULL) {
		*t = cseconds;
	}
	return cseconds;
	
}
time_t rtc_mktime(const struct tm *time)
{
	u32 year;
	u32 mon;
	u32 day;
	u32 hour;
	u32 min;
	u32 sec;

	year = time->tm_year;
	mon  = time->tm_mon;
	day  = time->tm_mday;
	hour = time->tm_hour;
	min  = time->tm_min;
	sec  = time->tm_sec;

    if (0 >= (s32) (mon -= 2)){    /**//* 1..12 -> 11,12,1..10 */
         mon += 12;      /**//* Puts Feb last since it has leap day */
         year -= 1;
    }

    return (((
             (unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
             year*365ul - 719499
          )*24ul + hour /**//* now have hours */
       )*60ul + min /**//* now have minutes */
    )*60ul + sec; /**//* finally seconds */
}

struct tm * rtc_gmtime(time_t *time_s,struct tm *time)
{
	//,struct tm time
	u32 year1,day2;
	u32 month;
	time->tm_sec = *time_s % 60ul;
	*time_s /= 60;
	time->tm_min = *time_s % 60ul;
	*time_s /= 60;
	time->tm_hour = *time_s % 24;
	*time_s /= 24;
	time->tm_wday = (*time_s+4)%7ul;
	year1= *time_s/(365*4+1);
	day2 = *time_s%(365*4+1);
	if(day2 == 789)
	{
		time->tm_year = year1*4 + 2 +1970;
		month = 2;
		day2 = 28;
	}
	else{
		if(day2 > 788){ day2--;}
		time->tm_year = year1*4 + day2/365 +1970;
		day2 %= 365;
		month = 1;
		while(day2 >= MonthDays[month])
		{
			day2 -= MonthDays[month];
			month++;
		}
	}
	time->tm_mon = month;
	time->tm_mday = day2+1;
    time->tm_isdst = 0;
	//sec  = time->tm_sec;
	return time;
}
struct tm *rtc_localtime(time_t *time_s,struct tm *time) {
	int32_t timezonesecond;
	timezonesecond = TimeZone;
	timezonesecond *= 60;
	*time_s += timezonesecond;
	rtc_gmtime(time_s,time);
	return time;
}
int32_t rtc_difftime(time_t time2, time_t time1) {
	return (int32_t)(time2-time1);
}
/*

%a 星期几的简写 
%A 星期几的全称 
%b 月分的简写 
%B 月份的全称 
%c 标准的日期的时间串 
%C 年份的后两位数字 
%d 十进制表示的每月的第几天 
%D 月/天/年 
%e 在两字符域中，十进制表示的每月的第几天 
%F 年-月-日 
%g 年份的后两位数字，使用基于周的年 
%G 年分，使用基于周的年 
%h 简写的月份名 
%H 24小时制的小时 
%I 12小时制的小时
%j 十进制表示的每年的第几天 
%m 十进制表示的月份 
%M 十时制表示的分钟数 
%n 新行符 
%p 本地的AM或PM的等价显示 
%r 12小时的时间 
%R 显示小时和分钟：hh:mm 
%S 十进制的秒数 
%t 水平制表符 
%T 显示时分秒：hh:mm:ss 
%u 每周的第几天，星期一为第一天 （值从0到6，星期一为0）
%U 第年的第几周，把星期日做为第一天（值从0到53）
%V 每年的第几周，使用基于周的年 
%w 十进制表示的星期几（值从0到6，星期天为0）
%W 每年的第几周，把星期一做为第一天（值从0到53） 
%x 标准的日期串 
%X 标准的时间串 
%y 不带世纪的十进制年份（值从0到99）
%Y 带世纪部分的十制年份 
%z，%Z 时区名称，如果不能得到时区名称则返回空字符。
%% 百分号
*/

#define TIME_BUFFER_SIZE_CHECK(A) \
if(maxsize < A+1) { \
	break; \
} \
maxsize -= A

size_t rtc_strftime (char* ptr, size_t maxsize, const char* format, const struct tm* timeptr )
{
	int temp;
    char *temp_ptr = ptr;
	while(*format) {
		if(*format != '%') {
			*ptr++ = *format;
			TIME_BUFFER_SIZE_CHECK(1);
		}
		else{
			format++;
			switch(*format) {
				//case 'a':
				//break;
				//case 'A':
				//break;
				//case 'b':
				//break;
				//case 'B':
				//break;
				case 'c':
				TIME_BUFFER_SIZE_CHECK(4);
				*ptr++ = timeptr->tm_year/1000 + '0';
				*ptr++ = timeptr->tm_year%1000/100 + '0';
				*ptr++ = timeptr->tm_year%100/10 + '0';
				*ptr++ = timeptr->tm_year%10 + '0';
				break;
				case 'C':
				TIME_BUFFER_SIZE_CHECK(2);
				*ptr++ = timeptr->tm_year%100/10 + '0';
				*ptr++ = timeptr->tm_year%10 + '0';
				break;
				case 'd':
				TIME_BUFFER_SIZE_CHECK(2);
				if(timeptr->tm_mday > 9) {
					*ptr++ = timeptr->tm_mday/10 + '0';
				}
				*ptr++ = timeptr->tm_mday%10 + '0';
				break;
				case 'D':
				TIME_BUFFER_SIZE_CHECK(8);
				if(timeptr->tm_mon > 9) {
					*ptr++ = timeptr->tm_mon/10 + '0';
				}
				*ptr++ = timeptr->tm_mon%10 + '0';
				*ptr++ = '/';
				if(timeptr->tm_mday > 9) {
					*ptr++ = timeptr->tm_mday/10 + '0';
				}
				*ptr++ = timeptr->tm_mday%10 + '0';
				*ptr++ = '/';
				*ptr++ = timeptr->tm_year%100/10 + '0';
				*ptr++ = timeptr->tm_year%10 + '0';
				break;
				case 'e':
				TIME_BUFFER_SIZE_CHECK(2);
				*ptr++ = timeptr->tm_mday/10 + '0';
				*ptr++ = timeptr->tm_mday%10 + '0';
				break;
				case 'F':
				TIME_BUFFER_SIZE_CHECK(8);
				*ptr++ = timeptr->tm_year%100/10 + '0';
				*ptr++ = timeptr->tm_year%10 + '0';
				*ptr++ = '-';
				if(timeptr->tm_mon > 9) {
					*ptr++ = timeptr->tm_mon/10 + '0';
				}
				*ptr++ = timeptr->tm_mon%10 + '0';
				*ptr++ = '-';
				if(timeptr->tm_mday > 9) {
					*ptr++ = timeptr->tm_mday/10 + '0';
				}
				break;
				//case 'g':
				//break;
				//case 'G':
				//break;
				//case 'h':
				//break;
				case 'H':
				TIME_BUFFER_SIZE_CHECK(2);
				*ptr++ = timeptr->tm_hour/10 + '0';
				*ptr++ = timeptr->tm_hour%10 + '0';
				break;
				case 'I':
				TIME_BUFFER_SIZE_CHECK(2);
				temp = timeptr->tm_hour;
				if(temp > 13) {
					temp -= 12;
				}
				*ptr++ = temp/10 + '0';
				*ptr++ = temp%10 + '0';
				break;
				case 'j':
				break;
				case 'm':
				TIME_BUFFER_SIZE_CHECK(2);
				if(timeptr->tm_mon > 9) {
					*ptr++ = timeptr->tm_mon/10 + '0';
				}
				*ptr++ = timeptr->tm_mon%10 + '0';
				break;
				case 'M':
				TIME_BUFFER_SIZE_CHECK(2);
				*ptr++ = timeptr->tm_min/10 + '0';
				*ptr++ = timeptr->tm_min%10 + '0';
				break;
				case 'n':
				TIME_BUFFER_SIZE_CHECK(1);
				*ptr++ = '\n';
				break;
				case 'p':
				TIME_BUFFER_SIZE_CHECK(2);
				if(timeptr->tm_hour < 12) {
					*ptr++ = 'A';
				}
				else {
					*ptr++ = 'P';
				}
				*ptr++ = 'M';
				break;
				case 'r':
				TIME_BUFFER_SIZE_CHECK(11);
				temp = timeptr->tm_hour;
				if(temp > 13) {
					temp -= 12;
				}
				*ptr++ = temp/10 + '0';
				*ptr++ = temp%10 + '0';
				*ptr++ = ':';
				*ptr++ = timeptr->tm_min/10 + '0';
				*ptr++ = timeptr->tm_min%10 + '0';
				*ptr++ = ':';
				*ptr++ = timeptr->tm_sec/10 + '0';
				*ptr++ = timeptr->tm_sec%10 + '0';
				*ptr++ = ' ';
				if(timeptr->tm_hour < 12) {
					*ptr++ = 'A';
				}
				else {
					*ptr++ = 'P';
				}
				*ptr++ = 'M';
				break;
				case 'R':
				TIME_BUFFER_SIZE_CHECK(5);
				*ptr++ = timeptr->tm_hour/10 + '0';
				*ptr++ = timeptr->tm_hour%10 + '0';
				*ptr++ = ':';
				*ptr++ = timeptr->tm_min/10 + '0';
				*ptr++ = timeptr->tm_min%10 + '0';
				break;
				case 'S':
				TIME_BUFFER_SIZE_CHECK(2);
				*ptr++ = timeptr->tm_sec/10 + '0';
				*ptr++ = timeptr->tm_sec%10 + '0';
				break;
				case 't':
				TIME_BUFFER_SIZE_CHECK(1);
				*ptr++ = '\t';
				break;
				case 'T':
				TIME_BUFFER_SIZE_CHECK(8);
				*ptr++ = timeptr->tm_hour/10 + '0';
				*ptr++ = timeptr->tm_hour%10 + '0';
				*ptr++ = ':';
				*ptr++ = timeptr->tm_min/10 + '0';
				*ptr++ = timeptr->tm_min%10 + '0';
				*ptr++ = ':';
				*ptr++ = timeptr->tm_sec/10 + '0';
				*ptr++ = timeptr->tm_sec%10 + '0';
				break;
				case 'u':
				TIME_BUFFER_SIZE_CHECK(1);
				if(timeptr->tm_wday > 0) {
					*ptr++ = timeptr->tm_wday-1 + '0';
				}
				else{
					*ptr++ = '6';
				}
				break;
				case 'U': //第年的第几周，把星期日做为第一天（值从0到53）
				break;
				//case 'V': //每年的第几周，使用基于周的年 
				//break;
				case 'w': //十进制表示的星期几（值从0到6，星期天为0）
				TIME_BUFFER_SIZE_CHECK(2);
				if(timeptr->tm_wday > 6) {
					*ptr++ = '0';
				}
				else{
					*ptr++ = timeptr->tm_wday + '0';
				}
				break;
				case 'W': //每年的第几周，把星期一做为第一天（值从0到53） 
				{
					time_t cseconds1,cseconds2;
					struct tm time2;
					time2.tm_year = timeptr->tm_year;
					time2.tm_mon = 1;
					time2.tm_mday = 1;
					time2.tm_hour = 0;
					time2.tm_min = 0;
					time2.tm_sec = 0;
					cseconds1 = rtc_mktime(timeptr);
					cseconds2 = rtc_mktime(&time2);
					rtc_gmtime(&cseconds2,&time2);
					cseconds1 =- cseconds2;
					cseconds1 /= (3600*24);
					cseconds1 += time2.tm_wday;
				}
				break;
				case 'x': //标准的日期串 
				break;
				case 'X': //标准的时间串 
				break;
				case 'y':
				TIME_BUFFER_SIZE_CHECK(2);
				*ptr++ = timeptr->tm_year%100/10 + '0';
				*ptr++ = timeptr->tm_year%10 + '0';
				break;
				case 'Y':
				TIME_BUFFER_SIZE_CHECK(4);
				*ptr++ = '2';
				*ptr++ = '0';
				*ptr++ = timeptr->tm_year%100/10 + '0';
				*ptr++ = timeptr->tm_year%10 + '0';
				break;
				case 'z':
				break;
				default:
				TIME_BUFFER_SIZE_CHECK(1);
				*ptr++ = *format;
				break;
			}
		}
		format++;
		//ptr++;
	}
	*ptr = '\0';
	return strlen(temp_ptr);
}
