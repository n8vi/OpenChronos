/* rtc service routines */

// Include section

// system
#include "project.h"
#ifdef CONFIG_RTC
#include "rtc.h"

void setRTC(u16 year, u8 month, u8 mday, u8 hour, u8 minute, u8 second)
{
  RTCCTL1 = 0x60; // RTCMODE + RTCHOLD;
  RTCSEC = second;
  RTCMIN = minute;
  RTCHOUR = hour;
  RTCDAY = mday;
  RTCMON = month;
  RTCYEARH = year / 0x100;
  RTCYEARL = year % 0x100;
  RTCCTL1 &= 0xb0; // ~RTCHOLD;
}

void getRTC(u16 *year, 


#endif /* CONFIG_RTC */
