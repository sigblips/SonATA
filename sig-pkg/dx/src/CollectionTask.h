/*******************************************************************************

 File:    CollectionTask.h
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
// Data collection task
//
// This task controls data collection for an observation; it starts the
// DSPs and all the data input tasks which receive data from the DSP
// boards.
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CollectionTask.h,v 1.4 2009/02/22 04:43:10 kes Exp $
//
#ifndef _CollectionTaskH
#define _CollectionTaskH

#include <bitset>
#include <map>
#include <set>
#include "System.h"
#include "Args.h"
#include "Buffer.h"
#include "DxStruct.h"
#include "InputTask.h"
#include "Msg.h"
#include "Partition.h"
#include "ReceiverTask.h"
#include "State.h"
#include "Task.h"
#include "WorkerTask.h"

using namespace sonata_lib;

namespace dx {

// start/stop mask bits
enum {
	RECEIVER_INPUT_BIT,
	WORKER_INPUT_BIT,
	NBITS_IN_COLLECTION_MASK
};

typedef std::bitset<NBITS_IN_COLLECTION_MASK> CollectionMask;
//typedef std::set<Udp *> UdpSet;

//
// startup arguments
//
struct CollectionArgs {
	Queue *controlQ;				// control task queue

	CollectionArgs(): controlQ(0) {}
	CollectionArgs(Queue *controlQ_): controlQ(controlQ_) {}
};

#ifdef notdef
struct ReaderTask {
	EtherReaderTask *task;
	EtherReaderArgs args;
};

struct WorkerTask {
	EtherWorkerTask *task;
	EtherReaderArgs args;
};

typedef std::map<EtherReaderTask *, EtherReaderArgs> ReaderMap;
typedef std::map<WorkerTask *, WorkerArgs> WorkerMap;
#endif

/**
 * Handle data collection.
 */
class CollectionTask: public QTask {
public:
	static CollectionTask *getInstance();
	~CollectionTask();

private:
	static CollectionTask *instance;

	bool started;
	int32_t activityId;					// activity id
	Activity *activity;					// current activity
	Channel *channel;					// channel
	CollectionMask tuneMask;
//	CollectionMask startMask;			// mask to register collection started
	CollectionMask stopMask;			// mask to register collection stopped
	InputArgs inputArgs;				// input task arguments
	InputTask *input;					// input (packet processor) task
	ReceiverArgs receiverArgs;			// receiver task arguments
	ReceiverTask *receiver;				// receiver (packet input) task
	WorkerArgs workerArgs;				// worker task arguments
	WorkerTask *worker;					// worker task

	Queue *controlQ;					// ptr to control task queue
	Queue *inQ;							// ptr to input task queue

	Args *cmdArgs;
	MsgList *msgList;
	PartitionSet *partitionSet;
	State *state;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);

	void tune(Msg *msg);
	void startActivity(Msg *msg);
	void stopActivity(Msg *msg, bool stopAll = false);
	void setBaselineStarted(Msg *msg);
	void setBaselineComplete(Msg *msg);
	void setCollectionStarted(Msg *msg);
	void setCollectionStopped(Msg *msg);
	void sendActivityStopped(Msg *msg);

	void setStartObs();
	void createReceiver();
	void initWorker();
	void startReceiver();
	void stopReceiver();
void startWorker(Activity *act);
	void stopWorker(Activity *act);

	// hidden
	CollectionTask(string name_);

	// forbidden
	CollectionTask(const CollectionTask&);
	CollectionTask& operator=(const CollectionTask&);
};

}

#endif