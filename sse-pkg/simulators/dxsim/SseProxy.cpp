/*******************************************************************************

 File:    SseProxy.cpp
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


#include <ace/OS.h>
#include "SseProxy.h"
#include <iostream>
#include "SseDxMsg.h"
#include "Dx.h"
#include "HostNetByteorder.h"
#include "Assert.h"

using namespace std;

//#define TEST_SEND_BAD_MESSAGE_SIZE 1


SseProxy::SseProxy(ACE_SOCK_STREAM &stream)
    : NssProxy(stream),
      dx_(0)
{
    NssProxy::setVerboseLevel(2);   // 2 = maximum verbose output
}

SseProxy::~SseProxy()
{
}

void SseProxy::setDx(Dx *dx)
{
    dx_ = dx;
}

string SseProxy::getName()
{
    return "sse";
}

// outgoing messages to sse

void SseProxy::sendDxMessage(const NssMessage &nssMessage, int activityId)
{
    cout << "SseProxy: sendDxMessage" << endl;

    DxMessageCode code = SEND_DX_MESSAGE;
    int dataLength = sizeof(nssMessage);

    NssMessage marshall = nssMessage;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::sendIntrinsics(const DxIntrinsics &intrinsics)
{
    cout << "SseProxy: sendIntrinsics" << endl;

    DxMessageCode code = SEND_INTRINSICS;
    int dataLength = sizeof(intrinsics);

    // marshall a copy of the intrinsics
    DxIntrinsics marshall = intrinsics;
    marshall.marshall();
    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(code, activityId, dataLength, &marshall);

}


void SseProxy::sendDxStatus(const DxStatus &status)
{
    cout << "SseProxy: sendStatus" << endl;


    DxMessageCode code = SEND_DX_STATUS;
    int dataLength = sizeof(status);

    // marshall a copy of the status
    DxStatus marshall = status;
    marshall.marshall();

    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::activityComplete(const DxActivityStatus &status, int activityId)
{
    cout << "SseProxy: activityComplete" << endl;

    DxMessageCode code = DX_ACTIVITY_COMPLETE;
    int dataLength = sizeof(status);

    // marshall a copy of the status (TBD)
    DxActivityStatus marshall = status;
    marshall.marshall(); 
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::dxTuned(DxTuned &dxTuned, int activityId)
{
    cout << "SseProxy: sending dxTuned" << endl;

    DxMessageCode code = DX_TUNED;
    int dataLength = sizeof(dxTuned);

    // marshall a copy of the data
    DxTuned marshall = dxTuned;
    marshall.marshall(); 
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::sendBaseline(const BaselineHeader &hdr, BaselineValue values[],
			    int activityId)
{
    cout << "SseProxy: sendBaseline" << endl;

    DxMessageCode code = SEND_BASELINE;
    sendBaselineMsg(code, activityId, hdr, values);
}

void SseProxy::sendComplexAmplitudes(const ComplexAmplitudeHeader &hdr,
	SubchannelCoef1KHz subchannels[], int activityId)
{
    cout << "SseProxy: sendComplexAmplitudes" << endl;

    DxMessageCode code = SEND_COMPLEX_AMPLITUDES;
    sendComplexAmplitudesMsg(code, activityId, hdr, subchannels);
}

void SseProxy::sendBaselineStatistics(const BaselineStatistics &stats,
			    int activityId)
{
    cout << "SseProxy: sendBaselineStatistics" << endl;

    DxMessageCode code = SEND_BASELINE_STATISTICS;
    int dataLength = sizeof(stats);

    // marshall a copy of the data
    BaselineStatistics marshall = stats;
    marshall.marshall(); 
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::baselineWarningLimitsExceeded(
    const BaselineLimitsExceededDetails &details, int activityId)
{
    cout << "SseProxy: baselineWarningLimitsExceeded" << endl;

    DxMessageCode code = BASELINE_WARNING_LIMITS_EXCEEDED;
    int dataLength = sizeof(details);

    // marshall a copy of the data
    BaselineLimitsExceededDetails marshall = details;
    marshall.marshall(); 
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::baselineErrorLimitsExceeded(
    const BaselineLimitsExceededDetails &details, int activityId)
{
    cout << "SseProxy: baselineErrorLimitsExceeded" << endl;

    DxMessageCode code = BASELINE_ERROR_LIMITS_EXCEEDED;
    int dataLength = sizeof(details);

    // marshall a copy of the data
    BaselineLimitsExceededDetails marshall = details;
    marshall.marshall(); 
    sendMessage(code, activityId, dataLength, &marshall);

}


void SseProxy::archiveSignal(const ArchiveDataHeader &hdr,
			     int activityId)
{
    cout << "SseProxy: archiveSignal" << endl;

    DxMessageCode messageCode = ARCHIVE_SIGNAL;
    int dataLength = sizeof(hdr);

    // marshall a copy of the data
    ArchiveDataHeader marshall = hdr;
    marshall.marshall(); 
    sendMessage(messageCode, activityId, dataLength, &marshall);

}

void SseProxy::beginSendingArchiveComplexAmplitudes(
    const Count &nHalfFrames, int activityId)
{
    cout << "SseProxy: beginSendingArchiveComplexAmplitudes" << endl;

    DxMessageCode code = BEGIN_SENDING_ARCHIVE_COMPLEX_AMPLITUDES;
    int dataLength = sizeof(Count);

    Count marshall = nHalfFrames;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendArchiveComplexAmplitudes(
    const ComplexAmplitudeHeader &hdr,
    SubchannelCoef1KHz subchannels[], int activityId)
{
    cout << "SseProxy: sendArchiveComplexAmplitudes" << endl;

    DxMessageCode code = SEND_ARCHIVE_COMPLEX_AMPLITUDES;
    sendComplexAmplitudesMsg(code, activityId, hdr, subchannels);
}


void SseProxy::doneSendingArchiveComplexAmplitudes(int activityId)
{
    cout << "SseProxy:doneSendingArchiveComplexAmplitudes" << endl;

    DxMessageCode code =  DONE_SENDING_ARCHIVE_COMPLEX_AMPLITUDES;
    sendMessage(code, activityId);
}



void SseProxy::baselineInitAccumStarted(int activityId)
{
    cout << "SseProxy: sending baselineInitAccumStarted" << endl;

    DxMessageCode code = BASELINE_INIT_ACCUM_STARTED;
    sendMessage(code, activityId);
}


void SseProxy:: baselineInitAccumComplete(int activityId)
{
    cout << "SseProxy: sending baselineInitAccumComplete" << endl;

    DxMessageCode code = BASELINE_INIT_ACCUM_COMPLETE;
    sendMessage(code, activityId);
}



void SseProxy::dataCollectionStarted(int activityId)
{
    cout << "SseProxy: sending DC started" << endl;

    DxMessageCode code = DATA_COLLECTION_STARTED;
    sendMessage(code, activityId);
}


void SseProxy::dataCollectionComplete(int activityId)
{
    cout << "SseProxy: sending DC complete" << endl;

    DxMessageCode code = DATA_COLLECTION_COMPLETE;
    sendMessage(code, activityId);
}

void SseProxy::signalDetectionStarted(int activityId)
{
    cout << "SseProxy: sending SD started" << endl;

    DxMessageCode code = SIGNAL_DETECTION_STARTED;
    sendMessage(code, activityId);
}


void SseProxy::signalDetectionComplete(int activityId)
{
    cout << "SseProxy: sending SD complete" << endl;

    DxMessageCode code =  SIGNAL_DETECTION_COMPLETE;
    sendMessage(code, activityId);
}

void SseProxy::beginSendingCandidates(const Count &count, int activityId)
{
    cout << "SseProxy: beginSendingCandidates" << endl;

    DxMessageCode code = BEGIN_SENDING_CANDIDATES;
    int dataLength = sizeof(Count);

    Count marshall = count;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}



void SseProxy::sendCandidatePulseSignal(const PulseSignalHeader &hdr,
					Pulse pulses[], int activityId)
{
    cout << "SseProxy: sendCandidatePulseSignal" << endl;

    DxMessageCode code = SEND_CANDIDATE_PULSE_SIGNAL;
    sendPulseSignalMsg(code, activityId, hdr, pulses);
}

void SseProxy::sendCandidateCwPowerSignal(const CwPowerSignal &cwPowerSignal,
					  int activityId)
{
    cout << "SseProxy: sendCandidateCwPowerSignal" << endl;

    DxMessageCode code = SEND_CANDIDATE_CW_POWER_SIGNAL;
    int dataLength = sizeof(CwPowerSignal);

    CwPowerSignal marshall = cwPowerSignal;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::doneSendingCandidates(int activityId)
{
    cout << "SseProxy:doneSendingCandidates" << endl;

    DxMessageCode code =  DONE_SENDING_CANDIDATES;
    sendMessage(code, activityId);
}

void SseProxy::beginSendingSignals(const DetectionStatistics &stats,
				    int activityId)
{
    cout << "SseProxy: beginSendingSignals" << endl;

    DxMessageCode code = BEGIN_SENDING_SIGNALS;
    int dataLength = sizeof(DetectionStatistics);

    DetectionStatistics marshall = stats;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendPulseSignal(const PulseSignalHeader &hdr, Pulse pulses[],
			       int activityId)
{
    cout << "SseProxy: sendPulseSignal" << endl;

    DxMessageCode code = SEND_PULSE_SIGNAL;
    sendPulseSignalMsg(code, activityId, hdr, pulses);
}

void SseProxy::sendCwPowerSignal(const CwPowerSignal &cwPowerSignal, int activityId)
{
    cout << "SseProxy: sendCwPowerSignal" << endl;

    DxMessageCode code = SEND_CW_POWER_SIGNAL;
    int dataLength = sizeof(CwPowerSignal);

    CwPowerSignal marshall = cwPowerSignal;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::doneSendingSignals(int activityId)
{
    cout << "SseProxy:doneSendingSignals" << endl;

    DxMessageCode code =  DONE_SENDING_SIGNALS;
    sendMessage(code, activityId);
}


void SseProxy::beginSendingBadBands(const Count &count, int activityId)
{
    cout << "SseProxy: beginSendingBadBands" << endl;

    DxMessageCode code = BEGIN_SENDING_BAD_BANDS;
    int dataLength = sizeof(Count);

    Count marshall = count;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendCwBadBand(const CwBadBand &cwBadBand, int activityId)
{
    cout << "SseProxy: sendCwBadBand" << endl;

    DxMessageCode code = SEND_CW_BAD_BAND;
    int dataLength = sizeof(CwBadBand);

    CwBadBand marshall = cwBadBand;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::sendPulseBadBand(const PulseBadBand &pulseBadBand, int activityId)
{
    cout << "SseProxy: sendPulseBadBand" << endl;

    DxMessageCode code = SEND_PULSE_BAD_BAND;
    int dataLength = sizeof(PulseBadBand);

    PulseBadBand marshall = pulseBadBand;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::doneSendingBadBands(int activityId)
{
    cout << "SseProxy:doneSendingBadBands" << endl;

    DxMessageCode code =  DONE_SENDING_BAD_BANDS;
    sendMessage(code, activityId);
}



void SseProxy::beginSendingCwCoherentSignals(const Count &count, int activityId)
{
    cout << "SseProxy: beginSendingCwCoherentSignals" << endl;

    DxMessageCode code = BEGIN_SENDING_CW_COHERENT_SIGNALS;
    int dataLength = sizeof(Count);

    Count marshall = count;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendCwCoherentSignal(const CwCoherentSignal &cwCoherentSignal,
				    int activityId)
{
    cout << "SseProxy: sendCwCoherentSignal" << endl;

    DxMessageCode code = SEND_CW_COHERENT_SIGNAL;
    int dataLength = sizeof(CwCoherentSignal);

    CwCoherentSignal marshall = cwCoherentSignal;
    marshall.marshall();
    sendMessage(code,  activityId, dataLength, &marshall);

}


void SseProxy::doneSendingCwCoherentSignals(int activityId)
{
    cout << "SseProxy:doneSendingCwCoherentSignals" << endl;

    DxMessageCode code =  DONE_SENDING_CW_COHERENT_SIGNALS;
    sendMessage(code, activityId);
}

void SseProxy::beginSendingCandidateResults(const Count &count, int activityId)
{
    cout << "SseProxy: beginSendingCandidateResults" << endl;

    DxMessageCode code = BEGIN_SENDING_CANDIDATE_RESULTS;
    int dataLength = sizeof(Count);

    Count marshall = count;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendPulseCandidateResult(const PulseSignalHeader &hdr, Pulse pulses[],
					int activityId)
{
    cout << "SseProxy: sendPulseCandidateResult" << endl;

    DxMessageCode code = SEND_PULSE_CANDIDATE_RESULT;
    sendPulseSignalMsg(code, activityId, hdr, pulses);
}

void SseProxy::sendCwCoherentCandidateResult(const CwCoherentSignal &cwCoherentSignal,
					     int activityId)
{
    cout << "SseProxy: sendCwCoherentCandidateResult" << endl;

    DxMessageCode code = SEND_CW_COHERENT_CANDIDATE_RESULT;
    int dataLength = sizeof(CwCoherentSignal);

    CwCoherentSignal marshall = cwCoherentSignal;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}



void SseProxy::doneSendingCandidateResults(int activityId)
{
    cout << "SseProxy:doneSendingCandidateResults" << endl;

    DxMessageCode code =  DONE_SENDING_CANDIDATE_RESULTS;
    sendMessage(code, activityId);
}




void SseProxy::archiveComplete(int activityId)
{
    cout << "SseProxy: sending archiveComplete" << endl;

    DxMessageCode code = ARCHIVE_COMPLETE;
    sendMessage(code, activityId);
}



// --- utilities -----
// send a marshalled message to the SSE
void SseProxy::sendMessage(int messageCode, int activityId, 
			   int dataLength, const void *msgBody)
{
    cout << "Send: " << SseDxMsg::messageCodeToString(messageCode) << endl;

    NssProxy::sendMessage(messageCode, activityId, dataLength, msgBody);

}

// process incoming variable length messages, passing them
// on to the dx.

void SseProxy::sendPermRfiMaskToDx(void *msgBody)
{
    cout << "SseProxy::sendPermRfiMaskToDx" << endl;

    FrequencyMaskHeader *hdr;
    FrequencyBand *freqBandArray;
    SseDxMsg::extractFrequencyMaskFromMsg(msgBody, &hdr, &freqBandArray);

    dx_->setPermRfiMask(hdr, freqBandArray);
}

void SseProxy::sendBirdieMaskToDx(void *msgBody)
{
    cout << "SseProxy::sendBirdieMaskToDx" << endl;

    FrequencyMaskHeader *hdr;
    FrequencyBand *freqBandArray;
    SseDxMsg::extractFrequencyMaskFromMsg(msgBody, &hdr, &freqBandArray);

    dx_->setBirdieMask(hdr, freqBandArray);
}

void SseProxy::sendRcvrBirdieMaskToDx(void *msgBody)
{
    cout << "SseProxy::sendRcvrBirdieMaskToDx" << endl;

    FrequencyMaskHeader *hdr;
    FrequencyBand *freqBandArray;
    SseDxMsg::extractFrequencyMaskFromMsg(msgBody, &hdr, &freqBandArray);

    dx_->setRcvrBirdieMask(hdr, freqBandArray);
}


void SseProxy::sendTestSignalMaskToDx(void *msgBody)
{
    cout << "SseProxy::sendTestSignalMaskToDx" << endl;

    FrequencyMaskHeader *hdr;
    FrequencyBand *freqBandArray;
    SseDxMsg::extractFrequencyMaskFromMsg(msgBody, &hdr, &freqBandArray);

    dx_->setTestSignalMask(hdr, freqBandArray);
}

void SseProxy::sendRecentRfiMaskToDx(void *msgBody)
{
    cout << "SseProxy::sendRecentRfiMaskToDx" << endl;

    RecentRfiMaskHeader *hdr;
    FrequencyBand *freqBandArray;
    SseDxMsg::extractRecentRfiMaskFromMsg(msgBody, &hdr, &freqBandArray);

    dx_->setRecentRfiMask(hdr, freqBandArray);
}

void SseProxy::sendCandidatePulseSignalToDx(void *msgBody)
{
    cout << "SseProxy::sendCandidatePulseSignalToDx" << endl;

    PulseSignalHeader *hdr;
    Pulse *pulseArray;
    SseDxMsg::extractPulseSignalFromMsg(msgBody, &hdr, &pulseArray);

    dx_->sendCandidatePulseSignal(hdr, pulseArray);
}

void SseProxy::sendPulseSignalMsg(int messageCode,
				  int activityId,
				  const PulseSignalHeader &hdr,
				  Pulse pulses[])
{
    int dataLength;
    char *msgBody = 
	SseDxMsg::encodePulseSignalIntoMsg(hdr, pulses, &dataLength);

    sendMessage(messageCode, activityId, dataLength, msgBody);

    delete[] msgBody;

}

void SseProxy::sendComplexAmplitudesMsg(int messageCode,
					int activityId,
					const ComplexAmplitudeHeader &hdr,
					SubchannelCoef1KHz subchannels[])
{
    int dataLength;

    char *msgBody = SseDxMsg::encodeComplexAmplitudesIntoMsg(
	hdr, subchannels, &dataLength);

#ifdef  TEST_SEND_BAD_MESSAGE_SIZE
    // force bad message length for testing
    dataLength = 2000000;
#endif

    sendMessage(messageCode, activityId, dataLength, msgBody);

    delete[] msgBody;
}

void SseProxy::sendBaselineMsg(int messageCode,
			       int activityId,
			       const BaselineHeader &hdr, 
			       BaselineValue values[])
{
    int dataLength;

    char *msgBody = SseDxMsg::encodeBaselineIntoMsg(
	hdr, values, &dataLength);

    sendMessage(messageCode, activityId, dataLength, msgBody);

    delete[] msgBody;
}



//--------------------------------------------------

// dispatch incoming sse message to appropriate
// dx method
void SseProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{

  cout << "SseProxy Recv: " << SseDxMsg::messageCodeToString(hdr->code) << endl;
  cout << *hdr;

  DxConfiguration *config;
  DxActivityParameters *actParam;
  StartActivity *startAct;
  DxScienceDataRequest *dataRequest;
  Count *count;
  CwPowerSignal *cwPowerSignal;
  CwCoherentSignal *cwCoherentSignal;
  ArchiveRequest *archiveRequest;
  FollowUpCwSignal *followUpCwSignal;
  FollowUpPulseSignal *followUpPulseSignal;

  Assert(dx_);

  // dispatch to Dx here
  // process message body, if there is one
  switch (hdr->code)
  {
  case REQUEST_INTRINSICS:
      dx_->requestIntrinsics(this);
      break;

  case CONFIGURE_DX:
      Assert(hdr->dataLength == sizeof(DxConfiguration));
      config = static_cast<DxConfiguration *>(bodybuff);
      config->demarshall();
      dx_->configureDx(config);
      dx_->printConfiguration();
      break;

  case PERM_RFI_MASK:
      Assert(hdr->dataLength > 0);
      sendPermRfiMaskToDx(bodybuff);
      break;

  case BIRDIE_MASK:
      Assert(hdr->dataLength > 0);
      sendBirdieMaskToDx(bodybuff);
      break;

  case RCVR_BIRDIE_MASK:
      Assert(hdr->dataLength > 0);
      sendRcvrBirdieMaskToDx(bodybuff);
      break;

  case TEST_SIGNAL_MASK:
      Assert(hdr->dataLength > 0);
      sendTestSignalMaskToDx(bodybuff);
      break;

  case RECENT_RFI_MASK:
      Assert(hdr->dataLength > 0);
      sendRecentRfiMaskToDx(bodybuff);
      break;

  case SEND_DX_ACTIVITY_PARAMETERS: 
      Assert(hdr->dataLength == sizeof(DxActivityParameters));
      actParam =  static_cast<DxActivityParameters *>(bodybuff);
      actParam->demarshall();
      dx_->setDxActivityParameters(actParam);
      dx_->printActivityParameters();
      break;

  case DX_SCIENCE_DATA_REQUEST:
      Assert(hdr->dataLength == sizeof(DxScienceDataRequest));
      dataRequest =  static_cast<DxScienceDataRequest *>(bodybuff);
      dataRequest->demarshall();
      dx_->dxScienceDataRequest(dataRequest);
      dx_->printScienceDataRequest();
      break;

  case REQUEST_DX_STATUS:
      dx_->requestDxStatus();
      break;

  case START_TIME:
      Assert(hdr->dataLength == sizeof(StartActivity));
      startAct =  static_cast<StartActivity *>(bodybuff);
      startAct->demarshall();
      dx_->setStartTime(startAct);
      break;

  case BEGIN_SENDING_CANDIDATES:
      Assert(hdr->dataLength == sizeof(Count));
      count = static_cast<Count *>(bodybuff);
      count->demarshall();
      dx_->beginSendingCandidates(count);
      break;

  case SEND_CANDIDATE_CW_POWER_SIGNAL:
      Assert(hdr->dataLength == sizeof(CwPowerSignal));
      cwPowerSignal = static_cast<CwPowerSignal *>(bodybuff);
      cwPowerSignal->demarshall();
      dx_->sendCandidateCwPowerSignal(cwPowerSignal);
      break;

  case SEND_CANDIDATE_PULSE_SIGNAL:
      Assert(hdr->dataLength > 0);
      sendCandidatePulseSignalToDx(bodybuff);
      break;

  case DONE_SENDING_CANDIDATES:
      dx_->doneSendingCandidates();
      break;


  case BEGIN_SENDING_FOLLOW_UP_SIGNALS:
      Assert(hdr->dataLength == sizeof(Count));
      count = static_cast<Count *>(bodybuff);
      count->demarshall();
      dx_->beginSendingFollowUpSignals(count);
      break;

  case SEND_FOLLOW_UP_CW_SIGNAL:
      Assert(hdr->dataLength == sizeof(FollowUpCwSignal));
      followUpCwSignal = static_cast<FollowUpCwSignal *>(bodybuff);
      followUpCwSignal->demarshall();
      dx_->sendFollowUpCwSignal(followUpCwSignal);
      break;

  case SEND_FOLLOW_UP_PULSE_SIGNAL:
      Assert(hdr->dataLength == sizeof(FollowUpPulseSignal));
      followUpPulseSignal = static_cast<FollowUpPulseSignal *>(bodybuff);
      followUpPulseSignal->demarshall();
      dx_->sendFollowUpPulseSignal(followUpPulseSignal);
      break;

  case DONE_SENDING_FOLLOW_UP_SIGNALS:
      dx_->doneSendingFollowUpSignals();
      break;


  case BEGIN_SENDING_CW_COHERENT_SIGNALS:
      Assert(hdr->dataLength == sizeof(Count));
      count = static_cast<Count *>(bodybuff);
      count->demarshall();
      dx_->beginSendingCwCoherentSignals(count);
      break;

  case SEND_CW_COHERENT_SIGNAL:
      Assert(hdr->dataLength == sizeof(CwCoherentSignal));
      cwCoherentSignal = static_cast<CwCoherentSignal *>(bodybuff);
      cwCoherentSignal->demarshall();
      dx_->sendCwCoherentSignal(cwCoherentSignal);
      break;

  case DONE_SENDING_CW_COHERENT_SIGNALS:
      dx_->doneSendingCwCoherentSignals();
      break;

  case REQUEST_ARCHIVE_DATA:
      Assert(hdr->dataLength == sizeof(ArchiveRequest));
      archiveRequest = static_cast<ArchiveRequest *>(bodybuff);
      archiveRequest->demarshall();
      dx_->requestArchiveData(archiveRequest);
      break;

  case DISCARD_ARCHIVE_DATA:
      Assert(hdr->dataLength == sizeof(ArchiveRequest));
      archiveRequest = static_cast<ArchiveRequest *>(bodybuff);
      archiveRequest->demarshall();
      dx_->discardArchiveData(archiveRequest);
      break;

  case STOP_DX_ACTIVITY:
      dx_->stopDxActivity(hdr->activityId);
      break;

  case SHUTDOWN_DX:
      dx_->shutdown();
      break;

  case RESTART_DX:
      dx_->restart();
      break;

  default: 
      cout << "SseProxy::handleSseMsg: ";
      cout << "unexpected message code received:" << hdr->code << endl;
      break;
  };  


}
