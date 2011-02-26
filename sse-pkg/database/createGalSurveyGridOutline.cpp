/*******************************************************************************

 File:    createGalSurveyGridOutline.cpp
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


/*
  Create a set of ra/dec glat/glong points that show an
outline of the primary pointing grid created for
the galactic survey.

 */

// Create a target catalog for ATA/Prelude galactic survey
// Output is suitable for loading into the seeker TargetCat database table.
#include "SseAstro.h"
#include <iostream>
#include <cmath>
#include <string>

using namespace std;

void printDbTableLine(int targetId, double raHours, double decDeg);
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

    double startGlongDeg = -5.0;
    double endGlongDeg = 5.0;

    double startGlatDeg = -1.0;
    double endGlatDeg = 1.0;

    int targetId = 500000;

    double glongDegIncr = 0.01;
    double glatDegIncr = 1.0;

    const double DegPerHour = 15.0;

    /* first 3 lines along entire gal lat */
    for (double glongDeg=startGlongDeg;
	 glongDeg<=endGlongDeg; glongDeg+=glongDegIncr)
    {
	for (double glatDeg = startGlatDeg; glatDeg <= endGlatDeg;
	     glatDeg += glatDegIncr)
	{
	    double glongRads = degreesToRadians(glongDeg);
	    double glatRads = degreesToRadians(glatDeg);
	    
	    double raRads;
	    double decRads;
            SseAstro::galToEqu2000(glongRads, glatRads, &raRads, &decRads);
	    
	    double raDeg = radiansToDegrees(raRads);
	    double decDeg = radiansToDegrees(decRads);
	    double raHour = raDeg / DegPerHour;

	    printDbTableLine(targetId, raHour, decDeg);
	    targetId++;

	}
    }


    /* now 11 shorter lines at 1 deg glong steps */

    glongDegIncr = 1.0;
    glatDegIncr = 0.01;

    for (double glatDeg = startGlatDeg; glatDeg <= endGlatDeg;
	 glatDeg += glatDegIncr)
    {
       for (double glongDeg=startGlongDeg;
	    glongDeg<=endGlongDeg; glongDeg+=glongDegIncr)
       {
	  double glongRads = degreesToRadians(glongDeg);
	  double glatRads = degreesToRadians(glatDeg);
	  
	  double raRads;
	  double decRads;
          SseAstro::galToEqu2000(glongRads, glatRads, &raRads, &decRads);
	  
	  double raDeg = radiansToDegrees(raRads);
	    double decDeg = radiansToDegrees(decRads);
	    double raHour = raDeg / DegPerHour;
	    
	    printDbTableLine(targetId, raHour, decDeg);
	    targetId++;
	    
       }
    }


#if 0
    // TBD: overlap the pointings?  For now, no overlap.


    double glongDegIncrement = beamsizeDeg;
    double glatDegIncrement = beamsizeDeg;
    for (double glongDeg = startGlongDeg; glongDeg < endGlongDeg; 
	 glongDeg+=glongDegIncrement)
    {	
	for (double glatDeg = startGlatDeg; glatDeg < endGlatDeg;
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

	    printDbTableLine(targetId, raHour, decDeg);
	    targetId++;
	    
	}
    }

#endif
    
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


void printDbTableLine(int targetId, double raHours, double decDeg)
{
    string whichlist("GS");  // GS for Galactic Survey
    string targetName("");
    int mura(0);
    int mudec(0);
    int radialvel(0);
    string spectralType("");

    // set parallax to 0.1 arcsec == 1 parsec (~3.26 LY)
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
	 << "parxerr, vismag, bv, v, autoschedule, ephemFilename, "
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
	 << "'" << autoschedule << "', "
	 << "'" << ephemFilename << "', "
	 << "'" << remarks << "'"
	 << ");" << endl;
}
