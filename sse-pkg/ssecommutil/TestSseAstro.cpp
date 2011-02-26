/*******************************************************************************

 File:    TestSseAstro.cpp
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


#include "TestRunner.h"
#include "TestSseAstro.h"
#include "SseUtil.h"
#include "SseAstro.h"
#include <sstream>
#include <stdio.h>
#include <math.h>
#include <vector>

static const double HatCreekLongWestDeg(121.47);
static const double HatCreekLongWestRads(SseAstro::degreesToRadians(
                                            HatCreekLongWestDeg));

static const double HatCreekLatDeg(40.81667);
//static const double HatCreekLatRads(SseAstro::degreesToRadians(HatCreekLatDeg));
static const double HatCreekHorizonDeg(18);

static const char PlusSign('+');
static const char MinusSign('-');
static const double MasPerArcSec(1000);
const double DegPerHour(15);

static void printRaRadsInHms(double raRads)
{
   int hours, mins;
   double secs;
   SseAstro::decimalHoursToHms(SseAstro::radiansToHours(raRads),
                              &hours, &mins, &secs);
   cout << "RA: " << hours << " " << mins << " " << secs << endl;
}

static void printDecRadsInDms(double decRads)
{
   char sign;
   int degs, mins;
   double secs;
   SseAstro::decimalDegreesToDms(SseAstro::radiansToDegrees(decRads),
                                &sign, &degs, &mins, &secs);
   cout << "Dec: " << sign << degs << " " << mins << " " << secs << endl;
}


void TestSseAstro::setUp ()
{
}

void TestSseAstro::tearDown()
{
}

void TestSseAstro::testFoo()
{
//   cu_assert(0);
}

void TestSseAstro::testRadianConversion()
{
    // test conversion from degrees/hours to/from radians
    double degrees;
    double expectedRadians;
    double tolerance(10e-8);

    // radians to/from degrees
    
    degrees = 0.0;
    expectedRadians = 0.0;
    assertDoublesEqual(expectedRadians, SseAstro::degreesToRadians(degrees),
		       tolerance);

    assertDoublesEqual(degrees, SseAstro::radiansToDegrees(expectedRadians),
		       tolerance);


    double radians = M_PI;
    double expectedDegrees = 180;
    assertDoublesEqual(expectedDegrees, SseAstro::radiansToDegrees(radians), 
		       tolerance);

    assertDoublesEqual(radians, SseAstro::degreesToRadians(expectedDegrees), 
		       tolerance);

    radians = -M_PI;
    expectedDegrees = -180;
    assertDoublesEqual(expectedDegrees, SseAstro::radiansToDegrees(radians),
			tolerance);

    assertDoublesEqual(radians, SseAstro::degreesToRadians(expectedDegrees),
		       tolerance);


    // hours to/from radians

    double hours = 0;
    expectedRadians = 0;
    assertDoublesEqual(expectedRadians, SseAstro::hoursToRadians(hours), 
		       tolerance);
    assertDoublesEqual(SseAstro::radiansToHours(expectedRadians), hours,
		       tolerance);

    hours = 12;
    expectedRadians=M_PI;
    assertDoublesEqual(SseAstro::hoursToRadians(hours), expectedRadians,
		       tolerance);
    assertDoublesEqual(SseAstro::radiansToHours(expectedRadians), hours, 
		       tolerance);


    // radiansToArcSecs

    const double ArcSecPerDeg = 3600;
    radians = M_PI;
    double expectedArcSecs = SseAstro::radiansToDegrees(radians) * 
	ArcSecPerDeg;
    assertDoublesEqual(expectedArcSecs, SseAstro::radiansToArcSecs(radians),
		       tolerance);
    

}

void TestSseAstro::testEclipticToEquatorial()
{
   double eclipLongRads = 0;
   double eclipLatRads = 0;
   double raRads = -1;
   double decRads = -1;
   double expectedRaRads = 0;
   double expectedDecRads = 0;

   SseAstro::eclipticToEquatorial(eclipLongRads, eclipLatRads, 
				  SseAstro::EclipObliqJ2000Rads,
				  raRads, decRads);
   
   double radsTol = 10e-6;
   assertDoublesEqual(expectedRaRads, raRads, radsTol);
   assertDoublesEqual(expectedDecRads, decRads, radsTol);


   eclipLongRads = SseAstro::degreesToRadians(45);
   eclipLatRads = SseAstro::degreesToRadians(45);
   expectedRaRads = SseAstro::hoursToRadians(1.30279);
   expectedDecRads = SseAstro::degreesToRadians(57.95659);
   SseAstro::eclipticToEquatorial(eclipLongRads, eclipLatRads,
				  SseAstro::EclipObliqJ2000Rads,
				  raRads, decRads);
   assertDoublesEqual(expectedRaRads, raRads, radsTol);
   assertDoublesEqual(expectedDecRads, decRads, radsTol);


   eclipLongRads = SseAstro::degreesToRadians(265);
   eclipLatRads = SseAstro::degreesToRadians(-20);
   expectedRaRads = SseAstro::hoursToRadians(17.56904);
   expectedDecRads = SseAstro::degreesToRadians(-43.32716);
   SseAstro::eclipticToEquatorial(eclipLongRads, eclipLatRads,
				  SseAstro::EclipObliqJ2000Rads,
				  raRads, decRads);
   assertDoublesEqual(expectedRaRads, raRads, radsTol);
   assertDoublesEqual(expectedDecRads, decRads, radsTol);

}

void TestSseAstro::testDegsOrHoursToDecimal()
{
   double tol(0.000001);

   assertDoublesEqual(-60.0, SseAstro::degreesToDecimal('-',60,0,0), tol);
   assertDoublesEqual(60.0, SseAstro::degreesToDecimal('+',60,0,0), tol);
   assertDoublesEqual(60.0, SseAstro::degreesToDecimal('x',60,0,0), tol);
   assertDoublesEqual(-60.0, SseAstro::degreesToDecimal('-',-60,0,0), tol);

   assertDoublesEqual(-15.5, SseAstro::degreesToDecimal('-',-15,30,0), tol);
   assertDoublesEqual(-85.2601, SseAstro::degreesToDecimal('-',-85,15,36.36), tol);
   assertDoublesEqual(-0.5125, SseAstro::degreesToDecimal('-',-00,30,45), tol);

   assertDoublesEqual(12.0, SseAstro::hoursToDecimal(12,0,0), tol);
   assertDoublesEqual(6.51, SseAstro::hoursToDecimal(6,30,36), tol);

}

void TestSseAstro::testHoursToHms()
{
   double decimalHours(2.736937500);  // 2 44 12.975

   int hours, mins;
   double secs;
   SseAstro::decimalHoursToHms(decimalHours, &hours, &mins, &secs);

   cout << "decimalHours: " << decimalHours
        << " h:m:s: " << hours << ":" << mins << ":" << secs << endl;

   double valueTol(1e-6);
   assertLongsEqual(2, hours);
   assertLongsEqual(44, mins);
   assertDoublesEqual(12.975, secs, valueTol);


}

void TestSseAstro::testDegsToDms()
{
   double decimalDeg(-49.227750000);  // -49 13 39.90

   char sign;
   int degs, mins;
   double secs;
   SseAstro::decimalDegreesToDms(decimalDeg, &sign, &degs, &mins, &secs);

   cout << "decimalDeg: " << decimalDeg
        << " d:m:s: " << sign << degs << ":" << mins << ":" << secs << endl;

   double valueTol(1e-6);
   cu_assert(sign == '-');
   assertLongsEqual(49, degs);
   assertLongsEqual(13, mins);
   assertDoublesEqual(39.9, secs, valueTol);

   // test negative zero degrees
   decimalDeg = -0.5;  // -00 30 00
   SseAstro::decimalDegreesToDms(decimalDeg, &sign, &degs, &mins, &secs);

   cout << "decimalDeg: " << decimalDeg
        << " d:m:s: " << sign << degs << ":" << mins << ":" << secs << endl;
   cu_assert(sign == '-');
   assertLongsEqual(00, degs);
   assertLongsEqual(30, mins);
   assertDoublesEqual(00, secs, valueTol);

   decimalDeg = 45.5;  // +45 30 00
   SseAstro::decimalDegreesToDms(decimalDeg, &sign, &degs, &mins, &secs);

   cout << "decimalDeg: " << decimalDeg
        << " d:m:s: " << sign << degs << ":" << mins << ":" << secs << endl;

   cu_assert(sign == '+');
   assertLongsEqual(45, degs);
   assertLongsEqual(30, mins);
   assertDoublesEqual(00, secs, valueTol);

   // handle roundoff of seconds to next minute
   decimalDeg = 41.9;  // +41 54 00  (not: +41 53 60)
   SseAstro::decimalDegreesToDms(decimalDeg, &sign, &degs, &mins, &secs);

   cout << "decimalDeg: " << decimalDeg
        << " d:m:s: " << sign << degs << ":" << mins << ":" << secs << endl;
   cu_assert(sign == '+');
   assertLongsEqual(41, degs);
   assertLongsEqual(54, mins);
   assertDoublesEqual(00.0, secs, valueTol); 
}


void TestSseAstro::testAtmosRefract()
{
   double horizDeg(18.0);
   double expectedRefractDeg(0.05032);

   double tolDeg(0.0001);
   double refractDeg = SseAstro::atmosRefractDeg(horizDeg);
   assertDoublesEqual(expectedRefractDeg, refractDeg, tolDeg);
}


void TestSseAstro::testComputeHourAngle()
{
     const double horizDeg(18.0);
     const double siteLatDeg(40.8173);
     double hourAngleTol(0.0001);

     double decDeg(0.0);
     double hourAngle = SseAstro::hourAngle(decDeg, siteLatDeg, horizDeg);
     double expectedHourAngle(4.393370);

     assertDoublesEqual(expectedHourAngle, hourAngle, hourAngleTol);
		     

     // test circumpolar
     double NorthPoleDecDeg(90);
     decDeg = NorthPoleDecDeg - (siteLatDeg * 0.5);
     expectedHourAngle = 12.0;
     hourAngle = SseAstro::hourAngle(decDeg, siteLatDeg, horizDeg);
     assertDoublesEqual(expectedHourAngle, hourAngle, hourAngleTol);


     // test never visible
     double SouthPoleDecDeg(-90);
     decDeg = SouthPoleDecDeg + (siteLatDeg * 0.5);
     expectedHourAngle = 0.0;
     hourAngle = SseAstro::hourAngle(decDeg, siteLatDeg, horizDeg);
     assertDoublesEqual(expectedHourAngle, hourAngle, hourAngleTol);

     // Create hour angle table for hat creek
     // decDeg values chosen for 18deg horizon
     if (1)
     {
        stringstream strm;
        strm.precision(2);
        strm.setf(std::ios::fixed);  // show all decimal places up to precision
        vector<double> decDegVect;

        decDegVect.push_back(-31.2);
        decDegVect.push_back(-31.1);

        for (double decDeg = -31; decDeg <= -25; decDeg+=1)
        {
           decDegVect.push_back(decDeg);
        }

        for (double decDeg = -20; decDeg <= 60; decDeg+=5)
        {
           decDegVect.push_back(decDeg);
        }

        for (double decDeg = 61; decDeg <= 68; decDeg+=1)
        {
           decDegVect.push_back(decDeg);
        }

        strm << "ATA Hour Angle.  HorizDeg: " << horizDeg << endl;
        strm << endl;
        strm << "DecDeg  HARise  HASet  UpTime" << endl;

        for (unsigned int i=0; i<decDegVect.size(); i++)
        {
           double hourAngle = 
              SseAstro::hourAngle(decDegVect[i], siteLatDeg, horizDeg);
           
           strm << decDegVect[i] 
                << "  " << hourAngle * -1 
                << "  " << hourAngle
                << "  " << hourAngle * 2 
                << endl;
        }

        cout << endl;
        cout << strm.str();
     }

}

void TestSseAstro::testSiderealTimeConvert()
{
   double siderealAngleRads(SseAstro::hoursToRadians(1.0));
   int expectedSolarTimeSecs(static_cast<int>(SseAstro::SecsPerHour * 
			    SseAstro::SolarDaysPerSideralDay));
   
   int solarTimeSecs = 
      SseAstro::siderealRadsToSolarTimeSecs(siderealAngleRads);

   //cout << "solarTimeSecs=" << solarTimeSecs << endl;
   
   cu_assert(expectedSolarTimeSecs == solarTimeSecs);

}

void TestSseAstro::testTimeToJulianEpoch()
{
   // j2000 epoch exactly
   double epochTol(1e-7);
   double expectedEpoch(2000.0);   
   double epoch(SseAstro::timeToJulianEpoch(SseAstro::J2000UnixTimeSecs));
   assertDoublesEqual(expectedEpoch, epoch, epochTol);

   // Meeus, 2nd Ed, Example 21.b
   time_t time(1857702816); // 2028-11-13 04:33:36 UTC
   expectedEpoch = 2028.8670500;
   epoch = SseAstro::timeToJulianEpoch(time);
   assertDoublesEqual(expectedEpoch, epoch, epochTol);
}


void TestSseAstro::testJulianEpochToMjd()
{
   // j2000 epoch exactly
   double tol(1e-7);
   double j2000Epoch(2000.0);   
   double expectedMjd(51544.50000);
   double mjd(SseAstro::julianEpochToMjd(j2000Epoch));
   assertDoublesEqual(expectedMjd, mjd, tol);
}

void TestSseAstro::testMjdConvert()
{
   // mjdToJd
   double tol(1e-7);
   double mjd(51544.5);
   double expectedJd(2451545.0);
   assertDoublesEqual(expectedJd, SseAstro::mjdToJd(mjd), tol);

   // timeToMjd
   time_t timeUtcSecs(SseAstro::J2000UnixTimeSecs); 
   double expectedMjd(51544.5);   // JD=2451545.0 (Meeus, 2nd ed, ch 7.)
   assertDoublesEqual(expectedMjd, SseAstro::timeToMjd(timeUtcSecs), tol);
}

void TestSseAstro::testGmst()
{
    double mjd(SseAstro::julianEpochToMjd(2001.0));
    double expectedGmstRads(0.182707);
    double gmstRads(SseAstro::gmstRads(mjd));
    double tolRads(1e-6);
    assertDoublesEqual(expectedGmstRads, gmstRads, tolRads);
}

void TestSseAstro::testLmst()
{
   // Meeus, 2nd Ed, example 12.b
   // LMST at Greenwich at 1987 10 Apr 19:21:00 UT
   // is: 8h 34m 57.0896s

   cout.precision(9);
   cout.setf(std::ios::fixed); 

   time_t time(545080860); // 1987-04-10 19:21:00 UTC
   double eastLongRads(0); // Greenwich

   double expectedLmstHours(SseAstro::hoursToDecimal(8, 34, 57.0896));
   double expectedLmstRads(SseAstro::hoursToRadians(expectedLmstHours));
   double lmstRads(SseAstro::lmstRads(time, eastLongRads));

   double lmstTolRads(1e-8);
   assertDoublesEqual(expectedLmstRads, lmstRads, lmstTolRads); 

   /*
     Hat Creek, from ATA
     obs@antcntl /home/obs> date; atamainsystime -l
     Fri Jan 18 16:43:19 PST 2008
     UTC       2008-01-19T00:43:19.748Z
     LAST      0.4776152               
     MJD(UTC)  54484.03008968          
     ATATAI    1200703432748000000     
   */

   time = 1200703399;  // 2008-01-19 00:43:19 UTC
   expectedLmstRads = SseAstro::hoursToRadians(0.4776152);
   lmstRads = SseAstro::lmstRads(time, HatCreekLongWestRads);

   lmstTolRads = 1e-3;  // off by about a second
   assertDoublesEqual(expectedLmstRads, lmstRads, lmstTolRads);
}

