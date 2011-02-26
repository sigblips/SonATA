/*******************************************************************************

 File:    ArchiveTask.h
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
// Archive task
//
// This task connects to the archive server and sends archive
// data to it for candidates which have been confirmed at both
// the main and remote sites.
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiveTask.h,v 1.4 2009/02/22 04:48:36 kes Exp $
//
#ifndef _ArchiveTaskH
#define _ArchiveTaskH

#include <map>
#include "ArchiverCmdTask.h"
#include "ArchiverConnectionTask.h"
#include "ArchiverInputTask.h"
#include "ArchiverOutputTask.h"
#include "ConfirmationChannel.h"
#include "Msg.h"
#include "Partition.h"
#include "Queue.h"
#include "QTask.h"
#include "Signal.h"
#include "State.h"
#include "DxStruct.h"

using namespace sonata_lib;

namespace dx {

// startup structure
struct ArchiveArgs {
	Queue *controlQ;			// ptr to control task queue

	ArchiveArgs(): controlQ(0) {}
	ArchiveArgs(Queue *controlQ_): controlQ(controlQ_) {}
};

typedef map<int32_t, ArchiveRequest> ArchiveList;

class ArchiveTask: public QTask {
public:
	static ArchiveTask *getInstance();
	~ArchiveTask();

private:
	static ArchiveTask *instance;

	bool allCandidatesReceived;
	ArchiveList archiveList;
	ArchiveList discardList;
	Activity *activity;
	Channel *channel;
	Queue *controlQ;
	Queue *archiverOutQ;

	MsgList *msgList;
	PartitionSet *partitionSet;
	State *state;

	// dependent task info
	ArchiverCmdTask *archiverCmdTask;
	ArchiverConnectionTask *archiverConnectionTask;
	ArchiverInputTask *archiverInputTask;
	ArchiverOutputTask *archiverOutputTask;

	ArchiverCmdArgs archiverCmdArgs;
	ArchiverConnectionArgs archiverConnectionArgs;
	ArchiverInputArgs archiverInputArgs;
	ArchiverOutputArgs archiverOutputArgs;

	virtual void extractArgs();
	void createTasks();
	void startTasks();
	virtual void handleMsg(Msg *msg_);

	void startActivity(Msg *msg);
	void stopActivity(Msg *msg);
	void resolveCandidate(Msg *msg);
	void processConfirmationComplete(Msg *msg);
	void requestArchive(Msg *msg, ArchiveRequest& archiveRequest);
	void requestArchive(Msg *msg);
//	void retrieveCdData(Activity *act, Signal *candidate);
	void discardArchive(Msg *msg, ArchiveRequest& discardRequest);
	void discardArchive(Msg *msg);

	void sendArchiveComplete(Msg *msg);
	void sendActivityStopped(Msg *msg);

	void sendArchive(Signal *signal);
	void sendArchiveHeader(Signal *signal);
	void sendArchiveData(Signal *signal);
	void sendArchiveDone();

	void sendHalfFrame(Polarization pol, int32_t subchannel, int32_t hf);
//	void extractSubchannelHalfFrame(int32_t halfFrame, int32_t subchannel,
//			ComplexPair *cdData, SubchannelCoef1KHz *data);

	// hidden
	ArchiveTask(string tname_);

	// forbidden
	ArchiveTask(const ArchiveTask&);
	ArchiveTask& operator=(const ArchiveTask&);
};

}

#endif