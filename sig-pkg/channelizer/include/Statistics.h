/*******************************************************************************

 File:    Statistics.h
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

// Statisticsging task
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/include/Statistics.h,v 1.5 2009/02/13 03:02:28 kes Exp $
//
#ifndef STATISTICS_H_
#define STATISTICS_H_

#include "System.h"
#include "Args.h"
#include "Beam.h"
#include "Partition.h"
#include "Transmitter.h"

using namespace sonata_lib;

namespace chan {

class StatisticsTask: public Task {
public:
	static StatisticsTask *getInstance();
	~StatisticsTask();

private:
	static StatisticsTask *instance;

	bool sendStats;
	int32_t nChannels;
	HostSpec statistics;
	Udp *connection;

	Args *args;							// command-line args
	Beam *beam;							// singleton beam
	PartitionSet *partitionSet;			// partition set for block allocation
	TransmitterTask *transmitter;		// transmitter

	// methods
	void extractArgs();
	void *routine();
	void sendSseStats(const BeamStatistics& beamStats,
			const SampleStatistics *channelStats);
	void printOutputStats(const SampleStatistics& stats);
	void printChannelStats(const SampleStatistics *channelStats);

	// hidden
	StatisticsTask(string name_, int prio_);

	// forbidden
	StatisticsTask(const StatisticsTask&);
	StatisticsTask &operator=(const StatisticsTask&);
};

}
#endif /*STATISTICS_H_*/