/*******************************************************************************

 File:    SseConnection.h
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SseConnectionTask.h,v 1.3 2009/02/22 04:48:38 kes Exp $
//
#ifndef _SseConnectiontTaskH
#define _SseConnectionTaskH

#include <sseChannelizerInterface.h>
#include "System.h"
#include "Args.h"
#include "Msg.h"
#include "QTask.h"
#include "Tcp.h"
#include "Udp.h"

using namespace ssechan;
using namespace sonata_lib;

namespace chan {

// SSE connection task startup arguments
struct SseConnectionArgs {
	Connection *sse;					// ptr to sse connection

	SseConnectionArgs(): sse(0) {}
	SseConnectionArgs(Connection *sse_): sse(sse_) {}
};

class SseConnectionTask: public QTask {
public:
	static SseConnectionTask *getInstance();
	~SseConnectionTask();

protected:
	void extractArgs();
	void handleMsg(Msg *msg);

private:
	static SseConnectionTask *instance;

	int32_t port;
//	int32_t responseSleep;
	int32_t retrySleep;
	IpAddress address;
	Tcp *sse;

	Args *cmdArgs;
	MsgList *msgList;

	Error contact();

	// hidden
	SseConnectionTask(string tname_);

	// forbidden
	SseConnectionTask(const SseConnectionTask&);
	SseConnectionTask& operator=(const SseConnectionTask&);
};

}

#endif