void TestSseAstro::testTimeLeft()
{
   double tolHours(1e-6);

   // nominal case, rise < set, no midnight straddle
   double riseHours(3);
   double setHours(6);
   double currentHours(4);
   double timeLeftHours(-1);
   double expectedTimeLeftHours(0);

   expectedTimeLeftHours = 2;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);
   
   currentHours = 2;
   expectedTimeLeftHours = 0;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   // rise > set (cross midnight)
   riseHours = 22;
   setHours = 4;
   currentHours = 21;
   expectedTimeLeftHours = 0;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   currentHours = 23;
   expectedTimeLeftHours = 5;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   currentHours = 3;
   expectedTimeLeftHours = 1;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   currentHours = 5;
   expectedTimeLeftHours = 0;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   // up time longer than 12 hours, across midnight
   riseHours = 22;
   setHours = 20;
   currentHours = 21;
   expectedTimeLeftHours = 0;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   currentHours = 23;
   expectedTimeLeftHours = 21;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

   currentHours = 19;
   expectedTimeLeftHours = 1;
   timeLeftHours = SseAstro::timeLeftHours(riseHours, setHours, currentHours);
   assertDoublesEqual(expectedTimeLeftHours, timeLeftHours, tolHours);

}

void TestSseAstro::testRiseTransitSet()
{
   time_t obsTime(1202415956); // 2008-02-07 20:25:56 UTC
   double aldebRaHours = SseAstro::hoursToDecimal(4,35,55.2);
   double aldebDecDeg = SseAstro::degreesToDecimal('+',16,30,33.0);

   /*
     expected rise, transit, set at Hat Creek from xephemin UT:
     lmst: 21:29:26
     rise: 22:11,  trans: 03:32, set: 08:56  uptime: 10:45
   */
   double riseHours, transitHours, setHours;
   double untilRiseHours, untilSetHours;
   SseAstro::riseTransitSet(aldebRaHours, aldebDecDeg,
                            HatCreekLongWestDeg, HatCreekLatDeg,
                            HatCreekHorizonDeg, obsTime,
                            &riseHours, &transitHours, &setHours,
                            &untilRiseHours, &untilSetHours);

   cout << "riseHours: " << riseHours << endl;
   cout << "transitHours: " << transitHours << endl;
   cout << "setHours: " << setHours << endl;
   cout << "untilRiseHours: " << untilRiseHours << endl;
   cout << "untilSetHours: " << untilSetHours << endl;

   double minPerHour(60);
   double hoursTol(4/minPerHour);  // off by a few minutes is ok
   double expectedRiseHours(SseAstro::hoursToDecimal(22,14,0));
   assertDoublesEqual(expectedRiseHours, riseHours, hoursTol);

   double expectedTransitHours(SseAstro::hoursToDecimal(3,32,0));
   assertDoublesEqual(expectedTransitHours, transitHours, hoursTol);

   double expectedSetHours(SseAstro::hoursToDecimal(8,56,0));
   assertDoublesEqual(expectedSetHours, setHours, hoursTol);

   // before rise, target not currently visible
   double expectedUntilRiseHours(SseAstro::hoursToDecimal(1,46,0));
   assertDoublesEqual(expectedUntilRiseHours, untilRiseHours, hoursTol);

   double expectedUntilSetHours(SseAstro::hoursToDecimal(0,0,0));
   assertDoublesEqual(expectedUntilSetHours, untilSetHours, hoursTol);


   // one hour after rise, 2008-02-07 23:11 UTC
   obsTime = 1202425860; 
   SseAstro::riseTransitSet(aldebRaHours, aldebDecDeg,
                            HatCreekLongWestDeg, HatCreekLatDeg,
                            HatCreekHorizonDeg, obsTime,
                            &riseHours, &transitHours, &setHours,
                            &untilRiseHours, &untilSetHours);

   // rise,transit,set should not change
   assertDoublesEqual(expectedRiseHours, riseHours, hoursTol);
   assertDoublesEqual(expectedTransitHours, transitHours, hoursTol);
   assertDoublesEqual(expectedSetHours, setHours, hoursTol);

   expectedUntilRiseHours = SseAstro::hoursToDecimal(0,0,0);
   assertDoublesEqual(expectedUntilRiseHours, untilRiseHours, hoursTol);

   expectedUntilSetHours = SseAstro::hoursToDecimal(9,45,0);
   assertDoublesEqual(expectedUntilSetHours, untilSetHours, hoursTol);

   // one hour before setting, 2008-02-08 7:56 UTC
   obsTime = 1202457360;

   SseAstro::riseTransitSet(aldebRaHours, aldebDecDeg,
                            HatCreekLongWestDeg, HatCreekLatDeg,
                            HatCreekHorizonDeg, obsTime,
                            &riseHours, &transitHours, &setHours,
                            &untilRiseHours, &untilSetHours);

   // rise,transit,set should not change
   assertDoublesEqual(expectedRiseHours, riseHours, hoursTol);
   assertDoublesEqual(expectedTransitHours, transitHours, hoursTol);
   assertDoublesEqual(expectedSetHours, setHours, hoursTol);

   expectedUntilRiseHours = SseAstro::hoursToDecimal(0,0,0);
   assertDoublesEqual(expectedUntilRiseHours, untilRiseHours, hoursTol);

   expectedUntilSetHours = SseAstro::hoursToDecimal(1,0,0);
   assertDoublesEqual(expectedUntilSetHours, untilSetHours, hoursTol);

   // circumpolar
   double targetRaHours(8);
   double targetDecDeg(85);

   SseAstro::riseTransitSet(targetRaHours, targetDecDeg,
                            HatCreekLongWestDeg, HatCreekLatDeg,
                            HatCreekHorizonDeg, obsTime,
                            &riseHours, &transitHours, &setHours,
                            &untilRiseHours, &untilSetHours);

   expectedRiseHours = 0;
   expectedTransitHours = 0;
   expectedSetHours = 0;
   assertDoublesEqual(expectedRiseHours, riseHours, hoursTol);
   assertDoublesEqual(expectedTransitHours, transitHours, hoursTol);
   assertDoublesEqual(expectedSetHours, setHours, hoursTol);

   expectedUntilRiseHours = SseAstro::hoursToDecimal(0,0,0);
   assertDoublesEqual(expectedUntilRiseHours, untilRiseHours, hoursTol);

   expectedUntilSetHours = 24;
   assertDoublesEqual(expectedUntilSetHours, untilSetHours, hoursTol);

   // below horizon (never visible)
   targetRaHours = 8;
   targetDecDeg = -85;

   SseAstro::riseTransitSet(targetRaHours, targetDecDeg,
                            HatCreekLongWestDeg, HatCreekLatDeg,
                            HatCreekHorizonDeg, obsTime,
                            &riseHours, &transitHours, &setHours,
                            &untilRiseHours, &untilSetHours);

   expectedRiseHours = 0;
   expectedTransitHours = 0;
   expectedSetHours = 0;
   expectedUntilRiseHours = 0;
   expectedUntilSetHours = 0;
   assertDoublesEqual(expectedRiseHours, riseHours, hoursTol);
   assertDoublesEqual(expectedTransitHours, transitHours, hoursTol);
   assertDoublesEqual(expectedSetHours, setHours, hoursTol);
   assertDoublesEqual(expectedUntilRiseHours, untilRiseHours, hoursTol);
   assertDoublesEqual(expectedUntilSetHours, untilSetHours, hoursTol);

}


