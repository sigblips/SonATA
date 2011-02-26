/*******************************************************************************

 File:    angularSep.cpp
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
#include "SseUtil.h"
#include "Angle.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
   int nArgsExpected = 5;
   if (argc != nArgsExpected)
   {
       cerr << "Determine angular separation between two points" << endl;
       cerr << "usage: " << argv[0] 
            << " <ra1Hours> <dec1Deg> <ra2Hours> <dec2Deg>" << endl;
       exit(1);
    }

    try {
       
       double ra1Hours = SseUtil::strToDouble(argv[1]);
       double dec1Deg = SseUtil::strToDouble(argv[2]);
       double ra2Hours = SseUtil::strToDouble(argv[3]);
       double dec2Deg = SseUtil::strToDouble(argv[4]);

       double ra1Rads = SseAstro::hoursToRadians(ra1Hours);
       double dec1Rads = SseAstro::degreesToRadians(dec1Deg);

       double ra2Rads = SseAstro::hoursToRadians(ra2Hours);
       double dec2Rads = SseAstro::degreesToRadians(dec2Deg);

       double diffRads = SseAstro::angSepRads(ra1Rads, dec1Rads, ra2Rads, dec2Rads);
       double diffDeg = SseAstro::radiansToDegrees(diffRads);
       double diffArcSec = diffDeg * SseAstro::ArcSecPerDeg;

       cout.precision(9);
       cout  << "Deg: " << diffDeg 
             << ",  ArcSec: " << diffArcSec
             << ",  Rad: " << diffRads << endl;

    }
    catch (SseException &except)
    {
	cerr << except << endl;
	exit(1);
    }

}