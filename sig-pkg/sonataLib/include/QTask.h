/*******************************************************************************

 File:    QTask.h
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
// Queue task
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/QTask.h,v 1.3 2008/11/18 23:19:43 kes Exp $
//
#ifndef _QTaskH
#define _QTaskH

#include "Sonata.h"
#include "Task.h"
#include "Queue.h"
#include "Msg.h"

namespace sonata_lib {

struct QTaskTiming {
	bool first;							// first time through?
	uint64_t msgs;						// number of message received
	uint64_t maxRdCnt;					// maximum # of messages waiting
	float recv;							// time waiting for messages
	float handle;						// time handling messages
	float free;							// time freeing messages
	float total;						// total time per loop

	QTaskTiming(): first(true), msgs(0), maxRdCnt(0), recv(0), handle(0),
			free(0), total(0) {}
};

//
// base class for system tasks which accept input via a queue; derived
// from Task
//
class QTask: public Task {
public:
	QTask(string name_, int prio_, bool realtime_ = true,
			bool detach_ = true);
	virtual ~QTask();

	Queue *getInputQueue() { return (inQ); }
	void setInputQueue(Queue *q) {
		delete inQ;
		inQ = q;
	}
	void terminate() { terminated = true; }

protected:
	MsgList *msgList;
	
private:
	bool terminated;
	Queue *inQ;
	QTaskTiming timing;

	virtual void *routine();
	virtual void extractArgs() = 0;
	virtual void handleMsg(Msg *msg_) = 0;

	// forbidden
	QTask(const QTask&);
	QTask& operator=(const QTask&);
};

}

#endif