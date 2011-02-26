/*******************************************************************************

 File:    SseOutput.cpp
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
// Channelizer->SSE output handler task
//
#include <stdio.h>
#include "Args.h"
#include "Err.h"
#include "Log.h"
#include "SseOutput.h"
#include "Timer.h"

namespace chan {

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
		 QTask(tname_, SSE_OUTPUT_PRIO), msgNumber(0)
{
}

SseOutputTask::~SseOutputTask()
{
}

void
SseOutputTask::extractArgs()
{
	// extract startup parameters
	SseOutputArgs *outputArgs = static_cast<SseOutputArgs *> (args);
	Assert(outputArgs);
	sse = outputArgs->sse;
	Assert(sse);

	cmdArgs = Args::getInstance();
	Assert(cmdArgs);
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
	// if we are running in local mode, ignore all messages to the SSE
	if (cmdArgs->noSse())
		return;

	msg->setMsgNumber(++msgNumber);
	int32_t len = msg->getDataLength();
	void *data = msg->getData();
	SseInterfaceHeader hdr = msg->getHeader();

	if (hdr.code >= CHANNELIZER_MSG_CODE_END) {
		Err(hdr.code);
		Err(msg->getUnit());
		Fatal(ERR_IMT);
	}

	Debug(DEBUG_NEVER, (int32_t) hdr.code, "msg code");

	// marshall the associated data if necessary
	if (len) {
		if (!data)
			Fatal(ERR_IDL);
		switch (hdr.code) {
		case ssechan::SEND_INTRINSICS:
			(static_cast<Intrinsics *> (data))->marshall();
			break;
		case ssechan::SEND_STATUS:
			(static_cast<Status *> (data))->marshall();
			break;
		case ssechan::STARTED:
			(static_cast<Started *> (data))->marshall();
			break;
		case SEND_MESSAGE:
			(static_cast<NssMessage *> (data))->marshall();
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

}