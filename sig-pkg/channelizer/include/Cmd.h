/*******************************************************************************

 File:    Cmd.h
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
// Command processor task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CmdTask.h,v 1.4 2009/02/22 04:48:37 kes Exp $
//
#ifndef _CmdTaskH
#define _CmdTaskH

#include "Beam.h"
#include "Msg.h"
#include "Partition.h"
#include "QTask.h"
#include "SseOutput.h"
#include "State.h"
#include "Transmitter.h"

using namespace sonata_lib;

namespace chan {

struct CmdArgs {
	Queue *respQ;
//	Queue *controlQ;

	CmdArgs(): respQ(0) {}
	CmdArgs(Queue *respQ_): respQ(respQ_)
			{}
};

class CmdTask: public QTask {
public:
	static CmdTask *getInstance(string s = "");
	~CmdTask();

protected:
	void extractArgs();
	void handleMsg(Msg *msg);

private:
	static CmdTask *instance;

	Queue *respQ;

	Beam *beam;
	MsgList *msgList;
	PartitionSet *partitionSet;
	State *state;
	TransmitterTask *transmitter;

	void sendIntrinsics(Msg *msg);
	void sendStatus(Msg *msg);
	void startChannelizer(Msg *msg);
	void stopChannelizer(Msg *msg);
	void shutdown(Msg *msg);

	// hidden
	CmdTask(string tname_);

	// forbidden
	CmdTask(const CmdTask&);
	CmdTask& operator=(const CmdTask&);
};

}

#endif