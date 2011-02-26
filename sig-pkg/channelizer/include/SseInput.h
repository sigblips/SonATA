/*******************************************************************************

 File:    SseInput.h
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
// SSE->Channelizer input handler task
//
#ifndef _SseInputTaskH
#define _SseInputTaskH

#include <unistd.h>
#include <sseInterface.h>
#include <sseChannelizerInterface.h>
#include "Args.h"
#include "Cmd.h"
#include "Msg.h"
#include "Partition.h"
#include "Queue.h"
#include "Task.h"
#include "Tcp.h"

using namespace sonata_lib;

namespace chan {

// Channelizer/SSE input task arguments
struct SseInputArgs {
	Connection *sse;
	Queue *connectionQ;
	Queue *cmdQ;

	SseInputArgs(): sse(0), connectionQ(0), cmdQ(0) {}
	SseInputArgs(Connection *sse_, Queue *connectionQ_, Queue *cmdQ_):
			sse(sse_), connectionQ(connectionQ_), cmdQ(cmdQ_) {}
};

//
// This task receives input from the SSE and sends it on to the
// command processor via a queue.  If communication is lost (i.e.,
// there is no connection to the SSE), then a request is sent to
// the broadcast task to re-establish the connection.
//
// Notes:
//		This task does not have an input queue - all input comes from
//		the SSE
//
class SseInputTask: public Task {
public:
	static SseInputTask *getInstance(string tname_ = "");
	~SseInputTask();

protected:
	void extractArgs();
	void *routine();

private:
	static SseInputTask *instance;

	Connection *sse;
	Queue *connectionQ;
	Queue *cmdQ;

	Args *cmdArgs;
	MsgList *msgList;
	PartitionSet *partitionSet;

	void requestConnection();

	// hidden
	SseInputTask(string tname_);
	// forbidden
	SseInputTask(const SseInputTask&);
	SseInputTask& operator=(const SseInputTask&);
};

}

#endif