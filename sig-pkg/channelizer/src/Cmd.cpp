/*******************************************************************************

 File:    Cmd.cpp
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
// Command processor task
//
#include <math.h>
#include <sseInterface.h>
#include <sseChannelizerInterface.h>
#include "Cmd.h"

using namespace ssechan;

namespace chan {

CmdTask *CmdTask::instance = 0;

CmdTask *
CmdTask::getInstance(string tname_)
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new CmdTask(tname_);
	l.unlock();
	return (instance);
}

CmdTask::CmdTask(string tname_): QTask(tname_, CMD_PRIO),
		beam(0), msgList(0), partitionSet(0), state(0)
{
}

CmdTask::~CmdTask()
{
}

void
CmdTask::extractArgs()
{
	CmdArgs *cmdArgs = static_cast<CmdArgs *> (args);
	Assert(cmdArgs);
	respQ = cmdArgs->respQ;
	Assert(respQ);

	beam = Beam::getInstance();
	Assert(beam);
	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	state = State::getInstance();
	Assert(state);
	transmitter = TransmitterTask::getInstance();
	Assert(transmitter);
}

void
CmdTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	case ssechan::REQUEST_INTRINSICS:
		sendIntrinsics(msg);
		break;
	case ssechan::REQUEST_STATUS:
		sendStatus(msg);
		break;
	case ssechan::START:
		startChannelizer(msg);
		break;
	case ssechan::STOP:
		stopChannelizer(msg);
		break;
	case ssechan::SHUTDOWN:
		shutdown(msg);
		break;
	default:
		FatalStr((int32_t) msg->getCode(), "msg code");
		LogFatal(ERR_IMT, msg->getActivityId(), "code %d", msg->getCode());
		break;
	}
}

void
CmdTask::sendIntrinsics(Msg *msg)
{
	MemBlk *blk = partitionSet->alloc(sizeof(Intrinsics));
	Assert(blk);
	Intrinsics *intrinsics = static_cast<Intrinsics *> (blk->getData());
	*intrinsics = state->getIntrinsics();
	Msg *rMsg = msgList->alloc((DxMessageCode) ssechan::SEND_INTRINSICS, -1,
			intrinsics, sizeof(Intrinsics), blk);
	respQ->send(rMsg);
}

void
CmdTask::sendStatus(Msg *msg)
{
	MemBlk *blk = partitionSet->alloc(sizeof(Status));
	Assert(blk);
	Status *status = static_cast<Status *> (blk->getData());
	*status = state->getStatus();

	Msg *rMsg = msgList->alloc((DxMessageCode) ssechan::SEND_STATUS, -1,
			status, sizeof(Status), blk);
	respQ->send(rMsg);
}

/**
 * Start the channelizer.
 */
void
CmdTask::startChannelizer(Msg *msg)
{
	const SseInterfaceHeader& hdr = msg->getHeader();
	Assert(hdr.dataLength == sizeof(Start));

	// stop the channelizer if necessary
	stopChannelizer(msg);

	const Start *start = static_cast<Start *> (msg->getData());
	beam->setFreq(start->centerSkyFreqMhz);
	beam->setStartTime(start->startTime);
	beam->setState(STATE_PENDING);
}

void
CmdTask::stopChannelizer(Msg *msg)
{
	beam->stop();
	transmitter->restart();
}

void
CmdTask::shutdown(Msg *msg)
{
	stopChannelizer(msg);
	::exit(0);
}

}