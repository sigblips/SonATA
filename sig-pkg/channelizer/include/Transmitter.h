/*******************************************************************************

 File:    Transmitter.h
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
// TransmitterTaskTask class
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/include/Transmitter.h,v 1.11 2009/02/13 18:11:55 kes Exp $
//
#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include <map>
#include <vector>
#include <arpa/inet.h>
#include <sseInterface.h>
#include "System.h"
#include "Beam.h"
#include "ChannelPacketVector.h"
#include "ChTypes.h"
#include "ChErr.h"
#include "Msg.h"
#include "QTask.h"
#include "TransmitterQ.h"
#include "Udp.h"

using namespace sonata_lib;

namespace chan {

typedef std::map<int32_t, ChannelPacketVector *> TransmitList;

struct TransmitterTiming {
	uint64_t vectors;
	uint64_t waits;
	uint64_t wakeups;
	float setup;
	float send;
	float free;
	float total;

	TransmitterTiming(): vectors(0), waits(0), wakeups(0),
			setup(0), send(0), free(0), total(0) {}
};

class TransmitterTask: public QTask {
public:
	static TransmitterTask *getInstance();
	~TransmitterTask();

	void restart();
	int32_t getSeq() { return (curSeq); }
	int32_t getWaits();
	const SampleStatistics& getOutputStats() { return (outputStats); }
	const SampleStatistics *getChannelStats() { return (channelStats); }

private:
	static TransmitterTask *instance;

	bool abort;
	int32_t curSeq;
	HostSpec base;
	ATADataPacketHeader::PolarizationCode code;
	chan::Unit unit;
	TransmitterTiming timing;
	Udp *connection;
	SampleStatistics outputStats;
	SampleStatistics channelStats[MAX_TOTAL_CHANNELS];
	TransmitList transmitList;

	Beam *beam;
	ChannelPacketVectorList *vectorList;
	MsgList *msgList;
	Queue *transmitterQ;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	void transmit(Msg *msg);
	void sendVector(ChannelPacketVector *vector);
	Error send(ChannelPacket& pkt);
	void convertChanToIp(IpAddress& ipAddr, int32_t chan);
	void recordOutputStats(int32_t chan, const ComplexInt16 *s);

	// hidden
	TransmitterTask(const string name_, const int prio_);

	// forbidden
	TransmitterTask(const TransmitterTask&);
	TransmitterTask& operator=(const TransmitterTask&);
};

}

#endif /*TRANSMITTER_H_*/