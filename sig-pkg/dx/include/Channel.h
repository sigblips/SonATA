/*******************************************************************************

 File:    Channel.h
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
* Channel class
*
* Description:\n
*	A channel represents ~100-400KHz of spectrum.  This class maintains
*	the state of a single channel throughout the collection, detection
*	and confirmation processes.  As such, it encapsulates all the
*	information required to perform processing at any given point.
*	In most cases, buffers are allocated when the channel is instantiated
*	and released when the channel is destroyed.\n
* Notes:\n
*	Input packet queues buffer the X and Y incoming Ethernet
*	packets until they can be paired and converted to LCP and RCP.
*	LCP and RCP input buffers contain LCP and RCP input data prior to
*	the DFB/FFT which produces the subchannels.  These must be large
*	enough to allow buffering of at least one DFB process' worth of
*	data; in practice, they are much larger.  There is a single buffer
*	for each polarization.\n
*	Half frame buffers each contain a half frame of corner-turned data
*	for all subchannels.  They are paired in left and right, and are
*	used by the spectrometry task to create spectra of all selected
*	resolutions.  Three buffer pairs are allocated because
*	multiple buffers are required for spectrometry.  They are actually
*	part of the State class.\n
*	CD, CW and Pulse buffers are contained in memory; they are allocated
*	when the channel is created, and are sized to handle the maximum
*	observation length.
*	A single detection buffer is allocated by the State
*	"Disk" buffers contain the actual detection data created by the
*	spectrometry task.  [In reality, the disk buffers may be in memory
*	rather than on magnetic disk.]  There are buffers for CD, CWD
*	and Pulse data; two buffers of each type are allocated for each
*	polarization, to allow for simultaneous collection and detection.
*	For the CD and CWD data, they are partitions, while for pulse
*	data they are ordinary files.\n
*	A single detection buffer is shared by CW, pulse and confirmation
*	detectors, so they must be done sequentially.  It is actually part
*	detectors, of the State clas.
*/
#ifndef _ChannelH
#define _ChannelH

#include <complex>
#include <map>
#include <queue>
#include <vector>
#include <fftw3.h>
#include <sseInterface.h>
#include "Args.h"
#include "Buffer.h"
#include "Dfb.h"
#include "DfbCoeff.h"
#include "System.h"
#include "ChannelPacketList.h"
#include "DxStruct.h"
#include "InputBuffer.h"
#include "Msg.h"
#include "Partition.h"
#include "Queue.h"
#include "ReadFilter.h"
//#include "Spectra.h"
//#include "State.h"
#include "WorkQ.h"
//#include "Xfer.h"

using std::map;
using std::queue;
using std::vector;
//using dfb::Dfb;
//using dfb::DfbInfo;
using namespace sonata_lib;

namespace dx {

// forward declaration
class Activity;
class Channel;
class State;

typedef queue<ChannelPacket *> PacketQueue;
//typedef std::vector<dfb::Dfb *> DfbList;
typedef std::map<uint64_t, bool> PendingList;
typedef vector<dx::Pulse> PulseList;

// channel statistics
struct ChannelStatistics {
	NetStatistics netStats;				// network statistics
	SampleStatistics inputStats;		// input sample statistics (channel)
	SampleStatistics outputStats;		// output sample statistics (subchannels)

	ChannelStatistics() { reset(); }
	void reset() { netStats.reset(); inputStats.reset(); outputStats.reset(); }
};

// channel specification
struct ChannelSpec {
	int32_t chan;						// channel number
	uint32_t src;						// beam source
	::uint8_t pol;						// polarization
	float64_t freq;						// center frequency
	float64_t bandwidth;				// bandwidth
	float64_t oversampling;				// oversampling(%)

