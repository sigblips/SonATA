/*******************************************************************************

 File:    SseAstro.h
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


#ifndef SseAstro_H
#define SseAstro_H

#include <time.h>

class SseAstro
{
 public:

   // some useful constants:
   static const int SecsPerDay = 86400;
   static const int SecsPerHour = 3600;
   static const double DaysPerJulianYear = 365.25;
   static const double ArcMinPerDeg = 60.0;
   static const double ArcSecPerDeg = 3600.0;
   static const double SolarDaysPerSideralDay = 0.99726956633;  // Meeus
   static const double EclipObliqJ2000Rads = 0.4090928;  //Meeus
   static const double HzPerMhz = 1e6;
   static const double KhzPerMhz = 1e3;
   static const double JulianEpochBaseYear = 2000.0;
   static const time_t J2000UnixTimeSecs = 946728000; // 2000-01-01 12:00:00 UTC

   static double degreesToRadians(double degrees);
   static double hoursToRadians(double hours);
   static double radiansToDegrees(double radians);
   static double radiansToArcSecs(double radians);
   static double radiansToHours(double radians);
   static double degreesToDecimal(char sign, int deg, int min, double sec);
   static double hoursToDecimal(int hour, int min, double sec);
   
   static void decimalHoursToHms(double decimalHours, int *hour,
                                 int *min, double *sec);
   
   static void decimalDegreesToDms(double decimalDegs, char *sign, int *deg,
                                    int *min, double *sec);
   
   static void eclipticToEquatorial(double eclipLongRads, double eclipLatRads,
				    double eclipObliqRads,
				    double &raRads, double &decRads);

   static void galToEqu2000(double galLongRads, double galLatRads,
                            double *raRads, double *decRads);

   static void equ2000ToGal(double raRads, double decRads,
                            double *galLongRads, double *galLatRads);

   static double atmosRefractDeg(double apparentAltDeg);

   static double hourAngle(double decDeg, double siteLatDeg, 
			   double horizonDeg);

   static int siderealRadsToSolarTimeSecs(double siderealRads);

   static double mjdToJd(double mjd);

   static double timeToMjd(time_t time);

   static double timeToJulianEpoch(time_t time);

   static double julianEpochToMjd(double epoch);

   static double gmstRads(double mjd);

   static double lmstRads(time_t time, double longWestRads);

   static double timeLeftHours(double riseHours, double setHours,
                               double currentHours);
   
   static void riseTransitSet(double targetRaHours, double targetDecDeg,
                              double siteLongWestDeg, double siteLatDeg,
                              double horizonDeg, time_t obsTime,
                              double *riseHoursUtc, double *transitHoursUtc,
                              double *setHoursUtc, 
                              double *untilRiseHours, double *untilSetHours);
   
   static double angSepRads(double pointACoord1Rads, double pointACoord2Rads, 
                            double pointBCoord1Rads, double pointBCoord2Rads);

   static void sunPosition(time_t time, double *ra2000Rads, double *dec2000Rads);

   static void moonPosition(time_t time, double *ra2000Rads, double *dec2000Rads);

   static void positionAtNewEpoch(
      time_t epochTime, double ra2000Rads, double dec2000Rads,
      double pmRaMasYr, double pmDecMasYr,
      double *newRaRads, double *newDecRads);

   static void positionAtNewEpochAndEquinox(
      time_t epochTime, double ra2000Rads, double dec2000Rads,
      double pmRaMasYr, double pmDecMasYr,
      double *newRaRads, double *newDecRads);

   static double geosatDecDeg(double latDeg);

 private:

   SseAstro();
   ~SseAstro();

};

#endif // SseAstro_H