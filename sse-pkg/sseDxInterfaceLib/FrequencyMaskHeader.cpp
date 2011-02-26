/*******************************************************************************

 File:    FrequencyMaskHeader.cpp
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

ostream& operator << (ostream &strm, const FrequencyMaskHeader &maskHdr)
{
    strm << "Frequency Mask Header:" << endl;
    strm << "----------------------" << endl;

    strm << "numberOfFreqBands: " << maskHdr.numberOfFreqBands << endl;
    strm << "maskVersionDate: ";
    strm << maskHdr.maskVersionDate << endl;
    strm << "freq Band covered: " << maskHdr.bandCovered;
    strm << endl;

    return strm;
}

FrequencyMaskHeader::FrequencyMaskHeader()
    :
    numberOfFreqBands(0),
    alignPad(0)  
{
    // use default constructors for remaining fields
}

void FrequencyMaskHeader::marshall()
{
    HTONL(numberOfFreqBands);
    maskVersionDate.marshall();
    bandCovered.marshall();    // Frequency Band
}

void FrequencyMaskHeader::demarshall()
{
    marshall();
}

// Extract & demarshall a FrequencyMask from a message buffer.
// Returns pointers to the hdr & FrequencyBand array.
// Note that the msgBody buffer is modified.
//
void SseDxMsg::extractFrequencyMaskFromMsg(void *msgBody,
                                          FrequencyMaskHeader **returnHdr,
                                          FrequencyBand **returnFreqBandArray)
{
    FrequencyMaskHeader *hdr = reinterpret_cast<FrequencyMaskHeader *>(msgBody);
    hdr->demarshall();

    // Get data array
    int nFreqBands = hdr->numberOfFreqBands;
    int arrayOffset = sizeof(FrequencyMaskHeader);
    FrequencyBand *freqBandArray = 
	reinterpret_cast<FrequencyBand*>(static_cast<char *>(msgBody) + arrayOffset);
    for (int i=0; i<nFreqBands; ++i)
    {
	freqBandArray[i].demarshall();
    }

    *returnHdr = hdr;
    *returnFreqBandArray = freqBandArray;
}

// The FrequencyMaskHeader and the variable length FrequencyBand array
// are copied into a contiguous allocated buffer & marshalled.
// Buffer is returned as the function value, and the buffer
// size is returned in the bufferLength parameter.
// Caller is responsible for deleting the buffer.

char * SseDxMsg::encodeFrequencyMaskIntoMsg(
    const FrequencyMaskHeader &hdr,
    const FrequencyBand freqBandArray[],
    int *bufferLength)
{
    int nFreqBands = hdr.numberOfFreqBands;
    Assert (nFreqBands > 0);

    // create a temp buffer that can hold the header & the variable array
    int dataLength = sizeof(FrequencyMaskHeader) +
	nFreqBands * sizeof(FrequencyBand);
    char *outbuff = new char [dataLength];

    // copy the header & marshall it
    FrequencyMaskHeader *outHdr = reinterpret_cast<FrequencyMaskHeader *>(outbuff);
    *outHdr = hdr;
    outHdr->marshall();

    // copy & marshall the data array
    int arrayOffset = sizeof(FrequencyMaskHeader);
    FrequencyBand *outBandArray = 
	reinterpret_cast<FrequencyBand*>(outbuff + arrayOffset);
    for (int i=0; i<nFreqBands; ++i)
    {
	outBandArray[i] = freqBandArray[i];
	outBandArray[i].marshall();
    }


   *bufferLength = dataLength;
    return outbuff;
}