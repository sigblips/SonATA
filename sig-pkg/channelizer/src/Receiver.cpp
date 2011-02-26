/*******************************************************************************

 File:    Receiver.cpp
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
// Packet receiver task
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/src/Receiver.cpp,v 1.26 2009/02/13 03:02:29 kes Exp $

#include "Receiver.h"

namespace chan {

ReceiverTask *ReceiverTask::instance = 0;

ReceiverTask *
ReceiverTask::getInstance(string name_, int prio_)
{
	static Lock l;
	l.lock();
	Assert(!instance);
	if (!instance)
		instance = new ReceiverTask("Receiver", RECEIVER_PRIO);
	l.unlock();
	return (instance);
}

ReceiverTask *
ReceiverTask::getInstance()
{
	static Lock l;
	l.lock();
//	Assert(instance);
	if (!instance)
		instance = new ReceiverTask("Receiver", RECEIVER_PRIO);
	l.unlock();
	return (instance);
}

/**
 * Create the receiver task.
*/
ReceiverTask::ReceiverTask(string name_, int prio_): Task(name_, prio_),
		packets(0)
{
}

/**
 * Destroy the receiver task.
 *
 * Description:\n
 * 	Frees all resources before destroying the task.
*/
ReceiverTask::~ReceiverTask()
{
	delete buf;
}

void
ReceiverTask::extractArgs()
{
}

/**
 * Receive and process incoming packets.
 *
 * Description:\n
 * 	Processes packets from the Udp socket and passes them onto the
 *	beam object for processing.
 */
void *
ReceiverTask::routine()
{
	args = Args::getInstance();
	Assert(args);

#if ASSIGN_CPUS
	// assign the receiver to processor 0 in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	if (nCpus > 2) {
		// set the affinity to a cpu
		int32_t cpu = 0;
		if (args->getReceiverCpu() != -1)
			cpu = args->getReceiverCpu();
		cpu_set_t affinity;
		CPU_ZERO(&affinity);
		CPU_SET(cpu, &affinity);
		pid_t tid = gettid();
		int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
		Assert(rval >= 0);
	}
#endif

	beam = Beam::getInstance();
	Assert(beam);
	beamPktList = BeamPacketList::getInstance();
	Assert(beamPktList);
	inputQ = InputQ::getInstance();
	Assert(inputQ);
	msgList = MsgList::getInstance();
	Assert(msgList);

	udp = new Udp("Receiver", (sonata_lib::Unit) UnitReceive);
	Assert(udp);
	IpAddress any = "0.0.0.0";
	IpAddress addr;
	memcpy(addr, args->getInputAddr(), sizeof(addr));
	int32_t port = args->getInputPort();
	udp->setAddress(any, port, false, true);
	Assert(!udp->addGroup(addr));

	// set the buffer size
	size_t size = DEFAULT_RCV_BUFSIZE;
	udp->setRcvBufsize(size);

	// process incoming packets
	while (1) {
#if RECEIVER_TIMING
		uint64_t t0 = getticks();
#endif
		BeamPacket *pkt = static_cast<BeamPacket *> (beamPktList->alloc());
		Assert(pkt);
		Error err = udp->recv((void *) pkt->getPacket(), pkt->getPacketSize());
		if (err) {
			switch (err) {
			case EAGAIN:
			case EINTR:
				continue;
			default:
				Fatal(err);
				break;
			}
		}
		// got a packet; route it unless we're idle, in which case the
		// packet is freed without processing
		if (beam->getState() == STATE_IDLE) {
			beamPktList->free(pkt);
			continue;
		}
#if RECEIVER_TIMING
		uint64_t t1 = getticks();
#endif
		++packets;
		pkt->demarshall();
#if RECEIVER_TIMING
		uint64_t t2 = getticks();
#endif
		Msg *msg = msgList->alloc((DxMessageCode) InputPacket, 0, pkt,
				sizeof(BeamPacket), 0, USER);
#if RECEIVER_TIMING
		uint64_t t3 = getticks();
#endif
		Assert(!inputQ->send(msg, 0));

#if RECEIVER_TIMING
		uint64_t t4 = getticks();
		++timing.packets;
		timing.recv += elapsed(t1, t0);
		timing.demarshall += elapsed(t2, t1);
		timing.alloc += elapsed(t3, t2);
		timing.send += elapsed(t4, t3);
		timing.total += elapsed(t4, t0);
#endif
	}
}

}