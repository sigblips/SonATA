/*******************************************************************************

 File:    SseAstro.cpp
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/


#include "SseAstro.h" 
#include "AA+.h"  // must come before wcs.h to avoid PI conflict
#include "wcs.h"
#include "novas.h"
#include <cmath>
#include <iostream>

// wcs library
extern "C" {
   void gal2fk5(double *coord1, double *coord2);
   void fk52gal(double *coord1, double *coord2);
   double epj2mjd(double epoch);
}

using namespace std;

static const double HoursPerDay = 24.0;
static const double TwoPi = 2.0 *M_PI;
static const double CircumpolarHourAngle = 12.0;
static const double SubHorizonHourAngle = 0.0;
static const double MasPerArcSec = 1000;

static double normalize(double value, double valueRange)
{
   if (value >= valueRange)
   {
      value -= valueRange;
   }
   else if (value < 0)
   {
      value += valueRange;
   }

   return value;
}

double SseAstro::degreesToRadians(double degrees)
{
   const double radiansPerDegree = TwoPi/360.0;
   return (degrees * radiansPerDegree);
}

double SseAstro::hoursToRadians(double hours)
{
   const double radiansPerHour = TwoPi/24.0;
   return (hours * radiansPerHour);
}

double SseAstro::radiansToDegrees(double radians)
{
   const double degreesPerRadian = 360/TwoPi;
   return (radians * degreesPerRadian);
}

double SseAstro::radiansToArcSecs(double radians)
{
   const double ArcSecPerDeg = 3600;
   return (radiansToDegrees(radians) * ArcSecPerDeg);
}


double SseAstro::radiansToHours(double radians)
{
   const double hoursPerRadian = 24 / TwoPi;
   return (radians * hoursPerRadian);
}

/*
  Convert degrees to decimal.

  If (sign) == '-' and/or (deg) is negative, the resulting value will
  be negative.    This takes care of the -00 case, eg -00 05 45.

  All other sign values are ignored (ie, considered positive).

*/

double SseAstro::degreesToDecimal(char sign, int deg, int min, double sec)
{
   const double MinPerDeg(60);
   const double SecPerDeg(3600);

   double decimalValue = fabs(static_cast<double>(deg))
      + fabs(static_cast<double>(min) / MinPerDeg) +
      + fabs(sec/SecPerDeg);

   const char minusSign('-');
   if ((deg < 0) || (sign == minusSign))
   {
      decimalValue *= -1;
   }

   return decimalValue;
}

double SseAstro::hoursToDecimal(int hour, int min, double sec)
{
   const char plusSign('+');
   return SseAstro::degreesToDecimal(plusSign, hour, min, sec);
}

void SseAstro::decimalHoursToHms(double decimalHours, int *hour,
                                 int *min, double *sec)
{
   char sign;
   decimalDegreesToDms(decimalHours, &sign, hour, min, sec);
}

void SseAstro::decimalDegreesToDms(double decimalDegs, char *sign,
                                  int *deg, int *min, double *sec)
{
   *deg = static_cast<int>(fabs(decimalDegs));

   const double MinPerDeg(60);
   double decimalMin = (fabs(decimalDegs) - *deg) * MinPerDeg;
   *min = static_cast<int>(decimalMin);

   const double SecPerMin(60);
   *sec = (decimalMin - *min) * SecPerMin;
  
   // handle seconds rollover
   double tolerance(1e-8);
   if (*sec + tolerance >= SecPerMin)
   {
      *sec = 0;
      *min += 1;
   }

   *sign='+';
   if (decimalDegs < 0)
   {
      *sign='-';
   }
}

// From Meeus, Astronomical Algorithms, 2nd Ed, Formulas 13.3 & 13.4

void SseAstro::eclipticToEquatorial(double eclipLongRads, double eclipLatRads,
				    double eclipObliqRads,
				    double &raRads, double &decRads)
{
   raRads = atan2(sin(eclipLongRads) * cos(eclipObliqRads) - tan(eclipLatRads)
                  * sin(eclipObliqRads), cos(eclipLongRads));
   if (raRads < 0)
   {
      raRads += TwoPi;
   }

   decRads = asin(sin(eclipLatRads) * cos(eclipObliqRads) +
		  cos(eclipLatRads) * sin(eclipObliqRads) * sin(eclipLongRads));

}


