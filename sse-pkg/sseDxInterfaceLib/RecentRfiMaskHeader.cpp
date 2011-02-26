/*******************************************************************************

 File:    RecentRfiMaskHeader.cpp
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

static const int PrintPrecision(9);  // MilliHz

ostream& operator << (ostream &strm, const RecentRfiMaskHeader &maskHdr)
{
    strm << "Recent RFI Mask Header:" << endl;
    strm << "-----------------------" << endl;

    strm << "numberOfFreqBands: " << maskHdr.numberOfFreqBands << endl;
    strm << "excludedTargetId: " << maskHdr.excludedTargetId << endl;
    strm << "freq band covered: " << maskHdr.bandCovered;
    strm << endl;

    return strm;
}

RecentRfiMaskHeader::RecentRfiMaskHeader()
    :
    numberOfFreqBands(0),
    excludedTargetId(0)
{
}

void RecentRfiMaskHeader::marshall()
{
    HTONL(numberOfFreqBands);
    HTONL(excludedTargetId);
    bandCovered.marshall();    // Frequency Band

}

void RecentRfiMaskHeader::demarshall()
{
    marshall();
}

// Extract & demarshall a RecentRfiMask from a message buffer.
// Returns pointers to the hdr & FrequencyBand array.
// Note that the msgBody buffer is modified.
//
void SseDxMsg::extractRecentRfiMaskFromMsg(void *msgBody,
                                          RecentRfiMaskHeader **returnHdr,
                                          FrequencyBand **returnFreqBandArray)
{
    RecentRfiMaskHeader *hdr = reinterpret_cast<RecentRfiMaskHeader *>(msgBody);
    hdr->demarshall();

    // Get data array
    int nFreqBands = hdr->numberOfFreqBands;
    int arrayOffset = sizeof(RecentRfiMaskHeader);
    FrequencyBand *freqBandArray = 
	reinterpret_cast<FrequencyBand*>(static_cast<char *>(msgBody) + arrayOffset);
    for (int i=0; i<nFreqBands; ++i)
    {
	freqBandArray[i].demarshall();
    }

    *returnHdr = hdr;
    *returnFreqBandArray = freqBandArray;
}

// The RecentRfiMaskHeader and the variable length FrequencyBand array
// are copied into a contiguous allocated buffer & marshalled.
// Buffer is returned as the function value, and the buffer
// size is returned in the bufferLength parameter.
// Caller is responsible for deleting the buffer.

char * SseDxMsg::encodeRecentRfiMaskIntoMsg(
    const RecentRfiMaskHeader &hdr,
    const FrequencyBand freqBandArray[],
    int *bufferLength)
{
    int nFreqBands = hdr.numberOfFreqBands;
    Assert (nFreqBands > 0);

    // create a temp buffer that can hold the header & the variable array
    int dataLength = sizeof(RecentRfiMaskHeader) +
	nFreqBands * sizeof(FrequencyBand);
    char *outbuff = new char [dataLength];

    // copy the header & marshall it
    RecentRfiMaskHeader *outHdr = reinterpret_cast<RecentRfiMaskHeader *>(outbuff);
    *outHdr = hdr;
    outHdr->marshall();

    // copy & marshall the data array
    int arrayOffset = sizeof(RecentRfiMaskHeader);
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


void SseDxMsg::printRecentRfiMask(ostream &strm, 
			const RecentRfiMaskHeader &rfiMaskHdr,
			FrequencyBand freqBandArray[])
{
    stringstream localstrm;

    localstrm.setf(std::ios::fixed);  // show all decimal places up to precision
    localstrm.precision(PrintPrecision);  // show N places after the decimal

    localstrm << rfiMaskHdr;

    localstrm << "        centerFreq          bandwidth" << endl;
    localstrm << "        ----------          ---------" << endl;

    int indexPrintWidth=5;

    for (int i=0; i<rfiMaskHdr.numberOfFreqBands; ++i)
    {
	localstrm.width(indexPrintWidth);
	localstrm << i << ")  "
		  << freqBandArray[i].centerFreq << " MHz  "
		  << freqBandArray[i].bandwidth << " MHz "
		  << endl;
    }

    strm << localstrm.str();
}