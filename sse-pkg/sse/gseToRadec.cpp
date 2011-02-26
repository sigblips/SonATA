/*******************************************************************************

 File:    gseToRadec.cpp
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


/* convert GSE to J2000 RA/Dec

GSE - Geocentric Solar Ecliptic, in km
    X = Earth-Sun Line 
    Z = Ecliptic North Pole
    Y = perpendicular to x & z

Input: 
GSE coords (x,y,z) in km.
Sun ecliptic coords (long, lat) in deg, j2000

Output:
RA/Dec in degrees, j2000

Test data:
gse xyz (km) :  1.43e6 -4.1e4 8.7e4
sun eclip long lat (deg): 139.274 -0.001
ra dec (deg): 141.197237 18.860488
*/

#include "SseAstro.h"
#include "SseUtil.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace std;

double getDoubleValue()
{
   double value;
   cin >> value;
   if (!cin)
   {
      cerr << "error: invalid input" << endl;
      exit(1);
   }
   return value;
}

int main(int argc, char * argv[])
{
   cout << "Convert GSE to J2000 RA/Dec" << endl;
   cout << "Enter GSE coords in km (x y z): " << endl;

   double xKm = getDoubleValue();
   double yKm = getDoubleValue();
   double zKm = getDoubleValue();

   cout << "Enter sun eclip coords, J2000 deg (long lat): " << endl;
 
   // sun eclip coords, j2000
   double sunEclipLongDeg = getDoubleValue();
   double sunEclipLatDeg = getDoubleValue();

   double sunEclipLongRads = SseAstro::degreesToRadians(sunEclipLongDeg);
   double sunEclipLatRads = SseAstro::degreesToRadians(sunEclipLatDeg);

   // Find the y (long) and z (lat) offset angles of the target 
   double yAngleRads = atan2(yKm, xKm);
   double zAngleRads = atan2(zKm, xKm);

   // add the offset angles to the sun ecliptic coords
   double targetEclipLongRads = sunEclipLongRads + yAngleRads;
   double targetEclipLatRads = sunEclipLatRads + zAngleRads;

   // convert to j2000 RA/DEC
   double raRads;
   double decRads;
   SseAstro::eclipticToEquatorial(targetEclipLongRads, targetEclipLatRads,
                                  SseAstro::EclipObliqJ2000Rads,
                                  raRads, decRads);

   cout.precision(6);           // show N places after the decimal
   cout.setf(std::ios::fixed);  // show all decimal places up to precision

   cout << "RA & Dec (degrees J2000): "
	<< SseAstro::radiansToDegrees(raRads)
	<< " " << SseAstro::radiansToDegrees(decRads)
	<< endl;

}