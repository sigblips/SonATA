/*******************************************************************************

 File:    PulseTask.cpp
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
// Pulse detection task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseTask.cpp,v 1.7 2009/05/24 23:07:20 kes Exp $
//
#include <algorithm>
#include <DxOpsBitset.h>
#include "DxErr.h"
#include "System.h"
#include "PulseTask.h"

using std::cout;
using std::endl;

namespace dx {

PulseTask *PulseTask::instance = 0;

PulseTask *
PulseTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new PulseTask("PulseTask");
	l.unlock();
	return (instance);
}


PulseTask::PulseTask(string name_): QTask(name_, PULSE_PRIO),
		badBand(false), binsPerSlice(0), binsPerSpectrum(0), overlapBins(0),
		pulseLimit(0), slicePulses(0), slice(0), slices(0), spectra(0),
		startBin(0), endBin(0), tripletLimit(0), sliceTriplets(0),
		maxDrift(0), tripletThreshold(0), singletonThreshold(0),
		resolution(RES_UNINIT), activity(0), channel(0), badBandList(0),
		clusterer(0), inputQ(0), detectionQ(0), superClusterer(0),
		msgList(0), state(0)
{
	for (int32_t i = 0; i < MAX_RESOLUTIONS; ++i)
		pulseClusterer[i] = 0;
	pulseMap.clear();
	singletonMap.clear();
	sliceList.clear();
}

PulseTask::~PulseTask()
{
}

void
PulseTask::extractArgs()
{
#if ASSIGN_CPUS
	// assign the task processor affinity in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	cpu_set_t affinity;
	CPU_ZERO(&affinity);
	int32_t n = 0;
	if (nCpus > 2) {
		// remove affinity for cpu 1
		++n;
	}
	if (nCpus > 3) {
		// remove affinity for cpu 2
		++n;
	}
	// assign affinity
	for (int32_t i = n; i < nCpus; ++i)
		CPU_SET(i, &affinity);
	pid_t tid = gettid();
	int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
	Assert(rval >= 0);
#endif

	// extract arguments
	PulseArgs *pulseArgs = static_cast<PulseArgs *> (args);
	Assert(pulseArgs);
	detectionQ = pulseArgs->detectionQ;
	Assert(detectionQ);
#ifdef notdef
	superClusterer = pulseArgs->superClusterer;
	Assert(superClusterer);
#endif

	msgList = MsgList::getInstance();
	Assert(msgList);
	state = State::getInstance();
	Assert(state);

	inputQ = getInputQueue();
#ifdef notdef
	for (int i = 0; i < MAX_RESOLUTIONS; ++i)
		pulseClusterer[i] = new PulseClusterer(superClusterer, (Resolution) i);
#endif
}

void
PulseTask::handleMsg(Msg *msg)
{
	Debug(DEBUG_PD, (int32_t) msg->getCode(), "msg code");
	switch (msg->getCode()) {
	case StartDetection:
		startDetection(msg);
		break;
	case SliceComplete:
		processSlice();
		break;
	case ResolutionComplete:
		processNextResolution(msg);
		break;
	case STOP_DX_ACTIVITY:
		stopDetection(msg);
		break;
	default:
		FatalStr((int32_t) msg->getCode(), "msg code");
		break;
	}
}

//
// startDetection: start pulse detection
//
void
PulseTask::startDetection(Msg *msg)
{
	Debug(DEBUG_PD, (void *) activity, "current act");
	// activity must exist and be in correct state
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;

	Debug(DEBUG_PD, (void *) act, "act");
	Debug(DEBUG_PD, act->getActivityId(), "actid");

	if (act->getState() == DX_ACT_PEND_SD)
		act->setState(DX_ACT_RUN_SD);

	if (act->getState() != DX_ACT_RUN_SD) {
		LogError(ERR_IAS, act->getActivityId(), "act %d, state %d",
				act->getActivityId(), act->getState());
		return;
	}

	// record the activity and get its parameters
	activity = act;
	channel = activity->getChannel();
	Assert(channel);
	const DxActivityParameters& params = activity->getActivityParams();
	activity->getDetectionStatistics(statistics);

	DxOpsBitset operations(params.operations);
	if (!operations.test(PULSE_DETECTION)) {
		sendDetectionComplete();
		return;
	}

	Debug(DEBUG_PD, 0, "building pulse list");

	// set up clustering
	superClusterer = activity->getSuperClusterer();
	Assert(superClusterer);
	Assert(!superClusterer->getCount());

	for (int i = 0; i < MAX_RESOLUTIONS; ++i)
		pulseClusterer[i] = new PulseClusterer(superClusterer, (Resolution) i);

	singletonThreshold = params.pd[RES_1HZ].singletThreshold;
	buildPulseMap();
	Debug(DEBUG_PD, (int32_t) pulseMap.size(), "built pulse list, count");

	// find the first resolution
	int32_t i;
	for (i = 0; i < MAX_RESOLUTIONS && !params.requestPulseResolution[i]; ++i)
		;
	resolution = (Resolution) i;
	if (i < MAX_RESOLUTIONS)
		detectPulseResolution();
	else
		sendDetectionComplete();
}

/**
 * Process the next requested resolution.
 *
 * Description:\n
 * 	Called via message when the previous resolution has been completed.
 * 	Searches the resolution array for the next resolution to detect.
 */
