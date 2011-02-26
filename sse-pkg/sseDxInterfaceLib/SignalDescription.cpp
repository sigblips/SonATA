/*******************************************************************************

 File:    SignalDescription.cpp
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

static const int PrintPrecision(9); // MilliHz

static string boolToYesNoString(bool value)
{
    if (value) 
        return "yes";
    else
        return "no";
}


ostream& operator << (ostream &strm, const SignalDescription &sig)
{
    stringstream localstrm;

    localstrm.precision(PrintPrecision);  // show N places after the decimal
    localstrm.setf(std::ios::fixed);  // show all decimal places up to precision

    // Signal Description
    localstrm
	<< "Signal Description: " << endl
	<< "====================" << endl
	<<  sig.path
	<< "pol: "   << SseMsg::polarizationToString(sig.pol) << endl
	<< "class: " << SseDxMsg::signalClassToString(sig.sigClass) << endl
	<< "reason: " << SseDxMsg::signalClassReasonToString(sig.reason)
	<< endl
	<< "subchan: " << sig.subchannelNumber << endl 
	<< "containsBadBands: " << boolToYesNoString(sig.containsBadBands)
	<< endl
	<< sig.signalId
	<< "Original " 
	<< sig.origSignalId;

    strm << localstrm.str();

    return strm;
}

SignalDescription::SignalDescription()
    :
    pol(POL_UNINIT),
    sigClass(CLASS_UNINIT),
    reason(CLASS_REASON_UNINIT),
    subchannelNumber(-1),
    containsBadBands(SSE_FALSE),
    alignPad(0)
{
    // use default constructor for:
    // path
    // signalId
    // origSignalId
}

void SignalDescription::marshall()
{
    path.marshall();
    SseMsg::marshall(pol);  // Polarization
    SseDxMsg::marshall(sigClass);  // SignalClass  
    SseDxMsg::marshall(reason);    // ClassReason
    HTONL(subchannelNumber);
    SseMsg::marshall(containsBadBands);  // bool_t
    signalId.marshall();
    origSignalId.marshall();

}

void SignalDescription::demarshall()
{
    marshall();
}