/* OpenChronos date/time routines - Brad J. Tarratt, N8VI 2011.01.23
 *
 * Here we deal with two representations of time, human and machine,
 * and three timescales:  TAI, UTC, local.  You want TAI/machine for
 * calculating the number of seconds between two times.  You'll want
 * local/human to tell the user the time.  Time is stored in the RTC
 * as TAI/machine.
 */

#include "project.h"
#ifndef RTC_H
#define RTC_H

struct _machine_date 
{
  u32 epochsecs;
  u8 isleapsec;
};

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

#define machine_date struct _machine_date
#define human_date struct _human_date

/*
  relevant defines:
  CONFIG_RTC
  NOSAVE_CURRENT_DST_OFFSET
  NOSAVE_NEXT_DST_SWITCH
  NOSAVE_NEXT_DST_OFFSET
  NOSAVE_NEXT_LS_OFFSET
  NOSAVE_NEXT_LS_SWITCH
  NOSAVE_CURRENT_LS_OFFSET
*/

/* don't call these directly, use the macros below */
extern machine_date *read_rtc();
extern void write_rtc(machine_date *md);

extern machine_date *human_to_machine_date(human_date *hd);
extern human_date *machine_to_human_date(machine_date *md);

#define get_leapseconds_tai(td) (td.epochsecs>NOSAVE_NEXT_LS_SWITCH?NOSAVE_NEXT_LS_OFFSET:NOSAVE_CURRENT_LS_OFFSET)
#define get_dstoffset_utc(ud) (ud.epochsecs>NOSAVE_NEXT_DST_SWITCH?NOSAVE_NEXT_DST_OFFSET:NOSAVE_CURRENT_DST_OFFSET)
#define get_leapseconds_utc(ud) (ud.epochsecs-NOSAVE_NEXT_LS_OFFSET>NOSAVE_CUR_LS_SWITCH?NOSAVE_NEXT_LS_OFFSET:NOSAVE_CUR_LS_OFFSET)
#define get_dstoffset_local(ud) (ud.epochsecs-NOSAVE_NEXT_DST_OFFSET>NOSAVE_CUR_DST_SWITCH?NOSAVE_NEXT_DST_OFFSET:NOSAVE_CUR_DST_OFFSET)

#define utc_to_tai_machine_date(ud)   ( ud.epochsecs + get_leapseconds_utc(ud.epochsecs) )
#define tai_to_utc_machine_date(td)   ( td.epochsecs + get_leapseconds_tai(td.epochsecs) )
#define local_to_utc_machine_date(ld) ( ld.epochsecs + get_dstoffset_local(ld.epochsecs) )
#define utc_to_local_machine_date(ud) ( ud.epochsecs + get_dstoffset_utc  (ud.epochsecs) )
#define tai_to_local_machine_date(ld) ( utc_to_local_date ( tai_to_utc_date (ld.epochsecs) ) )
#define local_to_tai_machine_date(ud) ( utc_to_tai_date ( local_to_utc_date (ud.epochsecs) ) )

#define utc_machine_date_to_jdn(ud) ( (ud.epochsecs / 86400) + 2440587 + ((ud.epochsecs % 86400)/43200) )
#define utc_machine_date_to_jdn_fracpart(ud) ( (((ud.epochsecs+43200)%86400)*10000)/864  ) /* 6 digit precision */
#define tai_machine_date_to_jdn(ud) ( utc_machine_date_to_jdn(tai_to_utc_machine_date(td)) )
#define tai_machine_date_to_jdn_fracpart(ud) ( utc_machine_date_to_jdn_fracpart(tai_to_utc_machine_date(ud)) )

#define get_now_human_utc_date          ( machine_to_human_date(tai_to_utc_machine_date  (read_rtc(  ))) )
#define get_now_human_local_date        ( machine_to_human_date(tai_to_local_machine_date(read_rtc(  ))) ) 
#define get_now_human_tai_date          ( machine_to_human_date(                         (read_rtc(  ))) ) 
#define get_now_machine_local_date      (                      (tai_to_local_machine_date(read_rtc(  ))) ) 
#define get_now_machine_tai_date        (                      (                         (read_rtc(  ))) ) 
#define get_now_machine_utc_date        (                      (tai_to_utc_machine_date  (read_rtc(  ))) ) 

#define set_now_human_utc_date(hd)      ( write_rtc(utc_to_tai_machine_date  (human_to_machine_date(hd))) )
#define set_now_human_local_date(hd)    ( write_rtc(local_to_tai_machine_date(human_to_machine_date(hd))) )
#define set_now_human_tai_date(hd)      ( write_rtc(                         (human_to_machine_date(hd))) )
#define set_now_machine_local_date(md)  ( write_rtc(local_to_tai_machine_date(                     (md))) )
#define set_now_machine_tai_date(md)    ( write_rtc(                         (                     (md))) )
#define set_now_machine_utc_date(md)    ( write_rtc(utc_to_tai_machine_date  (                     (md))) )

#endif /* RTC_H */
