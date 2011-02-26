/*******************************************************************************

 File:    dopplerShift.cpp
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


#include <iostream>
#include <cstdlib>
#include "SseUtil.h"

using namespace std;

/*
Given nominal transmitter freq in MHz
and deldot (range-rate to spacecraft in Km/S)
compute expected receive freq and doppler shift.

'deldot' info from JPL Horizons website.

Example use:

matrix% dopplerShift 8437.895  42.7734281
Transmit Freq MHz.....: 8437.895000
Delta-dot Km/Sec......: 42.773428
Expected Recv Freq MHz: 8436.691108
Doppler Shift KHz.....: -1203.891844

*/

const double CeeKmSec(299792.458);
const double KhzPerMhz(1000);

int main(int argc, char *argv[])
{

   if (argc != 3)
   {   
      cout
	 << "Compute expected receive freq & doppler shift for a given "
	 << "transmitted freq." << endl
	 << "Usage: " << argv[0] 
	 << " <transmit freq MHz> <range-rate (aka 'deldot') Km/Sec>\n"
	 << endl
	 << "where 'deldot' is a projection along the line-of-sight\n"
	 << "from the coordinate center and indicates direction of motion.\n"
	 << "A positive deldot means the target center is moving away from\n"
	 << "the observer (coordinate center). A negative deldot means the\n"
	 << "target center is moving toward the observer. Units are Km/sec.\n"
	 << "(See JPL Horizons website)." 
	 << endl;

      exit(1);
   }

   cout.precision(6);   // show N places after the decimal
   cout.setf(std::ios::fixed);  // show all decimal places up to precision

   try {

      double transmitFreqMhz(SseUtil::strToDouble(argv[1]));
      double deltaDotKmSec(SseUtil::strToDouble(argv[2]));

      // use negative of deltadot to make freq shift go in the
      // correct direction
      double dopplerShiftMhz((-deltaDotKmSec / CeeKmSec) * transmitFreqMhz);
      
      double expectedRecvFreqMhz(transmitFreqMhz + dopplerShiftMhz);
      
      cout << "Transmit freq MHz.....: " << transmitFreqMhz << endl
	   << "Delta-dot Km/Sec......: " << deltaDotKmSec << endl
	   << "Expected recv freq MHz: " << expectedRecvFreqMhz << endl
	   << "Doppler shift KHz.....: " << dopplerShiftMhz * KhzPerMhz
	   << endl;

   }
   catch (SseException & except)
   {
      cerr << except << endl;
      exit(1);
   }
}