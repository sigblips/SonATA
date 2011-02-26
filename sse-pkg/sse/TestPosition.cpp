/*******************************************************************************

 File:    TestPosition.cpp
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


#include <ace/OS.h>
#include "TestRunner.h"
#include "TestPosition.h"
#include "TargetPosition.h"
#include "SseAstro.h"

// Use GNU date to convert to unix time:
//% date --date "2011-01-01 00:00:00 UTC" +'%s'
//1293868800

const double MasPerArcSec(1000);
const double DegPerHour(15);
const char PlusSign('+');
const char MinusSign('-');

static const double HatCreekEastLongRad((2 * M_PI) - 2.12007378);
static const double HatCreekLatRad(0.71240225);


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

void TestPosition::setUp ()
{

}

void TestPosition::tearDown()
{

}

void TestPosition::testTargetPositionDistance()
{
   RaDec targetRaDec;
   targetRaDec.ra.setRadian(0);
   targetRaDec.dec.setRadian(0);

   double pmRaMasYr(0);
   double pmDecMasYr(0);
   double parallaxArcSec(0.1);
   double parallaxMas(parallaxArcSec * MasPerArcSec);
   
   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   double parTolArcSec(0.1);
   assertDoublesEqual(parallaxArcSec, pos.getParallaxArcSec(),
                      parTolArcSec);

   double expectedLy(32.6);
   double distTolLy(0.1);
   assertDoublesEqual(expectedLy, pos.distanceLightYears(),
                      distTolLy);
}

void TestPosition::testTargetNoPrecessionNoProperMotion()
{
   cout << "testTargetNoPrecessionNoProperMotion" << endl;

   /* Request j2000 epoch for a target with no proper motion.
      There should be no change from the original coordinates */

   RaDec targetRaDec;
   targetRaDec.ra.setRadian(M_PI);
   targetRaDec.dec.setRadian(M_PI/4);
   double pmRaMasYr(0);
   double pmDecMasYr(0);
   double parallaxMas(0.0);  // Not used

   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, 
                      parallaxMas);

   time_t time(SseAstro::J2000UnixTimeSecs); 
   cout << "j2000 time: " << time << endl;

   RaDec resultRaDec = pos.positionAtNewEpochAndEquinox(time);

   cout << "raDec orig: " << targetRaDec << endl;
   cout << "raDec result: " << resultRaDec << endl;

   double tolRads(1e-9);

   assertDoublesEqual(targetRaDec.ra, resultRaDec.ra, tolRads);
   assertDoublesEqual(targetRaDec.dec, resultRaDec.dec, tolRads);
}

void TestPosition::testTargetEpochAndEquinoxChange()
{

   cout << "testTargetEpochAndEquinoxChange" << endl;
   cout.precision(9);
   cout.setf(std::ios::fixed); 

   /*
     From Meeus, Astronomical Alg.
     Example 20.b, p. 127


     Star: Theta Persei
     J2000.0 Equinox and Epoch
     RA hms = 02 44 11.986  (2.736662778 hours)
     Dec dms = +49 13 42.48 (49.228466667 deg)
     PM Ra secs (Note: *not* arcsecs) = 0.03425
     PM Dec arcsecs = -0.0895


   */

   cout << "Start RA/Dec: " << endl;

   // epoch change first (ie, pm change only):

   RaDec targetRaDec;
   double raHours(SseAstro::hoursToDecimal(02, 44, 11.986));
   targetRaDec.ra.setRadian(SseAstro::hoursToRadians(raHours));

   printRaRadsInHms(targetRaDec.ra.getRadian());


   double decDeg(SseAstro::degreesToDecimal(PlusSign, 49, 13, 42.48));
   targetRaDec.dec.setRadian(SseAstro::degreesToRadians(decDeg));

   printDecRadsInDms(targetRaDec.dec.getRadian());

   // convert mu RA from secs to arcdist in mas
   double pmRaMasYr(DegPerHour * 0.03425 * cos(targetRaDec.dec.getRadian()) * 
                    MasPerArcSec);

   cout << "pmRaMasYr: " << pmRaMasYr <<endl;

   double pmDecMasYr(-0.0895 * MasPerArcSec);
   double parallaxMas(0.0); // unused

   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   // New epoch: 
   time_t time(1857702816); // 2028-11-13 04:33:36 UTC
   cout << "time: " << time << endl;

   RaDec resultRaDec = pos.positionAtNewEpoch(time);

   cout << "raDec orig: " << targetRaDec << endl;
   cout << "raDec result: " << resultRaDec << endl;

   cout.precision(9);
   cout.setf(std::ios::fixed);

   double tolRads(1e-7);

   // epoch change only 
   double expectedRaHours(SseAstro::hoursToDecimal(02, 44, 12.975));
   double expectedRaRads(SseAstro::hoursToRadians(expectedRaHours));

   cout << "expectedRaRads: " << expectedRaRads
        << " got: " << resultRaDec.ra.getRadian() << endl;
   printRaRadsInHms(resultRaDec.ra.getRadian());

   double expectedDecDeg(SseAstro::degreesToDecimal(PlusSign, +49, 13, 39.90));
   double expectedDecRads(SseAstro::degreesToRadians(expectedDecDeg));

   cout << "expectedDecRads: " << expectedDecRads
        << " got: " << resultRaDec.dec.getRadian() << endl;

   printDecRadsInDms(resultRaDec.dec.getRadian());


   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

   // New epoch and equinox
   // ----------------------
   expectedRaHours = SseAstro::hoursToDecimal(02, 46, 11.331);
   expectedRaRads = SseAstro::hoursToRadians(expectedRaHours);

   expectedDecDeg = SseAstro::degreesToDecimal(PlusSign, +49, 20, 54.54);
   expectedDecRads = SseAstro::degreesToRadians(expectedDecDeg);

   RaDec newEpochAndEquinoxRaDec = pos.positionAtNewEpochAndEquinox(time);
   printRaRadsInHms(newEpochAndEquinoxRaDec.ra.getRadian());
   printDecRadsInDms(newEpochAndEquinoxRaDec.dec.getRadian());

   assertDoublesEqual(expectedRaRads, newEpochAndEquinoxRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, newEpochAndEquinoxRaDec.dec, tolRads);

}

