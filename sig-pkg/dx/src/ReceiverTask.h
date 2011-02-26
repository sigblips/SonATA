/*******************************************************************************

 File:    ReceiverTask.h
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
// Task to handle input of ethernet data packets
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ReceiverTask.h,v 1.4 2009/02/22 04:48:37 kes Exp $
//
#ifndef _ReceiverTaskH
#define _ReceiverTaskH

#include <sseDxInterface.h>
#include "System.h"
#include "Activity.h"
#include "ChannelPacketList.h"
#include "Msg.h"
#include "Queue.h"
#include "State.h"
#include "Task.h"
#include "Udp.h"

using namespace sonata_lib;

namespace dx {

const int32_t RESTART_TIME = 5;		// time before start time to re-add group
const int32_t DELAY_MOD = 24;		// modulus for reducing Dx id to integer
const int32_t DELAY_FACTOR_MS = 100; // # of msec between consecutive Dx's
const int32_t MULTICAST_ADDS = 1;	// (HACK!) # of group adds to do
const int32_t DELAY_ADD_MS = 10;	// # of msec to delay between group adds

struct ReceiverArgs {
	dx::Unit unit;					// unit
	Activity *activity;				// activity
	string rAddr;					// address of right polarization
	int32_t rPort;					// port of right polarization
	string lAddr;					// address of left polarization
	int32_t lPort;					// port of left polarization
	Queue *inQ;						// input task queue

	ReceiverArgs(): unit(dx::UnitNone), activity(0), rPort(-1), lPort(-1),
			inQ(0) {}
	ReceiverArgs(dx::Unit unit_, Activity *activity_, string rAddr_,
			int32_t rPort_, string lAddr_, int32_t lPort_, Queue *inQ_):
			unit(unit_), activity(activity_), rAddr(rAddr_), rPort(rPort_),
			lAddr(lAddr_), lPort(lPort_), inQ(inQ_) {}
};

// forward declarations
class ReceiverTask;
class RcvrRestartTask;
class RcvrTimeoutTask;

struct RcvrRestartArgs {
	int32_t activityId;				// activity id
	Lock *lock;						// mutex lock
	NssDate restart;				// restart time
	Udp *udp;						// receiver task socket
	IpAddress addr;					// multicast group address

	RcvrRestartArgs(): activityId(-1), lock(0), udp(0) {}
	RcvrRestartArgs(int32_t i, const NssDate& r, Udp *u,
			const IpAddress& a, Lock *l): activityId(i), lock(l), udp(u) {
		restart = r;
		strcpy(addr, a);
	}
};

struct RcvrTimeoutArgs {
	Lock *lock;						// mutex lock
	NssDate start;					// start time
	ReceiverTask *receiver;			// parent receiver task
	Udp *udp;						// receiver task socket

	RcvrTimeoutArgs(): lock(0), receiver(0), udp(0) {}
	RcvrTimeoutArgs(ReceiverTask *r, const NssDate& s, Udp *u, Lock *l):
		lock(l), receiver(r), udp(u) {
		start = s;
	}
};

class ReceiverTask: public Task {
public:
	ReceiverTask(string name_);
	~ReceiverTask();

private:
	dx::Unit unit;
	int32_t rPort, lPort;			// right, left port numbers of data stream
	IpAddress rAddr, lAddr;			// right, left IP addresses
	Activity *activity;				// ptr to current activity
	ChannelPacketList *pktList;		// free packet list
	Lock lock;						// mutex lock
	Queue *inQ;						// input task queue
	Udp *udp;						// UDP multicast socket
	RcvrRestartArgs restartArgs;	// args to restart task
	RcvrRestartTask *restart;		// receiver restart task
	RcvrTimeoutArgs timeoutArgs;	// args to timeout task
	RcvrTimeoutTask *timeout;		// receiver timeout task

	MsgList *msgList;
	State *state;

	// methods
	void extractArgs();
	void *routine();
	void startTimeout();
	void stopTimeout();

	// forbidden
	ReceiverTask(const ReceiverTask&);
	ReceiverTask& operator=(const ReceiverTask&);
};

/**
 * Receiver restart task.
 *
 * Description:\n
 * 	Utility task to retry the multicast group add if no
 * 	packets are received by halfway to the start time.  This task
 * 	will try one time to add the group, then terminate itself.
 */
class RcvrRestartTask: public Task {
public:
	RcvrRestartTask(string name_, int prio_);
	~RcvrRestartTask();

private:
	int32_t activityId;
	IpAddress addr;
	Lock *lock;
	NssDate restartTime;
	Udp *udp;

	// methods
	void extractArgs();
	void *routine();
};

/**
 * Receiver timeout task.
 *
 * Description:\n
 * 	Utility task to terminate the receiver task if packets are not
 * 	received before the scheduled start of baseline accumulation.  Sets
 * 	a timer; when the timer expires, checks to see whether the receiver
 * 	task has killed the timeout task.  If so, it terminates; if not,
 * 	it closes the udp receiver socket and sends a kill message to the
 * 	receiver task.
 */
class RcvrTimeoutTask: public Task {
public:
	RcvrTimeoutTask(string name_, int prio_);
	~RcvrTimeoutTask();

private:
	Lock *lock;
	NssDate startTime;
	ReceiverTask *receiver;
	Udp *udp;

	// methods
	void extractArgs();
	void *routine();
};

}

#endif