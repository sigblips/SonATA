/*******************************************************************************

 File:    Beam.h
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

/**
* Channelizer beam class
*
* Description:\n
*	A beam represents ~104MHz of spectrum.  This class maintains
*	the state of a single-polarization (x or y) beam during the
* 	channelization process.  As such, it encapsulates all the
*	information required to perform processing at any given point.
*	Buffers are allocated when the beam is instantiated
*	and released when the beam is destroyed.\n
* Notes:\n
*	An input buffer queues the data until there is enough to perform
* 	a channelization, which involves a multi-folding WOLA digital
* 	filter followed by an FFT.\n
* 	Since DFB's must be performed more quickly than the time it
* 	takes a single processor to perform a DFB, we need to allow
* 	multiple processors to perform concurrent DFB's.  This means
* 	that access to the input data must be controlled.
*/
#ifndef _BeamH
#define _BeamH

#include <complex>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <sseChannelizerInterface.h>
#include "System.h"
#include "Args.h"
#include "BeamPacketList.h"
#include "Buffer.h"
#include "ChannelPacketList.h"
#include "ChStruct.h"
#include "ChTypes.h"
#include "Dfb.h"
#include "DfbCoeff.h"
#include "System.h"
#include "InputBuffer.h"
#include "Msg.h"
#include "Partition.h"
#include "Queue.h"
#include "WorkQ.h"

using std::map;
using std::queue;
using std::vector;
using std::ostream;

namespace chan {

// forward declaration
class Args;

using namespace sonata_lib;
using sonata_lib::NetStatistics;
using sonata_lib::SampleStatistics;

// output packet data
struct PacketInfo {
	uint64_t sample;					// sample # in input buffer
	int32_t totalChannels;				// total # of channels created
	int32_t usableChannels;				// # of channels output
	float64_t bandwidth;				// total bandwidth in MHz
	ATADataPacketHeader hdr;			// header info

	PacketInfo(): sample(0), totalChannels(0), usableChannels(0), bandwidth(0),
			hdr(ATADataPacketHeader::ATA, ATADataPacketHeader::CHAN_400KHZ,
					ATADataPacketHeader::CHANNEL_SAMPLES) {}

	friend ostream& operator << (ostream& s, const PacketInfo& packet);
};

// output packet timing info
struct PacketTiming {
	struct b {							// beam
		float96_t time;					// time first sample received
		float96_t secPerSample;			// seconds/sample

		b(): time(0), secPerSample(0) {}
	} beam;
	struct c {							// channel
		float96_t time;					// time of next packet
		float96_t secPerSample;			// seconds/sample
		float96_t secPerPacket;			// seconds/packet (steady-state)

		c(): time(0), secPerSample(0), secPerPacket(0) {}
	} channel;
};

struct BeamStatistics {
	NetStatistics netStats;				// network statistics
	SampleStatistics inputStats;		// input sample statistics (beam)
	SampleStatistics outputStats;		// output sample statistics (channels)

	void reset() { netStats.reset(); inputStats.reset(); outputStats.reset(); }

	friend ostream& operator << (ostream& s, const BeamStatistics& beamStats);
};

struct DfbTiming {
	uint64_t dfbs;
	float load;
	float dfb;
	float list;
	float total;

	DfbTiming(): dfbs(0), load(0), dfb(0), list(0), total(0) {}

	friend ostream& operator << (ostream& s, const DfbTiming& dfbTiming);
};

struct BeamTiming {
	uint64_t packets;
	float maxFlush;
	float flush;
	float store;
	float set;
	float total;
	DfbTiming dfb;

	BeamTiming(): packets(0), maxFlush(0), flush(0), store(0), set(0),
			total(0) {}

	friend ostream& operator << (ostream& s, const BeamTiming& beamTiming);
};

typedef std::queue<ChannelPacket *>		PacketQueue;
typedef std::vector<dfb::Dfb *>			DfbList;
typedef std::map<uint64_t, bool>		PendingList;
typedef std::vector<SampleStatistics>	OutputStatistics;

class Beam {
public:
	static Beam *getInstance();
	~Beam();

	void setup();
	int32_t allocDfb();

	void getStats(BeamStatistics& stats_) { lock(); stats_ = stats; unlock(); }

	// methods to set up the beam parameters
	void setState(ChannelizerState state_);
	void setFreq(float64_t f) { beam.freq = f; }
	void setStartTime(const NssDate& start) { startTime = start; }

