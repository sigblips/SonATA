/*******************************************************************************

 File:    Receiver.h
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

// Receiver task
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/include/Receiver.h,v 1.10 2009/02/13 03:02:28 kes Exp $
//
#ifndef RECEIVER_H_
#define RECEIVER_H_

#include "System.h"
#include "Args.h"
#include "Beam.h"
#include "BeamPacketList.h"
#include "ChTypes.h"
#include "ChErr.h"
#include "InputQ.h"
#include "Lock.h"
#include "Msg.h"
#include "Task.h"
#include "Udp.h"

using namespace sonata_lib;

namespace chan {

struct ReceiverTiming {
	uint64_t packets;
	float recv;
	float demarshall;
	float alloc;
	float send;
	float total;

	ReceiverTiming(): packets(0), recv(0), demarshall(0), alloc(0), send(0),
			total(0) {}
};

class ReceiverTask: public Task {
public:
	static ReceiverTask *getInstance(string name_, int prio_);
	static ReceiverTask *getInstance();
	~ReceiverTask();

	void setup();

private:
	static ReceiverTask *instance;

	int32_t packets;					// # of packets received
	InputBuffer *buf;					// input buffer
	Udp *udp;							// receiver connection
	ReceiverTiming timing;

	Args *args;							// command-line args
	Beam *beam;							// singleton beam
	BeamPacketList *beamPktList;		// beam packet list
	MsgList *msgList;
	Queue *inputQ;						// input queue for packets

	// methods
	void extractArgs();
	void *routine();

	// hidden
	ReceiverTask(string name_, int prio_);

	// forbidden
	ReceiverTask(const ReceiverTask&);
	ReceiverTask &operator=(const ReceiverTask&);
};

}
#endif /*RECEIVER_H_*/