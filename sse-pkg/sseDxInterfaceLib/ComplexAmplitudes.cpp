/*******************************************************************************

 File:    ComplexAmplitudes.cpp
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

// Compamps Header
ostream& operator << (ostream &strm, const ComplexAmplitudeHeader &hdr)
{
    // header
    strm << "ComplexAmplitudeHeader: " << endl
	 << "======================= " << endl;
    strm << "rf center freq: " << hdr.rfCenterFreq << " MHz" << endl;
    strm << "halfFrameNumber: " << hdr.halfFrameNumber << endl;
    strm << "activityId: " << hdr.activityId << endl;
    strm << "hzPerSubchannel: " << hdr.hzPerSubchannel << endl;
    strm << "startSubId: " << hdr.startSubchannelId << endl;
    strm << "numberOfsubchannels: " << hdr.numberOfSubchannels << endl;
    strm << "overSampling " << hdr.overSampling << endl;
    strm << "pol: " << SseMsg::polarizationToString(hdr.pol) << endl;

    return strm;
}


ComplexAmplitudeHeader::ComplexAmplitudeHeader()
    :
    rfCenterFreq(0),
    halfFrameNumber(0),
    activityId(NSS_NO_ACTIVITY_ID),
    hzPerSubchannel(0),
    startSubchannelId(0),
    numberOfSubchannels(0),
    overSampling(0.25),
    pol(POL_UNINIT)
{
}

void ComplexAmplitudeHeader::marshall()
{
    HTOND(rfCenterFreq);
    HTONL(halfFrameNumber);
    HTONL(activityId);
    HTOND(hzPerSubchannel);
    HTONL(startSubchannelId);
    HTONL(numberOfSubchannels);
    HTONF(overSampling);
    SseMsg::marshall(pol);
}

void ComplexAmplitudeHeader::demarshall()
{
    marshall();
}


// Comamps coefs
ostream& operator << (ostream &strm, const SubchannelCoef1KHz &subchannel)
{
    strm << "Coefs: ";
    strm << hex;

    int maxCoefsToPrint = 5;
    for (int i=0; i<maxCoefsToPrint && i < MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; ++i)
    {
	// cast to int so it's not printed as a char
	strm << static_cast<int>(subchannel.coef[i].pair);
	strm << " ";
    }
    strm << " ... " << endl;
    strm << dec;

    return strm;
}

SubchannelCoef1KHz::SubchannelCoef1KHz()
{
    // use default constructors for all fields
}

void SubchannelCoef1KHz::marshall()
{
    // coef pairs are 8 bits, so they don't need to be marshalled
}

void SubchannelCoef1KHz::demarshall()
{
    marshall();
}

ComplexPair::ComplexPair()
    : pair(0)
{
}




// Extract & demarshall Complex Amplitudes from a message buffer.
// Returns pointers to the hdr & subchannel array.
// Note that the msgBody buffer is modified.
//
void SseDxMsg::extractComplexAmplitudesFromMsg(void *msgBody,
                                          ComplexAmplitudeHeader **returnHdr,
                                          SubchannelCoef1KHz **returnSubchannelArray)
{
    ComplexAmplitudeHeader *hdr =
	reinterpret_cast<ComplexAmplitudeHeader *>(msgBody);
    hdr->demarshall();

    // Get data array
    int nSubchannels = hdr->numberOfSubchannels;
    int arrayOffset = sizeof(ComplexAmplitudeHeader);
    SubchannelCoef1KHz *subchannelArray = 
	reinterpret_cast<SubchannelCoef1KHz*>(static_cast<char *>(msgBody) + arrayOffset);
    for (int i=0; i<nSubchannels; ++i)
    {
	subchannelArray[i].demarshall();
    }

    *returnHdr = hdr;
    *returnSubchannelArray = subchannelArray;
}

// The ComplexAmplitudeHeader and the variable length subchannel array
// are copied into a contiguous allocated buffer & marshalled.
// Buffer is returned as the function value, and the buffer
// size is returned in the bufferLength parameter.
// Caller is responsible for deleting the buffer.

char * SseDxMsg::encodeComplexAmplitudesIntoMsg(
    const ComplexAmplitudeHeader &hdr,
    const SubchannelCoef1KHz subchannelArray[],
    int *bufferLength)
{
    int nSubchannels = hdr.numberOfSubchannels;
    Assert (nSubchannels > 0);

    // create a temp buffer that can hold the header & the variable array
    int dataLength = sizeof(ComplexAmplitudeHeader) +
	nSubchannels * sizeof(SubchannelCoef1KHz);
    char *outbuff = new char [dataLength];

    // copy the header & marshall it
    ComplexAmplitudeHeader *outHdr = reinterpret_cast<ComplexAmplitudeHeader *>(outbuff);
    *outHdr = hdr;
    outHdr->marshall();

    // copy & marshall the data array
    int arrayOffset = sizeof(ComplexAmplitudeHeader);
    SubchannelCoef1KHz *outBandArray = 
	reinterpret_cast<SubchannelCoef1KHz*>(outbuff + arrayOffset);
    for (int i=0; i<nSubchannels; ++i)
    {
	outBandArray[i] = subchannelArray[i];
	outBandArray[i].marshall();
    }

   *bufferLength = dataLength;
    return outbuff;
}







// ----- Deprecated structs & methods ---------

// combined compamps (header & coefs)
ostream& operator << (ostream &strm, const ComplexAmplitudes &compAmps)
{
    strm << compAmps.header;
    strm << compAmps.compamp;

    return strm;
}

void ComplexAmplitudes::marshall()
{
    header.marshall();
    compamp.marshall();
}

void ComplexAmplitudes::demarshall()
{
    marshall();
}