	// methods to return beam parameters
	const NssDate& getStartTime() { return (startTime); }
	const HostSpec& getSse() { return (sse); }
	const HostSpec& getInput() { return (input); }
	const HostSpec& getOutput() { return (output); }
	uint32_t getSrc() { return (beam.src); }
	::uint8_t getPol() { return (beam.pol); }
	float64_t getFreq() { return (beam.freq); }
	float64_t getBandwidth() { return (beam.bandwidth); }
	float64_t getOversampling() { return (beam.oversampling); }
	const ChannelSpec& getChannels() { return (channels); }
	int32_t getTotalChannels() { return (channels.total); }
	int32_t getUsableChannels() { return (channels.usable); }
	int32_t getFoldings() { return (channels.foldings); }
	int32_t getThreshold() { return (threshold); }

	int32_t getInputSeq() { return (inputSeq); }
	int32_t getOutputSeq() { return (outputSeq); }
	int32_t getTotal() { return (stats.netStats.total); }
	int32_t getWrong() { return (stats.netStats.wrong); }
	int32_t getMissed() { return (stats.netStats.missed); }
	int32_t getLate() { return (stats.netStats.late); }
	int32_t getInvalid() { return (stats.netStats.invalid); }
	void getBeamStats(BeamStatistics& bs);
	void getChannelStats(SampleStatistics *cs);
	void getNetStats(NetStatistics& ns);

	/**
	* Get the state of the beam.
	*/
	ChannelizerState getState() { return (state); }
	void stop();
	Error handlePacket(BeamPacket *pkt, bool& startFlag);
	void dfbProcess(int32_t dfbId_, uint64_t sample_, ComplexFloat32 *sampleBuf,
			ComplexFloat32 **buf_);
	void getSamples(ComplexFloat32 *sampleBuf, ComplexInt8 *samples,
			int32_t len);
	void getSwappedSamples(ComplexFloat32 *sampleBuf, ComplexInt8 *samples,
			int32_t len);

private:
	static Beam *instance;

	bool armed;							// are we waiting to start input
	bool swap;							// swap real and imaginary inputs
	int32_t decimation;					// decimation of input samples
	int32_t schedules;					// # of dfb's scheduled
	int32_t dones;
	int32_t flushes;
	int32_t consumed;					// # of samples consumed by dfb call
	int32_t threshold;					// threshold for calling DFB
	uint32_t inputSeq;					// current input packet sequence #
	uint32_t outputSeq;					// current output packet sequence #
	uint64_t sampleCnt;					// # of samples
	float64_t maxwlock;					// maximum time to obtain wlock
	float64_t maxlock;					// maximum time to obtain a lock
	ChannelizerState state;				// current beam state
	NssDate startTime;					// start time
	BeamStatistics stats;				// beam statistics
	SampleStatistics channelStats[MAX_TOTAL_CHANNELS]; // channel statistics
	dfb::DfbInfo dfbInfo;				// DFB description
	ATADataPacketHeader hdr;			// packet header
	ChannelSpec channels;				// total and usable channels
	BeamSpec beam;						// beam specification
	DfbList dfbList;					// vector of DFB's
	HostSpec sse;						// SSE host info
	HostSpec input;						// beamformer input info
	HostSpec output;					// channelizer output info
	PacketInfo packetInfo;				// information for packets
	PacketTiming packetTiming;			// output packet timing info
	PendingList pendingList;			// list of pending DFB's
	BeamTiming timing;

	// buffers
	InputBuffer *buf;					// sample input buffer
	Buffer *tdBuf;						// time domain data buffer (one pol)

	PacketQueue out;					// queue of output data packets
	Lock bLock;							// synchronization lock
	Lock sLock;							// statistics synchronization lock

	// singletons
	MsgList *msgList;					// queue message list
	Args *args;							// command-line arguments
	Queue *workQ;						// worker task input queue
	BeamPacketList *beamPktList;		// free list of beam packets
	ChannelPacketList *chanPktList;		// free list of channel packets
	PartitionSet *partitionSet;			// partition set for block allocation

	// methods
	void createEmptyPacket(BeamPacket *empty, BeamPacket *pkt);
	void addPacket(BeamPacket *pkt);
	void addSampleData(BeamPacket *pkt);
	void schedule(uint64_t sample);
	void recordInputStats(const ComplexInt8 *);
	void recordOutputStats(ComplexFloat32 **s);
	void recordOutputStats(ComplexFloat32 **s, int32_t dIndex, int32_t n);
	void setPacketTime(BeamPacket *pkt);

	void lock() { bLock.lock(); }
	void unlock() { bLock.unlock(); }

	void slock() { sLock.lock(); }
	void sunlock() { sLock.unlock(); }

	// hidden
	Beam();

	// forbidden
	Beam(const Beam&);
	Beam& operator=(const Beam&);
};

}

#endif