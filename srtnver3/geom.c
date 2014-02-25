#include <stdio.h>
#include <math.h>
#include "d1cons.h"
#include "d1proto.h"
#include "d1typ.h"
#include "d1glob.h"


double vlsr(double time, double ra, double dec)
  // calculate velocity of the local standard of rest
{
    double decc, rac, vsun, vearth, x0, y0, z0, sunlong, soulong, soulat, x, y,
        z, dp, rp, gwest, grad, gpole, lon0, ggwest;
    decc = -(28.0 + 56.0 / 60.0) * PI / 180.0;
    rac = (17.0 + 45.5 / 60.0) * PI / 12.0;
    dp = 27.1 * PI / 180.0;
    rp = (12.0 + 51.4 / 60.0) * PI / 12.0;
    x0 = 20.0 * cos(18.0 * PI / 12.0) * cos(30.0 * PI / 180.0);
    y0 = 20.0 * sin(18.0 * PI / 12.0) * cos(30.0 * PI / 180.0);
    z0 = 20.0 * sin(30.0 * PI / 180.0); /* sun 20km/s towards ra=18h dec=30.0 */
    vsun = -x0 * cos(ra) * cos(dec) - y0 * sin(ra) * cos(dec)
        - z0 * sin(dec);
    x0 = cos(ra) * cos(dec);
    y0 = sin(ra) * cos(dec);
    z0 = sin(dec);
    x = x0;
    y = y0 * cos(23.5 * PI / 180.0) + z0 * sin(23.5 * PI / 180.0);
    z = z0 * cos(23.5 * PI / 180.0) - y0 * sin(23.5 * PI / 180.0);
    soulat = atan2(z, sqrt(x * x + y * y));
    soulong = atan2(y, x);
    sunlong = (time * 360.0 / (365.2422 * 86400.0) + 280.0) * PI / 180.0; /* long=280 day 1 */
    vearth = -30.0 * cos(soulat) * sin(sunlong - soulong);
    gwest = cos(decc) * cos(rp - rac);
    grad = cos(decc) * sin(rp - rac);
    ggwest = gwest * sin(dp) - sin(decc) * cos(dp);
    gpole = gwest * cos(dp) + sin(decc) * sin(dp);
    lon0 = (atan2(ggwest, grad)) * 180.0 / PI;
    gwest = cos(dec) * cos(rp - ra);
    grad = cos(dec) * sin(rp - ra);
    ggwest = gwest * sin(dp) - sin(dec) * cos(dp);
    gpole = gwest * cos(dp) + sin(dec) * sin(dp);
    d1.glat = (atan2(gpole, sqrt(ggwest * ggwest + grad * grad))) * 180.0 / PI;
    d1.glon = (atan2(ggwest, grad)) * 180.0 / PI - lon0;
    if (d1.glon < 0.0)
        d1.glon += 360.0;
    if (d1.glon > 360.0)
        d1.glon += -360.0;
    return vsun + vearth;
}

void GalactictoRadec(double glat, double glon, double *ra, double *dec)
  /* galactic to radec  2000 epoch pole at 12h51.4 27.1 */
{
    double a, xg, yg, zg, xr, yr, zr, d0, dp, r0, rp;
    d0 = -(28.0 + 56.0 / 60.0) * PI / 180.0;
    r0 = (17.0 + 45.5 / 60.0) * PI / 12.0;
    dp = 27.1 * PI / 180.0;
    rp = (12.0 + 51.4 / 60.0) * PI / 12.0;
    zr = sin(d0);
    xr = cos(r0 - rp) * cos(d0);
    yr = sin(r0 - rp) * cos(d0);
    xg = xr * sin(dp) - zr * cos(dp);
    yg = yr;
    a = atan2(yg, xg);
    xg = cos((glon * PI / 180.0) + a) * cos(glat * PI / 180.0);
    yg = sin((glon * PI / 180.0) + a) * cos(glat * PI / 180.0);
    zg = sin(glat * PI / 180.0);
    xr = xg * sin(dp) + zg * cos(dp);
    yr = yg;
    zr = zg * sin(dp) - xg * cos(dp);
    *dec = atan2(zr, sqrt(xr * xr + yr * yr));
    *ra = atan2(yr, xr) + rp;
    if (*ra < 0)
        *ra += TWOPI;
}

