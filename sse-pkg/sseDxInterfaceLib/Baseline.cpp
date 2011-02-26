/*******************************************************************************

 File:    Baseline.cpp
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

ostream& operator << (ostream &strm, const BaselineHeader &hdr)
{
    strm << "activity id: " << hdr.activityId;
    strm << ", halfFrame #: " << hdr.halfFrameNumber;
    strm << ", pol: " << SseMsg::polarizationToString(hdr.pol);
    strm << ", rf center freq: " << hdr.rfCenterFreq << " MHz";
    strm << ", bandwidth: " << hdr.bandwidth << " MHz";
    strm << ", #subchannels: " << hdr.numberOfSubchannels;
    strm << endl;

    return strm;
}

BaselineHeader::BaselineHeader()
    :
    rfCenterFreq(0),
    bandwidth(0),
    halfFrameNumber(0),
    numberOfSubchannels(0),
    pol(POL_UNINIT),
    activityId(NSS_NO_ACTIVITY_ID)
{
}

void BaselineHeader::marshall()
{
    NTOHD(rfCenterFreq);
    NTOHD(bandwidth);
    NTOHL(halfFrameNumber);
    NTOHL(numberOfSubchannels);
    SseMsg::marshall(pol);
    NTOHL(activityId);
}

void BaselineHeader::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const BaselineValue &bv)
{
    strm << bv.value;

    return strm;
}

BaselineValue::BaselineValue()
    : value(0)
{
}

void BaselineValue::marshall()
{
    HTONF(value);
}

void BaselineValue::demarshall()
{
    marshall();
}

// Extract & demarshall Baseline from a message buffer.
// Returns pointers to the hdr & value array.
// Note that the msgBody buffer is modified.
//
void SseDxMsg::extractBaselineFromMsg(void *msgBody, 
				       BaselineHeader **returnHdr,
				       BaselineValue **returnValueArray)
{
    BaselineHeader *hdr =
	reinterpret_cast<BaselineHeader *>(msgBody);
    hdr->demarshall();

    // Get data array
    int nSubchannels = hdr->numberOfSubchannels;
    int arrayOffset = sizeof(BaselineHeader);
    BaselineValue *valueArray = 
	reinterpret_cast<BaselineValue*>(static_cast<char *>(msgBody) + arrayOffset);
    for (int i=0; i<nSubchannels; ++i)
    {
	valueArray[i].demarshall();
    }

    *returnHdr = hdr;
    *returnValueArray = valueArray;

}

// The BaselineHeader and the variable length values array
// are copied into a contiguous allocated buffer & marshalled.
// Buffer is returned as the function value, and the buffer
// size is returned in the bufferLength parameter.
// Caller is responsible for deleting the buffer.

char * SseDxMsg::encodeBaselineIntoMsg(const BaselineHeader &hdr, 
					BaselineValue values[],
					int *bufferLength)
{
    int nSubchannels = hdr.numberOfSubchannels;
    Assert (nSubchannels > 0);

    // create a temp buffer that can hold the header & the variable array
    int dataLength = sizeof(BaselineHeader) +
	nSubchannels * sizeof(BaselineValue);
    char *outbuff = new char [dataLength];

    // copy the header & marshall it
    BaselineHeader *outHdr = reinterpret_cast<BaselineHeader *>(outbuff);
    *outHdr = hdr;
    outHdr->marshall();

    // copy & marshall the data array
    int arrayOffset = sizeof(BaselineHeader);
    BaselineValue *outValueArray = 
	reinterpret_cast<BaselineValue*>(outbuff + arrayOffset);
    for (int i=0; i<nSubchannels; ++i)
    {
	outValueArray[i] = values[i];
	outValueArray[i].marshall();
    }

   *bufferLength = dataLength;
    return outbuff;

}




//  --- deprecated Baseline struct -----

ostream& operator << (ostream &strm, const Baseline &baseline)
{
    strm << baseline.header;
    
    strm << "baselineValues: ";
    
    int maxBandsToPrint = 5;
    for (int i=0; i<baseline.header.numberOfSubchannels &&
	     i < maxBandsToPrint; ++i)
    {
	strm << baseline.baselineValues[i];
	strm << " ";
    }
    strm << " ... " << endl;

    return strm;

}


void Baseline::marshall()
{
    header.marshall();

    // marshall baselineValues.

    // TBD: change this loop to run up to numberOfSubchannels?
    // if so, need to write a separate demarshall routine below
    // that demarshalls numberOfSubchannels field first.

    for (int i=0; i<MAX_BASELINE_SUBCHANNELS; ++i)
    {
	NTOHF(baselineValues[i]);
    }


}

void Baseline::demarshall()
{
    marshall();
}
