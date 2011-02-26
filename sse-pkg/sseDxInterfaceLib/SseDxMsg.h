/*******************************************************************************

 File:    SseDxMsg.h
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


#ifndef _sse_dx_msg_h
#define _sse_dx_msg_h

#include "sseDxInterface.h"
#include "DxOpsBitset.h"

using std::string;
using std::ostream;


class SseDxMsg
{
 public:

    static string signalClassToString(SignalClass signalClass);
    static SignalClass stringToSignalClass(const string &classString);

    static string signalClassReasonToString(SignalClassReason reason);

    static string signalClassReasonToBriefString(SignalClassReason reason);
    static SignalClassReason briefStringToSignalClassReason(
	const string & briefSigClassString);

    static string resolutionToString(Resolution res);
    static Resolution stringToResolution(const string &resString);

    static string messageCodeToString(uint32_t code);
    static string baselineStatusToString(BaselineStatus status);

    static SciDataRequestType stringToSciDataRequestType(
	const string &reqTypeStr);  
    static string sciDataRequestTypeToString(
	SciDataRequestType reqType); 

    static void printDxOpsMaskNames(
	ostream &strm, const DxOpsBitset &dxOpsBitset);

    static void printPulseSignal(ostream &strm, const PulseSignalHeader &hdr,
				 Pulse pulses[]);

    static void printRecentRfiMask(ostream &strm, 
				   const RecentRfiMaskHeader &rfiMaskHdr,
				   FrequencyBand freqBandArray[]);

    // methods for encoding & extracting variable length messages:

    static void extractPulseSignalFromMsg(
	void *msgBody, PulseSignalHeader **returnHdr, 
	Pulse **returnPulseArray);

    static char *encodePulseSignalIntoMsg(
	const PulseSignalHeader &hdr,
	const Pulse pulses[],
	int *dataLength);


    static void extractFrequencyMaskFromMsg(
	void *msgBody,
	FrequencyMaskHeader **returnHdr,
	FrequencyBand **returnFreqBandArray);
    
    static char *encodeFrequencyMaskIntoMsg(
	const FrequencyMaskHeader &hdr,
	const FrequencyBand freqBandArray[],
	int *bufferLength);


    static void extractRecentRfiMaskFromMsg(
	void *msgBody,
	RecentRfiMaskHeader **returnHdr,
	FrequencyBand **returnFreqBandArray);
    
    static char *encodeRecentRfiMaskIntoMsg(
	const RecentRfiMaskHeader &hdr,
	const FrequencyBand freqBandArray[],
	int *bufferLength);


    static void extractComplexAmplitudesFromMsg(
	void *msgBody,
	ComplexAmplitudeHeader **returnHdr,
	SubchannelCoef1KHz **returnSubchannelArray);

    static char * encodeComplexAmplitudesIntoMsg(
	const ComplexAmplitudeHeader &hdr,
	const SubchannelCoef1KHz subchannelArray[],
	int *bufferLength);

    static void extractBaselineFromMsg(
	void *msgBody,
	BaselineHeader **returnHdr,
	BaselineValue **returnValueArray);

    static char * encodeBaselineIntoMsg(
	const BaselineHeader &hdr, 
	BaselineValue values[],
	int *bufferLength);

    static void marshall(Resolution &arg);
    static void demarshall(Resolution &arg);

    static void marshall(SignalClass &arg);
    static void demarshall(SignalClass &arg);

    static void marshall(SignalClassReason &arg);
    static void demarshall(SignalClassReason &arg);

    static void marshall(DxActivityState &arg);
    static void demarshall(DxActivityState &arg);

 private:
    SseDxMsg();
    ~SseDxMsg();
};


#endif 