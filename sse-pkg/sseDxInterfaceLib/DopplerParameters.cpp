/*******************************************************************************

 File:    DopplerParameters.cpp
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

ostream& operator << (ostream &strm, const DopplerParameters &dp)
{
    stringstream localstrm;

    localstrm.precision(14);  // N digits after the decimal
    localstrm << "Doppler Parameters:" << endl
	      << "freqRatio:        " << dp.freqRatio 
	      << endl 
	      << "driftOffsetFactor:     " << dp.driftOffsetFactor
	      << endl  
	      << "curvatureMainFactor:   " << dp.curvatureMainFactor
	      << endl  
	      << "curvatureRemoteFactor: " << dp.curvatureRemoteFactor
	      << endl;  

    strm <<localstrm.str() << endl;

    return strm;
}

DopplerParameters::DopplerParameters()
    :
    freqRatio(0),
    driftOffsetFactor(0),
    curvatureMainFactor(0),
    curvatureRemoteFactor(0)
{
}

void DopplerParameters::marshall()
{
    HTOND(freqRatio);
    HTOND(driftOffsetFactor);
    HTOND(curvatureMainFactor);
    HTOND(curvatureRemoteFactor);
}

void DopplerParameters::demarshall()
{
    marshall();
}