void TestSseAstro::testAngSepRads()
{
   double const deltaRads = 0.001;

   double raOneRads(0.0);
   double decOneRads(0.0);
   double raTwoRads(0.0);
   double decTwoRads(0.0);
   double expectedSepRads = 0.0;
   double sepRads = SseAstro::angSepRads(raOneRads, decOneRads, raTwoRads, decTwoRads);
   assertDoublesEqual(expectedSepRads, sepRads, deltaRads);

   raOneRads = M_PI;
   decOneRads = 0.0;
   raTwoRads = 0.0;
   decTwoRads = 0.0;
   expectedSepRads = M_PI;
   sepRads = SseAstro::angSepRads(raOneRads, decOneRads, raTwoRads, decTwoRads);
   assertDoublesEqual(expectedSepRads, sepRads, deltaRads);

   raOneRads = 0.0;
   decOneRads = M_PI/4;
   raTwoRads = 0.0;
   decTwoRads = -M_PI/4;
   expectedSepRads = M_PI/2;
   sepRads = SseAstro::angSepRads(raOneRads, decOneRads, raTwoRads, decTwoRads);
   assertDoublesEqual(expectedSepRads, sepRads, deltaRads);
}

void TestSseAstro::testGalCoordConvert()
{
    // Sirius - From Simbad
    const double siriusRa2000Hours(6.752476944);
    const double siriusDec2000Deg(-16.716116667);

    const double siriusGalLong2000Deg(227.2303);
    const double siriusGalLat2000Deg(-08.8903);

    // RA/Dec to Gal
    double galLongRads;
    double galLatRads;
    SseAstro::equ2000ToGal(
        SseAstro::hoursToRadians(siriusRa2000Hours),
        SseAstro::degreesToRadians(siriusDec2000Deg),
        &galLongRads, &galLatRads);

    double tolDegs(0.0001);

    assertDoublesEqual(siriusGalLong2000Deg, 
                       SseAstro::radiansToDegrees(galLongRads), tolDegs);

    assertDoublesEqual(siriusGalLat2000Deg, 
                       SseAstro::radiansToDegrees(galLatRads), tolDegs);

    // Gal to Ra/Dec
    double raRads;
    double decRads;

    SseAstro::galToEqu2000(
        SseAstro::degreesToRadians(siriusGalLong2000Deg),
        SseAstro::degreesToRadians(siriusGalLat2000Deg),
        &raRads, &decRads);
    
    const double degPerHour(15.0);
    assertDoublesEqual(siriusRa2000Hours * degPerHour,
                       SseAstro::radiansToDegrees(raRads), tolDegs);

    assertDoublesEqual(siriusDec2000Deg,
                       SseAstro::radiansToDegrees(decRads), tolDegs);

}

