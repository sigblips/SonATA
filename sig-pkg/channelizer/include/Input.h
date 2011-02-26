/*******************************************************************************

 File:    Input.h
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
// Input task
//
// Input task which performs input packet handling.  Several tasks may
// be created to handle the full input rate.
//
// $Heaer: $
//
#ifndef _InputTaskH
#define _InputTaskH

#include "System.h"
#include "Beam.h"
#include "ChErr.h"
#include "ChTypes.h"
#include "InputQ.h"
#include "Lock.h"
#include "Msg.h"
#include "Partition.h"
#include "QTask.h"

using namespace sonata_lib;

namespace chan {

struct InputArgs {
	Queue *respQ;						// sse output queue

	InputArgs(): respQ(0) {}
	InputArgs(Queue *respQ_): respQ(respQ_) {}
};

struct InputTiming {
	uint64_t packets;
	float handlePacket;

	InputTiming(): packets(0), handlePacket(0) {}
};

class InputTask: public QTask {
public:
	InputTask(string name_);
	~InputTask();

private:
	chan::Unit unit;
	Queue *inputQ;
	Queue *respQ;
	InputTiming timing;

	Beam *beam;
	BeamPacketList *beamPktList;
	MsgList *msgList;
	PartitionSet *partitionSet;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	void sendStart();
};

}

#endif