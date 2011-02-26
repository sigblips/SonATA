/*******************************************************************************

 File:    createAllskyStarcat.cpp
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


// Create a target catalog for all-sky pointings.
// Output is suitable for loading into the seeker TargetCat database table.

#include <iostream>
#include <cmath>
#include <string>

using namespace std;

void printDbTableLine(int targetId, double raHours, double decDeg);
double degreesToRadians(double degrees);

int main(int argc, char *argv[])
{
    // create a grid covering the entire sky visible at ATA

    double startRaHours = 0.0;
    double startDecDeg = -34.0;   // ATA lowest visible dec limit

    double endRaHours = 23.9;
    double endDecDeg = 89.0;

    const double DegPerHour = 15.0;
    
    double skyFreqGhz = 1.42;  // hydrogen line
    double ataBeamsizeDegAtOneGhz = 3.5;
    double beamsizeDeg = ataBeamsizeDegAtOneGhz / skyFreqGhz;
    double beamsizeHours = beamsizeDeg / DegPerHour;

    cout.precision(6);           // show N places after the decimal
    cout.setf(std::ios::fixed);  // show all decimal places up to precision

    int firstTargetId = 10000;  // skip over spacecraft & old starlist.

    cout << "-- $"   // split over 2 lines so does not get expanded here
	 << "Id$ --" << endl
	 << "-- ATA all sky survey pointing data" << endl
	 << "-- for loading into the seeker TargetCat database table" << endl
	 << " " << endl
	 << "-- skyFreqGhz: " <<  skyFreqGhz << endl
	 << "-- beamsize deg: " << beamsizeDeg << endl
	 << "-- RA hours start: " << startRaHours 
	 << " end: " << endRaHours << endl
	 << "-- Dec Deg start: " << startDecDeg 
	 << " end: " << endDecDeg << endl
	 << endl;

    int targetId = firstTargetId;
    double decDegIncrement = beamsizeDeg;
    int decCount = 0;
    for (double decDeg = startDecDeg; decDeg < endDecDeg; 
	 decDeg+=decDegIncrement)
    {	
	// account for smaller circles in RA as Dec increases
	double raHoursIncrement = beamsizeHours / 
	    cos(degreesToRadians(decDeg));
    
	// alternate RA start position by 1/2 the RA increment
	// to more evenly distribute pointings from Dec row to row
	
	decCount++;
	double startRowRaHours = startRaHours;
	if (decCount % 2 == 0)
	{
	    startRowRaHours += raHoursIncrement / 2;
	}

	for (double raHours = startRowRaHours; raHours < endRaHours;
	     raHours+=raHoursIncrement)
	{
	    //cout << targetId << "), " << raHours << ", " << decDeg << ", " << endl;
	    printDbTableLine(targetId, raHours, decDeg);
	    targetId++;
	    
	}
    }
    
}


double degreesToRadians(double degrees)
{
  const double radiansPerDegree = 2.0 * M_PI/360.0;
  return (degrees * radiansPerDegree);

}

void printDbTableLine(int targetId, double raHours, double decDeg)
{
    string whichlist("SS");  // SS for Sky Survey
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