void TestSseAstro::testSunPosition()
{
   cout << "testSunPosition" << endl;
   time_t positionTime(1103665980);  // Tue Dec 21 21:53:00 GMT 2004

   double raRads;
   double decRads;
   SseAstro::sunPosition(positionTime, &raRads, &decRads);

   cout << "calculated sun pos: " << endl;
   printRaRadsInHms(raRads);
   printDecRadsInDms(decRads);

   // expected value from JPL Horizons
   // geocentric observer
   // 2004-Dec-21 21:53 UT, RA (hms)=18 01 25.93, Dec (dms) = -23 26 17.5

   double expectedSunRaRads(SseAstro::hoursToRadians(
      SseAstro::hoursToDecimal(18, 01, 25.93)));
    
   double expectedSunDecRads(SseAstro::degreesToRadians(
      SseAstro::degreesToDecimal('-',-23,26,17.5)));

   cout << "expected sun pos: " << endl;
   printRaRadsInHms(expectedSunRaRads);
   printDecRadsInDms(expectedSunDecRads);
                                                        
   //double tolRads(1e-4);  // 1e-4 rads = ~21 arcsecs
   double tolRads(2e-3);  // 1e-3 rads = ~220 arcsecs

   assertDoublesEqual(expectedSunRaRads, raRads, tolRads);
   assertDoublesEqual(expectedSunDecRads, decRads, tolRads);
}

