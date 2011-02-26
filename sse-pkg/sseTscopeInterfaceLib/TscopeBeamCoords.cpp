/*******************************************************************************

 File:    TscopeBeamCoords.cpp
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


#include "sseTscopeInterfaceLib.h"
#include <sstream>
using namespace std;

static const int PrintPrecision(6);

TscopeBeamCoords::TscopeBeamCoords() 
    : 
   beam(TSCOPE_INVALID_BEAM),
   alignPad(0)
{
}

void TscopeBeamCoords::marshall()
{
   SseTscopeMsg::marshall(beam);
   pointing.marshall();
}

void TscopeBeamCoords::demarshall()
{
    marshall();
}

ostream& operator << (ostream& strm, const TscopeBeamCoords& coords)
{
    stringstream localstrm;

    localstrm.precision(PrintPrecision);  
    localstrm.setf(std::ios::fixed);  // show all decimal places up to precision

    localstrm << "tscope beam coords:" << endl;

    localstrm << SseTscopeMsg::beamToName(coords.beam) << endl;
    localstrm << coords.pointing << endl;

    strm << localstrm.str();

    return strm;
}