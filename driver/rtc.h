/* OpenChronos date/time routines - Brad J. Tarratt, N8VI 2011.01.23
 *
 * Here we deal with two representations of time, human and machine,
 * and three timescales:  TAI, UTC, local.  You want TAI/machine for
 * calculating the number of seconds between two times.  You'll want
 * local/human to tell the user about it.  Time is stored in the RTC
 * as UTC/human.  It is stored in human format because we cannot get
 * useful 1/min or 1/hour interrupts out of the RTC in counter mode.
 */

#include "project.h"
#ifndef RTC_H
#define RTC_H

#define machine_date u32;
/* yes, unsigned; posix compliance is not a goal */

struct _human_date
{
  u16 year;
  u8 month;
  u8 mday;
  u8 wday;
  u8 hour; 
  u8 minute;
  u8 second;
} human_date;


extern human_date *get_now_human_utc();
extern void set_now_human_utc(human_date *hd);

extern human_date *get_now_human_local();
extern human_date *get_now_human_tai();
extern human_date *get_now_machine_local();
extern human_date *get_now_machine_utc();
extern human_date *get_now_machine_tai();
extern void set_now_human_local(human_date *hd);
extern void set_now_human_tai(human_date *hd);
extern void set_now_machine_local(machine_date md);
extern void set_now_machine_utc(machine_date md);
extern void set_now_machine_tai(machine_date md);

extern machine_date human_to_machine_date(human_date *hd);
extern human_date *machine_to_human_date(machine_date md);
local_to_tai_date
tai_to_local_date
local_to_utc_date
utc_to_local_date
utc_to_tai_date
tai_to_utc_date


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


#endif /* RTC_H */