void TestSseAstro::testMoonPosition()
{
   cout << "testMoonPosition" << endl;
   time_t positionTime(1165011780); // 2006-12-01 22:23:00 UTC

   /*
     JPL Horizons, geocentric
     2006-Dec-01 22:23 UTC, RA(hms) = 01 39 51.80 dec (dms) = +13 32 45.1,
   */

   double raRads;
   double decRads;
   SseAstro::moonPosition(positionTime, &raRads, &decRads);

   cout << "calculated moon pos: " << endl;
   printRaRadsInHms(raRads);
   printDecRadsInDms(decRads);

   double expectedMoonRaRads(SseAstro::hoursToRadians(
      SseAstro::hoursToDecimal(01, 39, 51.80)));
    
   double expectedMoonDecRads(SseAstro::degreesToRadians(
      SseAstro::degreesToDecimal(PlusSign,13,32,45.1)));

   cout << "expected moon pos: " << endl;
   printRaRadsInHms(expectedMoonRaRads);
   printDecRadsInDms(expectedMoonDecRads);

   //double tolRads(2e-4);  // 2e-4 rads = ~41 arcsecs
   //double tolRads(0.002);  // 0.002 rads = ~410 arcsecs
   double tolRads(0.003);  // 0.003 rads = 10 arcmin

   assertDoublesEqual(expectedMoonRaRads, raRads, tolRads);
   assertDoublesEqual(expectedMoonDecRads, decRads, tolRads);
    
   // try 5 years later
   positionTime=1322880000;  // 2011-12-03 02:40:00 UTC

   /*
     JPL Horizons position (geocentric)
     2011-Dec-03 02:40 UT, RA (hms) = 23 08 24.93  dec (dms) = +00 12 07.6
   */

   SseAstro::moonPosition(positionTime, &raRads, &decRads);

   cout << "calculated moon pos: " << endl;
   printRaRadsInHms(raRads);
   printDecRadsInDms(decRads);

   expectedMoonRaRads = SseAstro::hoursToRadians(
      SseAstro::hoursToDecimal(23, 8, 24.93));
    
   expectedMoonDecRads = SseAstro::degreesToRadians(
      SseAstro::degreesToDecimal(PlusSign,00,12,7.6));

   cout << "expected moon pos: " << endl;
   printRaRadsInHms(expectedMoonRaRads);
   printDecRadsInDms(expectedMoonDecRads);

   assertDoublesEqual(expectedMoonRaRads, raRads, tolRads);
   assertDoublesEqual(expectedMoonDecRads, decRads, tolRads);
}

