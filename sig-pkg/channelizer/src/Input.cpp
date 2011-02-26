/*******************************************************************************

 File:    Input.cpp
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
// Input packet handler task
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/src/Input.cpp,v 1.10 2009/05/24 22:24:10 kes Exp $
//

#include "Input.h"
#include "Log.h"

namespace chan {

Queue *InputQ::instance = 0;

InputTask::InputTask(string name_): QTask(name_, INPUT_PRIO, true, false),
		unit(UnitChannelize), inputQ(0), respQ(0), beam(0), beamPktList(0),
		msgList(0), partitionSet(0)
{
}

InputTask::~InputTask()
{
}

void
InputTask::extractArgs()
{
#if ASSIGN_CPUS
	// if there are more than 3 processors, run on cpu 2
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	if (nCpus > 3) {
		cpu_set_t affinity;
		CPU_ZERO(&affinity);
		CPU_SET(1, &affinity);
		pid_t tid = gettid();
		int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
		Assert(rval >= 0);
	}
#endif

	InputArgs *inputArgs = static_cast<InputArgs *> (args);
	respQ = inputArgs->respQ;

	beam = Beam::getInstance();
	Assert(beam);
	beamPktList = BeamPacketList::getInstance();
	Assert(beamPktList);
	inputQ = InputQ::getInstance();
	Assert(inputQ);
	setInputQueue(inputQ);
	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
}

void
InputTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	case InputPacket:
		{
#if INPUT_TIMING
		uint64_t t0 = getticks();
#endif
		BeamPacket *pkt = static_cast<BeamPacket *> (msg->getData());
		bool startFlag = false;
		Error err = beam->handlePacket(pkt, startFlag);
		if (err == ERR_STAP)
			LogError(err, -1, "channelizer not synchronized");
		else if (err == ERR_IPV) {
			cout << endl << "!!!!!!! Wrong packet version, should be " <<
					ATADataPacketHeader::CURRENT_VERSION << " !!!!!!!!!" << endl
					<< "!! STOPPING CHANNELIZATION !!" << endl << endl;
			if (!Args::getInstance()->noSse()) {
				Timer timer;
				timer.sleep(4000);
				LogError(err, -1, "should be  %0x",
						ATADataPacketHeader::CURRENT_VERSION);
			}
		}
		if (startFlag)
			sendStart();
#if INPUT_TIMING
		uint64_t t1 = getticks();
		++timing.packets;
		timing.handlePacket += elapsed(t1, t0);
#endif
		}
		break;
	default:
		Fatal(ERR_IMT);
		break;
	}
}

/**
 * Notify the SSE that the channelizer has started
 */
void
InputTask::sendStart()
{
	MemBlk *blk = partitionSet->alloc(sizeof(Started));
	Assert(blk);
	Started *started = static_cast<Started *> (blk->getData());
	started->startTime = beam->getStartTime();

	Msg *msg = msgList->alloc((DxMessageCode) ssechan::STARTED, -1,
			started, sizeof(Started), blk);
	respQ->send(msg);
}

}