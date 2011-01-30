/* rtc service routines */

// note to self: DO SOMETHING with yday!

// note to self: fill in wday please!

// omfg IT'S ALREADY 1026 BYTES long!

// Include section

// system
#include "project.h"
#ifdef CONFIG_RTC
#include "rtc.h"

/***************************************************************/

/*
struct _human_date
{
  u16 year;
  u8 mon;
  u8 mday;
  u8 wday;
  u8 hour; 
  u8 minute;
  u8 second;
} human_date;
*/

human_date *get_now_human_utc_date(void)
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

void set_now_human_utc_date(human_date *hd)
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

machine_date human_to_machine_date(human_date *hd)
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

  result = 3600*hd->hour + 60*hd->min + hd->sec + (epochdays * 86400);

  return result;
}

human_date *machine_to_human_date(machine_date md)
{
  u32 dsecs, y,m,epochdays;
  u8 mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  static human_date ret;

  epochdays = md / 86400;
  dsecs = md % 86400;

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

  return &ret;
}

machine_date local_to_utc_date(machine_date ld)
{
}

machine_date utc_to_local_date(machine_date ud)
{
}

u32 get_leapseconds_utc(machine_date ud)
{
}

u32 get_leapseconds_tai(machine_date td)
{
}

#endif /* CONFIG_RTC */
