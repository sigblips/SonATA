/*******************************************************************************

 File:    PulseBadBand.cpp
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

static string boolToYesNoString(bool value)
{
    if (value) 
        return "yes";
    else
        return "no";
}


// print message bodies
ostream& operator << (ostream &strm, const PulseBadBand & pulseBadBand)
{
    strm 
    << "PulseBadBand:" << endl
    << "--------------" << endl
    << "band: " << pulseBadBand.band
    << "res: " <<  SseDxMsg::resolutionToString(pulseBadBand.res) << endl
    << "pol: " << SseMsg::polarizationToString(pulseBadBand.pol) << endl
    << "pulses: " << pulseBadBand.pulses << endl
    << "maxPulseCount: " << pulseBadBand.maxPulseCount << endl
    << "triplets: " << pulseBadBand.triplets << endl
    << "maxTripletCount: " << pulseBadBand.maxTripletCount << endl
    << "tooManyTriplets: " << boolToYesNoString(pulseBadBand.tooManyTriplets)
    << endl;
	
    return strm;
}



PulseBadBand::PulseBadBand()
    :
    res(RES_UNINIT),
    pol(POL_UNINIT),
    pulses(0),
    maxPulseCount(0),
    triplets(0),
    maxTripletCount(0),
    tooManyTriplets(SSE_FALSE),
    alignPad(0)
{
}

void PulseBadBand::marshall()
{
    band.marshall();    // FrequencyBand
    SseDxMsg::marshall(res); // Resolution
    SseMsg::marshall(pol); // Polarization
    HTONL(pulses);
    HTONL(maxPulseCount);
    HTONL(triplets);
    HTONL(maxTripletCount);
    SseMsg::marshall(tooManyTriplets);  // bool_t
}

void PulseBadBand::demarshall()
{
    marshall();
}