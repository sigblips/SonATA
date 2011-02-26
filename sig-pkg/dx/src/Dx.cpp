/*******************************************************************************

 File:    Dx.cpp
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
// DX main task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/Dx.cpp,v 1.5 2009/05/24 22:47:08 kes Exp $
//

#include "Dx.h"
#include "DxErrMsg.h"
#include "Lock.h"
#include "State.h"

namespace dx {

Dx *Dx::instance = 0;

Dx *
Dx::getInstance()
{
	Lock l;
	l.lock();
	if (!instance)
		instance = new Dx();
	l.unlock();
	return (instance);
}

Dx::Dx(): sse(0), cmdTask(0), controlTask(0), sseConnectionTask(0),
		sseInputTask(0), sseOutputTask(0)
{
}

void
Dx::run(int argc, char **argv)
{
	// parse and record the command line arguments
	Args *args = Args::getInstance(argc, argv);
	Assert(args);
//	Assert(!args->parse(argc, argv));

//	SetDebugMask(DEBUG_PD_INPUT);
//	SetDebugMask(DEBUG_DSP_INPUT);
//	SetDebugMask(DEBUG_INPUT);
//	SetDebugMask(DEBUG_CONTROL);
//	SetDebugMask(DEBUG_ARCHIVE);
//	SetDebugMask(DEBUG_CONFIRM);
//	SetDebugMask(DEBUG_SIGNAL);
//	SetDebugMask(DEBUG_CWD);
//	SetDebugMask(DEBUG_PD);
//	SetDebugMask(DEBUG_DADD);
//	SetDebugMask(DEBUG_LOCK);
//	SetDebugMask(DEBUG_DETECT);
//	SetDebugMask(DEBUG_CWD_CONFIRM);
//	SetDebugMask(DEBUG_COLLECT);
//	SetDebugMask(DEBUG_BASELINE);
//	SetDebugMask(DEBUG_BIRDIE_MASK);
//	SetDebugMask(DEBUG_FREQ_MASK);
//	SetDebugMask(DEBUG_SIGNAL_CLASS);
//	SetDebugMask(DEBUG_CMD);
//	SetDebugMask(DEBUG_QTASK);
//	SetDebugMask(DEBUG_DSP_CMD);
//	SetDebugMask(DEBUG_MSGLIST);
//	SetDebugMask(DEBUG_STATE);
//	SetDebugMask(DEBUG_SUBCHANNEL);
//	SetDebugMask(DEBUG_PASS_THRU);
//	SetDebugMask(DEBUG_SPECTROMETRY);

	//	Debug(DEBUG_ALWAYS, 0, "do init");

	// initialize the system, creating data and tasks
	init();

	Timer timer;
	for (uint64_t i = 0; ; ++i)
		timer.sleep(100);
	return;
}

void
Dx::init()
{
#if DX_TIMING
	// determine the tick value
	Timer ct;
	uint64_t t0 = getticks();
	ct.sleep(10000);
	uint64_t t1 = getticks();
	float64_t dt = elapsed(t1, t0);
	std::cout << 10.0 / dt << " s per tick" << std::endl;
#endif
	// initialize the error list
	buildErrList();

	// allocate block memory
	createPartitions();
	MsgList *msgList = MsgList::getInstance("MsgList", DEFAULT_MSGS);
	Assert(msgList);

	State *state = State::getInstance();
	Assert(state);

	createConnections();
	createTasks();
	startTasks();
}

/**
 * Create the partition set list.
 *
 * Fixed-length partitions are used to minimize the time required to
 * allocate blocks of memory.
 */
void
Dx::createPartitions()
{
	PartitionSet *partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	Partition *partition = new Partition(PART1_SIZE, PART1_BLKS);
	Assert(partition);
	partitionSet->addPartition(partition);
	partition = new Partition(PART2_SIZE, PART2_BLKS);
	Assert(partition);
	partitionSet->addPartition(partition);
	partition = new Partition(PART3_SIZE, PART3_BLKS);
	Assert(partition);
	partitionSet->addPartition(partition);
	partition = new Partition(PART4_SIZE, PART4_BLKS);
	Assert(partition);
	partitionSet->addPartition(partition);
#ifdef notdef
	// add a partition size for the maximum pulse train if no partition
	// is large enough
	size_t size = sizeof(Msg) + sizeof(PulseSignalHeader)
			+ MAX_TRAIN_PULSES * sizeof(::Pulse);
	if (size > PART4_SIZE) {
		partition = new Partition(size, PART4_BLKS);
		Assert(partition);
		partitionSet->addPartition(partition);
	}
#endif
}

/**
 * Create connections.
 */
void
Dx::createConnections()
{
	sse = new Tcp("Sse", (sonata_lib::Unit) UnitSse, ActiveTcpConnection);
	Assert(sse);
}

/**
 * Create tasks.
 *
 * Note:\n
 * 	Only top-level tasks are created here; in general, tasks are created
 * 	by the parent tasks which control them.  This is to minimize task
 * 	knowledge.
 */
void
Dx::createTasks()
{
	// create the SSE communication tasks
	sseConnectionTask = SseConnectionTask::getInstance();
	Assert(sseConnectionTask);
	sseInputTask = SseInputTask::getInstance();
	Assert(sseInputTask);
	sseOutputTask = SseOutputTask::getInstance();
	Assert(sseOutputTask);

	// create the main processing tasks
	cmdTask = CmdTask::getInstance();
	Assert(cmdTask);
	controlTask = ControlTask::getInstance();
	Assert(controlTask);

	// create the data log
	Log *log = Log::getInstance(SEVERITY_WARNING, SEND_DX_MESSAGE, sse);
	Assert(log);
}

/**
 * Start the tasks.
 *
 * Notes:\n
 * 	Builds the argument structures.
 */
void
Dx::startTasks()
{
	sseConnectionArgs = SseConnectionArgs(sse);
	sseConnectionTask->start(&sseConnectionArgs);

	sseInputArgs = SseInputArgs(sse, sseConnectionTask->getInputQueue(),
			cmdTask->getInputQueue());
	sseInputTask->start(&sseInputArgs);

	sseOutputArgs = SseOutputArgs(sse);
	sseOutputTask->start(&sseOutputArgs);

	cmdArgs = CmdArgs(sseOutputTask->getInputQueue(),
			controlTask->getInputQueue());
	cmdTask->start(&cmdArgs);

	controlArgs = ControlArgs(sseOutputTask->getInputQueue());
	controlTask->start(&controlArgs);
}

}