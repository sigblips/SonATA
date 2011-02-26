/*******************************************************************************

 File:    Worker.h
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
// Worker task
//
// Worker task which performs polyphase filter and cornerturn processing
// to produce output channels.  A number of these tasks may be created
// to handle the full input rate.
//
// $Heaer: $
//
#ifndef _WorkerTaskH
#define _WorkerTaskH

#include "System.h"
#include "Beam.h"
#include "ChannelPacketVector.h"
#include "ChTypes.h"
#include "Dfb.h"
#include "Lock.h"
#include "Msg.h"
#include "QTask.h"
#include "Transmitter.h"

using namespace sonata_lib;

namespace chan {

#undef INPUT_ONLY

struct WorkerArgs {
};

struct cv {
	uint64_t creates;					// # of packets created
	float allocate;
	float putHeader;
	float putSamples;
	float total;

	cv(): creates(0), allocate(0), putHeader(0), putSamples(0), total(0) {}
};
struct WorkerTiming {
	uint64_t iterations;				// total # of times run
	float dfb;
	float createVector;
	float transmit;
	float total;
	cv vector;


	WorkerTiming(): iterations(0), dfb(0), createVector(0),
			transmit(0), total(0) {}
};

class WorkerTask: public QTask {
public:
	WorkerTask(string name_);
	~WorkerTask();

private:
	int32_t messages;
	int32_t dfbId;						// DFB identifier
	chan::Unit unit;
	Queue *workQ;
	ComplexFloat32 *sampleBuf;			// DFB sample buffer
	ComplexFloat32 *buf;				// DFB output buffer
	ComplexFloat32 *outArray[MAX_TOTAL_CHANNELS];
	ComplexFloat32 *pktArray[MAX_TOTAL_CHANNELS];
	WorkerTiming timing;				// timing statistics

	Beam *beam;
	ChannelPacketVectorList *vectorList;
	MsgList *msgList;
	Queue *transmitterQ;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	void processData(Msg *msg);
	void buildOutputArray(ComplexFloat32 **array, ComplexFloat32 *buf,
			int32_t samples, int32_t channels, int32_t usable);
	void buildPacketArray(ComplexFloat32 **array, ComplexFloat32 *buf,
			int32_t samples, int32_t channels, int usable);
	ChannelPacketVector *createPacketVector(PacketInfo *pktInfo,
			ComplexFloat32 **out);
};

}

#endif