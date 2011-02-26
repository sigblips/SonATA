/*******************************************************************************

 File:    SseConnection.cpp
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
// Task to initiate connection with SSE
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SseConnectionTask.cpp,v 1.4 2009/05/24 23:03:29 kes Exp $
//
#include <errno.h>
#include <unistd.h>
#include "ChErr.h"
#include "Log.h"
#include "SseConnection.h"
#include "SseInput.h"
#include "SseOutput.h"
#include "Timer.h"

namespace chan {

static string defaultHost = DEFAULT_SSE_ADDR;

SseConnectionTask *SseConnectionTask::instance = 0;

SseConnectionTask *
SseConnectionTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new SseConnectionTask("SseConnectionTask");
	l.unlock();
	return (instance);
}

SseConnectionTask::SseConnectionTask(string name_):
		QTask(name_, SSE_CONNECTION_PRIO), port(-1),
		retrySleep(SSE_RETRY_SLEEP_TIME), sse(0), cmdArgs(0), msgList(0)
{
}

SseConnectionTask::~SseConnectionTask()
{
}

//
// Notes:
//		Waits forever for messages on the input queue.  There are
//		only a few message types: (1) initiate connection with SSE;
//		(2) terminate
//

void
SseConnectionTask::extractArgs()
{
	// extract startup parameters
	SseConnectionArgs *connectionArgs = static_cast<SseConnectionArgs *> (args);
	Assert(connectionArgs);
	sse = static_cast<Tcp *> (connectionArgs->sse);

	cmdArgs = Args::getInstance();
	Assert(cmdArgs);
	port = cmdArgs->getSsePort();
	strcpy(address, cmdArgs->getSseAddr());

	msgList = MsgList::getInstance();
	Assert(msgList);
}

void
SseConnectionTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	case InitiateConnection:
	{
		Error err = contact();
		if (err)
			Fatal(err);
	}
		break;
	default:
		break;
	}
}

//
// Notes:
//		Waits for the SSE to respond to a HereIAm broadcast message
//
Error
SseConnectionTask::contact()
{
	while (1) {
		sse->setAddress(address, port);
		// try to establish the connection; if we got a broadcast
		// response we should be able to connect; if we're trying to
		// connect to a specific host, retry if there's no response
		Error err = sse->establish();
		if (err) {
			// no connection; wait a while
			Timer timer;
			timer.sleep(retrySleep);
			continue;
		}
		// successfully opened the connection
		return (0);
	}
}

}
