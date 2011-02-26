/*******************************************************************************

 File:    LogTask.cpp
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
// Logging task
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/LogTask.cpp,v 1.2 2008/02/25 22:35:55 kes Exp $
//
#include <sstream>
#include <sseInterface.h>
#include "Sonata.h"
#include "Err.h"
#include "LogTask.h"
#include "Types.h"

using std::ostringstream;
using std::endl;

namespace sonata_lib {

LogTask::LogTask(string tname_,
		Connection *connection_): QTask(tname_, LOG_PRIO),
		connection(connection_)
{
	msgList = MsgList::getInstance();
	Assert(msgList);
#ifdef notdef
	connection->establish();
#endif
}

LogTask::~LogTask()
{
#ifdef notdef
	connection->terminate();
#endif
}

//
// routine: handles messages from the input queue and outputs them
//		to the connection
//
// Notes:
//		The args structure is used to set up the msgList and outQ
//		values.
//		Ideally, it would not be necessary to know what the target
//		is, but we need to know since the data format will be
//		different for a character output device (such as the
//		console) than for a packet device (such as a TCP
//		connection to the SSE).
//
void
LogTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	default:
		sendMsg(msg);
		break;
	}
}

//
// sendMsg: send the error message to the destination
//
void
LogTask::sendMsg(Msg *msg)
{
	uint32_t len;
	NssMessage *errMsg;
	ostringstream str;
	SseInterfaceHeader hdr;

	// this had better be an error message 
	if ((len = msg->getDataLength()) != sizeof(NssMessage))
		Fatal(ERR_IDL);
	hdr = msg->getHeader();
	errMsg = static_cast<NssMessage *> (msg->getData());
	switch (connection->type()) {
	case DisplayConnection:
	case SerialConnection:
	case FileConnection:
		str << "(" << errMsg->code << ":" << errMsg->severity << ") "
				<< errMsg->description << endl;
		connection->send((void *) (str.str().c_str()),
				str.str().length());
		break;
	case ActiveTcpConnection:
	case PassiveTcpConnection:
		// throw away messages if no connection
		if (!connection->isConnected())
			break;
	case UdpConnection:
	default:
		errMsg->marshall();
		hdr.marshall();
		connection->lockSend();
		connection->send((void *) &hdr, sizeof(hdr));
		connection->send((void *) errMsg, len);
		connection->unlockSend();
		break;
	}
}

}