	ChannelSpec(): chan(-1), src(ATADataPacketHeader::UNDEFINED),
			pol((::uint8_t) ATADataPacketHeader::UNDEFINED),
			freq(0.0), bandwidth(0.0), oversampling(0.0) {}
	ChannelSpec(uint32_t src_,::uint8_t pol_, int32_t chan_, float64_t freq_,
			float64_t bandwidth_, float64_t oversampling_): chan(chan_),
			src(src_), pol(pol_), freq(freq_), bandwidth(bandwidth_),
			oversampling(oversampling_) {}
	ChannelSpec& operator=(const ChannelSpec& rhs) {
		chan = rhs.chan;
		src = rhs.src;
		pol = rhs.pol;
		freq = rhs.freq;
		bandwidth = rhs.bandwidth;
		oversampling = rhs.oversampling;
		return (*this);
	}
};

/**
 * Half frame DFB processing data.
 */
struct HalfFrameInfo {
	uint64_t sample;					// sample # of input buffer
	int32_t halfFrame;					// half frame #
	int32_t totalSubchannels;
	int32_t usableSubchannels;
	Channel *channel;					// ptr to the channel

	HalfFrameInfo(): sample(0), halfFrame(0), totalSubchannels(0),
			usableSubchannels(0), channel(0) {}
};

/**
 * DFB timing structure.
 */
struct DfbTiming {
	uint64_t dfbs;
	float load;
	float dfb;
	float list;
	float total;

	DfbTiming(): dfbs(0), load(0), dfb(0), list(0), total(0) {}
};

struct ChannelTiming {
	uint64_t packets;
	float maxFlush;
	float flush;
	float store;
	float set;
	float total;
	DfbTiming dfb;

	ChannelTiming(): packets(0), maxFlush(0), flush(0), store(0), set(0),
			total(0) {}
};

/**
 * Channel class.
 *
 * Description:\n
 * 	The channel encapsulates all the information necessary to perform
 * 	data collection, spectrometry, signal detection and confirmation
 * 	for a single 400KHz channel.  Each activity contains a channel
 * 	structure which describes the channel being processed by that
 * 	activity.\n
 * Notes:\n
 * 	When an activity is allocated (i.e., scheduled for an observation),
 * 	the channel must be initialized by allocating all data areas
 * 	necessary for data collection and spectrometry.  Buffers for
 * 	signal detection are not allocated as part of the channel, but
 * 	are common to all activities, since they are very large.\n
 * 	This implementation assumes exactly one channel per activity.
 */
class Channel {
public:
	Channel(State *state_, Activity *activity_);
	~Channel();

	void setFilter(const FilterSpec& filter_);
	void setup(const DxActivityParameters& params, Polarization pol_);

	bool rightPolActive() { return (rPol != ATADataPacketHeader::NONE); }
	bool leftPolActive() { return (lPol != ATADataPacketHeader::NONE); }

	int32_t getMaxFrames();
	int32_t getMaxHalfFrames();

	int32_t getSamplesPerSubchannelHalfFrame();
	int32_t getBytesPerSubchannelHalfFrame();

	int32_t getTotalSubchannels();
	int32_t getUsableSubchannels();
	float64_t getSubchannelWidthMHz();
	float64_t getSubchannelRateMHz();
	int32_t getFoldings();
	float64_t getSubchannelOversampling();
	int32_t getSubchannelsPerArchiveChannel();

	int32_t getTotalBinsPerSubchannel(Resolution res);
	int32_t getUsableBinsPerSubchannel(Resolution res);
	int32_t getUsableBinsPerSpectrum(Resolution res);

	int32_t getSpectraPerFrame(Resolution res);
	int32_t getSpectra(Resolution res);

	void stopDataCollection();

//	Error setState(DxActivityState newState_) { channelState = newState_; }
	void setState(DxActivityState newState_);
//	DxActivityState getState() { return (channelState); }
	DxActivityState getState();

	void getStats(ChannelStatistics& stats_) { stats_ = stats; }

	// activity parameters
	int32_t getActivityId();
	int32_t getChannelNum() { return (channelSpec.chan); }
	float64_t getChannelCenterFreq() { return (channelSpec.freq); }
	float64_t getChannelWidthMHz() { return (channelSpec.bandwidth); }
	int32_t getFrames();
	int32_t getHalfFrames();

	float64_t getLowFreq();
	float64_t getHighFreq();
	float64_t getSubchannelCenterFreq(int32_t subchannel);

