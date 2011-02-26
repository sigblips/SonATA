/*******************************************************************************

 File:    Queue.h
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
// Queue class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Queue.h,v 1.3 2008/11/18 23:20:30 kes Exp $
//
#ifndef _QueueH
#define _QueueH

#include <string>

#include "Sonata.h"
#include "Lock.h"
#include "Semaphore.h"
#include "Types.h"

namespace sonata_lib {

struct QT {
	uint64_t msgs;
	float wait;
	float lock;
	float data;
	float unlock;
	float signal;
	float total;
	float ticks;

	QT(): msgs(0), wait(0), lock(0), data(0), unlock(0), signal(0), total(0),
			ticks(0) {}
};

struct QTiming {
	QT send;
	QT recv;
};

class Queue {
public:
	Queue(string name_, int slots_ = DEFAULT_QSLOTS);
	~Queue();

	// send and receive queue messages
	Error send(void *msg_, int milliseconds_ = -1);
	Error recv(void **msg_, int milliseconds_ = -1);

	void name(string& name_);

	int32_t getCount() { return (rSem.getCount()); }

private:
	void **data;
	string qname;
	int slots, sIndex, rIndex;
	Lock qLock;
	Semaphore rSem;
	Semaphore sSem;
	QTiming timing;

	// forbidden
	Queue(const Queue&);
	Queue& operator=(const Queue&);	
};

}

#endif