void TestSseAstro::testPositionChange()
{
   testTargetNoPrecessionNoProperMotion();
   testTargetEpochAndEquinoxChange();
   testTargetEpochChangeRaWrap();
   testTargetEpochChangeAtPoles();
   testTargetEpochChangeNearNorthPole();
   testTargetEpochChangeAgainstSimbad();
}


void TestSseAstro::testTargetNoPrecessionNoProperMotion()
{
   cout << "testTargetNoPrecessionNoProperMotion" << endl;

   /* Request j2000 epoch for a target with no proper motion.
      There should be no change from the original coordinates */

   double targetRaRads(M_PI);
   double targetDecRads(M_PI/4);
   double pmRaMasYr(0);
   double pmDecMasYr(0);

   double resultRaRads;
   double resultDecRads;

   time_t epochTime(SseAstro::J2000UnixTimeSecs); 
   cout << "j2000 time: " << epochTime << endl;

   SseAstro::positionAtNewEpochAndEquinox(
      epochTime, targetRaRads, targetDecRads,
      pmRaMasYr, pmDecMasYr,
      &resultRaRads, &resultDecRads);

   cout << "Expected RA/Dec: " << endl;
   printRaRadsInHms(targetRaRads);
   printDecRadsInDms(targetDecRads);

   cout << "Got RA/Dec: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   double tolRads(1e-9);

   assertDoublesEqual(targetRaRads, resultRaRads, tolRads);
   assertDoublesEqual(targetDecRads, resultDecRads, tolRads);
}


void TestSseAstro::testTargetEpochAndEquinoxChange()
{
   cout << "testTargetEpochAndEquinoxChange" << endl;
   cout.precision(9);
   cout.setf(std::ios::fixed); 

   /*
     From Meeus, Astronomical Alg. 2nd Ed.
     Example 21.b

     Star: Theta Persei
     J2000.0 Equinox and Epoch
     RA hms = 02 44 11.986  (2.736662778 hours)
     Dec dms = +49 13 42.48 (49.228466667 deg)
     PM Ra secs/Yr (Note: *not* arcsecs) = 0.03425
     PM Dec arcsecs/Yr = -0.0895
   */

   // epoch change first (ie, proper motion change only):

   double targetRaHours(SseAstro::hoursToDecimal(02, 44, 11.986));
   double targetRaRads(SseAstro::hoursToRadians(targetRaHours));
   double targetDecDeg(SseAstro::degreesToDecimal(PlusSign, 49, 13, 42.48));
   double targetDecRads(SseAstro::degreesToRadians(targetDecDeg));

   cout << "Start RA/Dec: " << endl;
   printRaRadsInHms(targetRaRads);
   printDecRadsInDms(targetDecRads);

   // convert mu RA from secs to arcdist in mas
   double pmRaSecYr(0.03425);
   double pmRaMasYr(DegPerHour * pmRaSecYr * cos(targetDecRads) * MasPerArcSec);

   cout << "pmRaSecYr: " << pmRaSecYr <<endl;
   cout << "pmRaMasYr: " << pmRaMasYr <<endl;

   double pmDecMasYr(-0.0895 * MasPerArcSec);
   //double parallaxMas(0.0); // unused

   // New epoch: 
   time_t epochTime(1857702816); // 2028-11-13 04:33:36 UTC
   cout << "time: " << epochTime << endl;

   double resultRaRads;
   double resultDecRads;
   SseAstro::positionAtNewEpoch(epochTime, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   double tolRads(1e-7);

   // epoch change only 
   double expectedRaHours(SseAstro::hoursToDecimal(02, 44, 12.975));
   double expectedRaRads(SseAstro::hoursToRadians(expectedRaHours));
   double expectedDecDeg(SseAstro::degreesToDecimal(PlusSign, +49, 13, 39.90));
   double expectedDecRads(SseAstro::degreesToRadians(expectedDecDeg));

   cout << "Expected RA/Dec: " << endl;
   printRaRadsInHms(expectedRaRads);
   printDecRadsInDms(expectedDecRads);

   cout << "Got RA/Dec: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   cout << "expectedRaRads: " << expectedRaRads
        << " got: " << resultRaRads << endl;

   cout << "expectedDecRads: " << expectedDecRads
        << " got: " << resultDecRads << endl;

   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);

   // New epoch and equinox
   // ----------------------
   expectedRaHours = SseAstro::hoursToDecimal(02, 46, 11.331);
   expectedRaRads = SseAstro::hoursToRadians(expectedRaHours);

   expectedDecDeg = SseAstro::degreesToDecimal(PlusSign, +49, 20, 54.54);
   expectedDecRads = SseAstro::degreesToRadians(expectedDecDeg);

   cout << "Expected RA/Dec: " << endl;
   printRaRadsInHms(expectedRaRads);
   printDecRadsInDms(expectedDecRads);

   SseAstro::positionAtNewEpochAndEquinox(
      epochTime, targetRaRads, targetDecRads,
      pmRaMasYr, pmDecMasYr,
      &resultRaRads, &resultDecRads);

   cout << "Got RA/Dec: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);
}

