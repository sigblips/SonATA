/*******************************************************************************

 File:    ReceiverTask.cpp
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
// Ethernet reader task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ReceiverTask.cpp,v 1.6 2009/05/24 23:00:30 kes Exp $
//
#include <arpa/inet.h>
#include <sys/time.h>
#include "Alarm.h"
#include "DxErr.h"
#include "Log.h"
#include "ReceiverTask.h"
#include "Timer.h"

namespace dx {

ReceiverTask::ReceiverTask(string name_):
		Task(name_, RECEIVER_PRIO, true, false), unit(UnitNone),
		rPort(-1), lPort(-1), activity(0),
		pktList(ChannelPacketList::getInstance()), udp(0), msgList(0), state(0)
{
	Assert(pktList);
}

ReceiverTask::~ReceiverTask()
{
}

void
ReceiverTask::extractArgs()
{
#if ASSIGN_CPUS
	// assign the receiver processor affinity in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	if (nCpus > 2) {
		cpu_set_t affinity;
		CPU_ZERO(&affinity);
		CPU_SET(0, &affinity);
		pid_t tid = gettid();
		int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
		Assert(rval >= 0);
	}
#endif

	ReceiverArgs *receiverArgs = static_cast<ReceiverArgs *> (args);
	Assert(receiverArgs);

	// extract arguments
	unit = receiverArgs->unit;
	activity = receiverArgs->activity;
	Assert(activity);
	int32_t chan = activity->getChannelNum();
	rPort = receiverArgs->rPort + chan;
	lPort = receiverArgs->lPort + chan;
	in_addr rIp, lIp;
	inet_aton(receiverArgs->rAddr.c_str(), &rIp);
	inet_aton(receiverArgs->lAddr.c_str(), &lIp);
	rIp.s_addr = htonl(ntohl(rIp.s_addr) + chan);
	lIp.s_addr = htonl(ntohl(lIp.s_addr) + chan);
	strcpy(rAddr, inet_ntoa(rIp));
	strcpy(lAddr, inet_ntoa(lIp));
	inQ = receiverArgs->inQ;
	Assert(inQ);

	msgList = MsgList::getInstance();
	Assert(msgList);
	state = State::getInstance();
	Assert(state);

}

/**
 * Receive and process incoming packets.
 *
 * Description:\n
 * 	Waits on the UDP socket for incoming packets, then processes them
 * 	as they are received.  Processing consists of adding the data
 * 	to the incoming channel data, synchronizing the two polarizations
 * 	and buffering the data.
 */
//
// routine: receive and process incoming packets
//
void *
ReceiverTask::routine()
{
	// get the arguments
	extractArgs();

	// open the input data stream for multicast
	udp = new Udp("Receiver", (sonata_lib::Unit) UnitReceiver);
	Assert(udp);

	// set the receive buffer size
	size_t size = DEFAULT_RCV_BUFSIZE;
	udp->setRcvBufsize(size);

	// set the multicast address
	IpAddress any = "0.0.0.0";
	udp->setAddress(any, rPort, false, true);

	Timer timer;
#if (DELAY_MULTICAST_ADD)
	// delay based on the Dx number before adding the multicast group
	int32_t id = state->getSerialNumber() % DELAY_MOD;
	int32_t delay = id * DELAY_FACTOR_MS;
	timer.sleep(delay);
#endif
	// possibly do multiple adds of the multicast group
	for (int32_t i = 0; i < MULTICAST_ADDS; ++i) {
		Assert(!udp->addGroup(rAddr));
		timer.sleep(DELAY_ADD_MS);
	}

	// notify the input task that data collection is starting
	Msg *msg = msgList->alloc((DxMessageCode) StartCollection,
			activity->getActivityId());
	Assert(!inQ->send(msg));

	// setup and start a timeout in case we don't get packets
	startTimeout();

	// process incoming packets
	bool done = false;
	bool first = true;
	while (!done) {
		ChannelPacket *pkt = pktList->alloc();
		Assert(pkt);
		Error err = udp->recv((void *) pkt->getPacket(), pkt->getPacketSize());
		if (first) {
			stopTimeout();
			first = false;
		}
		if (err) {
			switch (err) {
			case EAGAIN:
			case EINTR:
				continue;
			case EBADF:
			case ENOTSOCK:
			case ETIMEDOUT:
				// socket has been closed
				LogError(ERR_DCE, activity->getActivityId(), "");
				pktList->free(pkt);
				kill();
				break;
			default:
				Fatal(err);
				break;
			}
		}
		// see if we've been terminated
		lock.lock();
		if (!testCancel()) {
			// got a packet; route it
			pkt->demarshall();
			Msg *msg = msgList->alloc((DxMessageCode) InputPacket, 0,
					pkt, sizeof(ChannelPacket), 0, USER);
			Assert(!inQ->send(msg, 0));
		}
		else {
			udp->terminate();
			pktList->free(pkt);
			done = true;
		}
		lock.unlock();
	}
	delete udp;
	return (0);
}

