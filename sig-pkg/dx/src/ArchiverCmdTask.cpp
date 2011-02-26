/*******************************************************************************

 File:    ArchiverCmdTask.cpp
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
// Archiver command processor task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverCmdTask.cpp,v 1.4 2009/02/13 03:06:29 kes Exp $
//
#include <math.h>
#include "ArchiverCmdTask.h"
#include "Log.h"

namespace dx {

ArchiverCmdTask *ArchiverCmdTask::instance = 0;

ArchiverCmdTask *
ArchiverCmdTask::getInstance(string tname_)
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ArchiverCmdTask(tname_);
	l.unlock();
	return (instance);
}

ArchiverCmdTask::ArchiverCmdTask(string tname_):
		QTask(tname_, ARCHIVER_CMD_PRIO), archiverOutQ(0), msgList(0),
		partitionSet(0), state(0)
{
}

ArchiverCmdTask::~ArchiverCmdTask()
{
}

void
ArchiverCmdTask::extractArgs()
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

	// extract args
	ArchiverCmdArgs *cmdArgs = static_cast<ArchiverCmdArgs *> (args);
	Assert(cmdArgs);
	archiverOutQ = cmdArgs->archiverOutQ;

	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	state = State::getInstance();
	Assert(state);

}

void
ArchiverCmdTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	case REQUEST_INTRINSICS:
		sendIntrinsics(msg);
		break;
	default:
		LogFatal(ERR_IMT, msg->getActivityId(), "code %d", msg->getCode());
		Fatal(ERR_IMT);
		break;
	}
}

void
ArchiverCmdTask::sendIntrinsics(Msg *msg)
{
	MemBlk *blk = partitionSet->alloc(sizeof(DxIntrinsics));
	Assert(blk);
	DxIntrinsics *intrinsics = static_cast<DxIntrinsics *> (blk->getData());
	*intrinsics = state->getIntrinsics();
	Msg *rMsg = msgList->alloc((DxMessageCode) SEND_INTRINSICS, -1,
			intrinsics, sizeof(DxIntrinsics), blk);
	archiverOutQ->send(rMsg);
}

}