/*
  From Meeus, Astromical Algorithms, 2nd ed, formula 16.3.
  Given the apparent altitude (horizon) in degrees, 
  return the atmospheric refraction in degrees.
  This is accurate to 0.0012 deg.
  Assumes:  observation is made at sea level, 
  atmospheric pressure of 1010 millibars,
  air temp is 10 deg Celsius, for wavelength of yellow light.
*/

double SseAstro::atmosRefractDeg(double appAltDeg)
{
   double tanArgDeg(appAltDeg + (7.31 / (appAltDeg + 4.4)));
   double refractArcMin(1.0 / tan(SseAstro::degreesToRadians(tanArgDeg)));

   double refractDeg(refractArcMin / ArcMinPerDeg);

   return refractDeg;
}


/*
  Determine the rise/set hour angle for a declination.

  Inputs:
  dec (degrees)
  latitude of observing site (degrees)
  observing horizon (elevation) (degrees)

  Based on Meeus, 2nd ed, formula 15.1

  If dec is circumpolar, hour angle is 12.0.
  If dec is never visible, hour angle is zero.
*/

double SseAstro::hourAngle(double decDeg, double siteLatDeg, 
			   double horizonDeg)
{
   // convert args to radians
   double decRads(SseAstro::degreesToRadians(decDeg));
   double siteLatRads(SseAstro::degreesToRadians(siteLatDeg));
   double horizonRads(SseAstro::degreesToRadians(horizonDeg));

   double numerator = sin(horizonRads) - (sin(siteLatRads) *
                                          sin(decRads));
   double denom = cos(siteLatRads) * cos(decRads);
   double cosHourAngle(numerator/denom);

   // see if always visible
   if (cosHourAngle < -1.0)
   {
      return CircumpolarHourAngle;  
   }

   // see if never visible
   if (cosHourAngle > 1.0)
   {
      return SubHorizonHourAngle;
   }

   double hourAngleRads = acos(cosHourAngle);
   double hourAngleHours = SseAstro::radiansToHours(hourAngleRads);
 
   return hourAngleHours;
}

/* Input: a time interval as a sidereal angle in radians
   Returns: time interval converted to solar seconds
*/

int SseAstro::siderealRadsToSolarTimeSecs(double siderealRads)
{
   int solarTimeSecs = static_cast<int>(
      SseAstro::radiansToHours(siderealRads * SolarDaysPerSideralDay)
      * SecsPerHour);
   
   return solarTimeSecs;
}

/*
  Convert UNIX time in secs to Julian centuries (eg, 2006.25) 
*/
double SseAstro::timeToJulianEpoch(time_t time)
{
   const double secsPerJulianYear(SecsPerDay * DaysPerJulianYear);

   double julianEpoch = JulianEpochBaseYear + 
      static_cast<double>(time - J2000UnixTimeSecs) / 
      secsPerJulianYear;

   return julianEpoch;
}

/*
  Convert Julian centuries (eg, 2006.25) to Modified Julian Date
*/
double SseAstro::julianEpochToMjd(double epoch)
{
   // wcs lib:
   return epj2mjd(epoch);
}

double SseAstro::mjdToJd(double mjd)
{
   const double MjdOffset(2400000.5);
   return (mjd + MjdOffset);
}

double SseAstro::timeToMjd(time_t time)
{
   double mjd(julianEpochToMjd(
              timeToJulianEpoch(time)));

   return mjd;
}


/*
  Greenwich apparent sidereal time at modified julian date
*/
double SseAstro::gmstRads(double mjd)
{
   // From novas lib:
   double jdHigh(mjdToJd(mjd));
   double jdLow(0);
   double eqnEquinoxGreenwich(0);
   double gstHours;
   sidereal_time(jdHigh, jdLow, eqnEquinoxGreenwich, &gstHours);

   return hoursToRadians(gstHours);
}