void
PulseTask::processNextResolution(Msg *msg)
{
	const DxActivityParameters& params = activity->getActivityParams();
	int32_t i = (int) resolution;
	while (++i < MAX_RESOLUTIONS && !params.requestPulseResolution[i])
		;
	if (i < MAX_RESOLUTIONS) {
		resolution = (Resolution) i;
		detectPulseResolution();
	}
	else
		sendDetectionComplete();
}

//
// detectPulseResolution: perform pulse detection for a single
//		resolution
void
PulseTask::detectPulseResolution()
{
	// initialize the bad band list
	badBandList = activity->getPulseBadBandList(resolution);
	Assert(badBandList);

	badBandList->clear();
	badBandList->setObsParams(channel->getLowFreq(),
			activity->getBinWidthHz(resolution),
			activity->getSpectra(resolution));

	const DxActivityParameters& params = activity->getActivityParams();

	// initialize the clusterer
	clusterer = pulseClusterer[(int) resolution];
	clusterer->clearHits();
	clusterer->setClusterRange(params.pulseClusteringDeltaFreq);
	clusterer->setResolution(resolution);
	clusterer->setBinWidth(activity->getBinWidthHz(resolution));
	clusterer->setBins(activity->getUsableBinsPerSpectrum(resolution));
	clusterer->setSpectra(activity->getSpectra(resolution));
	clusterer->setPulseThreshold(params.pd[(int) resolution].pulseThreshold);

	tripletThreshold = params.pd[(int) resolution].tripletThreshold;
	singletonThreshold = params.pd[(int) resolution].singletThreshold;

	//
	// compute the number of bins per slice:
	// (1) Use the maximum drift rate in Hz/s and the length
	//	of the observation to compute the
	//	maximum total drift of a candidate signal in Hz.
	// (2) Use the width of a bin in Hz to compute the total number
	//	of bins the signal can drift during the observation, and round
	//	it up by one.  This is the overlap width in bins.
	// (3) The total slice width is the basic slice width for the
	//	resolution (in bins) plus the overlap width in bins.
	// (4) Compute the number of slices as the number of bins per
	//	spectrum at the specified resolution divided by the basic slice
	//	width.
	//
#if COMPUTE_MAX_DRIFT
	float32_t maxTotalDrift = activity->getDataCollectionTime() * MAX_DRIFT;
	maxTotalDrift *= MHZ_TO_GHZ(activity->getSkyFreq());
	float64_t binWidth = activity->getBinWidthHz(resolution);
	overlapBins = (int32_t) (maxTotalDrift / binWidth + 1);
#else
	overlapBins = activity->getSpectra(resolution);
#endif
//	float64_t spectrumTime = activity->getSpectrumTime(resolution);
//	cout << "spectrum time = " << spectrumTime;
//	cout << ", bin width = " << binWidth;
//	cout << ", max drift = " << binWidth / spectrumTime << endl;
//	cout << "bin width = " << binWidth << " Hz, overlap bins ";
//	cout << overlapBins << endl;
	binsPerSpectrum = activity->getUsableBinsPerSpectrum(resolution);
	binsPerSlice = activity->getBinsPerPulseSlice(resolution);
	float64_t kHzPerSlice = HZ_TO_KHZ(binsPerSlice
			* activity->getBinWidthHz(resolution));
//	cout << "bins per spectrum " << binsPerSpectrum;
//	cout << ", bins per slice " << binsPerSlice << endl;
	slices = binsPerSpectrum / binsPerSlice;
	if (slices * binsPerSlice < binsPerSpectrum)
		++slices;
	spectra = activity->getSpectra(resolution);
//	cout << "slices " << slices << ", spectra " << spectra << endl;
#if COMPUTE_MAX_DRIFT
	maxDrift = MAX_DRIFT
			/ (binWidth / activity->getSpectrumTime(resolution));
	maxDrift *= MHZ_TO_GHZ(activity->getSkyFreq());
#else
	maxDrift = 1;
#endif
//	cout << "max drift " << maxDrift << " bins per spectrum" << endl;

	// compute the pulse and triplet limits limit
	pulseLimit = (int32_t) (params.badBandPulseLimit * kHzPerSlice + 0.5);
	tripletLimit = (int32_t) (params.badBandPulseTripletLimit * kHzPerSlice
			+ 0.5);

//	cout << "pulse limit " << pulseLimit << endl;
//	cout << "triplet limit " << tripletLimit << endl;

	Debug(DEBUG_PD, pulseLimit, "pulse limit")
	Debug(DEBUG_PD, tripletLimit, "triplet limit");


	slice = 0;
	processSlice();
}

