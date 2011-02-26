/*******************************************************************************

 File:    InputTask.cpp
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
#include <arpa/inet.h>
#include <sys/time.h>
#include "DxErr.h"
#include "Log.h"
#include "InputTask.h"

namespace dx {

InputTask::InputTask(string name_):
		QTask(name_, INPUT_PRIO, true, false), unit(UnitNone),
		activity(0), channel(0), msgList(0), state(0)
{
}

InputTask::~InputTask()
{
}

void
InputTask::extractArgs()
{
#if ASSIGN_CPUS
	// assign the input processor affinity in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	if (nCpus > 3) {
		cpu_set_t affinity;
		CPU_ZERO(&affinity);
		CPU_SET(1, &affinity);
		// set affinity
		pid_t tid = gettid();
		int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
		Assert(rval >= 0);
	}
#endif

	InputArgs *inputArgs = static_cast<InputArgs *> (args);
	Assert(inputArgs);

	// extract arguments
	unit = inputArgs->unit;

	state = State::getInstance();
	Assert(state);

}

/**
 * Process incoming packets.
 *
 * Description:\n
 * 	Waits on the input queue for incoming packets, then processes them
 * 	as they are received.  Processing consists of adding the data
 * 	to the incoming channel data, synchronizing the two polarizations
 * 	and buffering the data.
 */
void
InputTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	case StartCollection:
		startActivity(msg);
		break;
	case InputPacket:
		processPacket(msg);
		break;
	default:
		Fatal(ERR_IMT);
		break;
	}
}

void
InputTask::startActivity(Msg *msg)
{
	if (!(activity = state->findActivity(msg->getActivityId()))) {
		LogError(ERR_NSA, msg->getActivityId(), "activity %d",
				msg->getActivityId());
		return;
	}
	channel = activity->getChannel();
	Assert(channel);
}

void
InputTask::processPacket(Msg *msg)
{
	ChannelPacket *pkt = static_cast<ChannelPacket *> (msg->getData());
	Assert(pkt);
	// got a packet; route it
	pkt->demarshall();
	Error err = channel->handlePacket(pkt);
	if (err)
		LogError(err, activity->getActivityId(), "");
}

}