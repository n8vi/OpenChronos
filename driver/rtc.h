/* OpenChronos date/time routines - Brad J. Tarratt, N8VI 2011.01.23
 *
 * Here we deal with two representations of time, cal and int,
 * and three timescales:  TAI, UTC, local.  You want TAI/int for
 * calculating the number of seconds between two times.  You'll want
 * local/cal to tell the user the time.  Time is stored in the RTC
 * as TAI/int.
 */

#include "project.h"
#ifndef RTC_H
#define RTC_H

struct _int_date 
{
  u32 epochsecs;
  u8 isleapsec;
};

struct _cal_date
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

#define int_date struct _int_date
#define cal_date struct _cal_date

/* don't call these directly, use the macros below */
extern int_date *read_rtc();
extern void write_rtc(int_date *id);

extern int_date *cal_to_int_date(cal_date *cd);
extern cal_date *int_to_cal_date(int_date *id);

#define get_leapseconds_tai(td) (td->epochsecs>NOSAVE_NEXT_LS_SWITCH?NOSAVE_NEXT_LS_OFFSET:NOSAVE_CURRENT_LS_OFFSET)
#define get_dstoffset_utc(ud)   (ud->epochsecs>NOSAVE_NEXT_DST_SWITCH?NOSAVE_NEXT_DST_OFFSET:NOSAVE_CURRENT_DST_OFFSET)
#define get_leapseconds_utc(ud) (ud->epochsecs-NOSAVE_NEXT_LS_OFFSET>NOSAVE_CUR_LS_SWITCH?NOSAVE_NEXT_LS_OFFSET:NOSAVE_CUR_LS_OFFSET)
#define get_dstoffset_local(ld) (ld->epochsecs-NOSAVE_NEXT_DST_OFFSET>NOSAVE_CUR_DST_SWITCH?NOSAVE_NEXT_DST_OFFSET:NOSAVE_CUR_DST_OFFSET)

#define utc_to_tai_int_date(ud)   ( ud->epochsecs + get_leapseconds_utc(ud->epochsecs) )
#define tai_to_utc_int_date(td)   ( td->epochsecs + get_leapseconds_tai(td->epochsecs) )
#define local_to_utc_int_date(ld) ( ld->epochsecs + get_dstoffset_local(ld->epochsecs) )
#define utc_to_local_int_date(ud) ( ud->epochsecs + get_dstoffset_utc  (ud->epochsecs) )
#define tai_to_local_int_date(td) ( utc_to_local_date ( tai_to_utc_date (td->epochsecs) ) )
#define local_to_tai_int_date(ld) ( utc_to_tai_date ( local_to_utc_date (ld->epochsecs) ) )

#define utc_int_to_jdn_date(ud)          ( (ud->epochsecs / 86400) + 2440587 + ((ud->epochsecs % 86400)/43200) )
#define utc_int_to_jdn_fracpart_date(ud) ( (((ud->epochsecs+43200)%86400)*10000)/864  ) /* 6 digit precision */
#define tai_int_to_jdn_date(ud)          ( utc_int_date_to_jdn(tai_to_utc_int_date(td)) )
#define tai_int_to_jdn_fracpart_date(ud) ( utc_int_date_to_jdn_fracpart(tai_to_utc_int_date(ud)) )

#define get_now_utc_cal_date             ( int_to_cal_date(tai_to_utc_int_date  (read_rtc(  ))) )
#define get_now_local_cal_date           ( int_to_cal_date(tai_to_local_int_date(read_rtc(  ))) ) 
#define get_now_tai_cal_date             ( int_to_cal_date(                     (read_rtc(  ))) ) 
#define get_now_local_int_date           (                (tai_to_local_int_date(read_rtc(  ))) ) 
#define get_now_tai_int_date             (                (                     (read_rtc(  ))) ) 
#define get_now_utc_int_date             (                (tai_to_utc_int_date  (read_rtc(  ))) ) 

#define set_now_utc_cal_date(cd)         ( write_rtc(utc_to_tai_int_date  (cal_to_int_date(cd))) )
#define set_now_local_cal_date(cd)       ( write_rtc(local_to_tai_int_date(cal_to_int_date(cd))) )
#define set_now_tai_cal_date(cd)         ( write_rtc(                     (cal_to_int_date(cd))) )
#define set_now_local_int_date(id)       ( write_rtc(local_to_tai_int_date(               (id))) )
#define set_now_tai_int_date(id)         ( write_rtc(                     (               (id))) )
#define set_now_utc_int_date(id)         ( write_rtc(utc_to_tai_int_date  (               (id))) )

#endif /* RTC_H */
