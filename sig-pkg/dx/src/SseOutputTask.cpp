/*******************************************************************************

 File:    SseOutputTask.cpp
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

//
// Detector->SSE output handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SseOutputTask.cpp,v 1.3 2009/02/13 03:06:32 kes Exp $
//
#include <stdio.h>
#include "Args.h"
#include "Err.h"
#include "Log.h"
#include "SseOutputTask.h"
#include "Timer.h"

namespace dx {

SseOutputTask *SseOutputTask::instance = 0;

SseOutputTask *
SseOutputTask::getInstance(string tname_)
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new SseOutputTask(tname_);
	l.unlock();
	return (instance);
}

SseOutputTask::SseOutputTask(string tname_):
		 QTask(tname_, OUTPUT_PRIO), msgNumber(0)
{
}

SseOutputTask::~SseOutputTask()
{
}

void
SseOutputTask::extractArgs()
{
#if ASSIGN_CPUS
	// assign the task processor affinity in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	cpu_set_t affinity;
	CPU_ZERO(&affinity);
	int32_t n = 0;
	if (nCpus > 2) {
		// remove affinity for cpu 1
		++n;
	}
	if (nCpus > 3) {
		// remove affinity for cpu 2
		++n;
	}
	// assign affinity
	for (int32_t i = n; i < nCpus; ++i)
		CPU_SET(i, &affinity);
	pid_t tid = gettid();
	int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
	Assert(rval >= 0);
#endif

	// extract startup parameters
	SseOutputArgs *outputArgs = static_cast<SseOutputArgs *> (args);
	Assert(outputArgs);
	sse = outputArgs->sse;
	Assert(sse);
}

//
// handleMsg: send a message to the SSE
//
// Notes:
//		The queue message is a pointer to a generic Msg,
//		which may or may not contain data associated with
//		the message.
//		The message as received is assumed to be in internal
//		byte order; this routine will marshall it before
//		sending it.
//
void
SseOutputTask::handleMsg(Msg *msg)
{
	msg->setMsgNumber(++msgNumber);
	int32_t len = msg->getDataLength();
	void *data = msg->getData();
	SseInterfaceHeader hdr = msg->getHeader();

	if (hdr.code >= DX_MESSAGE_CODE_END) {
		Err(hdr.code);
		Err(msg->getUnit());
		Fatal(ERR_IMT);
	}

	Debug(DEBUG_NEVER, (int32_t) hdr.code, "msg code");
	if (Args::getInstance()->logSseMessages()) {
		LogWarning(ERR_NE, hdr.activityId, "msg to SSE, code %d, len %d",
			hdr.code, len);
	}

	// marshall the associated data if necessary
	if (len) {
		if (!data)
			Fatal(ERR_IDL);
		switch (hdr.code) {
		case SEND_INTRINSICS:
			(static_cast<DxIntrinsics *> (data))->marshall();
			break;
		case SEND_DX_STATUS:
			(static_cast<DxStatus *> (data))->marshall();
			break;
		case DX_TUNED:
			(static_cast<DxTuned *> (data))->marshall();
			break;
		case BASELINE_INIT_ACCUM_STARTED:
		case DATA_COLLECTION_STARTED:
			break;
		case SEND_BASELINE:
			marshallBaseline(data);
			break;
		case SEND_BASELINE_STATISTICS:
			(static_cast<BaselineStatistics *> (data))->marshall();
			break;
		case BASELINE_WARNING_LIMITS_EXCEEDED:
		case BASELINE_ERROR_LIMITS_EXCEEDED:
			(static_cast<BaselineLimitsExceededDetails *> (data))->marshall();
			break;
		case SEND_COMPLEX_AMPLITUDES:
			marshallComplexAmplitudes(data);
			break;
		case BASELINE_INIT_ACCUM_COMPLETE:
		case DATA_COLLECTION_COMPLETE:
			break;
		case BEGIN_SENDING_CANDIDATES:
			(static_cast<Count *> (data))->marshall();
			break;
		case SEND_CANDIDATE_CW_POWER_SIGNAL:
			(static_cast<CwPowerSignal *> (data))->marshall();
			break;
		case SEND_CANDIDATE_PULSE_SIGNAL:
#ifdef notdef
			(static_cast<PulseSignal *> (data))->marshall();
#else
			marshallPulseSignal(data);
#endif
			break;
		case DONE_SENDING_CANDIDATES:
			break;
		case BEGIN_SENDING_SIGNALS:
			(static_cast<DetectionStatistics *> (data))->marshall();
			break;
		case SEND_CW_POWER_SIGNAL:
			(static_cast<CwPowerSignal *> (data))->marshall();
			break;
		case SEND_PULSE_SIGNAL:
#ifdef notdef
			(static_cast<PulseSignal *> (data))->marshall();
#else
			marshallPulseSignal(data);
#endif
			break;
		case DONE_SENDING_SIGNALS:
			break;
		case SIGNAL_DETECTION_STARTED:
			break;
		case BEGIN_SENDING_CANDIDATE_RESULTS:
			(static_cast<Count *> (data))->marshall();
			break;
		case SEND_PULSE_CANDIDATE_RESULT:
#ifdef notdef
			(static_cast<PulseSignal *> (data))->marshall();
#else
			marshallPulseSignal(data);
#endif
			break;
		case BEGIN_SENDING_BAD_BANDS:
			(static_cast<Count *> (data))->marshall();
			break;
		case SEND_PULSE_BAD_BAND:
			(static_cast<PulseBadBand *> (data))->marshall();
			break;
		case SEND_CW_BAD_BAND:
			(static_cast<CwBadBand *> (data))->marshall();
			break;
		case DONE_SENDING_BAD_BANDS:
			break;
		case BEGIN_SENDING_CW_COHERENT_SIGNALS:
			(static_cast<Count *> (data))->marshall();
			break;
		case SEND_CW_COHERENT_SIGNAL:
			(static_cast<CwCoherentSignal *> (data))->marshall();
			break;
		case SEND_CW_COHERENT_CANDIDATE_RESULT:
			CwCoherentSignal *seg;
			seg = static_cast<CwCoherentSignal *> (data);
			(static_cast<CwCoherentSignal *> (data))->marshall();
			break;
		case DONE_SENDING_CW_COHERENT_SIGNALS:
			break;
		case DONE_SENDING_CANDIDATE_RESULTS:
			break;
#ifdef notdef
		case BEGIN_SENDING_ARCHIVE_COMPLEX_AMPLITUDES:
			break;
		case SEND_ARCHIVE_COMPLEX_AMPLITUDES:
			(static_cast<ArchiveDataHeader *> (data))->marshall();
			break;
		case DONE_SENDING_ARCHIVE_COMPLEX_AMPLITUDES:
			break;
#endif
		case ARCHIVE_COMPLETE:
			break;
		case SIGNAL_DETECTION_COMPLETE:
			break;
		case SEND_DX_MESSAGE:
			(static_cast<NssMessage *> (data))->marshall();
			break;
		case DX_ACTIVITY_COMPLETE:
			(static_cast<DxActivityStatus *> (data))->marshall();
			break;
		default:
			ErrStr(hdr.code, "msg code");
			Fatal(ERR_IMT);
		}
	}
	// marshall the header before sending the message
	hdr.marshall();
	// lock the connection to ensure that the entire message
	// (header and data) are sent contiguously
	sse->lockSend();
	Error err;
	if (!(err = sse->send((void *) &hdr, sizeof(hdr))) && len)
		err = sse->send(data, len);
	sse->unlockSend();
}

//
// marshallPulseSignal: marshall a variable number of pulses
//
void
SseOutputTask::marshallBaseline(void *data)
{
	BaselineHeader *hdr = static_cast<BaselineHeader *> (data);
	BaselineValue *array = reinterpret_cast<BaselineValue *> (hdr + 1);
	for (int32_t i = 0; i < hdr->numberOfSubchannels; ++i)
		array[i].marshall();
	hdr->marshall();
}

//
// marshallComplexAmplitudes
//
void
SseOutputTask::marshallComplexAmplitudes(void *data)
{
	ComplexAmplitudeHeader *hdr = static_cast<ComplexAmplitudeHeader *> (data);
	SubchannelCoef1KHz *coeff = reinterpret_cast<SubchannelCoef1KHz *> (hdr + 1);
	for (int32_t i = 0; i < hdr->numberOfSubchannels; ++i)
		coeff[i].marshall();
	hdr->marshall();
}

//
// marshallPulseSignal: marshall a variable number of pulses
//
// Notes:
//		The header has not yet been marshalled for output.
void
SseOutputTask::marshallPulseSignal(void *data)
{
	PulseSignalHeader *hdr = static_cast<PulseSignalHeader *> (data);
	Pulse *array = reinterpret_cast<Pulse *> (hdr + 1);
	for (int32_t i = 0; i < hdr->train.numberOfPulses; ++i) {
		array[i].marshall();
	}
	hdr->marshall();
}

}
