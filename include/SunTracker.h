#pragma once
#include <Eigen/Dense>
#include <iostream>
#include <math.h>
#include <time.h>
#include "tm_r.h"


// Sun position tracker based on the Meeus method
// Reimplementation of the code by Jarmo Lammi
// Now it is wrapped in a class that makes the usage easier
class SunTracker {
public:
  SunTracker(double lat, double lon, double tzone)
      : latitude(lat), longitude(lon), tzone(tzone){};

  void setRelativeRotationAroundZ(double rot) {rotateAroundZ_deg = rot;}

  Eigen::Vector3d getSunDirection(tm_r tm) {
    double hour = tm.hour + (double)tm.min / 60.0;
    double UT = hour - tzone; // back to universal time

    // Julian Date
    double jd = FNday(tm.year, tm.month, tm.day, UT);
    //   Use FNsun to find the ecliptic longitude of the sun
    double lambda = FNsun(jd);
    double obliq = 23.4393 * rads - 3.563E-7 * rads * jd;
    // Sidereal time at Greenwich meridian
    double GMST0 = L * degs / 15.0 + 12.0; // hours
    double SIDTIME = GMST0 + UT + longitude / 15.0;
    // Hour Angle
    double ha = 15.0 * SIDTIME - RA; // degrees
    ha = FNrange(rads * ha);
    x = cos(ha) * cos(delta);
    y = sin(ha) * cos(delta);
    z = sin(delta);
    double xhor = x * sin(latitude * rads) - z * cos(latitude * rads);
    double yhor = y;
    double zhor = x * cos(latitude * rads) + z * sin(latitude * rads);
    double azi = atan2(yhor, xhor) + pi;
    azi = FNrange(azi + rads *rotateAroundZ_deg);
    double alt = asin(zhor);

    Eigen::Vector3d direction;

    double xnorm, ynorm, znorm;

    xnorm = std::cos(alt) * std::cos(azi);
    ynorm = -std::cos(alt) * std::sin(azi);
    znorm = sin(alt);

    direction << xnorm, ynorm, znorm;
    return (direction);
  }

  Eigen::Vector2d getAzimuthAltitude(tm_r tm) {
    double hour = tm.hour + (double)tm.min / 60.0;
    double UT = hour - tzone; // back to universal time

    // Julian Date
    double jd = FNday(tm.year, tm.month, tm.day, UT);
    //   Use FNsun to find the ecliptic longitude of the sun
    double lambda = FNsun(jd);
    double obliq = 23.4393 * rads - 3.563E-7 * rads * jd;
    // Sidereal time at Greenwich meridian
    double GMST0 = L * degs / 15.0 + 12.0; // hours
    double SIDTIME = GMST0 + UT + longitude / 15.0;
    // Hour Angle
    double ha = 15.0 * SIDTIME - RA; // degrees
    ha = FNrange(rads * ha);
    x = cos(ha) * cos(delta);
    y = sin(ha) * cos(delta);
    z = sin(delta);
    double xhor = x * sin(latitude * rads) - z * cos(latitude * rads);
    double yhor = y;
    double zhor = x * cos(latitude * rads) + z * sin(latitude * rads);
    double azi = atan2(yhor, xhor) + pi;
    azi = FNrange(azi + rads *rotateAroundZ_deg) ;
    double alt = asin(zhor);

    Eigen::Vector2d angles;

    angles << azi, alt;
    return (angles);
  }

private:
  double latitude;
  double longitude;
  double tzone;

  double rotateAroundZ_deg = 0;

  double pi = 3.14159265359;

  double tpi = 2 * pi;
  double degs = 180.0 / pi;
  double rads = pi / 180.0;

  double L, RA, g, daylen, delta, x, y, z;
  double SunDia = 0.53; // Sunradius degrees

  double AirRefr = 34.0 / 60.0; // athmospheric refraction degrees //

  double FNday(int y, int m, int d, float h) {
    int luku = -7 * (y + (m + 9) / 12) / 4 + 275 * m / 9 + d;
    luku += (long int)y * 367;
    return (double)luku - 730530.0 + h / 24.0;
  }

  double FNrange(double x) {
    double b = x / tpi;
    double a = tpi * (b - (long)(b));
    if (a < 0)
      a = tpi + a;
    return a;
  }

  double f0(double lat, double declin) {
    double fo, dfo;
    dfo = rads * (0.5 * SunDia + AirRefr);
    if (lat < 0.0)
      dfo = -dfo; // Southern hemisphere
    fo = tan(declin + dfo) * tan(lat * rads);
    if (fo > 0.99999)
      fo = 1.0; // to avoid overflow //
    fo = asin(fo) + pi / 2.0;
    return fo;
  }

  double FNsun(double d) {
    double w, M, v, r;
    //   mean longitude of the Sun
    w = 282.9404 + 4.70935E-5 * d;
    M = 356.047 + 0.9856002585 * d;
    // Sun's mean longitude
    L = FNrange(w * rads + M * rads);

    //   mean anomaly of the Sun

    g = FNrange(M * rads);

    // eccentricity
    double ecc = 0.016709 - 1.151E-9 * d;

    //   Obliquity of the ecliptic

    double obliq = 23.4393 * rads - 3.563E-7 * rads * d;
    double E = M + degs * ecc * sin(g) * (1.0 + ecc * cos(g));
    E = degs * FNrange(E * rads);
    x = cos(E * rads) - ecc;
    y = sin(E * rads) * sqrt(1.0 - ecc * ecc);
    r = sqrt(x * x + y * y);
    v = atan2(y, x) * degs;
    // longitude of sun
    double lonsun = v + w;
    lonsun -= 360.0 * (lonsun > 360.0);

    // sun's ecliptic rectangular coordinates
    x = r * cos(lonsun * rads);
    y = r * sin(lonsun * rads);
    double yequat = y * cos(obliq);
    double zequat = y * sin(obliq);
    RA = atan2(yequat, x);
    delta = atan2(zequat, sqrt(x * x + yequat * yequat));
    RA *= degs;

    //   Ecliptic longitude of the Sun

    return FNrange(L + 1.915 * rads * sin(g) + .02 * rads * sin(2 * g));
  }

  // Display decimal hours in hours and minutes
  void showhrmn(double dhr) {
    int hr, mn;
    hr = (int)dhr;
    mn = (dhr - (double)hr) * 60;
    if (hr < 10)
      std::cout << '0';
    std::cout << hr << ':';
    if (mn < 10)
      std::cout << '0';
    std::cout << mn;
  }
};