/*
  Local mean sidereal time.
  Observer position longitude is positive towards west.
*/
double SseAstro::lmstRads(time_t time, double longWestRads)
{
   // Convert Julian epoch to Modified Julian Date 
   double mjd(julianEpochToMjd(timeToJulianEpoch(time)));
   double lmstRads(gmstRads(mjd) - longWestRads);
   lmstRads = normalize(lmstRads, TwoPi);

   return lmstRads;
}

#ifdef DEBUG
static void printHms(double decimalHours)
{
   int hours, mins;
   double secs;
   SseAstro::decimalHoursToHms(decimalHours, &hours, &mins, &secs);
   cout << hours << ":" << mins << ":" << secs << " ";
}
#endif



/*
  Given rise time, set time, and current time
  in hours, determine how many hours are left.
*/
double SseAstro::timeLeftHours(double riseHours, double setHours,
                               double currentHours)
{
   double leftHours(0);

   // currently up
   if (riseHours < setHours) 
   {
      if (currentHours >= riseHours &&
          currentHours <= setHours)
      {
         leftHours = 
            normalize(setHours - currentHours, HoursPerDay);
      }
   }
   else  // rise > set, so straddling midnight
   {
      if (currentHours >= riseHours || 
          currentHours <= setHours)
      {
         leftHours = 
            normalize(setHours - currentHours, HoursPerDay);
      }
   }

   return leftHours;
}
                          

/*
  Get rise, transit and set times in decimal hours UTC for the specified
  target, for the day containing the specified obsTime.
  Also computes number of hours until target rises (if not up)
  and number of hours until target sets (if it is up).
  Results should be good to within a few minutes.
  Site longitude is positive towards west.

  Rise, transit, set based on Meeus, 2nd Ed., formula 15.2.
  
*/
void SseAstro::riseTransitSet(double targetRaHours, double targetDecDeg,
                              double siteLongWestDeg, double siteLatDeg,
                              double horizonDeg, time_t obsTime,
                              double *riseHoursUtc, double *transitHoursUtc,
                              double *setHoursUtc, 
                              double *untilRiseHours, double *untilSetHours)

{  
   // Compute apparent sidereal time at zero hours UT of obs day at Greenwich:
   double mjd(julianEpochToMjd(timeToJulianEpoch(obsTime)));
   double truncateMjd = static_cast<int>(mjd);  // midnight
   double currentHoursUtc((mjd - truncateMjd) * HoursPerDay);

   double targetRaRads(hoursToRadians(targetRaHours));
   double targetHa(hourAngle(targetDecDeg, siteLatDeg, horizonDeg));

   *riseHoursUtc = 0;
   *transitHoursUtc = 0;
   *setHoursUtc = 0;
   *untilRiseHours = 0;
   *untilSetHours = 0;
   
   if (targetHa <= SubHorizonHourAngle)
   {
      // never visible
      return;
   }
   else if (targetHa >= CircumpolarHourAngle)
   {
      *untilSetHours = HoursPerDay;  // always up
      return;
   }

   double transitDayFraction = (targetRaRads + degreesToRadians(siteLongWestDeg)
                                - gmstRads(truncateMjd)) / TwoPi;
   
   double risingDayFraction = transitDayFraction - (targetHa/HoursPerDay);
   double settingDayFraction = transitDayFraction + (targetHa/HoursPerDay);

   double oneDay(1.0);
   *riseHoursUtc = normalize(risingDayFraction, oneDay) * HoursPerDay;
   *transitHoursUtc = normalize(transitDayFraction, oneDay) * HoursPerDay;
   *setHoursUtc = normalize(settingDayFraction, oneDay) * HoursPerDay;
    
   *untilSetHours = timeLeftHours(*riseHoursUtc, *setHoursUtc, currentHoursUtc);

   // reverse rise & set to get time to rise
   *untilRiseHours = timeLeftHours(*setHoursUtc, *riseHoursUtc, currentHoursUtc);
}


