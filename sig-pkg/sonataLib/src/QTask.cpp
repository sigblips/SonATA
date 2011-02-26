/*******************************************************************************

 File:    QTask.cpp
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
// Queue task class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/QTask.cpp,v 1.4 2009/05/24 23:30:07 kes Exp $
//

#include <signal.h>
#include "QTask.h"
#include "Err.h"
#include "Semaphore.h"
#include "Types.h"

namespace sonata_lib {

//
// base class for all DX tasks which accept input via a queue
//
QTask::QTask(string tname_, int prio_, bool realtime_, bool detach_):
		Task(tname_, prio_, realtime_, detach_), terminated(false)
{
	inQ = new Queue("inQ" + tname_);
	Assert(inQ);
	msgList = MsgList::getInstance();
	Assert(msgList);
}

QTask::~QTask()
{
}

//
// routine: generic queue input handler
void *
QTask::routine()
{
	// extract the startup parameters
	extractArgs();

	// process incoming messages
	while (!terminated) {
#if QTASK_TIMING
		uint64_t t0 = getticks();
#endif
		Msg *msg;
		Error err = inQ->recv(reinterpret_cast<void **> (&msg));
		if (err)
			Fatal(err);
#if QTASK_TIMING
		int32_t rdCnt = inQ->getCount();
		if (rdCnt > timing.maxRdCnt)
			timing.maxRdCnt = rdCnt;
		uint64_t t1 = getticks();
#endif
		// process the message
		handleMsg(msg);
#if QTASK_TIMING
		uint64_t t2 = getticks();
#endif
		// then free it
		if (!msgList->free(msg)) {
			Debug(DEBUG_QTASK, 0, getName());
			Fatal(0);
		}
#if QTASK_TIMING
		if (!timing.first) {
			uint64_t t3 = getticks();
			++timing.msgs;
			timing.recv += elapsed(t1, t0);
			timing.handle += elapsed(t2, t1);
			timing.free += elapsed(t3, t2);
			timing.total += elapsed(t3, t0);
		}
		timing.first = false;
#endif

	}
	kill();
	return (0);
}

}
