/*******************************************************************************

 File:    ArchiverOutputTask.cpp
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
// Archiver output handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverOutputTask.cpp,v 1.3 2009/02/13 03:06:29 kes Exp $
//
#include <stdio.h>
#include "System.h"
#include "DxErr.h"
#include "ArchiverOutputTask.h"
#include "Log.h"
#include "Timer.h"

namespace dx {

ArchiverOutputTask *ArchiverOutputTask::instance = 0;

ArchiverOutputTask *
ArchiverOutputTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ArchiverOutputTask("ArchiverOutput");
	l.unlock();
	return (instance);
}

ArchiverOutputTask::ArchiverOutputTask(string tname_):
		 QTask(tname_, ARCHIVER_OUTPUT_PRIO), msgNumber(0)
{
}

ArchiverOutputTask::~ArchiverOutputTask()
{
}

void
ArchiverOutputTask::extractArgs()
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
	ArchiverOutputArgs *outputArgs = static_cast<ArchiverOutputArgs *> (args);
	archiver = outputArgs->archiver;
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
ArchiverOutputTask::handleMsg(Msg *msg)
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

	// marshall the associated data if necessary
	if (len) {
		if (!data)
			Fatal(ERR_IDL);
		switch (hdr.code) {
		case SEND_INTRINSICS:
			(static_cast<DxIntrinsics *> (data))->marshall();
			break;
		case ARCHIVE_SIGNAL:
			(static_cast<ArchiveDataHeader *> (data))->marshall();
			break;
		case BEGIN_SENDING_ARCHIVE_COMPLEX_AMPLITUDES:
			(static_cast<Count *> (data))->marshall();
			break;
		case SEND_ARCHIVE_COMPLEX_AMPLITUDES:
			marshallComplexAmplitudes(msg);
			break;
		case DONE_SENDING_ARCHIVE_COMPLEX_AMPLITUDES:
			break;
		case SEND_DX_MESSAGE:
			(static_cast<NssMessage *> (data))->marshall();
			break;
		default:
			ErrStr(hdr.code, "bad archiver message type, code");
			Fatal(ERR_IMT);
		}
	}
	if (!archiver->isConnected()) {
		Debug(DEBUG_ARCHIVE, (int32_t) hdr.code, "code");
		LogWarning(ERR_NAC, hdr.activityId, "code = %d", hdr.code);
		return;
	}

	// marshall the header before sending the message
	hdr.marshall();
	// lock the connection to ensure that the entire message
	// (header and data) are sent contiguously
	archiver->lockSend();
	Error err;
	if (!(err = archiver->send((void *) &hdr, sizeof(hdr))) && len)
		err = archiver->send(data, len);
	archiver->unlockSend();
}

void
ArchiverOutputTask::marshallComplexAmplitudes(Msg *msg)
{
	// get the complex amplitude header and the first coefficient array
	ComplexAmplitudeHeader *hdr = static_cast<ComplexAmplitudeHeader *>
			(msg->getData());
	SubchannelCoef1KHz *coeff = reinterpret_cast<SubchannelCoef1KHz *> (hdr + 1);

	// marshall the data
	for (int32_t i = 0; i < hdr->numberOfSubchannels; i++)
		coeff[i].marshall();
	hdr->marshall();
}

}