/**
 * Start receiver restart and timeout tasks.
 */
void
ReceiverTask::startTimeout()
{
	// create the restart task
	restart = new RcvrRestartTask("RcvrRestart", RECEIVER_PRIO - 1);
	Assert(restart);

	// create the timeout task
	timeout = new RcvrTimeoutTask("RcvrTimeout", RECEIVER_PRIO - 1);
	Assert(timeout);

	NssDate startTime = activity->getStartTime();
	float64_t startD = startTime.tv_sec
			+ (float64_t) startTime.tv_usec / USEC_PER_SEC;
	timeval now;
	gettimeofday(&now, NULL);
	float64_t nowD = now.tv_sec + (float64_t) now.tv_usec / USEC_PER_SEC;
	float64_t restartD = (nowD + startD) / 2;
	float64_t rsInt, rsFrac;
	rsFrac = modf(restartD, &rsInt);
	NssDate restartTime;
	restartTime.tv_sec = (time_t) rsInt;
	restartTime.tv_usec = (time_t) (rsFrac * USEC_PER_SEC);

	restartArgs = RcvrRestartArgs(activity->getActivityId(), restartTime, udp,
			rAddr, &lock);
	restart->start((void *) &restartArgs);

	timeoutArgs = RcvrTimeoutArgs(this, startTime, udp, &lock);
	timeout->start((void *) &timeoutArgs);
}

/**
 * Stop the receiver timeout.
 */
void
ReceiverTask::stopTimeout()
{
	lock.lock();
	if (restart) {
		restart->kill();
		restart = 0;
	}
	if (timeout && !testCancel()) {
		timeout->kill();
		timeout = 0;
	}
	lock.unlock();
}

/**
 * Receiver restart task.
 *
 * Description:\n
 * 	Utility task to handle situation where receiver has not received
 * 	any packets several seconds before the baseline accumulation start
 * 	time.
 */
RcvrRestartTask::RcvrRestartTask(string name_, int prio_):
		Task(name_, prio_), activityId(-1), lock(0), udp(0)
{
}

RcvrRestartTask::~RcvrRestartTask()
{
}


void
RcvrRestartTask::extractArgs()
{
	RcvrRestartArgs *rcvrRestartArgs = static_cast<RcvrRestartArgs *> (args);
	Assert(rcvrRestartArgs);

	activityId = rcvrRestartArgs->activityId;
	lock = rcvrRestartArgs->lock;
	Assert(lock);
	restartTime = rcvrRestartArgs->restart;
	udp = rcvrRestartArgs->udp;
	Assert(udp);
	strcpy(addr, rcvrRestartArgs->addr);
}

void *
RcvrRestartTask::routine()
{
	extractArgs();

	Alarm timeout("RestartAlarm");
	timeout.alarm(restartTime);

	lock->lock();
	// time has expired; check to see whether receiver must be shut down
	if (!testCancel()) {
		Assert(!udp->addGroup(addr));
		LogWarning(ERR_NPRR, activityId, "");
	}
	lock->unlock();
	return (0);
}

/**
 * Receiver timeout task.
 *
 * Description:\n
 * 	Utility task to handle situation where receiver does not receive
 * 	packets from the channelizer.
 */
RcvrTimeoutTask::RcvrTimeoutTask(string name_, int prio_):
		Task(name_, prio_), receiver(0), udp(0)
{
}

RcvrTimeoutTask::~RcvrTimeoutTask()
{
}

void
RcvrTimeoutTask::extractArgs()
{
	RcvrTimeoutArgs *rcvrTimeoutArgs = static_cast<RcvrTimeoutArgs *> (args);
	Assert(rcvrTimeoutArgs);

	receiver = rcvrTimeoutArgs->receiver;
	Assert(receiver);
	startTime = rcvrTimeoutArgs->start;
	udp = rcvrTimeoutArgs->udp;
	Assert(udp);
	lock = rcvrTimeoutArgs->lock;
	Assert(lock);
}

void *
RcvrTimeoutTask::routine()
{
	extractArgs();

	Alarm timeout("RcvrAlarm");
	timeout.alarm(startTime);

	lock->lock();
	// time has expired; check to see whether receiver must be shut down
	if (!testCancel()) {
		receiver->kill();
		udp->terminate();
	}
	lock->unlock();
	return (0);
}

}