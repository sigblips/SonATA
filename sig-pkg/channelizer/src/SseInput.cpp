/*******************************************************************************

 File:    SseInput.cpp
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
// SSE->Channelizer input handler task
//

#include <fstream>
#include <ssePorts.h>
#include "ChErr.h"
#include "Log.h"
#include "SseInput.h"
#include "SseOutput.h"
#include "Timer.h"

namespace chan {

SseInputTask *SseInputTask::instance = 0;

SseInputTask *
SseInputTask::getInstance(string tname_)
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new SseInputTask(tname_);
	l.unlock();
	return (instance);
}

SseInputTask::SseInputTask(string tname_): Task(tname_, SSE_INPUT_PRIO),
		sse(0), connectionQ(0), cmdQ(0), cmdArgs(0), msgList(0), partitionSet(0)

{
}

SseInputTask::~SseInputTask()
{
}

/**
 * Perform initialization, processing any arguments which are passed
 * to the task.  Executed when the task begins to run.
 */
void
SseInputTask::extractArgs()
{
	// extract the input args
	SseInputArgs *inputArgs = static_cast<SseInputArgs *> (args);
	Assert(inputArgs);
	sse = inputArgs->sse;
	Assert(sse);
	connectionQ = inputArgs->connectionQ;
	Assert(connectionQ);
	cmdQ = inputArgs->cmdQ;
	Assert(cmdQ);

	cmdArgs = Args::getInstance();
	Assert(cmdArgs);
	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
}

//
// routine: receive and send on incoming messages
//
// Notes:
//		No analysis of the message is performed - that is left
//		to the command processor task.  Instead, the local copy
//		of the header
//		is demarshalled to determined whether or not there
//		is associated data; if there is, space is allocated to
//		contain it.
void *
SseInputTask::routine()
{
	extractArgs();

	// run forever, waiting for messages from the SSE
	bool stopIssued = false;
	bool done = false;
	while (!done) {
		// if there's no connection, request that it be
		// established, then wait for that to happen
		if (!sse->isConnected()) {
			requestConnection();
			while (!sse->isConnected()) {
				Timer timer;
				timer.sleep(3000);
			}
		}
		stopIssued = false;

		// got a connection - wait for data to come in
		SseInterfaceHeader hdr;
		Error err = sse->recv((void *) &hdr, sizeof(hdr));
		if (err) {
			switch (err) {
			case EAGAIN:
			case EINTR:
			case ENOTCONN:
			case ECONNRESET:
				continue;
			default:
				Fatal(err);
				break;
			}
		}
		// demarshall the header
		hdr.demarshall();

		// allocate a message to hold the incoming message
		Msg *msg = msgList->alloc();
		msg->setHeader(hdr);
		msg->setUnit((sonata_lib::Unit) UnitSse);

		// if there's data associated with the message,
		// allocate space and retrieve it, demarshall it
		// based on the message type,
		// then send it on to the command processor
		void *data = 0;
		size_t len = hdr.dataLength;
		if (len) {
			MemBlk *blk = partitionSet->alloc(len);
			Assert(blk);
			if (!blk)
				Fatal(ERR_MAF);
			data = blk->getData();
			err = sse->recv(data, len);
			if (err) {
				switch (err) {
				case EAGAIN:
				case EINTR:
				case ENOTCONN:
				case ECONNRESET:
					blk->free();
					Assert(msgList->free(msg));
					continue;
				default:
					Fatal(err);
					break;
				}
			}
			msg->setData(data, len, blk);
		}

		// demarshall the data of the message depending on
		// the message type
		switch (hdr.code) {
		case ssechan::REQUEST_INTRINSICS:
			break;
		case ssechan::REQUEST_STATUS:
			break;
		case ssechan::START:
			(static_cast<Start *> (data))->demarshall();
			break;
		// the following commands arrive with no data
		case ssechan::STOP:
		case ssechan::SHUTDOWN:
			Debug(DEBUG_CONTROL, hdr.activityId,
					"STOP_CHANNELIZER, act");
			break;
		default:
			LogError(ERR_IMT, hdr.activityId, "activity %d, type %d",
					hdr.activityId, hdr.code);
			Err(ERR_IMT);
			ErrStr(hdr.code, "msg code");
			Assert(msgList->free(msg));
			continue;
		}

		// at this point, the entire marshalled message is in
		// a generic Msg; send the message on for processing,
		// then go back to waiting
		cmdQ->send(msg);
	}
	return (0);
}

//
// requestConnection: request that a connection be established to
//		the SSE
//
void
SseInputTask::requestConnection()
{
	Msg *msg = msgList->alloc((DxMessageCode) InitiateConnection);
	if (connectionQ->send(msg))
		Fatal(ERR_SE);
}

}