/*******************************************************************************

 File:    Queue.cpp
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
// Queue class methods
//
//
// Queues are local FIFO communication mechanisms which allow efficient
// communications between tasks.  While a queue is nominally a circular
// list of (void *), this class allows specification and allocation of
// a set of structures to be used by the list.
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Queue.cpp,v 1.4 2009/05/24 23:30:30 kes Exp $
//
#include "Err.h"
#include "Queue.h"

namespace sonata_lib {

Queue::Queue(string name_, int slots_) : data(0), qname(name_), slots(slots_),
		sIndex(0), rIndex(0),
		qLock("lQ" + name_), rSem("rQ" + name_, 0), sSem("sQ" + name_, slots_)
{
	// allocate the circular queue
	data = new void *[slots_];
	if (!data)
		Fatal(ERR_MAF);
}

Queue::~Queue()
{
    delete [] data;
}

Error
Queue::send(void *msg_, int seconds_)
{
#if QUEUE_TIMING
	uint64_t t0 = getticks();
#endif
	Error err = sSem.wait(seconds_);
	if (err)
		return (err);
#if QUEUE_TIMING
	uint64_t t1 = getticks();
#endif
	qLock.lock();
#if QUEUE_TIMING
	uint64_t t2 = getticks();
#endif
	data[sIndex++] = msg_;
	sIndex %= slots;
#if QUEUE_TIMING
	uint64_t t3 = getticks();
#endif
	qLock.unlock();
#if QUEUE_TIMING
	uint64_t t4 = getticks();
#endif
	rSem.signal();
#if QUEUE_TIMING
	uint64_t t5 = getticks();
	uint64_t t6 = getticks();
	++timing.send.msgs;
	timing.send.wait += elapsed(t1, t0);
	timing.send.lock += elapsed(t2, t1);
	timing.send.data += elapsed(t3, t2);
	timing.send.unlock += elapsed(t4, t3);
	timing.send.signal += elapsed(t5, t4);
	timing.send.total += elapsed(t5, t0);
	timing.send.ticks += elapsed(t6, t5);
#endif
	return (err);
}

Error
Queue::recv(void **msg_, int seconds_)
{
	*msg_ = NULL;

#if QUEUE_TIMING
	uint64_t t0 = getticks();
#endif
	Error err = rSem.wait(seconds_);
	if (err)
		return (err);
#if QUEUE_TIMING
	uint64_t t1 = getticks();
#endif
	qLock.lock();
#if QUEUE_TIMING
	uint64_t t2 = getticks();
#endif
	*msg_ = data[rIndex++];
	rIndex %= slots;
#if QUEUE_TIMING
	uint64_t t3 = getticks();
#endif
	qLock.unlock();
#if QUEUE_TIMING
	uint64_t t4 = getticks();
#endif
	sSem.signal();
#if QUEUE_TIMING
	uint64_t t5 = getticks();
	uint64_t t6 = getticks();
	++timing.recv.msgs;
	timing.recv.wait += elapsed(t1, t0);
	timing.recv.lock += elapsed(t2, t1);
	timing.recv.data += elapsed(t3, t2);
	timing.recv.unlock += elapsed(t4, t3);
	timing.recv.signal += elapsed(t5, t4);
	timing.recv.total += elapsed(t5, t0);
	timing.recv.ticks += elapsed(t6, t5);
#endif
	return (err);
}

void
Queue::name(string& name_)
{
	name_ = qname;
}

}
