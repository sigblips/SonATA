/*******************************************************************************

 File:    Channelizer.h
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
// Channelizer main task
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/include/Channelizer.h,v 1.6 2009/03/04 21:41:25 kes Exp $
//
#ifndef CHANNELIZER_H_
#define CHANNELIZER_H_

#include "Args.h"
#include "Beam.h"
#include "ChVersion.h"
#include "Cmd.h"
#include "Input.h"
#include "Statistics.h"
#include "Partition.h"
#include "Receiver.h"
#include "SseConnection.h"
#include "SseInput.h"
#include "SseOutput.h"
#include "Timer.h"
#include "Transmitter.h"
#include "Worker.h"

namespace chan {

class Channelizer {
public:
	static Channelizer *getInstance();
	~Channelizer();

	float32_t getVersion() { return (CHVERSION); }
	void run(int argc, char **argv);

private:
	static Channelizer *instance;

	Connection *sse;

	Args *args;							// command-line arguments
	Beam *beam;
	PartitionSet *partitionSet;

	CmdTask *cmd;
	InputTask *input;
	StatisticsTask *statistics;
	ReceiverTask *receiver;
	SseConnectionTask *sseConnection;
	SseInputTask *sseInput;
	SseOutputTask *sseOutput;
	TransmitterTask *transmitter;
	WorkerTask **worker;

	CmdArgs cmdArgs;
	InputArgs inputArgs;
	SseConnectionArgs sseConnectionArgs;
	SseInputArgs sseInputArgs;
	SseOutputArgs sseOutputArgs;


	void init();						// perform system initialization
	void createPartitions();			// allocate partition space
	void createConnections();			// create the set of connections
	void createTasks();					// create the system tasks
	void startTasks();					// start the system tasks

	// hidden
	Channelizer();

	// forbidden
	Channelizer& operator=(const Channelizer&);
	Channelizer(const Channelizer&);
};

}

#endif /*CHANNELIZER_H_*/