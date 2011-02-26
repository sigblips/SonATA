/*******************************************************************************

 File:    Beam.cpp
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
// Channelizer Beam class
//
//
#include <sys/time.h>
#include <xmmintrin.h>
#include "Beam.h"
#include "ChErr.h"
#include "Util.h"

using std::cout;
using std::endl;
using std::norm;

namespace chan {

/**
* Beam class.
*
* Description:\n
* 	The beam class is a singleton, since a channelizer processes
* 	only one beam.  To change parameters during
* 	operation, a setup call is provided which will reset the
* 	available parameters as well as all statistics.
*/
Beam *Beam::instance = 0;

Beam *
Beam::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new Beam();
	l.unlock();
	return (instance);
}

Beam::Beam(): armed(false), swap(false), decimation(1),
		schedules(0),
		dones(0), flushes(0), consumed(0), threshold(0), inputSeq(0),
		outputSeq(0), sampleCnt(0), maxwlock(0), maxlock(0),
		state(STATE_PENDING), buf(0), tdBuf(0),
		bLock("beam"), msgList(0), args(0), workQ(0),
		beamPktList(0), chanPktList(0), partitionSet(0)
{
	args = Args::getInstance();
	Assert(args);
	msgList = MsgList::getInstance();
	Assert(msgList);
	beamPktList = BeamPacketList::getInstance();
	Assert(beamPktList);
	chanPktList = ChannelPacketList::getInstance();
	Assert(chanPktList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	workQ = WorkQ::getInstance();
	Assert(workQ);
}

Beam::~Beam()
{
	// release all the DFB objects
	for (DfbList::iterator p = dfbList.begin(); p != dfbList.end(); ++p) {
		dfb::Dfb *dfb = *p;
		delete dfb;
	}
	dfbList.clear();
}

/**
 * Set up the beam DFB parameters
 *
 * Description:\n
 * 	Sets up the DFB parameters for all active DFB's.\n
 * Notes:\n
 * 	Sets up all current DFB objects with the current beam parameters.
 */
void
Beam::setup()
{
	beam.src = args->getBeamSrc();
	beam.pol = args->getPol();
	beam.freq = args->getCenterFreq();
	beam.bandwidth = args->getBandwidth();
	beam.oversampling = args->getOversampling();

	sse = args->getSse();
	input = args->getInput();
	output = args->getOutput();
	channels = args->getChannels();
	decimation = args->getDecimation();

	startTime.tv_sec = args->getStartTime();
	// if start time is forever, just go into idle
	if (startTime.tv_sec == -1)
		state = STATE_IDLE;

	swap = args->swapInputs();

	int32_t overlap = (int32_t) (getTotalChannels() * getOversampling());
	// overlap must be even
	Assert(!(overlap & 1));

	threshold = dfb::Dfb::getThreshold(getTotalChannels(), getFoldings(),
			overlap, ATADataPacketHeader::CHANNEL_SAMPLES);
	consumed = (getTotalChannels() - overlap) *
			ATADataPacketHeader::CHANNEL_SAMPLES;

	uint32_t size = BUF_COUNT * BEAM_SAMPLES;
	if (buf)
		delete buf;
	buf = new InputBuffer(size, sizeof(beamSample));

	// compute the beam packet timing information
	// Note: the time of the first beam packet will be recorded when it is
	// received
	packetTiming.beam.time = 0;
	packetTiming.beam.secPerSample = 1 / MHZ_TO_HZ(getBandwidth());

	// compute the channel packet timing information
	// Note: the time of the first channel packet will be computed from the
	// time of the first beam packet when processing is started
	packetTiming.channel.time = 0;
	packetTiming.channel.secPerSample = (getTotalChannels() - overlap) *
			packetTiming.beam.secPerSample;
	packetTiming.channel.secPerPacket = ATADataPacketHeader::CHANNEL_SAMPLES *
			packetTiming.channel.secPerSample;

	// set up packet info
	packetInfo.totalChannels = getTotalChannels();
	packetInfo.usableChannels = getUsableChannels();
	packetInfo.bandwidth = getBandwidth();
	packetInfo.hdr.src = args->getChannelSrc();
	packetInfo.hdr.polCode = getPol();
	packetInfo.hdr.freq = getFreq();
	packetInfo.hdr.len = ATADataPacketHeader::CHANNEL_SAMPLES;
	packetInfo.hdr.flags = ATADataPacketHeader::DATA_VALID;
}

/**
 * Instantiate a DFB and return its id.
 *
 * Description:\n
 * 	Creates a new copy of the DFB, initializes it with the proper setup
 * 	parameters, adds it to the DFB vector, and returns its index to the
 * 	caller, who will then provide the number each time it requires a
 * 	dfbProcess.\n
 * Notes:\n
 * 	This allows multiple channelize tasks to concurrently run DFB's
 * 	on separate processors.  It is necessary to instantiate separate
 * 	DFB's because the DFB allocates internal buffers which are used
 * 	during the WOLA and FFT operations.  [An alternative would be to
 * 	make the DFB itself reentrant, but I don't see an easy way to do
 * 	this without greatly complicating it - kes]
*/
int32_t
Beam::allocDfb()
{
	lock();
	dfb::Dfb *dfb = new dfb::Dfb();
	Assert(dfb);

	// initialize the DFB from the file if necessary
	if (args->useCustomFilter()) {
		const FilterSpec& filter = args->getFilter();
		dfb->setCoeff(filter.coeff, filter.fftLen, filter.foldings);
	}
	int32_t overlap = (int32_t) (getTotalChannels() * getOversampling());
	dfb->setup(getTotalChannels(), overlap,
			getFoldings(), ATADataPacketHeader::CHANNEL_SAMPLES);
	dfbList.push_back(dfb);
	int32_t n = dfbList.size() - 1;
	unlock();
	return (n);
}

/**
* Set the channel state.
*
* Description:\n
*	Sets the current channel state.  This should only be done by the
*	thread which "owns" the channel in its current state.
*
* @param	newState_ the new state being assigned to the channel beam.
*/
void
Beam::setState(ChannelizerState newState_)
{
	lock();
	state = newState_;
	unlock();
}

/**
 * Stop the channelizer.
 */
void
Beam::stop()
{
	setState(STATE_IDLE);
//	if (pendingList.size() > 0)
//		pendingList.clear();
//	buf->reset();
//	stats.reset();
}

/**
* Handle a single packet
*
* Description:\n
*	Given an input packet, checks to make sure that the packet belongs
*	to this beam (error if not).  If the packet is late (i.e.,
*	the current sequence number is greater than the packet sequence
*	number), the packet is discarded and the "out of sequence" counter
*	is bumped.  If the packet is early (i.e., the current sequence
* 	number is less than the packet sequence number), dummy packets
* 	are created to replace any missing packets.  If the packet is
* 	on time (i.e., the packet sequence number is the same as the
* 	current sequence number), the packet data is converted to floating
* 	point and stored in the input sample buffer.  If there is now
* 	enough data in the buffer to perform a DFB, a message is issued
* 	to the work queue and an entry is added to the pending list.
*
* Notes:\n
* 	If DATA_VALID is not set, the entire packet header is suspect,
* 	so we discard it immediately.
*
* @param	pkt the input data packet.
*
* @see		addPacket
*/
Error
Beam::handlePacket(BeamPacket *pkt, bool& startFlag)
{
	// if we're idle, just discard the packets
	if (getState() == STATE_IDLE) {
		beamPktList->free(pkt);
		return (0);
	}
	// assume not starting, no error
	startFlag = false;

	++stats.netStats.total;
	const ATADataPacketHeader& hdr = pkt->getHeader();
	if (!(hdr.flags & ATADataPacketHeader::DATA_VALID)) {
		// if the packet does not contain valid data, discard it
		++stats.netStats.invalid;
		stats.inputStats.reset();
//		started = false;
		armed = false;
		beamPktList->free(pkt);
		inputSeq = 0;
		return (0);
	}

	// test for correct source and polarization
	if (hdr.src != beam.src || hdr.polCode != beam.pol) {
		++stats.netStats.wrong;
		beamPktList->free(pkt);
		return (0);
	}

	// if we're pending, check to see whether it's time to go
	Error err = 0;
	if (getState() == STATE_PENDING) {
		// check for correct version number
		if (hdr.version != ATADataPacketHeader::CURRENT_VERSION) {
			setState(STATE_IDLE);
			beamPktList->free(pkt);
			return (ERR_IPV);
		}

		uint32_t t = hdr.absTime >> 32;
		uint32_t start = startTime.tv_sec;
		if (!armed || t < start) {
			armed = true;
			if (start && t >= start)
				err = ERR_STAP;
			beamPktList->free(pkt);
			return (err);
		}
		else {
			inputSeq = hdr.seq;
			outputSeq = 0;
			setPacketTime(pkt);
			if (pendingList.size())
				pendingList.clear();
			buf->reset();
			stats.reset();
			setState(STATE_RUNNING);
			startFlag = true;
			armed = false;
		}
	}
	// if packet is late, discard it
	if ((int32_t) (hdr.seq - inputSeq) < 0) {
		++stats.netStats.late;
		beamPktList->free(pkt);
	}
	else
		addPacket(pkt);
	return (err);
}

/**
* Add a packet to the input sample buffer.
*
* Description:\n
*	Called on receipt of a packet to convert a packet to floating point
* 	and add it to the sample buffer.  If there is then enough new data
* 	in the buffer to perform the DFB, a message is sent to the worker
* 	queue requesting the DFB.\n\n
* Notes:\n
* 	If the packet has the correct sequence number, it is converted
* 	and output.  If it has a greater sequence number, then dummy
* 	packets are output for the missing sequence numbers.
*
* @param	pkt input packet
*/
void
Beam::addPacket(BeamPacket *pkt)
{
	// insert dummy packets for missing ones
	const ATADataPacketHeader& hdr = pkt->getHeader();
	while ((int32_t) (inputSeq - hdr.seq) < 0) {
		BeamPacket tmp;
		createEmptyPacket(&tmp, pkt);
		addSampleData(&tmp);
	}
	// add the data packet
	addSampleData(pkt);
	beamPktList->free(pkt);
	// if we have enough samples, schedule a DFB
	uint64_t next = buf->getNext();
	while (buf->getSamples() >= threshold) {
		schedule(next);
		next += consumed;
		buf->setNext(next);
	}
}

/**
* Add sample data to the buffer.
*
* Description:\n
* 	Appends one packet of sample data to the end of the input sample buffer.
* 	The data is retained in integer format.  There must be enough room in
* 	the buffer.
* Notes:\n
* 	Only one thread at a time is allowed to write to the buffer; if there
* 	is more than one thread requesting to write, the one with the correct
* 	sample number will be selected first.  Many threads (workers) can be
* 	accessing the buffer to perform concurrent DFB's.\n
* 	The only locking necessary is when the input buffer indexes (first/
* 	last/next/done) are being updated.
*/
void
Beam::addSampleData(BeamPacket *pkt)
{
#if BEAM_TIMING
	uint64_t t0 = getticks();
#endif
	// flush only when necessary
	const ATADataPacketHeader& hdr = pkt->getHeader();
	if (buf->getFree() < hdr.len) {
		// need to lock the buffer while it is being flushed
		lock();
		// flush as much data as possible
		uint64_t done = buf->getDone();
		uint64_t next = buf->getNext();
		while (!pendingList.empty()) {
			PendingList::iterator p = pendingList.begin();
			// flush as many iterations as possible; an iteration can be
			// flushed when the DFB using its data has been completed.
			if (p->second) {
				if (done > p->first) {
					// we have a serious list problem; log the data and print
					cout << "done = " << done << ", next = " << next;
					cout << ", pfirst = " << p->first <<  ", psecond = " << p->second;
					cout << endl;
					cout << "list size = " << pendingList.size() << endl;
					Assert(done <= p->first);
				}
				Assert(p->first <= next);
				buf->setDone(done = p->first);
				pendingList.erase(p);
			}
			else
				break;
		}
		unlock();
		++flushes;
		// there'd better be room now
		if (buf->getFree() < hdr.len)
			Fatal(ERR_NBA);
	}
#if BEAM_TIMING
	uint64_t t1 = getticks();
#endif

	// copy the packet sample data to the input buffer.  Since the buffer
	// is a multiple of the packet sample length, there should always be
	// enough room to do the copy
	ComplexInt8 *s = static_cast<ComplexInt8 *> (pkt->getSamples());
	recordInputStats(s);
	ComplexInt8 *fData;
	int32_t len = hdr.len;
	if (decimation > 1) {
		// decimate the input by summing samples
		len /= decimation;
		fData = static_cast<ComplexInt8 *> (buf->getWrite(len));
		for (int32_t i = 0; i < len; ++i) {
			ComplexInt8 tmp(0, 0);
			for (int32_t j = 0; j < decimation; ++j)
				tmp += *s++;
			fData[i] = tmp;
		}
	}
	else {
		fData = static_cast<ComplexInt8 *> (buf->getWrite(len));
		memcpy(fData, s, pkt->getDataSize());
	}
#if BEAM_TIMING
	uint64_t t2 = getticks();
#endif
	sampleCnt += len;
	buf->setLast(len);
	inputSeq = hdr.seq + 1;
#if BEAM_TIMING
	uint64_t t3 = getticks();
	++timing.packets;
	float t = elapsed(t1, t0);
	if (t > timing.maxFlush)
		timing.maxFlush = t;
	timing.flush += t;
	timing.store += elapsed(t2, t1);
	timing.set += elapsed(t3, t2);
	timing.total += elapsed(t3, t0);
#endif
}

/**
 * Schedule a DFB iteration.
 *
 * Description:\n
 * 	Sends a message to the work queue specifying that a DFB iteration
 * 	should be performed beginning at the data sample specified.
 */
void
Beam::schedule(uint64_t sample)
{
	// add the starting sample to the pending list
	lock();
	pendingList.insert(std::make_pair(sample, false));
	unlock();
	packetInfo.sample = buf->getNext();
	packetInfo.hdr.seq = outputSeq++;
	packetInfo.hdr.freq = getFreq();
	float32_t fraction = 1.0 - beam.oversampling;
	packetInfo.hdr.sampleRate = (beam.bandwidth / fraction) / channels.total;
	packetInfo.hdr.usableFraction = fraction;
	packetInfo.hdr.absTime =
			ATADataPacketHeader::float96ToAbsTime(packetTiming.channel.time);
	packetTiming.channel.time += packetTiming.channel.secPerPacket;
	MemBlk *blk = partitionSet->alloc(sizeof(PacketInfo));
	Assert(blk);
	PacketInfo *pktInfo = static_cast<PacketInfo *> (blk->getData());
	*pktInfo = packetInfo;
	Msg *msg = msgList->alloc((DxMessageCode) DfbChannel, 0,
			pktInfo, sizeof(PacketInfo), blk);
	Error err = workQ->send(msg, 0);
	if (err)
		Fatal(err);

	++schedules;
}

/**
 * Create an empty packet to replace a missing packet
 *
 * Description:\n
 * 	Copies the header from the next "real" packet to the empty
 * 	packet, then sets the sequence number to the current sequence
 * 	number.
 */
void
Beam::createEmptyPacket(BeamPacket *empty, BeamPacket *pkt)
{
	++stats.netStats.missed;
	ATADataPacketHeader hdr = pkt->getHeader();
	hdr.seq = inputSeq;
	empty->putHeader(hdr);
	memset(pkt->getSamples(), 0, pkt->getDataSize());
}

/**
* Perform DFB filtering and channelization.
*
* Description:\n
*	Filters the input data using a digital filter bank (DFB), then
*	performs an FFT to produce subchannels.  Returns true if channelization
*	has been stopped.\n
* Notes:\n
*	Must lock each buffer during use, to ensure that flushing does
*	not occur while the buffer is in use.\n
*	Both left and right polarizations are processed by this call.
*/
void
Beam::dfbProcess(int32_t id, uint64_t sample, ComplexFloat32 *sampleBuf,
		ComplexFloat32 **buf_)
{
	Assert(id < (int32_t) dfbList.size());

#if BEAM_TIMING
	uint64_t t0 = getticks();
#endif
	// we must be channelizing; otherwise, just discard the list entry
	if (getState() != STATE_RUNNING) {
		lock();
		PendingList::iterator p = pendingList.find(sample);
		if (p == pendingList.end())
			Assert(p != pendingList.end());
		pendingList.erase(p);
		unlock();
		return;
	}

	// copy the data from the input buffer to the DFB buffer, converting
	// to floating point in the process.  Since the input buffer is
	// circular, it may be necessary to wrap around to the beginning
	// during the copy, so we have two loops.
	int32_t len = threshold;
	ComplexInt8 *samples = static_cast<ComplexInt8 *>
			(buf->getSampleBlk(sample, len));
	if (swap)
		getSwappedSamples(sampleBuf, samples, len);
	else
		getSamples(sampleBuf, samples, len);
//	samples = static_cast<complexInt8 *> (buf->getSampleBlk(sample, len));
	uint64_t s = sample + len;
	int32_t len2 = threshold - len;
	if (len2) {
		samples = static_cast<ComplexInt8 *> (buf->getSampleBlk(s, len2));
		if (swap)
			getSwappedSamples(sampleBuf + len, samples, len2);
		else
			getSamples(sampleBuf + len, samples, len2);
	}
#if BEAM_TIMING
	uint64_t t1 = getticks();
#endif

	// perform the DFB
	ComplexFloat32 *td[1];
	td[0] = sampleBuf;
	dfbList[id]->iterate((const ComplexFloat32 **) td, 1, threshold, buf_);
#if BEAM_TIMING
	uint64_t t2 = getticks();
#endif
	// the iteration is complete; now we can update the pending list
	// entry to show that this iteration is complete.
	lock();
	PendingList::iterator p = pendingList.find(sample);
	if (p == pendingList.end())
		Assert(p != pendingList.end());
	p->second = true;
	++dones;
	recordOutputStats(buf_);
	unlock();
#if BEAM_TIMING
	uint64_t t3 = getticks();
	++timing.dfb.dfbs;
	timing.dfb.load += elapsed(t1, t0);
	timing.dfb.dfb += elapsed(t2, t1);
	timing.dfb.list += elapsed(t3, t2);
	timing.dfb.total += elapsed(t3, t0);
#endif
	return;
}

/**
 * Compute and record input statistics
 */
void
Beam::recordInputStats(const ComplexInt8 *s)
{
	ComplexFloat64 val = ComplexFloat64(s[0].real(), s[0].imag());
	float64_t power = norm(val);
	float64_t min = norm(stats.inputStats.min);
	float64_t max = norm(stats.inputStats.max);
//	slock();
	++stats.inputStats.samples;
	if (power < min)
		stats.inputStats.min = val;
	if (power > max)
		stats.inputStats.max = val;
	stats.inputStats.sum += val;
	stats.inputStats.sumSq += power;
//	sunlock();
}

/**
 * Compute and record output statistics
 *
 * Description:\n
 * 	Uses the first sample from each output channel to compute the
 * 	running statistics for both the channel and the overall output
 * 	stream.\n
 * Notes:\n
 * 	The DFB produces output in normal FFT order, so the usable
 * 	channels are not sequential.  Instead, channels 0 - usable/2-1
 * 	are at indexes 0 - usable/2-1, while channels -usable/2 - -1
 * 	are at indexes total-usable/2 - total-1.
 */
void
Beam::recordOutputStats(ComplexFloat32 **s)
{
	// do the negative frequencies
	int32_t sIndex = getTotalChannels() - getUsableChannels() / 2;
	recordOutputStats(&s[sIndex], 0, getUsableChannels() / 2);
	// now the positive frequencies
	recordOutputStats(&s[0], getUsableChannels() / 2,
			(getUsableChannels() + 1) / 2);
}

void
Beam::recordOutputStats(ComplexFloat32 **s, int32_t index, int32_t n)
{
	for (int32_t i = 0; i < n; ++i) {
		ComplexFloat32 val = s[i][0];
		float64_t power = norm(val);
		float64_t min = norm(stats.outputStats.min);
		float64_t max = norm(stats.outputStats.max);
		// compute global statistics
		slock();
		++stats.outputStats.samples;
		if (power < min)
			stats.outputStats.min = val;
		if (power > max)
			stats.outputStats.max = val;
		stats.outputStats.sum += val;
		stats.outputStats.sumSq += power;
		sunlock();
		// compute channel statistics
		SampleStatistics& cStats = channelStats[index+i];
		min = norm(cStats.min);
		max = norm(cStats.max);
		slock();
		++cStats.samples;
		if (power < min)
			cStats.min = val;
		if (power > max)
			cStats.max = val;
		cStats.sum += val;
		cStats.sumSq += power;
		sunlock();
	}
}

void
Beam::getBeamStats(BeamStatistics& bs)
{
	slock();
	bs = stats;
	sunlock();
}

void
Beam::getChannelStats(SampleStatistics *cs)
{
	for (int32_t i = 0; i < getUsableChannels(); ++i) {
		slock();
		cs[i] = channelStats[i];
		sunlock();
	}
}

void
Beam::getNetStats(NetStatistics& ns)
{
	slock();
	ns = stats.netStats;
	sunlock();
}

void
Beam::getSamples(ComplexFloat32 *sampleBuf, ComplexInt8 *samples, int32_t len)
{
//	uint64_t t0 = getticks();
#ifdef notdef
//#if defined(__x86_64__) || defined(__SSE2__)
	int8_t *is = (int8_t *) samples;
	float32_t *os = (float32_t *) sampleBuf;
	// do fast conversion
	len *= 2;
	for (int32_t i = 0; i < len; i += 2) {
		register int a;
		register v4sf f;
		asm("movsx %1, %0" : "=r" (a) : "m" (is[i]));
		asm("cvtsi2ss %1, %0" : "=x" (f) : "r" (a));
		asm("movd %1, %0" : "=m" (os[i]) : "x" (f));
		asm("movsx %1, %0" : "=r" (a) : "m" (is[i+1]));
		asm("cvtsi2ss %1, %0" : "=x" (f) : "r" (a));
		asm("movd %1, %0" : "=m" (os[i+1]) : "x" (f));
	}
#else
	for (int32_t i = 0; i < len; ++i)
		sampleBuf[i] = samples[i];
#endif
//	uint64_t t1 = getticks();
//	++timing.packets;
//	timing.total += elapsed(t1, t0);
}

void
Beam::getSwappedSamples(ComplexFloat32 *sampleBuf, ComplexInt8 *samples,
		int32_t len)
{
//	uint64_t t0 = getticks();
#ifdef notdef
//#if defined(__x86_64__) || defined(__SSE2__)
	int8_t *is = (int8_t *) samples;
	float32_t *os = (float32_t *) sampleBuf;
	// do fast conversion
	len *= 2;
	for (int32_t i = 0; i < len; i += 2) {
		register int a;
		register v4sf f;
		asm("movsx %1, %0" : "=r" (a) : "m" (is[i]));
		asm("cvtsi2ss %1, %0" : "=x" (f) : "r" (a));
		asm("movd %1, %0" : "=m" (os[i+1]) : "x" (f));
		asm("movsx %1, %0" : "=r" (a) : "m" (is[i+1]));
		asm("cvtsi2ss %1, %0" : "=x" (f) : "r" (a));
		asm("movd %1, %0" : "=m" (os[i]) : "x" (f));
	}
#else
		for (int32_t i = 0; i < len; ++i)
			sampleBuf[i] = ComplexFloat32(samples[i].imag(), samples[i].real());
#endif
//	uint64_t t1 = getticks();
//	++timing.packets;
//	timing.total += elapsed(t1, t0);
}

/**
 * Set the channel packet start time.
 *
 * Description:\n
 * 	Given the starting time of the first beam packet, computes the starting
 *	time of the first channel packet vector; this time is then incremented
 *	by the packet time for all succeeding packets.\n
 *	It also sets the first packet starting time.
 */
void
Beam::setPacketTime(BeamPacket *pkt)
{
	const ATADataPacketHeader& hdr = pkt->getHeader();
	float96_t sec = hdr.absTime >> 32;
	float96_t fsec = (hdr.absTime & 0xffffffff) / exp2(32);
	packetTiming.beam.time = sec + fsec;
	float96_t dt = (getTotalChannels() * getFoldings()) *
			packetTiming.beam.secPerSample * 0.5;
	packetTiming.channel.time = packetTiming.beam.time + dt;
	timeval t = ATADataPacketHeader::absTimeToTimeval(hdr.absTime);
	NssDate nt;
	GetNssDate(nt, &t);
	setStartTime(nt);

}

}