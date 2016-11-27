#ifndef _RTC_H_
#define _RTC_H_
#include <stdint.h>
#include <string.h>
typedef uint32_t time_t;

struct tm
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
#ifdef __TM_GMTOFF
  long	__TM_GMTOFF;
#endif
#ifdef __TM_ZONE
  const char *__TM_ZONE;
#endif
};



void RTCInit(void);
time_t rtc_time(time_t *t);
time_t rtc_mktime(const struct tm *time);
struct tm *rtc_gmtime(time_t *time_s,struct tm *time);
struct tm *rtc_localtime(time_t *time_s,struct tm * time);
int32_t rtc_difftime(time_t time2, time_t time1);
size_t rtc_strftime (char* ptr, size_t maxsize, const char* format, const struct tm* timeptr );

#endif

