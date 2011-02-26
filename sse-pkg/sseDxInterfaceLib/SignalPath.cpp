/*******************************************************************************

 File:    SignalPath.cpp
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


#include "sseDxInterfaceLib.h"
#include <sstream>


static const int PrintPrecision(9);  //MilliHz

ostream& operator << (ostream &strm, const SignalPath &path)
{
    stringstream localstrm;

    localstrm.precision(PrintPrecision); // show N places after the decimal
    localstrm.setf(std::ios::fixed); // show all decimal places up to precision

    // Signal Path
    localstrm
	<< "Signal Path: " << endl
	<< "============ " << endl
	<< "rfFreq: " << path.rfFreq << " MHz" << endl
	<< "drift:  " << path.drift << " Hz/s" << endl
	<< "width:  " << path.width << " Hz" <<  endl
	<< "power:  " << path.power << endl;
    
    strm << localstrm.str();

    return strm;
}

SignalPath::SignalPath()
    :
    rfFreq(-1),
    drift(-99999),
    width(-1),
    power(-1),
    alignPad(0)
{

}

void SignalPath::marshall()
{
    HTOND(rfFreq);
    HTONF(drift);
    HTONF(width);
    HTONF(power);
}

void SignalPath::demarshall()
{
    marshall();
}