	int32_t getSubchannel(float64_t freq_);

	// channel values
	int32_t incrementHalfFrame() { return (++halfFrame); }
	int32_t getHalfFrame() { return (halfFrame); }
	int32_t getThreshold() { return (threshold); }
	in_addr getAddress() { return (address); }

	// half frame buffer calls
	BufPair *allocHfBuf();
	void freeHfBuf(BufPair *hfBuf);
	uint32_t getHfOfs(int32_t subchannel);
	ComplexFloat32 *getHfData(Polarization pol, int32_t subchannel,
			BufPair *hfBuf);

	// CD buffer calls
	Buffer *getCdBuf(Polarization pol);
	void *getCdData(Polarization pol);
	void *getCdData(Polarization pol, int32_t subchannel, int32_t hf);
	int32_t getCdSamplesPerSubchannelHalfFrame();
	int32_t getCdBytesPerSubchannelHalfFrame();
	int32_t getCdSamplesPerSubchannel();
	int32_t getCdBytesPerSubchannel();
	int32_t getCdStridePerSubchannel();
//	int32_t getCdBinsPerHalfFrame();
//	int32_t getCdBytesPerHalfFrame() {
//		return (getCdBytesPerSubchannelHalfFrame() * getSubchannels());
//	}

	// CW buffer calls
	Buffer *getCwBuf(Polarization pol);
//	void *getCwData(Polarization pol);
	void *getCwData(Polarization pol, Resolution res,
			int32_t subchannel = 0, int32_t spectrum = 0);
	int32_t getTotalCwBinsPerSubchannel(Resolution res);
	int32_t getCwBinsPerSubchannel(Resolution res);
	int32_t getCwBytesPerSubchannel(Resolution res);
	int32_t getCwStridePerSubchannel(Resolution res);
	int32_t getCwBinsPerSpectrum(Resolution res);
	int32_t getCwBytesPerSpectrum(Resolution res);

	// baseline buffer calls
	Buffer *getBlBuf(Polarization pol);
	float32_t *getBlData(Polarization pol);
	float32_t *getBlData(Polarization pol, int32_t subchannel);
	Buffer *getNewBlBuf(Polarization pol);
	float32_t *getNewBlData(Polarization pol);
	float32_t *getNewBlData(Polarization pol, int32_t subchannel);
	int32_t getBlBinsPerHalfFrame() { return (getUsableSubchannels()); }
	int32_t getBlBytesPerHalfFrame() {
		return (getBlBinsPerHalfFrame() * sizeof(float32_t));
	}

	// detection buffer calls
	Buffer *allocDetectionBuf();
	Buffer *getDetectionBuf();
	void freeDetectionBuf();

	// conversion functions
	Polarization getPol(ATADataPacketHeader::PolarizationCode polCode);
	ATADataPacketHeader::PolarizationCode getPolCode(Polarization pol);

	void clearBaselines();
	void clearInputBuffers();
	void clearPulseList();
	void addPulse(const Pulse& pulse);
	PulseList& getPulseList() { return (pulseList); }

	// data processing functions
	Error handlePacket(ChannelPacket *pkt) {
		lock(); Error err = handlePacket_(pkt); unlock(); return (err);
	}
	void dfbProcess(uint64_t sample, ComplexFloat32 *sampleBuf,
			ComplexFloat32 **lBuf, ComplexFloat32 **rBuf);
	void dfbFlush(uint64_t sample);

private:
	bool initialized;					// channel has been initialized
	bool swap;							// swap real and imaginary
//	bool scheduled;						// scheduled for worker task?
	bool singlePol;						// running a single polarization?
	bool armed;							// armed to start activity
	bool abortCollection;				// abort data collection
	uint8_t inactivePol;				// inactive polarization
	uint8_t rPol;						// right polarization
	uint8_t lPol;						// left polarization
	int32_t activityId;					// activity id
//	int32_t channelNum;					// channel number
	int32_t subchannels;				// # of subchannels
	int32_t schedules;					// # of times DFB has been scheduled
	int32_t flushes;					// # of times input buffers flushed
	int32_t dones;						// # of times dfbProcess executed
	int32_t consumed;					// # of samples consumed by DFB
	int32_t threshold;					// threshold for calling DFB
	int32_t inputHalfFrame;				// current input half frame
	int32_t halfFrame;					// currently completed half frame
//	int32_t halfFrames;					// total # of half-frames
	uint32_t startSeq;					// starting sequence #
	uint32_t curSeq;					// current sequence #
	struct d {
		int32_t err;
		int32_t maxErr;
		struct ls {
			uint32_t r, l;

