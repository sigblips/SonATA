/*******************************************************************************

 File:    createEclipticSurveyTargetCat.cpp
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


// Create a target catalog for ecliptic survey, 
// +- 90 degrees longitude from current sun position,
// zero degrees eclip latitude (single pointing per longitude).
// Freq is 2292 Mhz (Pioneer 6-8)

#include <iostream>
#include <cmath>
#include <string>
#include "SseAstro.h"

using namespace std;

void printDbTableLine(const string &whichList,
                      int targetId, double raHours, double decDeg);

int main(int argc, char *argv[])
{
   double sunEclipLongDeg = 240.7881593; // 23 nov 2005, 00 UT (JPL Horiz)
   double sunEclipLatDeg = -0.0020332;

   // sun RA =  238.56115 deg (15.90401 hours)
   // dec = -20.30110 deg on that date

   // sweep out the full earth's orbit (+- 90 degrees from the sun)
   //double maxSweepAngleFromSunDeg = 90;
   const double degreesPerCircle = 360;

   // double startEclipLongDeg = sunEclipLongDeg - maxSweepAngleFromSunDeg;
   double startEclipLongDeg = 0;
   if (startEclipLongDeg < 0)
   {
      startEclipLongDeg += degreesPerCircle;
   }

   //double endEclipLongDeg = sunEclipLongDeg + maxSweepAngleFromSunDeg;
   double endEclipLongDeg = 359.9;
   if (endEclipLongDeg > degreesPerCircle)
   {
      endEclipLongDeg -= degreesPerCircle;
   }

   double skyFreqGhz = 2.292;  // pioneer 6-8 downlink
   double ataBeamsizeDegAtOneGhz = 3.5;
   double beamsizeDeg = ataBeamsizeDegAtOneGhz / skyFreqGhz;

   cout.precision(6);           // show N places after the decimal
   cout.setf(std::ios::fixed);  // show all decimal places up to precision

   int firstTargetId = 20000;  // skip over spacecraft & old starlist.
   double beamOverlapPercent = 0.15;

   cout << "-- $"   // split over 2 lines so does not get expanded here
	<< "Id$ --" << endl
	<< "-- ATA Ecliptic survey pointing data" << endl
	<< "-- compatible with the seeker TargetCat database table" << endl
	<< "-- Parameters: " << endl
	<< "-- skyFreqGhz: " <<  skyFreqGhz << endl
	<< "-- beamsize deg: " << beamsizeDeg << endl
	<< "-- beamOverlapPercent: " << beamOverlapPercent << endl
	<< "-- sunEclipLongDeg: " << sunEclipLongDeg << endl
	<< "-- sunEclipLatDeg: " << sunEclipLatDeg << endl
	<< "-- start eclip long deg: " << startEclipLongDeg << endl
	<< "-- end eclip long deg: " << endEclipLongDeg << endl
	<< endl;

   int targetId = firstTargetId;
   double longDegIncrement = beamsizeDeg * (1.0 - beamOverlapPercent);
   double eclipLatRads = SseAstro::degreesToRadians(sunEclipLatDeg);

   // handle longitude wraparound.  If needed, add a full circle to the endpoint
   // so that the for loop runs correctly, then extract it in the loop
   // when computing the coordinates.
   if (endEclipLongDeg < startEclipLongDeg)
   {
      endEclipLongDeg += degreesPerCircle;
   }

   string skySurveyTargetList("SS");  
   for (double longDeg = startEclipLongDeg; longDeg < endEclipLongDeg; 
	longDeg+=longDegIncrement)
   {	
      // convert from ecliptic to to RA/Dec

      double adjustedLongDeg = fmod(longDeg, degreesPerCircle);
      cout << "-- eclip long deg = " << adjustedLongDeg << endl;
      double raRads = 0;
      double decRads = 0;

      double eclipLongRads = SseAstro::degreesToRadians(adjustedLongDeg);
      SseAstro::eclipticToEquatorial(eclipLongRads, eclipLatRads,
				     SseAstro::EclipObliqJ2000Rads,
				     raRads, decRads);

      double raHours = SseAstro::radiansToHours(raRads);
      double decDeg = SseAstro::radiansToDegrees(decRads);

      //cout << targetId << "), " << raHours << ", " << decDeg << ", " << endl;
      printDbTableLine(skySurveyTargetList, targetId, raHours, decDeg);

      // label the points in gnuplot format
      if ((int) longDeg % 20 == 0) 
      {
         cout << "-- set label '" << (int) longDeg << "' at " << raHours 
           << "," << decDeg << " right" <<  endl;
      }

      targetId++;
      
   }

   // Add in a ring of higher dec targets to give the scheduler
   // something to do when no real eclip targets are available.
   // Eclip survey dec range is > -25 && < 25 deg.

   const double decDeg(40.0);  
   const double degPerHour(15.0);
   const double beamsizeHours(beamsizeDeg / degPerHour);
   const double raHoursIncr = beamsizeHours / 
      cos(SseAstro::degreesToRadians(decDeg));
   string noTargetList("");   // low target merit, no list
   for (double raHours=0.0; raHours <= 23.9; raHours += raHoursIncr)
   {
      printDbTableLine(noTargetList, targetId++, raHours, decDeg);
   }
   
}

void printDbTableLine(const string &whichList, 
                      int targetId, double raHours, double decDeg)
{
   string targetName("");
   int mura(0);
   int mudec(0);
   int radialvel(0);
   string spectralType("");

   // set parallax to 1 arcsec == 1 parsec (~3.26 LY)
   // Make nonzero so LY calculation in SSE does not go to infinity.
   int parallax(1);  
   
   int parxerr(0);
   int vismag(0);
   int bv(0);
   int v(0);
   string autoschedule("Yes");  // enable auto scheduling
   string ephemFilename("");
   string remarks("");


   cout << "INSERT INTO TargetCat (targetId, whichlist, targetName, "
	<< " ra2000Hours, dec2000Deg, mura, mudec, radialvel, "
	<< "spectralType, parallax, "
	<< "parxerr, vismag, bv, v, autoschedule, ephemFilename, "
	<< "remarks) ";

   cout << "VALUES (" 
	<< targetId << ", "
	<< "'" << whichList << "', "
	<< "'" << targetName << "', "
	<< raHours << ", "
	<< decDeg << ", "
	<< mura << ", "
	<< mudec << ", "
	<< radialvel << ", "
	<< "'" << spectralType << "', "
	<< parallax << ", "
	<< parxerr << ", "
	<< vismag << ", "
	<< bv << ", "
	<< v << ", "
	<< "'" << autoschedule << "', "
	<< "'" << ephemFilename << "', "
	<< "'" << remarks << "'"
	<< ");" << endl;
}
