/* OpenChronos date/time routines - Brad J. Tarratt, N8VI 2011.01.23
 *
 * Here we deal with two representations of time, human and machine,
 * and three timescales:  TAI, UTC, local.  You want TAI/machine for
 * calculating the number of seconds between two times.  You'll want
 * local/human to tell the user about it.  Time is stored in the RTC
 * as UTC/human.  It is stored in human format because we cannot get
 * useful 1/min or 1/hour interrupts out of the RTC in counter mode.
 * It is stored in UTC because the RTC does not understand that some
 * minutes may have 61 seconds, thus can't represent some TAI times.
 * We compensate for this in the 1/minute interrupt handler.
 */

#include "project.h"
#ifndef RTC_H
#define RTC_H

struct _machine_date 
{
  u32 epochsecs;
  u8 isleapsec;
};

#define machine_date struct _machine_date;
/* yes, unsigned; posix compliance is not a goal */

struct _human_date
{
  u16 year;
  u16 yday;
  u8 mon;
  u8 mday;
  u8 wday;
  u8 hour; 
  u8 min;
  u8 sec;
};

#define human_date struct _human_date

/* don't call these directly, use the macros below */
extern human_date *read_rtc();
extern void write_rtc(human_date *hd);
extern u32 get_leapseconds_utc(machine_date ud);
extern u32 get_leapseconds_tai(machine_date td);
extern u32 get_tz_offset_local(machine_date lt);
extern u32 get_tz_offset_utc(machine_date ut);

extern machine_date human_to_machine_date(human_date *hd);
extern human_date *machine_to_human_date(machine_date md);

#define utc_to_tai_machine_date(ud)   ( ud + get_leapseconds_utc(ud) )
#define tai_to_utc_machine_date(td)   ( td + get_leapseconds_tai(td) )
#define local_to_utc_machine_date(ld) ( ld + get_offset_local   (ld) )
#define utc_to_local_machine_date(ud) ( ud + get_offset_utc     (ud) )
#define tai_to_local_machine_date(ld) ( utc_to_local_date ( tai_to_utc_date (ld) ) )
#define local_to_tai_machine_date(ud) ( utc_to_tai_date ( local_to_utc_date (ud) ) )

#define utc_machine_date_to_jdn(ud) ( (ud / 86400) + 2440587 + ((ud % 86400)/43200) )
#define utc_machine_date_to_jdn_fracpart(ud) ( (((ud+43200)%86400)*10000)/864  ) /* 6 digit precision */

#define get_now_human_utc_date          ( machine_to_human_date(                 (human_to_machine_date(read_rtc(  )))) )
#define get_now_human_local_date        ( machine_to_human_date(utc_to_local_date(human_to_machine_date(read_rtc(  )))) ) 
#define get_now_human_tai_date          ( machine_to_human_date(utc_to_tai_date  (human_to_machine_date(read_rtc(  )))) ) 
#define get_now_machine_local_date      (                      (utc_to_local_date(human_to_machine_date(read_rtc(  )))) ) 
#define get_now_machine_tai_date        (                      (utc_to_tai_date  (human_to_machine_date(read_rtc(  )))) ) 
#define get_now_machine_utc_date        (                      (                 (human_to_machine_date(read_rtc(  )))) ) 

#define set_now_human_utc_date(hd)      ( write_rtc(machine_to_human_date(                 (human_to_machine_date(hd)))) )
#define set_now_human_local_date(hd)    ( write_rtc(machine_to_human_date(local_to_utc_date(human_to_machine_date(hd)))) )
#define set_now_human_tai_date(hd)      ( write_rtc(machine_to_human_date(  tai_to_utc_date(human_to_machine_date(hd)))) )
#define set_now_machine_local_date(md)  ( write_rtc(machine_to_human_date(local_to_utc_date(                     (md)))) )
#define set_now_machine_tai_date(md)    ( write_rtc(machine_to_human_date(  tai_to_utc_date(                     (md)))) )
#define set_now_machine_utc_date(md)    ( write_rtc(machine_to_human_date(                 (                     (md)))) )

#endif /* RTC_H */