/*
  Angular separation of two points on a sphere, A & B.

  The spherical coordinates (coord1, coord2) are
  [RA,Dec], [Long,Lat] etc, in radians.
*/

double SseAstro::angSepRads(double pointACoord1Rads, double pointACoord2Rads, 
                            double pointBCoord1Rads, double pointBCoord2Rads)
{
   double sepDeg = wcsdist(
      radiansToDegrees(pointACoord1Rads),
      radiansToDegrees(pointACoord2Rads),
      radiansToDegrees(pointBCoord1Rads),
      radiansToDegrees(pointBCoord2Rads));

   return degreesToRadians(sepDeg);
}


void SseAstro::galToEqu2000(double galLongRads, double galLatRads,
                            double *raRads, double *decRads)
{
   double coord1Deg(radiansToDegrees(galLongRads));
   double coord2Deg(radiansToDegrees(galLatRads));

   // wcs library
   gal2fk5(&coord1Deg, &coord2Deg);

   *raRads = degreesToRadians(coord1Deg);
   *decRads = degreesToRadians(coord2Deg);
}

void SseAstro::equ2000ToGal(double raRads, double decRads,
                            double *galLongRads, double *galLatRads)
{
   double coord1Deg(radiansToDegrees(raRads));
   double coord2Deg(radiansToDegrees(decRads));

   // wcs library
   fk52gal(&coord1Deg, &coord2Deg);

   *galLongRads = degreesToRadians(coord1Deg);
   *galLatRads = degreesToRadians(coord2Deg);
}

void SseAstro::sunPosition(time_t time, double *ra2000Rads, double *dec2000Rads)
{ 
   double jd(mjdToJd(timeToMjd(time)));
   double sunLong = CAASun::ApparentEclipticLongitude(jd);
   double sunLat = CAASun::ApparentEclipticLatitude(jd);
   CAA2DCoordinate sunCoord =
      CAACoordinateTransformation::Ecliptic2Equatorial(sunLong, sunLat, CAANutation::
                                                       TrueObliquityOfEcliptic(jd));

   *ra2000Rads = hoursToRadians(sunCoord.X);
   *dec2000Rads = degreesToRadians(sunCoord.Y);
}

void SseAstro::moonPosition(time_t time, double *ra2000Rads, double *dec2000Rads)
{
   double jd(mjdToJd(timeToMjd(time)));
   double lambda = CAAMoon::EclipticLongitude(jd); 
   double beta = CAAMoon::EclipticLatitude(jd); 
   double epsilon = CAANutation::TrueObliquityOfEcliptic(jd);
   CAA2DCoordinate lunarcoord = 
      CAACoordinateTransformation::Ecliptic2Equatorial(lambda, beta, epsilon);

   *ra2000Rads = hoursToRadians(lunarcoord.X);
   *dec2000Rads = degreesToRadians(lunarcoord.Y);
}

/*
  Convert to epochTime (apply proper motion);
 */
