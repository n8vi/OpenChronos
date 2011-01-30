/* OpenChronos date/time routines - Brad J. Tarratt, N8VI 2011.01.23
 *
 * Here we deal with two representations of time, human and machine,
 * and three timescales:  TAI, UTC, local.  You want TAI/machine for
 * calculating the number of seconds between two times.  You'll want
 * local/human to tell the user about it.  Time is stored in the RTC
 * as UTC/human.  It is stored in human format because we cannot get
 * useful 1/min or 1/hour interrupts out of the RTC in counter mode.
 * It is stored in UTC because the RTC does not understand that some
 * minutes may have 60 seconds, thus can't represent some TAI times.
 * We compensate for this in the 1/minute interrupt handler.
 */

#include "project.h"
#ifndef RTC_H
#define RTC_H

#define machine_date u32
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

extern human_date *get_now_human_utc_date();
extern void set_now_human_utc_date(human_date *hd);

extern machine_date human_to_machine_date(human_date *hd);
extern human_date *machine_to_human_date(machine_date md);

extern machine_date local_to_tai_date(machine_date ld);
extern machine_date tai_to_local_date(machine_date td);
extern machine_date local_to_utc_date(machine_date ld);
extern machine_date utc_to_local_date(machine_date ud);
extern u32 get_leapseconds_utc(machine_date ud);
extern u32 get_leapseconds_tai(machine_date td);

#define utc_to_tai_date(ud) ( ud + get_leapseconds_utc(ud) )
#define tai_to_utc_date(td) ( ud + get_leapseconds_tai(td) )


#define get_now_human_local_date        ( machine_to_human_date(utc_to_local_date(human_to_machine_date(get_now_human_utc_date(  )))) ) 
#define get_now_human_tai_date          ( machine_to_human_date(utc_to_tai_date  (human_to_machine_date(get_now_human_utc_date(  )))) ) 
#define get_now_machine_local_date      (                      (utc_to_local_date(human_to_machine_date(get_now_human_utc_date(  )))) ) 
#define get_now_machine_tai_date        (                      (utc_to_tai_date  (human_to_machine_date(get_now_human_utc_date(  )))) ) 
#define get_now_machine_utc_date        (                      (                 (human_to_machine_date(get_now_human_utc_date(  )))) ) 

#define set_now_human_local_date(hd)    ( set_now_human_utc_date(machine_to_human_date(local_to_utc_date(human_to_machine_date(hd)))) )
#define set_now_human_tai_date(hd)      ( set_now_human_utc_date(machine_to_human_date(  tai_to_utc_date(human_to_machine_date(hd)))) )
#define set_now_machine_local_date(md)  ( set_now_human_utc_date(machine_to_human_date(local_to_utc_date(                     (md)))) )
#define set_now_machine_tai_date(md)    ( set_now_human_utc_date(machine_to_human_date(  tai_to_utc_date(                     (md)))) )
#define set_now_machine_utc_date(md)    ( set_now_human_utc_date(machine_to_human_date(                 (                     (md)))) )

#endif /* RTC_H */