void TestSseAstro::testTargetEpochChangeAgainstSimbad()
{
   cout << "testTargetEpochChangeAgainstSimbad" << endl;

   /*
     Compare target epoch change with simbad results
     Simbad j2000 epoch & equinox:

     Habcat star: HIP46598
     coords at j2000 epoch & equinox:  RA: 09 30 08.593, Dec: -00 57 58.76
     pm ra & dec mas:  -258.19, 184.57
     parallax mas: 22.02

     2008.04 epoch (15 jan 2008), j2000 equinox:
     RA: 09 30 08.454, Dec: -00 57 57.27

   */
   double targetRaHours(SseAstro::hoursToDecimal(9, 30, 8.593));
   double targetRaRads(SseAstro::hoursToRadians(targetRaHours));
   double targetDecDeg(SseAstro::degreesToDecimal(MinusSign, 0, 57, 58.76));
   double targetDecRads(SseAstro::degreesToRadians(targetDecDeg));

   cout << "Orig target coords: " << endl;
   printRaRadsInHms(targetRaRads);
   printDecRadsInDms(targetDecRads);

   double pmRaMasYr(-258.19);
   double pmDecMasYr(184.57);
   time_t time(1200436564); // 2008-01-15 22:36:04 UTC

   double resultRaRads;
   double resultDecRads;
   SseAstro::positionAtNewEpoch(time, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   double expectedRaHours(SseAstro::hoursToDecimal(9, 30, 8.454));
   double expectedRaRads(SseAstro::hoursToRadians(expectedRaHours));

   double expectedDecDeg(SseAstro::degreesToDecimal(MinusSign, 0, 57, 57.27));
   double expectedDecRads(SseAstro::degreesToRadians(expectedDecDeg));

   cout << "Expected coords: " << endl;
   printRaRadsInHms(expectedRaRads);
   printDecRadsInDms(expectedDecRads);

   cout << "Got coords: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   double tolRads(1e-6);
   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);
}