void
PulseTask::stopDetection(Msg *msg)
{
	// if no activity is in collection state, ignore the message
	if (!activity)
		return;

	// if the activity ID doesn't match, ignore the message
	if (activity->getActivityId() != msg->getActivityId())
		return;

	// we're either stopping any collecting activity or the one
	// which is currently collecting
	activity->setState(DX_ACT_STOPPING);
	sendDetectionComplete();
}

void
PulseTask::sendDetectionComplete()
{
	statistics.triplets = 0;
	statistics.pulseClusters = 0;
	statistics.pulseTrains = 0;

	// perform clustering and bad band processing for all resolutions
	PulseBadBandList *badBandList;
	for (int i = 0; i < MAX_RESOLUTIONS; ++i) {
		pulseClusterer[i]->allHitsLoaded();
		statistics.triplets += pulseClusterer[i]->getTriplets();
		statistics.pulseClusters += pulseClusterer[i]->getCount();
		statistics.pulseTrains += statistics.pulseClusters;
		badBandList = activity->getPulseBadBandList((Resolution) i);
		Assert(badBandList);
		badBandList->done();
	}

	activity->setDetectionStatistics(statistics);

	Msg *dMsg = msgList->alloc((DxMessageCode) PulseComplete,
			activity->getActivityId());
	dMsg->setUnit((sonata_lib::Unit) UnitPulse);
	detectionQ->send(dMsg);
	activity = 0;
}

/**
 * Build a map of the complete set of pulses for the observation.
 *
 * Description:\n
 * 	The raw set of pulses is in the PulseList maintained by the Channel.
 * 	We build a map of pulses, combining polarizations.  The map key
 * 	is (res, bin, spectrum); this allows us to (a) process resolutions
 * 	easily in sequential order and (b) to slice the data for a given
 * 	resolution to minimize the pulse train search.
 */
void
PulseTask::buildPulseMap()
{
	// create a map from the original vector
	pulseMap.clear();
	PulseList& pulseList = channel->getPulseList();
	for (PulseList::iterator p = pulseList.begin(); p != pulseList.end(); ++p) {
		dx::Pulse pulse = *p;
		if (pulse.pol == POL_LEFTCIRCULAR)
			++statistics.leftPulses;
		else
			++statistics.rightPulses;
		++statistics.totalPulses;

		// add the pulse to the pulse map
		addPulse(pulseMap, pulse);
	}
//	cout << "Pulse count: " << pulseMap.size() << endl;

	// pulse map is complete; now look for singleton pulses
	singletonMap.clear();
	for (PulseMap::iterator mp = pulseMap.begin(); mp != pulseMap.end(); ++mp) {
		dx::Pulse pulse = mp->second;
		if (pulse.power > singletonThreshold) {
			PulseKey pulseKey = PULSE_KEY(pulse);
			PulseMap::iterator sp = singletonMap.begin();
			singletonMap.insert(pair<PulseKey, dx::Pulse> (pulseKey, pulse));
		}
	}
}

