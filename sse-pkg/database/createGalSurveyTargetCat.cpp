/*******************************************************************************

 File:    createGalSurveyTargetCat.cpp
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


// Create a target catalog for ATA/Prelude galactic survey
// Output is suitable for loading into the seeker TargetCat database table.

#include "SseAstro.h"
#include <iostream>
#include <cmath>
#include <string>

using namespace std;

void printDbTableLine(int targetId, double raHours, double decDeg,
                      double glongDeg, double glatDeg);
double degreesToRadians(double degrees);
double radiansToDegrees(double radians);

int main(int argc, char *argv[])
{
   // create a grid covering the selected portion of the
   // galactic plane
   // 20 square degrees (2x10 degree strip around gal center)
   // Gal long deg: -5:5
   // Gal lat deg: -1:1
   // Convert gal coords to j2000 ra/dec for use with seeker

#ifdef SYNTH_BEAMS

    double startGlongDeg = -5.0;
    double startGlatDeg = -1.0;

    double endGlongDeg = 5.0;
    double endGlatDeg = 1.0;

    // Nominal freq range to be covered is the 
    // waterhole from 1410 to 1730 MHz.
    // For simplicity, compute the beam size at the
    // center of the band, or 1575 MHz.
    // Also assume for now that the beam is square.
    // From ATA NSF proposal of June 2005,
    // ATA42 beamsize is ~ 245 x 118 arcsec at 1.42 GHz (21cm),
    // or 345 x 168 arcsec at 1.0 GHz

    double Ata42BeamsizeAtOneGhzArcSec = 168;  // use smaller dimension
    double ArcSecPerDeg = 3600;
    double AtaBeamsizeAtOneGhzDeg = 
       Ata42BeamsizeAtOneGhzArcSec / ArcSecPerDeg;    

    double skyFreqGhz = 1.575;

#else
    // use ATA single dish as beam.
    // create grid across entire gal plane

    double startGlongDeg = 0;
    double startGlatDeg = -5.0;

    double endGlongDeg = 359;
    double endGlatDeg = 5.0;

    // Nominal freq range to be covered is the 
    // waterhole from 1410 to 1730 MHz.
    // Compute the beam size at the
    // high end of the band (smallest beamsize) for max coverage.

    double AtaBeamsizeAtOneGhzDeg = 3.5; 
    double skyFreqGhz = 1.730;

#endif

    double beamsizeDeg = AtaBeamsizeAtOneGhzDeg / skyFreqGhz;

    cout.precision(6);           // show N places after the decimal
    cout.setf(std::ios::fixed);  // show all decimal places up to precision

    int firstTargetId = 20000;  // skip over other target lists.

    cout << "-- $"   // split over 2 lines so does not get expanded here
	 << "Id$ --" << endl
	 << "-- ATA galactic survey pointing data" << endl
	 << "-- for loading into the seeker TargetCat database table" << endl
	 << " " << endl
	 << "-- skyFreqGhz: " <<  skyFreqGhz << endl
	 << "-- beamsize deg: " << beamsizeDeg << endl
	 << "-- Gal long deg start: " << startGlongDeg 
	 << " end: " << endGlongDeg << endl
	 << "-- Gal lat deg start: " << startGlatDeg
	 << " end: " << endGlatDeg << endl
	 << endl;

    int targetId = firstTargetId;

    const double DegPerHour = 15.0;
    double beamOverlapPercent = 0.10;

    // Assuming horizon is 15 deg el or higher
    double ataMinDecDeg = -35.0;

    double glongDegIncrement = beamsizeDeg * (1.0 - beamOverlapPercent);
    double glatDegIncrement = beamsizeDeg * (1.0 - beamOverlapPercent);
    int glongCount = 0;
    for (double glongDeg = startGlongDeg; glongDeg < endGlongDeg; 
	 glongDeg+=glongDegIncrement)
    {	
      // alternate row start position by 1/2 the increment
      // to more evenly distribute pointings from row to row
        
      glongCount++;
      double startRowGlatDeg = startGlatDeg;
      if (glongCount % 2 == 0)
      {
         startRowGlatDeg += glatDegIncrement / 2;
      }

      for (double glatDeg = startRowGlatDeg; glatDeg < endGlatDeg;
           glatDeg += glatDegIncrement)
      {
         //cout << targetId << "), " << glatDeg << ", " << glongDeg << ", " << endl;

         double glongRads = degreesToRadians(glongDeg);
         double glatRads = degreesToRadians(glatDeg);
	    
         double raRads;
         double decRads;
         SseAstro::galToEqu2000(glongRads, glatRads, &raRads, &decRads);
	    
         double raDeg = radiansToDegrees(raRads);
         double decDeg = radiansToDegrees(decRads);
         double raHour = raDeg / DegPerHour;

         if (decDeg > ataMinDecDeg)
         {
            printDbTableLine(targetId, raHour, decDeg, glongDeg, glatDeg);
            targetId++;
         }	    
      }
    }
    
}


double degreesToRadians(double degrees)
{
  const double radiansPerDegree = 2.0 * M_PI/360.0;
  return (degrees * radiansPerDegree);
}

double radiansToDegrees(double radians)
{
    const double degreesPerRadian = 360 / (2.0 * M_PI);
    return (radians * degreesPerRadian);
}


void printDbTableLine(int targetId, double raHours, double decDeg,
                      double glongDeg, double glatDeg)
{
    string whichlist("GS");  // GS for Galactic Survey
    string targetName("");
    int mura(0);
    int mudec(0);
    int radialvel(0);
    string spectralType("");

    // set parallax to 0.1 arcsec.   1 parsec = ~3.26 LY
    // Make nonzero so LY calculation in SSE does not go to infinity.
    double parallax(0.1);  
   
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
	 << "parxerr, vismag, bv, v, "
         << "galLongDeg, galLatDeg, "
         << "autoschedule, ephemFilename, "
	 << "remarks) ";

    cout << "VALUES (" 
	 << targetId << ", "
	 << "'" << whichlist << "', "
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
         << glongDeg << ", "
         << glatDeg << ", "
	 << "'" << autoschedule << "', "
	 << "'" << ephemFilename << "', "
	 << "'" << remarks << "'"
	 << ");" << endl;
}