void SseAstro::positionAtNewEpoch(
   time_t time, double ra2000Rads, double dec2000Rads,
   double pmRaMasYr, double pmDecMasYr,
   double *newRaRads, double *newDecRads)
{
   const double degPerHour = 15;

   // If very near North or South Pol, then don't move in RA
   double pmRaSecsYr(0.0);
   double maxDecDeg(89.99);
   if (fabs(dec2000Rads) < degreesToRadians(maxDecDeg))
   {
      double pmRaArcSecYr = pmRaMasYr / MasPerArcSec;
      pmRaSecsYr = pmRaArcSecYr / (degPerHour * cos(dec2000Rads));
   }

   double YearsPerCent(100);
   double pmRaSecsCent(pmRaSecsYr * YearsPerCent);

   double pmDecArcsecsYr(pmDecMasYr / MasPerArcSec);
   double pmDecArcsecsCent(pmDecArcsecsYr * YearsPerCent);

#if 0
   cout << "pmRaMasYr: " << pmRaMasYr << endl;
   cout << "pmDecMasYr: " << pmDecMasYr << endl;

   cout << "pmRaSecsCent: " << pmRaSecsCent <<endl;
   cout << "pmDecArcsecsCent: " << pmDecArcsecsCent << endl;
#endif

   // apply proper motion
   double radialVelKmSec(0); 
   double parallaxArcSec(0); 
   double startEpochYears(JulianEpochBaseYear);  
   double newEpochYears(timeToJulianEpoch(time));

   // NOVAS library:
   cat_entry catEntry;
   catEntry.catalog[0] = '\0';
   catEntry.starname[0] = '\0';
   catEntry.starnumber = 0;
   catEntry.ra = radiansToHours(ra2000Rads);
   catEntry.dec = radiansToDegrees(dec2000Rads);
   catEntry.promora = pmRaSecsCent;
   catEntry.promodec = pmDecArcsecsCent;
   catEntry.parallax = parallaxArcSec;
   catEntry.radialvelocity = radialVelKmSec;

   short int transformOpt(1);  // 1 = new epoch only (ie, apply PM)
   const int catStrSize(4);
   char catalog[catStrSize];
   catalog[0] = '\0';

   cat_entry newCatEntry;
   transform_cat(transformOpt, startEpochYears, &catEntry,
                 newEpochYears, catalog, &newCatEntry);

   *newRaRads = hoursToRadians(newCatEntry.ra);
   *newDecRads = degreesToRadians(newCatEntry.dec);
}

/*
  Covert to new epoch and equinox (apply proper motion and
  precession).
 */
void SseAstro::positionAtNewEpochAndEquinox(
   time_t epochTime, double ra2000Rads, double dec2000Rads,
   double pmRaMasYr, double pmDecMasYr,
   double *newRaRads, double *newDecRads)
{
   // Apply proper motion
   double newEpochRaRads;
   double newEpochDecRads;
   positionAtNewEpoch(epochTime, ra2000Rads, dec2000Rads,
                      pmRaMasYr, pmDecMasYr,
                      &newEpochRaRads, &newEpochDecRads);

   // Apply precession   
   double startEpochYears(JulianEpochBaseYear);  
   double endEpochYears(timeToJulianEpoch(epochTime));

   // NOVAS library:
   cat_entry catEntry;
   catEntry.catalog[0] = '\0';
   catEntry.starname[0] = '\0';
   catEntry.starnumber = 0;
   catEntry.ra = radiansToHours(newEpochRaRads);
   catEntry.dec = radiansToDegrees(newEpochDecRads);
   catEntry.promora = 0;
   catEntry.promodec = 0;
   catEntry.parallax = 0;
   catEntry.radialvelocity = 0;

   const int catStrSize(4);
   char catalog[catStrSize];
   catalog[0] = '\0';

   short int transformOpt(2);  // 2 = precess only (no proper motion)
   cat_entry newCatEntry;
   transform_cat(transformOpt, startEpochYears, &catEntry,
                 endEpochYears, catalog, &newCatEntry);

   *newRaRads = hoursToRadians(newCatEntry.ra);
   *newDecRads = degreesToRadians(newCatEntry.dec);
}

/* 
Given the observer's latitude (deg), return
the declination (deg) of the geostationary
satellite band.

Note: this is computed for the meridian; the
declination will move closer to the equator
(on the order of 0.5 deg) as you approach the 
eastern or western horizon.
See http://www.skythisweek.info/tstw20090910.htm.
*/

double SseAstro::geosatDecDeg(double latDeg)
{
   /* From
      http://www.geo-orbit.org/sizepgs/decchartp.html

      ARCTAN((ER*SIN(L)/(ESG+ER(1-COS(L)))))

      L = site latitude
      ER = earth radius
      ESG = distance from the surface of the earth to
            the satellite belt 
   */
   double earthRadiusKm(6371);
   double geostatSurfaceDistKm(35785);

   double latRads(degreesToRadians(latDeg));
   double angleRads = atan2(earthRadiusKm * sin(latRads),
       geostatSurfaceDistKm + earthRadiusKm * (1 - cos(latRads)));

   // turn into a declination
   angleRads *= -1;

   return (radiansToDegrees(angleRads));
}