void TestPosition::testTargetEpochChangeRaWrap()
{
   RaDec targetRaDec;
   double raHours(SseAstro::hoursToDecimal(23, 59, 05));
   targetRaDec.ra.setRadian(SseAstro::hoursToRadians(raHours));

   double decDeg(SseAstro::degreesToDecimal(PlusSign, 0, 0, 0));
   targetRaDec.dec.setRadian(SseAstro::degreesToRadians(decDeg));

   double parallaxMas(0.0);  // unused

   // distance to move in RA
   double totalDistanceRaSecs(120);
   double totalYears(12);
   double ArcSecToSecRatio(15);
   double pmRaMasYr((totalDistanceRaSecs/totalYears)*ArcSecToSecRatio*MasPerArcSec);
   double pmDecMasYr(0);

   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   double expectedRaHours(SseAstro::hoursToDecimal(00, 01, 05));
   double expectedRaRads(SseAstro::hoursToRadians(expectedRaHours));

   // dec shouldn't change
   double expectedDecRads(SseAstro::degreesToRadians(decDeg));

   // Use j2000 + 12 years (includes 3 leap days to make calculation easy)
   time_t time(1325419200); // 2012-01-01 12:00:00 UTC
   RaDec resultRaDec = pos.positionAtNewEpoch(time);

   //printRaRadsInHms(resultRaDec.ra.getRadian());

   double tolRads(1e-5);
   cout << "expectedRaRads: " << expectedRaRads
        << "  resultRaDec.ra: " << resultRaDec.ra.getRadian() << endl;

   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

   // negative RA wrap
   raHours = SseAstro::hoursToDecimal(00, 01, 05);
   targetRaDec.ra.setRadian(SseAstro::hoursToRadians(raHours));
   pmRaMasYr *= -1;
   TargetPosition pos2(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);
   resultRaDec = pos2.positionAtNewEpoch(time);

   expectedRaHours = SseAstro::hoursToDecimal(23, 59, 05);
   expectedRaRads = SseAstro::hoursToRadians(expectedRaHours);

   printRaRadsInHms(resultRaDec.ra.getRadian());

   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

}

