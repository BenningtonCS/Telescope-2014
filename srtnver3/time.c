#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "d1cons.h"
#include "d1typ.h"
#include "d1glob.h"
#include "d1proto.h"



double gst(double ttime)
{
    double secs, pdum;
    int i;
//    secs = (1999 - 1970) * 31536000.0 + 17.0 * 3600.0 + 16.0 * 60.0 + 20.1948;
    secs = (2011 - 1970) * 31536000.0 + 17.0 * 3600.0 + 15.0 * 60.0 + 58.0778;
    for (i = 1970; i < 2011; i++) {
        if ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
            secs += 86400.0;
    }

    return (modf((ttime - secs) / 86164.09053, &pdum) * TWOPI);
/* 17 16 20.1948 UT at 0hr newyear1999 */
/* 17 15 58.0778 UT at 0hr newyear2011 */
}

/* Convert to Seconds since New Year 1970 */
double tosecs(int yr, int day, int hr, int min, int sec)
{
    int i;
    double secs;
    secs = (yr - 1970) * 31536000.0 + (day - 1) * 86400.0 + hr * 3600.0 + min * 60.0 + sec;
    for (i = 1970; i < yr; i++) {
        if ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
            secs += 86400.0;
    }
    if (secs < 0.0)
        secs = 0.0;
    return secs;
}


/* Convert Seconds to Yr/Day/Hr/Min/Sec */
void toyrday(double secs, int *pyear, int *pday, int *phr, int *pmin, int *psec)
{
    double days, day, sec;
    int i;

    day = floor(secs / 86400.0);
    sec = secs - day * 86400.0;
    for (i = 1970; day > 365; i++) {
        days = ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0) ? 366.0 : 365.0;
        day -= days;
    }
    *phr = sec / 3600.0;
    sec -= *phr * 3600.0;
    *pmin = sec / 60.0;
    *psec = sec - *pmin * 60;
    *pyear = i;
    day = day + 1;
    *pday = day;
    if (day == 366)             // fix for problem with day 366
    {
        days = ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0) ? 366 : 365;
        if (days == 365) {
            day -= 365;
            *pday = day;
            *pyear = i + 1;
        }
    }
}

int dayofyear(int year, int month, int day)
{
    int i, leap;
    int daytab[2][13] = {
        {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };
    leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    for (i = 1; i < month; i++)
        day += daytab[leap][i];
    return (day);
}

char *to_date(int year, int day)
{
    int day_tab[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
        0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    char *mon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
        "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    int k, leap;
    static char str[80];
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
        leap = 1;
    else
        leap = 0;
    for (k = 1; day > day_tab[leap * 13 + k]; k++)
        day -= day_tab[leap * 13 + k];
    sprintf(str, "UT %s %02d", mon[k - 1], day);
    return str;
}

char *to_radecp(double ra, double dec)
{
    int hr, min, sec, deg, dmin, dsec, sg;
    static char str[80];
    sec = (int) (ra * 12.0 * 3600.0 / PI);
    hr = sec / 3600;
    sec -= hr * 3600;
    min = sec / 60;
    sec = sec - min * 60;
    sg = 1;
    dsec = (int) (dec * 3600.0 * 180.0 / PI);
    if (dsec < 0) {
        sg = -1;
        dsec = -dsec;
    }
    deg = dsec / 3600;
    dsec -= deg * 3600;
    dmin = dsec / 60;
    dsec -= dmin * 60;
    sprintf(str, "%02d:%02d:%02d %02d:%02d:%02d", hr, min, sec, deg * sg, dmin, dsec);
    return str;
}


double readclock(void)
{
    time_t now;
    double secs;
    struct tm *t;
    now = time(NULL);
    t = gmtime(&now);
// gmtime Jan 1 is day 0
    secs = tosecs(t->tm_year + 1900, t->tm_yday + 1, t->tm_hour, t->tm_min, t->tm_sec);
    if (d1.azelsim) {
        if (d1.start_time == 0.0)
            d1.start_time = secs;
        if (d1.speed_up > 0)
            secs = d1.start_time + (secs - d1.start_time) * d1.speed_up;
        else
            secs += -d1.speed_up * 3600.0; // advance by hours
    }
    sprintf(d1.timsource, "PC ");
    return (secs);
}
