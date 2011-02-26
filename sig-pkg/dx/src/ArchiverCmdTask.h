/*******************************************************************************

 File:    ArchiverCmdTask.h
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
// Archiver command processor task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverCmdTask.h,v 1.4 2009/02/22 04:48:36 kes Exp $
//
#ifndef _ArchiverCmdTaskH
#define _ArchiverCmdTaskH

#include "Msg.h"
#include "Partition.h"
#include "State.h"

using namespace sonata_lib;

namespace dx {

struct ArchiverCmdArgs {
	Queue *archiverOutQ;

	ArchiverCmdArgs(): archiverOutQ(0) {}
	ArchiverCmdArgs(Queue *archiverOutQ_): archiverOutQ(archiverOutQ_) {}
};

class ArchiverCmdTask: public QTask {
public:
	static ArchiverCmdTask *getInstance(string tname_ = "");
	~ArchiverCmdTask();

protected:
	void extractArgs();
	void handleMsg(Msg *msg);

private:
	static ArchiverCmdTask *instance;

	Queue *archiverOutQ;

	MsgList *msgList;
	PartitionSet *partitionSet;
	State *state;

	void sendIntrinsics(Msg *msg);

	// hidden
	ArchiverCmdTask(string tname_);

	// forbidden
	ArchiverCmdTask(const ArchiverCmdTask&);
	ArchiverCmdTask& operator=(const ArchiverCmdTask&);
};

}

#endif