void TestPosition::testTargetEpochChangeAtPoles()
{
   // Test target at exactly north & south pol, where precession in RA goes
   // to infinity.

   RaDec targetRaDec;
   double raHours(SseAstro::hoursToDecimal(00, 00, 00));
   targetRaDec.ra.setRadian(SseAstro::hoursToRadians(raHours));

   double decDeg(SseAstro::degreesToDecimal(PlusSign, 90, 0, 0));
   targetRaDec.dec.setRadian(SseAstro::degreesToRadians(decDeg));

   double parallaxMas(0.0);  // unused
   double pmRaMasYr(100);
   double pmDecMasYr(0);

   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   // epoch j2000 + 1 year
   time_t time(978307200); // 2001-01-01 00:00 UTC
   RaDec resultRaDec = pos.positionAtNewEpoch(time);

   printRaRadsInHms(resultRaDec.ra.getRadian());
   printDecRadsInDms(resultRaDec.dec.getRadian());

   // should be no change
   double expectedRaRads(targetRaDec.ra.getRadian());
   double expectedDecRads(targetRaDec.dec.getRadian());
   double tolRads(1e-5);
   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

   // test south pol
   decDeg *= -1;
   targetRaDec.dec.setRadian(SseAstro::degreesToRadians(decDeg));
   TargetPosition posSouthPol(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   expectedRaRads = targetRaDec.ra.getRadian();
   expectedDecRads = targetRaDec.dec.getRadian();

   resultRaDec = posSouthPol.positionAtNewEpoch(time);
   
   printRaRadsInHms(resultRaDec.ra.getRadian());
   printDecRadsInDms(resultRaDec.dec.getRadian());

   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

}   


void TestPosition::testTargetEpochChangeNearNorthPole()
{
   // precess in dec over the north pole

   RaDec targetRaDec;
   double raHours(SseAstro::hoursToDecimal(00, 00, 00));
   targetRaDec.ra.setRadian(SseAstro::hoursToRadians(raHours));

   double decDeg(SseAstro::degreesToDecimal(PlusSign, 89, 59, 50));
   targetRaDec.dec.setRadian(SseAstro::degreesToRadians(decDeg));

   double parallaxMas(0.0);  // unused
   double pmRaMasYr(0);  // no change in RA
   double pmDecTotalSec(30);
   double pmDecMasYr(pmDecTotalSec*MasPerArcSec);

   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   // epoch j2000 + 1 year
   time_t time(978307200); // 2001-01-01 00:00 UTC
   RaDec resultRaDec = pos.positionAtNewEpoch(time);

   printRaRadsInHms(resultRaDec.ra.getRadian());
   printDecRadsInDms(resultRaDec.dec.getRadian());

   // Should move up over the pole and down the other side
   double expectedRaRads(SseAstro::hoursToRadians(
      SseAstro::hoursToDecimal(12, 00, 00)));

   double expectedDecRads(SseAstro::degreesToRadians(
      SseAstro::degreesToDecimal(PlusSign,89,59,40)));

   double tolRads(1e-5);
   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

}

void TestPosition::testTargetEpochChangeAgainstSimbad()
{
   cout << "testTargetEpochChangeAgainstSimbad" << endl;

   /*
     Compare target epoch change with simbad results
     Simbad j2000 epoch & equinox:

     Habcat star: HIP46598
     j2000 epoch & equinox:  09 30 08.593 -00 57 58.76
     pm ra & dec mas:  -258.19 184.57
     parallax mas: 22.02

     2008.04 epoch (15 jan 2008), j2000 equinox:
     09 30 08.454 -00 57 57.27

   */
   RaDec targetRaDec;
   double raHours(SseAstro::hoursToDecimal(9, 30, 8.593));
   targetRaDec.ra.setRadian(SseAstro::hoursToRadians(raHours));

   double decDeg(SseAstro::degreesToDecimal(MinusSign, 0, 57, 58.76));
   targetRaDec.dec.setRadian(SseAstro::degreesToRadians(decDeg));

   printRaRadsInHms(targetRaDec.ra.getRadian());
   printDecRadsInDms(targetRaDec.dec.getRadian());

   double pmRaMasYr(-258.19);
   double pmDecMasYr(184.57);
   double parallaxMas(22.02);

   TargetPosition pos(targetRaDec, pmRaMasYr, pmDecMasYr, parallaxMas);

   time_t time(1200436564); // 2008-01-15 22:36:04 UTC
   RaDec resultRaDec = pos.positionAtNewEpoch(time);

   printRaRadsInHms(resultRaDec.ra.getRadian());
   printDecRadsInDms(resultRaDec.dec.getRadian());

   RaDec expectedRaDec;
   double expectedRaHours(SseAstro::hoursToDecimal(9, 30, 8.454));
   expectedRaDec.ra.setRadian(SseAstro::hoursToRadians(expectedRaHours));

   double expectedDecDeg(SseAstro::degreesToDecimal(MinusSign, 0, 57, 57.27));
   expectedRaDec.dec.setRadian(SseAstro::degreesToRadians(expectedDecDeg));

   double expectedRaRads(expectedRaDec.ra.getRadian());
   double expectedDecRads(expectedRaDec.dec.getRadian());
   double tolRads(1e-6);
   assertDoublesEqual(expectedRaRads, resultRaDec.ra, tolRads);
   assertDoublesEqual(expectedDecRads, resultRaDec.dec, tolRads);

}

void TestPosition::runTest()
{
   testTargetPositionDistance();
   testTargetNoPrecessionNoProperMotion();
   testTargetEpochAndEquinoxChange();
   testTargetEpochChangeRaWrap();
   testTargetEpochChangeAtPoles();
   testTargetEpochChangeNearNorthPole();
   testTargetEpochChangeAgainstSimbad();
}

Test *TestPosition::suite()
{
   TestSuite *testSuite = new TestSuite("TestPosition");
   testSuite->addTest(new TestPosition("TestPosition"));
   
   return testSuite;
}