void TestSseAstro::testTargetEpochChangeRaWrap()
{
   cout << "testTargetEpochChangeRaWrap" << endl;

   double targetRaHours(SseAstro::hoursToDecimal(23, 59, 05));
   double targetRaRads(SseAstro::hoursToRadians(targetRaHours));

   double targetDecDeg(SseAstro::degreesToDecimal(PlusSign, 0, 0, 0));
   double targetDecRads(SseAstro::degreesToRadians(targetDecDeg));

   cout << "orig target coords: " << endl;
   printRaRadsInHms(targetRaRads);
   printDecRadsInDms(targetDecRads);

   // distance to move in RA
   double totalDistanceRaSecs(120);
   double totalYears(12);
   double ArcSecToSecRatio(15);
   double pmRaMasYr((totalDistanceRaSecs/totalYears)*ArcSecToSecRatio*MasPerArcSec);
   double pmDecMasYr(0);

   // Use j2000 + 12 years (includes 3 leap days to make calculation easy)
   time_t time(1325419200); // 2012-01-01 12:00:00 UTC

   double resultRaRads;
   double resultDecRads;
   SseAstro::positionAtNewEpoch(time, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   double expectedRaHours(SseAstro::hoursToDecimal(00, 01, 05));
   double expectedRaRads(SseAstro::hoursToRadians(expectedRaHours));

   // dec shouldn't change
   double expectedDecRads(targetDecRads);

   cout << "Expected coords: " << endl;
   printRaRadsInHms(expectedRaRads);
   printDecRadsInDms(expectedDecRads);

   cout << "Got coords: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   double tolRads(1e-5);
   cout << "expectedRaRads: " << expectedRaRads
        << "  resultRaRads: " << resultRaRads << endl;

   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);

   // negative RA wrap
   targetRaHours = SseAstro::hoursToDecimal(00, 01, 05);
   targetRaRads = SseAstro::hoursToRadians(targetRaHours);
   pmRaMasYr *= -1;

   SseAstro::positionAtNewEpoch(time, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   expectedRaHours = SseAstro::hoursToDecimal(23, 59, 05);
   expectedRaRads = SseAstro::hoursToRadians(expectedRaHours);

   cout << "Expected coords: " << endl;
   printRaRadsInHms(expectedRaRads);
   printDecRadsInDms(expectedDecRads);

   cout << "Got coords: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);
}


void TestSseAstro::testTargetEpochChangeAtPoles()
{
   cout << "testTargetEpochChangeAtPoles" << endl;

   // Test target at exactly north & south pol, where precession in RA goes
   // to infinity.

   double targetRaHours(SseAstro::hoursToDecimal(00, 00, 00));
   double targetRaRads(SseAstro::hoursToRadians(targetRaHours));

   double targetDecDeg(SseAstro::degreesToDecimal(PlusSign, 90, 0, 0));
   double targetDecRads(SseAstro::degreesToRadians(targetDecDeg));

   double pmRaMasYr(100);
   double pmDecMasYr(0);

   // epoch j2000 + 1 year
   time_t time(978307200); // 2001-01-01 00:00 UTC

   double resultRaRads;
   double resultDecRads;
   SseAstro::positionAtNewEpoch(time, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   cout << "got Ra/Dec: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   // should be no change
   double expectedRaRads(targetRaRads);
   double expectedDecRads(targetDecRads);
   double tolRads(1e-5);
   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);

   // test south pol
   targetDecDeg *= -1;
   targetDecRads = SseAstro::degreesToRadians(targetDecDeg);

   SseAstro::positionAtNewEpoch(time, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   expectedRaRads = targetRaRads;
   expectedDecRads = targetDecRads;

   cout << "got Ra/Dec: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);
}   

void TestSseAstro::testTargetEpochChangeNearNorthPole()
{
   cout << "testTargetEpochChangeNearNorthPole()" << endl;

   // precess in dec over the north pole

   double targetRaHours(SseAstro::hoursToDecimal(00, 00, 00));
   double targetRaRads(SseAstro::hoursToRadians(targetRaHours));

   double targetDecDeg(SseAstro::degreesToDecimal(PlusSign, 89, 59, 50));
   double targetDecRads(SseAstro::degreesToRadians(targetDecDeg));

   double pmRaMasYr(0);  // no change in RA
   double pmDecTotalSec(30);
   double pmDecMasYr(pmDecTotalSec*MasPerArcSec);

   // epoch j2000 + 1 year
   time_t time(978307200); // 2001-01-01 00:00 UTC

   double resultRaRads;
   double resultDecRads;
   SseAstro::positionAtNewEpoch(time, targetRaRads, targetDecRads,
                                pmRaMasYr, pmDecMasYr,
                                &resultRaRads, &resultDecRads);

   // Should move up over the pole and down the other side
   double expectedRaRads(SseAstro::hoursToRadians(
      SseAstro::hoursToDecimal(12, 00, 00)));

   double expectedDecRads(SseAstro::degreesToRadians(
      SseAstro::degreesToDecimal(PlusSign,89,59,40)));

   cout << "expected Ra/Dec: " << endl;
   printRaRadsInHms(expectedRaRads);
   printDecRadsInDms(expectedDecRads);

   cout << "got Ra/Dec: " << endl;
   printRaRadsInHms(resultRaRads);
   printDecRadsInDms(resultDecRads);

   double tolRads(1e-5);
   assertDoublesEqual(expectedRaRads, resultRaRads, tolRads);
   assertDoublesEqual(expectedDecRads, resultDecRads, tolRads);
}

void TestSseAstro::testGeosatDec()
{
   double obsLatDeg(43);
   double expectedGeosatDecDeg(-6.60);
   double tolDeg(1e-2);

   double geosatDecDeg(SseAstro::geosatDecDeg(obsLatDeg));
   assertDoublesEqual(expectedGeosatDecDeg, geosatDecDeg, tolDeg);

   obsLatDeg = 20;
   expectedGeosatDecDeg = -3.44;
   geosatDecDeg = SseAstro::geosatDecDeg(obsLatDeg);
   assertDoublesEqual(expectedGeosatDecDeg, geosatDecDeg, tolDeg);
}

Test *TestSseAstro::suite ()
{
   TestSuite *testSuite = new TestSuite("TestSseAstro");
   
   testSuite->addTest (new TestCaller<TestSseAstro>("testRadianConversion", &TestSseAstro::testRadianConversion));
   testSuite->addTest (new TestCaller<TestSseAstro>("testFoo", &TestSseAstro::testFoo));
   testSuite->addTest (new TestCaller<TestSseAstro>("testEclipticToEquatorial", &TestSseAstro::testEclipticToEquatorial));
   testSuite->addTest (new TestCaller<TestSseAstro>("testDegsOrHoursToDecimal", &TestSseAstro::testDegsOrHoursToDecimal));
   testSuite->addTest (new TestCaller<TestSseAstro>("testHoursToHms", &TestSseAstro::testHoursToHms));
   testSuite->addTest (new TestCaller<TestSseAstro>("testDegsToDms", &TestSseAstro::testDegsToDms));
   testSuite->addTest (new TestCaller<TestSseAstro>("testAtmosRefract", &TestSseAstro::testAtmosRefract));
   testSuite->addTest (new TestCaller<TestSseAstro>("testComputeHourAngle", &TestSseAstro::testComputeHourAngle));
   testSuite->addTest (new TestCaller<TestSseAstro>("testSiderealTimeConvert", &TestSseAstro::testSiderealTimeConvert));
   testSuite->addTest (new TestCaller<TestSseAstro>("testTimeToJulianEpoch", &TestSseAstro::testTimeToJulianEpoch));
   testSuite->addTest (new TestCaller<TestSseAstro>("testJulianEpochToMjd", &TestSseAstro::testJulianEpochToMjd));
   testSuite->addTest (new TestCaller<TestSseAstro>("testGmst", &TestSseAstro::testGmst));
   testSuite->addTest (new TestCaller<TestSseAstro>("testLmst", &TestSseAstro::testLmst));
   testSuite->addTest (new TestCaller<TestSseAstro>("testRst", &TestSseAstro::testRiseTransitSet));
   testSuite->addTest (new TestCaller<TestSseAstro>("testTimeLeft", &TestSseAstro::testTimeLeft));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testAngSepRads", &TestSseAstro::testAngSepRads));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testGalCoordConvert", &TestSseAstro::testGalCoordConvert));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testMjdConvert", &TestSseAstro::testMjdConvert));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testSunPosition", &TestSseAstro::testSunPosition));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testMoonPosition", &TestSseAstro::testMoonPosition));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testPositionChange", &TestSseAstro::testPositionChange));  
   testSuite->addTest (new TestCaller<TestSseAstro>("testGeosatDec", &TestSseAstro::testGeosatDec));  
   return testSuite;
}