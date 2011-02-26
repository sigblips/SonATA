/*******************************************************************************

 File:    FollowUpSignal.cpp
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

using std::stringstream;

static const int PrintPrecision(9);  // MilliHz

ostream& operator << (ostream &strm, const FollowUpSignal &fs)
{
    stringstream localstrm;
    localstrm.precision(PrintPrecision);           // show N places after the decimal
    localstrm.setf(std::ios::fixed);  // show all decimal places up to precision

    localstrm << "Follow Up Signal:" << endl
	      << "====================" << endl
	      << "rfFreq: " << fs.rfFreq << " MHz" << endl
	      << "drift: " << fs.drift << " Hz/s" << endl
	      << "res: " << SseDxMsg::resolutionToString(fs.res) << endl 
	      << fs.origSignalId;

    strm << localstrm.str();

    return strm;
}

FollowUpSignal::FollowUpSignal()
    :
    rfFreq(-1),
    drift(-999),
    res(RES_UNINIT)
{
    // use default constructor for:
    // origSignalId 
}

void FollowUpSignal::marshall()
{
    HTOND(rfFreq);
    HTONF(drift);
    SseDxMsg::marshall(res);  // resolution
    origSignalId.marshall();
}

void FollowUpSignal::demarshall()
{
    marshall();
}

//------------------------------

FollowUpCwSignal::FollowUpCwSignal()
{
    // use default constructors for all fields
}

void FollowUpCwSignal::marshall()
{
    sig.marshall();
}

void FollowUpCwSignal::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const FollowUpCwSignal &fcw)
{
    strm << "(CW) " << fcw.sig << endl;

    return strm;
}

//------------------------------

FollowUpPulseSignal::FollowUpPulseSignal()
{
    // use default constructors for all fields
}

void FollowUpPulseSignal::marshall()
{
    sig.marshall();
}

void FollowUpPulseSignal::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const FollowUpPulseSignal &fp)
{
    strm << "(Pulse) " << fp.sig << endl;

    return strm;
}
