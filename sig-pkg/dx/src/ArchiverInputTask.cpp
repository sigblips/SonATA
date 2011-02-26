/*******************************************************************************

 File:    ArchiverInputTask.cpp
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
// Archiver input handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverInputTask.cpp,v 1.5 2009/05/24 22:43:34 kes Exp $
//

#include <ssePorts.h>
#include "ArchiverInputTask.h"
#include "Struct.h"
#include "Timer.h"
#include <fstream>

namespace dx {

ArchiverInputTask *ArchiverInputTask::instance = 0;

ArchiverInputTask *
ArchiverInputTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ArchiverInputTask("ArchiverInput");
	l.unlock();
	return (instance);
}

ArchiverInputTask::ArchiverInputTask(string tname_):
		Task(tname_, ARCHIVER_INPUT_PRIO), archiver(0), cmdQ(0), connectionQ(0),
		msgList(0), partitionSet(0)
{
}

ArchiverInputTask::~ArchiverInputTask()
{
}

void
ArchiverInputTask::extractArgs()
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

	// extract the input args
	ArchiverInputArgs *inputArgs
			= static_cast<ArchiverInputArgs *> (args);
	Assert(inputArgs);
	archiver = inputArgs->archiver;
	Assert(archiver);
	cmdQ = inputArgs->cmdQ;
	Assert(cmdQ);
	connectionQ = inputArgs->connectionQ;
	Assert(connectionQ);

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
ArchiverInputTask::routine()
{
	extractArgs();

	// run forever, waiting for messages from the archiver
	Timer timer;
	while (1) {
		// if there's no connection, just wait for it to be
		// reestablished by a CONFIGURE_DX message
		if (!archiver->isConnected()) {
			requestConnection();
			timer.sleep(3000);
			continue;
		}

		// got a connection - wait for data to come in
		SseInterfaceHeader hdr;
		if (Error err = archiver->recv((void *) &hdr, sizeof(hdr))) {
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
		msg->setUnit((sonata_lib::Unit) UnitArchiver);

		// if there's data associated with the message,
		// allocate space and retrieve it, demarshall it
		// based on the message type,
		// then send it on to the command processor
		size_t len = hdr.dataLength;
		if (len) {
			MemBlk *blk = partitionSet->alloc(len);
			Assert(blk);
			if (!blk)
				Fatal(ERR_MAF);
			void *data = blk->getData();
			if (Error err = archiver->recv(data, len)) {
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
			// demarshall the data of the message depending on
			// the message type
			switch (hdr.code) {
			case REQUEST_INTRINSICS:
				break;
			case SEND_DX_MESSAGE:
				(static_cast<NssMessage *> (data))->demarshall();
				break;
			default:
				LogError(ERR_IMT, hdr.activityId, "activity %d, type %d",
						hdr.activityId, hdr.code);
				Err(ERR_IMT);
				ErrStr(hdr.code, "msg code");
				Assert(msgList->free(msg));
				continue;
			}
		}

		// at this point, the entire marshalled message is in
		// a generic Msg; send the message on for processing,
		// then go back to waiting
		cmdQ->send(msg);
	}
}

void
ArchiverInputTask::requestConnection()
{
	Msg *msg = msgList->alloc((DxMessageCode) InitiateConnection);
	if (connectionQ->send(msg))
		Fatal(ERR_SE);
}

}

