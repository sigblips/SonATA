/*******************************************************************************

 File:    SseProxy.h
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


#ifndef _sseproxy_h
#define _sseproxy_h

#include "sseDxInterface.h"
#include "NssProxy.h"

class Dx;

class SseProxy : public NssProxy
{

public:
    SseProxy(ACE_SOCK_STREAM &stream);
    virtual ~SseProxy();

    void setDx(Dx *dx);
    string getName();

    // outgoing messages to sse
    void sendDxMessage(const NssMessage &nssMessage, int activityId = -1);
    void sendIntrinsics(const DxIntrinsics &intrinsics);
    void sendDxStatus(const DxStatus &status);
    void sendBaseline(const BaselineHeader &hdr, BaselineValue values[],
		      int activityId);
    void sendComplexAmplitudes(const ComplexAmplitudeHeader &hdr,
	SubchannelCoef1KHz subchannels[], int activityId);
    void sendBaselineStatistics(const BaselineStatistics &stats, int activityId);
    void baselineWarningLimitsExceeded(
	const BaselineLimitsExceededDetails &details, int activityId);
    void baselineErrorLimitsExceeded(
	const BaselineLimitsExceededDetails &details, int activityId);

    void dxTuned(DxTuned &dxTuned, int activityId);
    void baselineInitAccumStarted(int activityId);
    void baselineInitAccumComplete(int activityId);
    void dataCollectionStarted(int activityId);
    void dataCollectionComplete(int activityId);
    void signalDetectionStarted(int activityId);
    void signalDetectionComplete(int activityId);
    void activityComplete(const DxActivityStatus &status, int activityId);

    void beginSendingCandidates(const Count &count, int activityId);
    void sendCandidatePulseSignal(const PulseSignalHeader &hdr, Pulse pulses[],
				  int activityId);
    void sendCandidateCwPowerSignal(const CwPowerSignal &cwPowerSignal,
				    int activityId);
    void doneSendingCandidates(int activityId);

    void beginSendingSignals(const DetectionStatistics &stats, int activityId);
    void sendPulseSignal(const PulseSignalHeader &hdr, Pulse pulses[], int activityId);
    void sendCwPowerSignal(const CwPowerSignal &cwPowerSignal, int activityId);
    void doneSendingSignals(int activityId);

    void beginSendingBadBands(const Count &count, int activityId);
    void sendPulseBadBand(const PulseBadBand & pulseBadBand, int activityId);
    void sendCwBadBand(const CwBadBand & cwBadBand, int activityId);
    void doneSendingBadBands(int activityId);

    void beginSendingCwCoherentSignals(const Count &count, int activityId);
    void sendCwCoherentSignal(const CwCoherentSignal &cwCoherentSignal, int activityId);
    void doneSendingCwCoherentSignals(int activityId);

    void beginSendingCandidateResults(const Count &count, int activityId);
    void sendPulseCandidateResult(const PulseSignalHeader &hdr, Pulse pulses[],
				  int activityId);
    void sendCwCoherentCandidateResult(const CwCoherentSignal &cwCoherentSignal,
				       int activityId);
    void doneSendingCandidateResults(int activityId);

    void archiveSignal(const ArchiveDataHeader &hdr, int activityId);
    void beginSendingArchiveComplexAmplitudes(const Count &nHalfFrames, int activityId);
    void sendArchiveComplexAmplitudes(const ComplexAmplitudeHeader &hdr,
	SubchannelCoef1KHz subchannels[], int activityId);
    void doneSendingArchiveComplexAmplitudes(int activityId);

    void archiveComplete(int activityId);

private:
    // Disable copy construction & assignment.
    // Don't define these.
    SseProxy(const SseProxy& rhs);
    SseProxy& operator=(const SseProxy& rhs);
    
    void sendMessage(int messageCode, int activityId = -1, int dataLength = 0,
			       const void *msgBody = 0);

    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff);

    void sendBirdieMaskToDx(void *msgBody);
    void sendRcvrBirdieMaskToDx(void *msgBody);
    void sendTestSignalMaskToDx(void *msgBody);
    void sendPermRfiMaskToDx(void *msgBody);
    void sendRecentRfiMaskToDx(void *msgBody);
    void sendCandidatePulseSignalToDx(void *msgBody);
    void sendFollowUpPulseSignalToDx(void *msgBody);
    void sendRequestPulseSignalArchiveDataToDx(void *msgBody);

    void sendPulseSignalMsg(int messageCode,
			    int activityId,
			    const PulseSignalHeader &hdr,
			    Pulse pulses[]);

    void sendComplexAmplitudesMsg(int messageCode, 
				  int activityId,
				  const ComplexAmplitudeHeader &hdr,
				  SubchannelCoef1KHz subchannels[]);

    void sendBaselineMsg(int messageCode,
			 int activityId,
			 const BaselineHeader &hdr, 
			 BaselineValue values[]);

    Dx *dx_;

};



#endif // _sseproxy_h