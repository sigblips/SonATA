/*******************************************************************************

 File:    ArchiverInputTask.h
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
// Archiver input handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverInputTask.h,v 1.4 2009/02/22 04:48:36 kes Exp $
//
#ifndef _ArchiverInputTaskH
#define _ArchiverInputTaskH

#include <unistd.h>
#include <sseDxInterface.h>
#include <sseInterface.h>
#include "System.h"
#include "Log.h"
#include "Msg.h"
#include "Partition.h"
#include "Queue.h"
#include "Task.h"
#include "Tcp.h"

using namespace sonata_lib;

namespace dx {

// DX/SSE input task arguments
struct ArchiverInputArgs {
	Connection *archiver;			// ptr to connection
	Queue *cmdQ;					// ptr to command queue
	Queue *connectionQ;

	ArchiverInputArgs(): archiver(0), cmdQ(0), connectionQ(0) {}
	ArchiverInputArgs(Connection *archiver_, Queue *cmdQ_,
			Queue *connectionQ_): archiver(archiver_), cmdQ(cmdQ_),
			connectionQ(connectionQ_) {}
};

//
// This task receives input from the archiver and sends it on to the
// command processor via a queue.
// Notes:
//		This task does not use its input queue - all input comes from
//		the SSE
//
class ArchiverInputTask: public Task {
public:
	static ArchiverInputTask *getInstance();

	~ArchiverInputTask();

protected:
	void extractArgs();
	void *routine();

private:
	static ArchiverInputTask *instance;

	Connection *archiver;
	Queue *cmdQ;
	Queue *connectionQ;

	MsgList *msgList;
	PartitionSet *partitionSet;

	void requestConnection();

	// hidden
	ArchiverInputTask(string tname_);

	// forbidden
	ArchiverInputTask(const ArchiverInputTask&);
	ArchiverInputTask& operator=(const ArchiverInputTask&);
};

}

#endif