/**
 * Add a pulse to a pulse map.
 *
 * Description:\n
 * 	Adds a pulse to the specified pulse map.  If the pulse is already
 * 	in the map with the same polarization, an error message is sent
 * 	and the pulse is discarded.  If the pulse is in the map with the
 * 	other polarization, the map pulse is made dual-polarization and
 * 	the powers are combined.
*/
void
PulseTask::addPulse(PulseMap& map, const dx::Pulse& pulse)
{
	PulseKey pulseKey = PULSE_KEY(pulse);
	PulseMap::iterator mp;
	// if the pulse is already in the map, then add the second
	// polarization and sum the powers
	if ((mp = map.find(pulseKey)) != map.end()) {
		// pulse is already in the map
		if (mp->second.pol == pulse.pol) {
			// if this is the same polarization, something is wrong; log
			// the error and discard the pulse
			LogError(ERR_DPF, activity->getActivityId(),
					"duplicate pol pulse", pulseKey);
		}
		else {
			// combine the pulses
			dx::Pulse p(pulse);
			p.power += mp->second.power;
			p.pol = POL_BOTH;
			mp->second = p;
		}
	}
	else
		map.insert(pair<PulseKey, dx::Pulse> (pulseKey, pulse));
}

/**
 * Process a frequency-based slice of pulse data.
 *
 * Description:\n
 * 	Processes a single slice of pulse data, looking for triplets.
 */
void
PulseTask::processSlice()
{
	// reset the bad band flag
	badBand = false;

	extractSliceData();

	findTriplets();
	buildTrains();
	// if this was a bad band, record it
	if (badBand) {
		dx::PulseBadBand badBand;

		badBand.band.bin = startBin;
		badBand.band.width = binsPerSlice;
		badBand.band.pol = POL_BOTH;
		badBand.res = resolution;
		badBand.maxPulses = pulseLimit;
		badBand.maxTriplets = tripletLimit;
		badBand.pulses = slicePulses;
		badBand.triplets = sliceTriplets;
		badBand.tooManyTriplets = sliceTriplets > tripletLimit;

		badBandList->record(badBand);

//		cout << "triplets processed " << sliceTriplets << endl;

	}

	if (++slice < slices)
		sendSliceComplete();
	else
		sendResolutionComplete();
}

//
// extract slice data: extract the pulses from a block of frequency
//		in the observation data
//
// Notes:
//		We have to take frequency slices to handle pulse trains
//		which cross the boundaries.
//		The data is stored in a vector to allow indexed access
//		to pulses during the triplet-detection process.
//		The pulses must be sorted by spectrum number in order
//		for the detection algorithm to work.
//
void
PulseTask::extractSliceData()
{
	Debug(DEBUG_PD, (void *) activity, "activity");
	Debug(DEBUG_PD, slice, "slice");
	Debug(DEBUG_PD, slices, "slices");
	Debug(DEBUG_PD, binsPerSpectrum, "binsPerSpectrum");
	Debug(DEBUG_PD, binsPerSlice, "binsPerSlice");

	// compute the start and end bins
	int32_t startBin = slice * binsPerSlice;
	int32_t extractStartBin = startBin - overlapBins;
	if (extractStartBin < 0)
		extractStartBin = 0;
	endBin = (slice + 1) * binsPerSlice;
	if (endBin > binsPerSpectrum)
		endBin = binsPerSpectrum;
	int32_t extractEndBin = endBin + overlapBins;
	if (extractEndBin > binsPerSpectrum)
		extractEndBin = binsPerSpectrum;
//	cout << "start bin " << startBin << ", end bin " << endBin << endl;
	sliceList.clear();
	slicePulses = 0;

	Debug(DEBUG_PD, startBin, "start bin");
	Debug(DEBUG_PD, endBin, "end bin");

	// create a dummy pulse
	dx::Pulse pulse(resolution, extractStartBin, 0, POL_RIGHTCIRCULAR);
	PulseKey pulseKey = PULSE_KEY(pulse);

	// copy to a vector all bins which are within the slice
	PulseMap::iterator pos;
	for (pos = pulseMap.lower_bound(pulseKey); pos != pulseMap.end(); ++pos) {
		pulse = pos->second;
		if (pulse.res == resolution && pulse.bin < extractEndBin) {
			// count all pulses, but only store to limit
			if (slicePulses++ < pulseLimit)
				sliceList.push_back(pulse);
			else
				badBand = true;
		}
		else
			break;
	}
	// sort the slice by spectrum, without scrambling the bins
	stable_sort(sliceList.begin(), sliceList.end(), compareSpectra);
}

