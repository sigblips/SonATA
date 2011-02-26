/*******************************************************************************

 File:    Statistics.cpp
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
// Channelizer statistics logging task
//
#include <iostream>
#include <sseChannelizerInterface.h>
#include "Statistics.h"

using std::cout;
using std::endl;
using namespace ssechan;

namespace chan {

StatisticsTask *StatisticsTask::instance = 0;

StatisticsTask *
StatisticsTask::getInstance()
{
	static Lock l;
	l.lock();
//	Assert(instance);
	if (!instance)
		instance = new StatisticsTask("Statistics", LOG_PRIO);
	l.unlock();
	return (instance);
}

/**
 * Create the receiver task.
*/
StatisticsTask::StatisticsTask(string name_, int prio_): Task(name_, prio_),
		sendStats(false), nChannels(0), connection(0), args(0), beam(0),
		partitionSet(0), transmitter(0)
{
}

/**
 * Destroy the receiver task.
 *
 * Description:\n
 * 	Frees all resources before destroying the task.
*/
StatisticsTask::~StatisticsTask()
{
}

void
StatisticsTask::extractArgs()
{
	args = Args::getInstance();
	Assert(args);
	beam = Beam::getInstance();
	Assert(beam);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	transmitter = TransmitterTask::getInstance();
	Assert(transmitter);

	sendStats = args->sendStats();
	nChannels = args->getUsableChannels();
	statistics = args->getStatistics();

	connection = new Udp(std::string("transmit"),
			(sonata_lib::Unit) UnitTransmit);
	if (!connection)
		Fatal(ERR_CCC);
	connection->setAddress(statistics.addr, statistics.port);
}

/**
 * Receive and process incoming packets.
 *
 * Description:\n
 * 	Processes packets from the Udp socket and passes them onto the
 *	beam object for processing.
 */
void *
StatisticsTask::routine()
{
	extractArgs();

	// sleep a while, then output statistics
	while (1) {
		sleep(10);
		BeamStatistics stats;
		beam->getStats(stats);
		SampleStatistics channelStats[MAX_TOTAL_CHANNELS];
		beam->getChannelStats(channelStats);
		if (sendStats)
			sendSseStats(stats, channelStats);
//		SampleStatistics outputStats = transmitter->getOutputStats();
//		const SampleStatistics *chanStats = transmitter->getChannelStats();
//		cout << stats;
//		cout << outputStats;
		if (args->printStats()) {
			cout << endl;
			cout << "Input packets, total: " << stats.netStats.total;
			cout << ", wrong: ";
			cout << stats.netStats.wrong << ", missed: " << stats.netStats.missed;
			cout << ", late: " << stats.netStats.late;
			cout << ", invalid: " << stats.netStats.invalid;
			cout << endl;
			cout << "Min: " << stats.inputStats.getMin();
			cout << ", max: " << stats.inputStats.getMax();
			cout << ", mean: " << stats.inputStats.getMean() << endl;
			cout << "Power: min: " << stats.inputStats.getMinPower();
			cout << ", max: " << stats.inputStats.getMaxPower();
			cout << ", mean: " << stats.inputStats.getMeanPower();
			cout << ", sd: " << stats.inputStats.getSD() << endl;
			cout << "Output packets, sequence: " << transmitter->getSeq();
			cout << ", waits: " << transmitter->getWaits() << endl;
			printOutputStats(stats.outputStats);
			if (args->printChannelStats())
				printChannelStats(channelStats);
		}
	}
	return (0);
}

void
StatisticsTask::sendSseStats(const BeamStatistics& stats,
		const SampleStatistics *channelStats)
{
	size_t len = sizeof(ChannelizerStatisticsHeader)
			+ nChannels * sizeof(ssechan::SampleStatistics);
	MemBlk *blk = partitionSet->alloc(len);
	Assert(blk);
	ChannelizerStatistics *sse =
			static_cast<ChannelizerStatistics *> (blk->getData());

	// copy the beam statistics
	// copy the network statistics
	ssechan::NetStatistics& netStats = sse->header.beam.netStats;
	netStats.total = stats.netStats.total;
	netStats.wrong = stats.netStats.wrong;
	netStats.invalid = stats.netStats.invalid;
	netStats.missed = stats.netStats.missed;
	netStats.late = stats.netStats.late;
	// copy the input sample statistics
	ssechan::SampleStatistics& in = sse->header.beam.inputStats;
	in.samples = stats.inputStats.samples;
	in.sumSq = stats.inputStats.sumSq;
	in.min = cfloat64(stats.inputStats.min.real(),
			stats.inputStats.min.imag());
	in.max = cfloat64(stats.inputStats.max.real(),
			stats.inputStats.max.imag());
	in.sum = cfloat64(stats.inputStats.sum.real(),
			stats.inputStats.sum.imag());
	// copy the output sample statistics
	ssechan::SampleStatistics& out = sse->header.beam.outputStats;
	out.samples = stats.outputStats.samples;
	out.sumSq = stats.outputStats.sumSq;
	out.min = cfloat64(stats.outputStats.min.real(),
			stats.outputStats.min.imag());
	out.max = cfloat64(stats.outputStats.max.real(),
			stats.outputStats.max.imag());
	out.sum = cfloat64(stats.outputStats.sum.real(),
			stats.outputStats.sum.imag());

	// copy the individual output channels
	sse->header.numberOfChannels = nChannels;
	ssechan::SampleStatistics *channels
			= (ssechan::SampleStatistics *) (&sse->header + 1);
	for (int32_t i = 0; i < sse->header.numberOfChannels; ++i) {
		channels[i].samples = channelStats[i].samples;
		channels[i].sumSq = channelStats[i].sumSq;
		channels[i].min = cfloat64(channelStats[i].min.real(),
				channelStats[i].min.imag());
		channels[i].max = cfloat64(channelStats[i].max.real(),
				channelStats[i].max.imag());
		channels[i].sum = cfloat64(channelStats[i].sum.real(),
				channelStats[i].sum.imag());
	}

	sse->marshall();

	// now transmit the data
	connection->send(sse, len, statistics.addr, statistics.port);
	blk->free();
}

void
StatisticsTask::printOutputStats(const SampleStatistics& stats)
{
	cout << "Min: " << stats.getMin();
	cout << ", max: " << stats.getMax();
	cout << ", mean: " << stats.getMean() << endl;
	cout << "Power: min: " << stats.getMinPower();
	cout << ", max: " << stats.getMaxPower();
	cout << ", mean: " << stats.getMeanPower();
	cout << ", sd: " << stats.getSD() << endl;
}

void
StatisticsTask::printChannelStats(const SampleStatistics *channelStats)
{
	for (int32_t i = 0; i < args->getUsableChannels(); ++i) {
		const SampleStatistics& stats = channelStats[i];
		cout << "Channel " << i;;
//		cout << endl << "Min: " << stats.getMin();
//		cout << ", max: " << stats.getMax();
//		cout << ", mean: " << stats.getMean() << endl;
		cout << ", power: min: " << stats.getMinPower();
		cout << ", max: " << stats.getMaxPower();
		cout << ", mean: " << stats.getMeanPower();
		cout << ", sd: " << stats.getSD() << endl;
	}
}

}