void moonradec(double time, double *ra, double *dec)
{
    double ttime, asnode, amon, peri, em, aim, aam, vsm, alamm, moonsun, evn, var;
    double x, y, z, xx, yy, zz, ram, decm, ha, inc;
/* calc moon ra and dec  */
/* see notes and formulae Astronomical Almanac page D2 Moon, 1999 */
    ttime = tosecs(1999, 0, 0, 0, 0); /* Jan 0 1999 */
    ttime = (time - ttime) / 86400.00;
    /* asnode=long of mean ascending node */
    asnode = 144.452077 - 0.05295377 * ttime;
    /* amon=omga plus mean lunar longitude */
    amon = 69.167124 + 13.17639648 * ttime;
    /* peri=asnode plus mean lunar longitude of perigee */
    peri = 42.524057 + 0.11140353 * ttime;
    /* moonsun is the elongation of moon from the sun */
    moonsun = 149.940812 + 12.19074912 * ttime;
    /* em is the eccentricity of lunar orbit */
    em = 0.054900489;
    /* aim=inclination of lunar orbit to ecliptic */
    aim = 5.1453964 * PI / 180.0;
    /* vsm=true anomaly */
    /* the following are correction terms */
    vsm = 2.0 * em * sin((amon - peri) * PI / 180.0); /* elliptical orbit */
    evn = (1.274 / 57.3) * sin((2 * moonsun - (amon - peri)) * PI / 180.0); /* evection */
    var = (0.658 / 57.3) * sin(2 * moonsun * PI / 180.0); /* variation */
    alamm = (amon - asnode) * PI / 180.0 + vsm + evn + var;
    x = cos(alamm);
    y = sin(alamm);
    z = 0;
    xx = x;
    yy = y * cos(aim);
    zz = y * sin(aim);
    ram = atan2(yy, xx) + asnode * PI / 180.0;
    decm = atan2(zz, sqrt(xx * xx + yy * yy));
    x = cos(ram) * cos(decm);
    y = sin(ram) * cos(decm);
    z = sin(decm);
    inc = 23.45 * PI / 180.0;
    xx = x;
    yy = y * cos(inc) - z * sin(inc);
    zz = z * cos(inc) + y * sin(inc);
    /* aam is the semi-major axis of orbit earth radii */
    aam = 60.2665;
    z = zz - sin(d1.lat) / aam; /* correct for parallax */
    ha = gst(time) - d1.lon;
    x = xx - cos(d1.lat) * cos(ha) / aam;
    y = yy - cos(d1.lat) * sin(ha) / aam;
    *ra = atan2(y, x);
    if (*ra < 0)
        *ra += TWOPI;
    *dec = atan2(z, sqrt(x * x + y * y));
}


/* Approximate Precession */
void precess(double rain, double decin, double *praout, double *pdecout, double epin, double epout)
{
    *pdecout = decin + 0.0000972 * cos(rain) * (epout - epin);
    *praout = rain + ((0.000223 + 0.0000972 * sin(rain) * tan(decin)) * (epout - epin));
}

void radec_azel(double ha, double dec, double latt, double *azs, double *elevs)
{
/* convert from sky to antenna coords (azel mount) */
/* input: ha,dec,latt
   output: azs=azimuth of source
   elevs=elevation of source
 */
    double p, w, r, zen, north;
    p = sin(dec);
    w = sin(ha) * cos(dec);
    r = cos(ha) * cos(dec);
    zen = r * cos(latt) + p * sin(latt);
    north = -r * sin(latt) + p * cos(latt);
    *elevs = atan2(zen, sqrt(north * north + w * w));
    *azs = atan2(-w, north);
    if (*azs < 0)
        *azs = *azs + TWOPI;
}


/* Calculate Sun ra and dec (approximate) */
/* see Astronomical Almanac page C24 Sun 1999 */
void sunradec(double time, double *ra, double *dec)
{
    double n, g, lonn, ecl;
    n = -365.5 + (time - tosecs(1999, 1, 0, 0, 0)) / 86400.0;
    g = (357.528 + 0.9856003 * n) * PI / 180.0;
    lonn = (280.460 + 0.9856474 * n + 1.915 * sin(g) + 0.02 * sin(2 * g)) * PI / 180.0;
    ecl = (23.439 - 0.0000004 * n) * PI / 180.0;
    *ra = atan2(sin(lonn) * cos(ecl), cos(lonn));
    if (*ra < 0)
        *ra += TWOPI;
    *dec = asin(sin(lonn) * sin(ecl));
}

void azel_to_radec(double time, double az, double el, double *ra, double *dec)
  // convert from azimuth and elevation to ra and dec
{
    double north, west, zen, pole, rad, ha;
    north = cos(az * PI / 180.0) * cos(el * PI / 180.0);
    west = -sin(az * PI / 180.0) * cos(el * PI / 180.0);
    zen = sin(el * PI / 180.0);
    pole = north * cos(d1.lat) + zen * sin(d1.lat);
    rad = zen * cos(d1.lat) - north * sin(d1.lat);
    *dec = atan2(pole, sqrt(rad * rad + west * west));
    ha = atan2(west, rad);
    *ra = -ha + gst(time) - d1.lon;
    if (*ra < 0)
        *ra += TWOPI;
}
