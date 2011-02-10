/* OpenChronos date/time routines - Brad J. Tarratt, N8VI 2011.01.23
 *
 * Here we deal with two representations of time, human and machine,
 * and three timescales:  TAI, UTC, local.  You want TAI/machine for
 * calculating the number of seconds between two times.  You'll want
 * local/human to tell the user the time.  Time is stored in the RTC
 * as TAI/machine.
 */

// Include section

// system
#include <stdlib.h>
#include "project.h"
#ifdef CONFIG_RTC
#include "rtc.h"

/*
  relevant defines:
  CONFIG_RTC
  NOSAVE_CUR_OFS
  NOSAVE_RTCNEXTSW
  NOSAVE_NEXT_OFS
*/

static u32 leapsecs_utc[] = {       /* this will only be a hardcoded array 
  0,          10,  // jan 1  1970    * until I figure out the right way to 
  78753600,   11,  // jun 30 1972    * update it.
  94651200,   12,  // dec 31 1972    */
  126187200,  13,  // dec 31 1972
  157723200,  14,  // dec 31 1974
  189259200,  15,  // dec 31 1975
  220881600,  16,  // dec 31 1976
  252417600,  17,  // dec 31 1977
  283953600,  18,  // dec 31 1978
  315489600,  19,  // dec 31 1979
  331214400,  20,  // jun 30 1980
  394286400,  21,  // jun 30 1982
  425822400,  22,  // jun 30 1983
  488980800,  23,  // jun 30 1985
  567950400,  24,  // dec 31 1987
  631108800,  25,  // dec 31 1989
  662644800,  26,  // dec 31 1990
  709905600,  27,  // jun 30 1992
  741441600,  28,  // jun 30 1993
  772977600,  29,  // jun 30 1994
  504878400,  30,  // dec 31 1995
  867672000,  31,  // jun 30 1997
  915105600,  32,  // dec 31 1998
  1136030400, 33,  // dec 31 2005
  1230724800, 34   // dec 31 2008
};

static u32 *leapsecs_tai;

void init_rtc(void)
{
  static human_date hd; /* statics initialized to zeros */
  int i;
  leapsecs_tai = (u32 *)malloc(sizeof(leapsecs_utc)/2);

  for (i=0; i<(sizeof(leapsecs_utc)); i+=2) {
    leapsecs_tai[i] = leapsecs_utc[i]+leapsecs_utc[i+1];
    }

  /* init things that can't be zero */
  hd.mon = 1;
  hd.mday = 1;
  hd.wday = 4;

  write_rtc(&hd);
}

human_date *read_rtc(void)
{
  static human_date hd;

  hd.year = RTCYEARH * 0x100 + RTCYEARL;
  hd.mon = RTCMON;
  hd.mday = RTCDAY;
  hd.wday = RTCDOW;
  hd.hour = RTCHOUR;
  hd.min = RTCMIN;
  hd.sec = RTCSEC;
}

void write_rtc(human_date *hd)
{
  RTCCTL1 = 0x60; // RTCMODE + RTCHOLD;
  RTCYEARH = hd->year / 0x100;
  RTCYEARL = hd->year % 0x100;
  RTCMON = hd->mon;
  if (hd->mday)
    RTCDAY = hd->mday;
  if (hd->wday)
    RTCDOW = hd->wday;
  RTCHOUR = hd->hour;
  RTCMIN = hd->min;
  RTCSEC = hd->sec;
  RTCCTL1 &= 0xb0; // ~RTCHOLD;
}

machine_date *human_to_machine_date(human_date *hd)
{
  u32 yday,m,c,q,cq,e,epochdays;

  u8 mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};

  static machine_date result;

  yday = 0;

  if ((hd->year % 4 == 0 && hd->year % 100 != 0) || hd->year % 400 == 0) {
    mdays[1] = 29;
    }

  for (m=0; m<hd->mon-1; m++)
    yday += mdays[m];

  yday += hd->mday;

  c = (hd->year/100 - 19);

  cq = (hd->year/400 - 4);

  q = (hd->year - 1969) / 4;

  e = q-c+cq;

  epochdays = 365 * (hd->year - 1970) + e + yday - 1;

  result.epochsecs = 3600*hd->hour + 60*hd->min + hd->sec + (epochdays * 86400);
  result.isleapsec = 0;

  return &result;
}

human_date *machine_to_human_date(machine_date *md)
{
  u32 dsecs, y,m,epochdays;
  u8 mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  static human_date ret;

  epochdays = md->epochsecs / 86400;
  ret.wday = (epochdays + 4) %7;

  dsecs = md->epochsecs % 86400;

  ret.hour = dsecs / 3600;
  ret.min = (dsecs / 60) % 60;
  ret.sec = dsecs % 60;

  ret.year = 1970;

  for (y=1970; y<2106; y++) {
    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) {
      if (epochdays < 366)
        break;
      epochdays -= 366;
      ret.year++;
    } else {
      if (epochdays < 365)
        break;
      epochdays -= 365;
      ret.year++;
      }
    }

  ret.yday = epochdays;

  if ((ret.year % 4 == 0 && ret.year % 100 != 0) || ret.year % 400 == 0) {
    mdays[1] = 29;
    }
  for (m=0; m<=11; m++) {
    if (epochdays < mdays[m]) {
      ret.mday = epochdays+1;
      ret.mon = m+1;
      break;
      }
    epochdays -= mdays[m];
    }

  if (md->isleapsec)
    ret.sec++;

  return &ret;
}

extern u32 get_leapseconds_utc(machine_date *ud)
{
  u32 ret, i;
  ret = 0;
  for (i=0; i<sizeof(leapsecs_utc); i+=2) {
    if (leapsecs_utc[i] < ud->epochsecs)
      return ret;
    ret = leapsecs_utc[i+1];
    }
  return ret;
}

extern u32 get_leapseconds_tai(machine_date *td)
{
  u32 ret, i;
  ret = 0;
  for (i=0; i<sizeof(leapsecs_tai); i++) {
    if (leapsecs_tai[i] < td->epochsecs)
      return ret;
    ret = leapsecs_utc[i*2+1];
    }
  return ret;
}

/*
extern u32 get_offset_local(machine_date *lt)
{
  // not sure if this makes sense to implement
  return 0;
}
*/

extern u32 get_offset_utc(machine_date *ut)
{
  if (ut->epochsecs > NOSAVE_NEXT_SWITCH) {
    return NOSAVE_NEXT_OFFSET;
  } else {
    return NOSAVE_CURRENT_OFFSET;
    }
}

#endif /* CONFIG_RTC */
