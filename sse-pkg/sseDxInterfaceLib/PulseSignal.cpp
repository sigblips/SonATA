/*******************************************************************************

 File:    PulseSignal.cpp
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


// -- Pulse --------------------------------------------

ostream& operator << (ostream &strm, const Pulse &pulse)
{
    strm << "rfFreq: " << pulse.rfFreq;
    strm << ", power: " << pulse.power;
    strm << ", spec#: " << pulse.spectrumNumber;
    strm << ", bin#: " << pulse.binNumber;
    strm << ", pol: " << SseMsg::polarizationToString(pulse.pol);
    strm << endl;

    return strm;
}

Pulse::Pulse()
    :
    rfFreq(-1),
    power(-1),
    alignPad(0),
    spectrumNumber(-1),
    binNumber(-1),
    pol(POL_UNINIT),
    alignPad2(0)
{
}

void Pulse::marshall()
{
    HTOND(rfFreq);
    HTONF(power);
    HTONL(spectrumNumber);
    HTONL(binNumber);
    SseMsg::marshall(pol);
}

void Pulse::demarshall()
{
    marshall();
}

// --- PulseTrainDescription -------------------------------------------

ostream& operator << (ostream &strm, const PulseTrainDescription &ptd)
{
    // PulseTrainDescription
    strm << "period: " << ptd.pulsePeriod;
    strm << ", nPulses: " << ptd.numberOfPulses;
    strm << ", res: " << SseDxMsg::resolutionToString(ptd.res); 
    strm << endl;

    return strm;
}

PulseTrainDescription::PulseTrainDescription()
    :
    pulsePeriod(-1),
    numberOfPulses(0),
    res(RES_UNINIT),
    alignPad(0)
{

}

void PulseTrainDescription::marshall()
{
    HTONF(pulsePeriod);
    HTONL(numberOfPulses);  
    SseDxMsg::marshall(res); // Resolution
}

void PulseTrainDescription::demarshall()
{
    marshall();
}


// --- PulseSignalHeader -------------------------------------------

ostream& operator << (ostream &strm, const PulseSignalHeader &ps)
{

    strm << "*Pulse*" << endl;
    strm << ps.sig;    // signal description
    strm << ps.cfm;    // confirmation stats
    strm << ps.train;  // pulse train description

    return strm;
}

PulseSignalHeader::PulseSignalHeader()
{
    // use default constructors for all fields
}

void PulseSignalHeader::marshall()
{
    sig.marshall();    // SignalDescription
    cfm.marshall();    // ConfirmationStats 
    train.marshall();  // PulseTrainDescription
}

void PulseSignalHeader::demarshall()
{
    sig.demarshall();
    cfm.demarshall();
    train.demarshall();
}

// ------ combined Pulse signal ---------------------------------

void SseDxMsg::printPulseSignal(ostream &strm, const PulseSignalHeader &hdr,
			     Pulse pulses[])
{
    strm << hdr;

    // print the pulses.
    for (int i=0; i < hdr.train.numberOfPulses; ++i)
    {
	strm << pulses[i];
    }
}





// Extract & demarshall a PulseSignal from a message buffer.
// Returns pointers to the hdr & pulse array.
// Note that the msgBody buffer is modified.
//
void SseDxMsg::extractPulseSignalFromMsg(void *msgBody,
					  PulseSignalHeader **returnHdr,
					  Pulse **returnPulseArray)
{
    PulseSignalHeader *hdr = reinterpret_cast<PulseSignalHeader *>(msgBody);
    hdr->demarshall();

    // Get data array
    int nPulses = hdr->train.numberOfPulses;
    int arrayOffset = sizeof(PulseSignalHeader);
    Pulse *pulseArray = 
        reinterpret_cast<Pulse*>(static_cast<char *>(msgBody) + arrayOffset);
    for (int i=0; i<nPulses; ++i)
    {
        pulseArray[i].demarshall();
    }

    *returnHdr = hdr;
    *returnPulseArray = pulseArray;
}

// The PulseSignalHeader and the variable length Pulse array
// are copied into a contiguous allocated buffer & marshalled.
// Buffer is returned as the function value, and the buffer
// size is returned in the bufferLength parameter.
// Caller is responsible for deleting the buffer.

char * SseDxMsg::encodePulseSignalIntoMsg(const PulseSignalHeader &hdr,
                                           const Pulse pulses[],
                                           int *bufferLength)
{
    int nPulses = hdr.train.numberOfPulses;
    Assert (nPulses > 0);

    // create a temp buffer that can hold the header & the variable array
    int dataLength = sizeof(PulseSignalHeader) +
        nPulses * sizeof(Pulse);

    char *outbuff = new char [dataLength];

    // copy the header & marshall it
    PulseSignalHeader *outHdr = reinterpret_cast<PulseSignalHeader *>(outbuff)
;
    *outHdr = hdr;
    outHdr->marshall();

    // copy & marshall the data array
    int arrayOffset = sizeof(PulseSignalHeader);
    Pulse *outPulseArray = 
        reinterpret_cast<Pulse*>(outbuff + arrayOffset);
    for (int i=0; i<nPulses; ++i)
    {
        outPulseArray[i] = pulses[i];
        outPulseArray[i].marshall();
    }

    *bufferLength = dataLength;
    return outbuff;

}