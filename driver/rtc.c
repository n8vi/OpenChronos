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
  NOSAVE_CURRENT_DST_OFFSET
  NOSAVE_NEXT_DST_SWITCH
  NOSAVE_NEXT_DST_OFFSET
  NOSAVE_NEXT_LS_OFFSET
  NOSAVE_NEXT_LS_SWITCH
  NOSAVE_CURRENT_LS_OFFSET
*/

machine_date *read_rtc(void)
{
  static machine_date ret;
  u32 i1, i2, i3;

  i1 = (RTCNT12*0x10000) + RTCNT34;
  i2 = (RTCNT12*0x10000) + RTCNT34;
  i3 = (RTCNT12*0x10000) + RTCNT34;

  if (i1 == i2) 
    ret.epochsecs = i1;
  else
    ret.epochsecs = i3;

  ret.isleapsec = 0;

  return &ret;

}

void write_rtc(machine_date *md)
{
  RTCCTL1 = 0x40; // RTCMODE + RTCHOLD;
  RTCNT12 = md->epochsecs >> 16;
  RTCNT34 = md->epochsecs % 0x10000;
  RTCCTL1 &= 0xbf; // ~RTCHOLD;
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

#endif /* CONFIG_RTC */
