/*******************************************************************************

 File:    Log.cpp
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
// System log class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Log.cpp,v 1.3 2009/05/24 23:28:27 kes Exp $
//
#include <stdio.h>
//#include "Args.h"
#include "Log.h"
#include "Msg.h"
//#include "PartitionSet.h"
#include "Util.h"

namespace sonata_lib {

Log *Log::instance = 0;

/**
 * Get the singleton instance.
 *
 * Notes:
 * 	For efficiency, locking is only done if we know that the instance
 * 	has not yet been created.
*/

Log *
Log::getInstance(NssMessageSeverity severity_, DxMessageCode msgCode_,
		Connection *connection_)
{
	static Lock l;
	if (!instance) {
		// first call must be the initialization call
		if (!connection_)
			Fatal(ERR_LNI);
		l.lock();
		if (!instance)
			instance = new Log("SseLog", severity_, msgCode_, connection_);
		l.unlock();
	}
	return (instance);
}

Log::Log(string name_, NssMessageSeverity severity_, DxMessageCode msgCode_,
		Connection *connection_):
		severity(severity_), llock("ll" + name_), msgCode(msgCode_),
		task("lt" + name_, connection_), connection(connection_),
		errList(0), msgList(0), partitionSet(0), logQ(0), lname(name_)
{
//	args = Args::getInstance();
//	Assert(args);
	errList = ErrMsg::getInstance();
	Assert(errList);
	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);

	logQ = task.getInputQueue();
	Assert(logQ);

	task.start();
}

Log::~Log()
{
}

void
Log::setSeverity(NssMessageSeverity severity_)
{
	lock();
	severity = severity_;
	unlock();
}

#ifdef notdef
void
Log::log(NssMessageSeverity severity_, DxMessageCode code_, ostringstream& str)
{
	NssMessage *errMsg;
	MemBlk *blk;
	Msg *msg;
	SseInterfaceHeader hdr;

	if (!isEnabled(severity_))
		return;

	// build the message
	blk = partitionSet->alloc(sizeof(NssMessage));
	errMsg = static_cast<NssMessage *> (blk->getData());
	errMsg->severity = severity_;
	errMsg->code = code_;
	strcpy(errMsg->description, str.str().c_str());

	// send the message
	msg = msgList->alloc(SEND_DX_MESSAGE, -1, errMsg, sizeof(NssMessage),
			blk);
	logQ->send(msg);
}
#endif

void
Log::log(NssMessageSeverity severity_, DxMessageCode code_, int activityId_,
		const char *file_, const char *func_, int32_t line_,
		const char *fmt_, ...)
{
	char str[MAX_NSS_MESSAGE_STRING];
	int32_t len;
	NssMessage *errMsg;
	MemBlk *blk;
	Msg *msg;
	SseInterfaceHeader hdr;
	va_list ap;

	if (!connection) {
		printf("%s(%d)[%s]: ", file_, line_, func_);
		va_start(ap, fmt_);
		vprintf(fmt_, ap);
		va_end(ap);
		printf("\n");
		return;
	}

	if (!isEnabled(severity_))
		return;

	// build the message
	blk = partitionSet->alloc(sizeof(NssMessage));
	errMsg = static_cast<NssMessage *> (blk->getData());
	errMsg->severity = severity_;
	errMsg->code = code_;
	memset(errMsg->description, 0, sizeof(errMsg->description));

	// standard info for message
	string& eMsg = (ErrMsg::getInstance())->getErrMsg((ErrCode) code_);
	sprintf(errMsg->description, "%s:%s:%d[%s](%d): ", eMsg.c_str(),
			file_, line_, func_, activityId_);

	va_start(ap, fmt_);
	vsprintf(str, fmt_, ap);
	va_end(ap);

	len = strlen(errMsg->description);
	strncat(errMsg->description, str, sizeof(errMsg->description) - (len + 1));

	// send the message
	msg = msgList->alloc(msgCode, activityId_, errMsg,
			sizeof(NssMessage), blk);
	logQ->send(msg);
}

void
Log::timeout(int32_t msec)
{
	static Timer timer;

	timer.sleep(300);
}

}