			ls() { reset(); }
			void reset() {
				r = l = 0;
			}
		} lastSeq;
		struct p {
			uint32_t r, l;

			p() { reset(); }
			void reset() {
				r = l = 0;
			}
		} pkts;

		d(): err(0), maxErr(0) {  reset(); }
		void reset() {
			err = 0;
			lastSeq.reset();
			pkts.reset();
		}
	} data;
//	int32_t missed;						// packets not received
//	int32_t late;						// packets received late
//	int32_t total;						// packets total
	uint64_t sampleCnt;					// current # of samples in L/R buffers
	float64_t freq;						// center freq
//	float64_t bandwidth;				// channel bandwidth
	Polarization pol;					// polarization
	in_addr address;					// channel multicast address
	dfb::DfbInfo dfbInfo;				// DFB description
	dfb::Dfb dfb;						// DFB
	Activity *activity;					// parent activity
	ChannelSpec channelSpec;			// channel specification
	ChannelStatistics stats;			// channel statistics
	FilterSpec filter;					// filter specification
//	NetStatistics netStats;				// network statistics
	PendingList pendingList;			// list of pending DFB's
//	spectra_lib::Spectra spectra;		// spectrum library
	ChannelTiming timing;				// channel timing info

	// buffers
	InputBuffer *left;					// LCP sample input buffer
	InputBuffer *right;					// RCP sample input buffer

	BufPair *blBuf;						// current baseline buffers (L & R)
	BufPair *newBlBuf;					// new baseline buffers
	BufPair *cdBuf;						// confirmation data buffers (L & R)
	BufPair *cwBuf;						// CW data buffers (L & R)
	PulseList pulseList;				// vector of pulses (L & R)
	Buffer *detectionBuf;				// detection buffer
	BufPairList *hfBufList;				// half frame buffer list

#ifdef notdef
	// corner turn xfer specifications
	XferSpec hfXfer[POLARIZATIONS];		// half frame buffer transfer spec
	XferSpec cdXfer[POLARIZATIONS];		// CD buffer xfer spec
	XferSpec cwXfer[POLARIZATIONS];		// CW buffer xfer spec
#endif
//	NssDate startTime;					// starting time
//	NssDate actualStartTime;			// actual starting time
	DxActivityState channelState;		// current channel state
	PacketQueue l, r;					// queue of data packets
	Lock cLock;							// synchronization lock
	Args *args;							// command-line arguments
	ChannelPacketList *pktList;			// free list of channel packets
	MsgList *msgList;					// queue message list
	PartitionSet *partitionSet;			// partitions
	Queue *workQ;						// worker task input queue
	State *state;						// State

	// methods
	void allocPulseList(const DxActivityParameters& params);
	void init();
	void clearPacketQueues();
	Error handlePacket_(ChannelPacket *pkt);
	void addData(ChannelPacket *xp, ChannelPacket *yp);
	void schedule(uint64_t sample__);
	void createEmptyPacket(ChannelPacket *pkt, uint8_t pol);
	void addPacket(ChannelPacket *pkt);
	int32_t dfbPol(uint64_t sample, InputBuffer *inBuf,
			ComplexFloat32 *sampleBuf, ComplexFloat32 **obuf);
	void getSamples(ComplexFloat32 *sampleBuf, ComplexInt16 *samples,
			int32_t len);
	void getSwappedSamples(ComplexFloat32 *sampleBuf, ComplexInt16 *samples,
			int32_t len);

	void lock() { cLock.lock(); }
	void unlock() { cLock.unlock(); }
};

}

#endif