//
// findTriplets: look through the entire array for triplets
//
// Notes:
//		Each triplet found is added to the PulseClusterer hit list.
//		This version processes the entire pulse list at once, rather
//		than breaking the list up into frequency slices.
//
void
PulseTask::findTriplets()
{
	dx::Pulse pulse0, pulse1, pulse2;

	sliceTriplets = 0;
	for (uint32_t i = 0; i < sliceList.size(); ++i) {
		pulse0 = sliceList[i];
//		cout << "pulse 0 " << pulse0;
		if (pulse0.bin >= startBin && pulse0.bin < endBin) {
			for (uint32_t k = i + 2; k < sliceList.size(); ++k) {
				pulse2 = sliceList[k];
				if (insideDriftCone(pulse0, pulse2)) {
#ifdef notdef
					cout << "got outside pair" << endl;
					cout << pulse0;
					cout << pulse2;
#endif
					for (uint32_t j = i + 1; j < k; ++j) {
						pulse1 = sliceList[j];
						if (tripletCheck(pulse0, pulse1, pulse2)) {
							addTriplet(pulse0, pulse1, pulse2);
#ifdef notdef
							cout << endl << "add a triplet" << endl;
							cout << pulse0;
							cout << pulse1;
							cout << pulse2;
#endif
						}
					}
				}
			}
		}
	}
}

void
PulseTask::addTriplet(dx::Pulse& pulse0, dx::Pulse& pulse1,
		dx::Pulse& pulse2)
{
	float32_t power = pulse0.power + pulse1.power + pulse2.power;
//	cout << "triplet power = " << power << ", thresh = " << tripletThreshold;
//	cout << endl;
	if (power > tripletThreshold) {
		Polarization pol = getTripletPol(pulse0.pol, pulse1.pol,
				pulse2.pol);
		if (sliceTriplets++ < tripletLimit || !badBand) {
			if (sliceTriplets > tripletLimit)
				badBand = true;
			Debug(DEBUG_NEVER, (int32_t) pol, "triplet pol");
//			cout << "add a triplet" << endl;
//			cout << "pulse0 " << pulse0;
//			cout << "pulse1 " << pulse1;
//			cout << "pulse2 " << pulse2;

			clusterer->recordTriplet(pulse0.spectrum, pulse0.bin,
				pulse0.power, pulse0.pol, pulse1.spectrum,
				pulse1.bin, pulse1.power, pulse1.pol,
				pulse2.spectrum, pulse2.bin, pulse2.power,
				pulse2.pol);
		}
	}
}

Polarization
PulseTask::getTripletPol(Polarization pol0, Polarization pol1,
		Polarization pol2)
{
	if (pol0 != pol1 || pol0 != pol2 || pol1 != pol2)
		return (POL_MIXED);
	return (pol0);
}


void
PulseTask::buildTrains()
{
}

bool
PulseTask::insideDriftCone(dx::Pulse& pulse0, dx::Pulse& pulse1)
{
	PulseDiff d(pulse0, pulse1);
	if (d.spectra < 2 * MIN_DELTA_SPECTRA)
		return (false);

	float32_t drift = (float32_t) d.bins / d.spectra;
	return (fabs(drift) < maxDrift);
}

bool
PulseTask::tripletCheck(dx::Pulse& pulse0, dx::Pulse& pulse1,
		dx::Pulse& pulse2)
{
	PulseDiff d0(pulse0, pulse1);
	PulseDiff d1(pulse1, pulse2);

	return (aboutEqual(d0, d1));
}

bool
PulseTask::aboutEqual(PulseDiff& d0, PulseDiff& d1)
{
	if (d0.spectra <= 0 || d1.spectra <= 0)
		return (false);
	if (d0.spectra < MIN_DELTA_SPECTRA || d1.spectra < MIN_DELTA_SPECTRA)
		return (false);

	int32_t dBins = abs(d0.bins - d1.bins);
	int32_t dSpectra = abs(d0.spectra - d1.spectra);
	return (dBins <= MAX_DIFF_BINS && dSpectra <= MAX_DIFF_SPECTRA);
}

void
PulseTask::sendSliceComplete()
{
	Msg *iMsg = msgList->alloc((DxMessageCode) SliceComplete,
			activity->getActivityId());
	inputQ->send(iMsg);
}

void
PulseTask::sendResolutionComplete()
{
	Msg *iMsg = msgList->alloc((DxMessageCode) ResolutionComplete,
			activity->getActivityId());
	inputQ->send(iMsg);
}

void
PulseTask::displayPulseMap()
{
	cout << "displaying combined pulse map, pulses = " << pulseMap.size();
	cout << endl;
	for (PulseMap::iterator p = pulseMap.begin(); p != pulseMap.end(); ++p)
		cout << p->second;
}

//
// compareSpectra: return whether the first pulse is at a smaller
//		spectrum than the second
//
static bool
compareSpectra(const dx::Pulse& p0, const dx::Pulse& p1)
{
	return (p0.spectrum <= p1.spectrum);
}

}
