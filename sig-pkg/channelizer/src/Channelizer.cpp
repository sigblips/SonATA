/*******************************************************************************

 File:    Channelizer.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/channelizer/src/Channelizer.cpp,v 1.17 2009/05/24 22:23:54 kes Exp $
//

#include "Channelizer.h"
#include "ChannelPacketVector.h"
#include "ChErrMsg.h"
#include "Lock.h"
#include "Log.h"

namespace chan {

Channelizer *Channelizer::instance = 0;

Channelizer *
Channelizer::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new Channelizer();
	l.unlock();
	return (instance);
}

Channelizer::Channelizer(): sse(0), args(0), beam(0), partitionSet(0),
		cmd(0), input(0), statistics(0), receiver(0),
		sseConnection(0), sseInput(0), sseOutput(0), transmitter(0), worker(0)
{
}

void
Channelizer::run(int argc, char **argv)
{
	args = Args::getInstance();
	Assert(args);

	Assert(!args->parse(argc, argv));

	// initialize the system, creating tasks
	init();

	Timer timer;
	for (uint64_t i = 0; ; ++i)
		timer.sleep(100);
	return;
}

void
Channelizer::init()
{
	// initialize the error list
	buildErrList();

#ifdef notdef
	ErrMsg *errMsg = ErrMsg::getInstance();
	string s = errMsg->getErrMsg((ErrCode) ERR_CCC);
	std::cout << s << std::endl;
	s = errMsg->getErrMsg(ERR_DLK);
	std::cout << s << std::endl;
	s = errMsg->getErrMsg(ERR_LNI);
	std::cout << s << std::endl;
#endif

#if CHANNELIZER_TIMING
	// determine the tick value
	Timer ct;
	uint64_t t0 = getticks();
	ct.sleep(10000);
	uint64_t t1 = getticks();
	float64_t dt = elapsed(t1, t0);
	std::cout << 10.0 / dt << " s per tick" << std::endl;
#endif

	// allocate block memory
	createPartitions();

	// create the message list
	MsgList *msgList = MsgList::getInstance("MsgList", CHANNELIZER_MESSAGES);
	Assert(msgList);

	beam = Beam::getInstance();
	Assert(beam);
	beam->setup();

	ChannelPacketVectorList *vectorList =
			ChannelPacketVectorList::getInstance(DEFAULT_VECTORS,
			args->getUsableChannels());
	Assert(vectorList);

	createConnections();
	createTasks();						// create tasks
	startTasks();						// start tasks
}

#ifdef notdef
void
Channelizer::createTasks()
{
	// start the transmitter (packet output) task
	transmitter->start();

	// create and start the worker tasks
	for (int32_t i = 0; i < args->getWorkers(); ++i) {
		std::string s = "Worker";
		s += i;
		WorkerTask *worker = new WorkerTask(s);
		Assert(worker);
		worker->start((void *) i);
	}

	// create
	SseConnectionTask *sseConnectionTask = SseConnectionTask::getInstance();
	Assert(sseConnectionTask);
	SseConnectionArgs sseConnectionArgs(sse);

	SseInputTask *sseInputTask = SseInputTask::getInstance();
	Assert(sseInputTask);
	SseInputArgs sseInputArgs(sse, sseConnectionTask->getInputQueue());

	SseOutputTask *sseOutputTask = SseOutputTask::getInstance();
	Assert(sseOutputTask);
	CmdTask *cmdTask =

	sseConnectionTask->start(&sseConnectionArgs);
}
#endif

void
Channelizer::createPartitions()
{
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	Partition *partition = new Partition(PART1_SIZE, PART1_BLKS);
	partitionSet->addPartition(partition);
	partition = new Partition(PART2_SIZE, PART2_BLKS);
	partitionSet->addPartition(partition);
	partition = new Partition(PART3_SIZE, PART3_BLKS);
	partitionSet->addPartition(partition);
}

void
Channelizer::createConnections()
{
	sse = new Tcp("Sse", (sonata_lib::Unit) UnitSse, ActiveTcpConnection);
	Assert(sse);
}

void
Channelizer::createTasks()
{
	// create the statistics task
	statistics = StatisticsTask::getInstance();
	Assert(statistics);

	// create the SSE tasks
	sseConnection = SseConnectionTask::getInstance();
	Assert(sseConnection);
	sseInput = SseInputTask::getInstance();
	Assert(sseInput);
	sseOutput = SseOutputTask::getInstance();
	Assert(sseOutput);
	cmd = CmdTask::getInstance();
	Assert(cmd);

	// create the input packet receiver task
	receiver = ReceiverTask::getInstance();
	Assert(receiver);

	// create the input packet processing task
	input = new InputTask("Input");
	Assert(input);

	// create and start the worker tasks
	worker = new WorkerTask *[args->getWorkers()];
	for (int32_t i = 0; i < args->getWorkers(); ++i) {
		std::string s = "Worker";
		s += i;
		worker[i] = new WorkerTask(s);
		Assert(worker[i]);
//		worker->start((void *) i);
	}

	// create the transmitter task
	transmitter = TransmitterTask::getInstance();
	Assert(transmitter);

}

/**
 * Start the system tasks.
 *
 * Description:\n
 * 	Start the system tasks in an order which guarantees that no
 * 	producer will be started before its consumer.
 */
void
Channelizer::startTasks()
{
	// start the transmitter
	transmitter->start();

	// start the workers
	for (int32_t i = 0; i < args->getWorkers(); ++i)
		worker[i]->start((void *) i);

	// start the input packet processor
	inputArgs = InputArgs(sseOutput->getInputQueue());
	input->start(&inputArgs);

	// sleep a little to allow the worker tasks to set themselves up; they
	// are lower priority than the receiver task
	Timer t;
	t.sleep(RECEIVER_DELAY);

	// start the packet receiver
	receiver->start();

	// start the command processor
	cmdArgs = CmdArgs(sseOutput->getInputQueue());
	cmd->start(&cmdArgs);

	// start the SSE tasks
	sseOutputArgs = SseOutputArgs(sse);
	sseOutput->start(&sseOutputArgs);

	// if we're running in local mode, don't start the SSE input task
	if (!args->noSse()) {
		sseInputArgs = SseInputArgs(sse, sseConnection->getInputQueue(),
				cmd->getInputQueue());
		sseInput->start(&sseInputArgs);
	}

	sseConnectionArgs = SseConnectionArgs(sse);
	sseConnection->start(&sseConnectionArgs);

	// start the statistics task
	statistics->start();

	// start the logger
	Log *log = Log::getInstance(SEVERITY_WARNING,
			(DxMessageCode) SEND_MESSAGE, sse);
	Assert(log);
}

}