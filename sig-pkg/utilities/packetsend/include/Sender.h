/*******************************************************************************

 File:    Sender.h
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
// Sender task class
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/Sender.h,v 1.8 2008/09/16 20:31:17 kes Exp $
//
#ifndef SENDER_H_
#define SENDER_H_

#include <arpa/inet.h>
#include "System.h"
#include "Args.h"
#include "BeamPacketList.h"
#include "ChannelPacketList.h"
#include "PsStruct.h"
#include "Queue.h"
#include "Task.h"
#include "Udp.h"

using namespace sonata_lib;

namespace sonata_packetsend {

class SenderTask: public Task {
public:
	static SenderTask *getInstance(string name_, int prio_);
	static SenderTask *getInstance();
	~SenderTask();

	Queue *getQueue() { return (&inQ); }

private:
	static SenderTask *instance;

	bool reseq;							// reset the sequence numbers
	bool resetPol;						// reset the Polarization
	int32_t nchan;						// Number of channels to send
	int32_t burst;						// # of packets in a burst
	int32_t delay;						// delay between bursts
	uint32_t seq;						// sequence #
	uint32_t src;						// source
	ATADataPacketHeader::PolarizationCode code;
	HostSpec base;
	Queue inQ;
	Udp *connection;

	Args *args;
	BeamPacketList *pktList;

	// methods
	void extractArgs();
	void *routine();
	Error send(Udp *udp, ATAPacket *pkt, const IpAddress& ipAddr,
			int32_t port);
	void printStats(uint64_t n, const timespec& start, const timespec& end);

	// hidden
	SenderTask(string name_, int prio_);

	// forbidden
	SenderTask(const SenderTask&);
	SenderTask& operator=(const SenderTask&);
};

}

#endif /*SENDER_H_*/