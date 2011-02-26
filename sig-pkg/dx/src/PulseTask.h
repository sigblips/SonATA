/*******************************************************************************

 File:    PulseTask.h
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseTask.h,v 1.4 2009/03/07 19:15:56 kes Exp $
//
#ifndef _PulseTaskH
#define _PulseTaskH

#include <map>
#include "Activity.h"
//#include "Err.h"
#include "Msg.h"
#include "PulseClusterer.h"
//#include "Pulse.h"
#include "QTask.h"
#include "State.h"
#include "Struct.h"
#include "SuperClusterer.h"

//using dx_lib::Pulse;
using namespace sonata_lib;

namespace dx {

typedef uint64_t PulseKey;

#define PULSE_KEY(p)	((PulseKey) ((((PulseKey) p.res) << 48) \
						| (((PulseKey) p.bin) << 24) | ((PulseKey) p.spectrum)))

typedef map<PulseKey, dx::Pulse> PulseMap;
typedef vector<dx::Pulse> SliceList;

struct PulseArgs {
	Queue *detectionQ;					// detection task queue
#ifdef notdef
	SuperClusterer *superClusterer;		// super cluster object

	PulseArgs(): detectionQ(0), superClusterer(0) {}
	PulseArgs(Queue *detectionQ_, SuperClusterer *superClusterer_):
			detectionQ(detectionQ_), superClusterer(superClusterer_) {}
#else
	PulseArgs(): detectionQ(0) {}
	PulseArgs(Queue *detectionQ_):
			detectionQ(detectionQ_) {}
#endif
};

// comparison function for sort
static bool compareSpectra(const dx::Pulse& p0, const dx::Pulse& p1);

class PulseTask: public QTask {
public:
	static PulseTask *getInstance();
	~PulseTask();

private:
	static PulseTask *instance;

	bool badBand;						// current pulse slice is bad
	int32_t binsPerSlice;
	int32_t binsPerSpectrum;
	int32_t overlapBins;
	int32_t pulseLimit;
	int32_t slicePulses;
	int32_t slice;						// current slice
	int32_t slices;						// # of slices
	int32_t spectra;					// # of spectra in the observation
	int32_t startBin, endBin;
	int32_t tripletLimit;
	int32_t sliceTriplets;
	float32_t maxDrift;					// max # of bins drift / spectrum
	float32_t tripletThreshold;			// minimum triplet power
	float32_t singletonThreshold;		// singleton pulse threshold
	Resolution resolution;				// current resolution
	DetectionStatistics statistics;
	Activity *activity;					// current activity
	Channel *channel;					// channel
//	DxActivityParameters params;		// activity parameters
	PulseBadBandList *badBandList;		// bad band list for current res
	PulseClusterer *clusterer;
	PulseClusterer *pulseClusterer[MAX_RESOLUTIONS];
	Queue *inputQ;
	Queue *detectionQ;
	PulseMap pulseMap;					// complete list of pulses
	PulseMap singletonMap;				// map of singleton pulses
	SliceList sliceList;				// list of pulses for a single slice
	SuperClusterer *superClusterer;
										// (actually a vector)
	MsgList *msgList;
	State *state;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	void startDetection(Msg *msg);
	void stopDetection(Msg *msg);
	void detectPulseResolution();
	void buildPulseMap();
	void addPulse(PulseMap& map, const dx::Pulse& pulse);
	void processNextResolution(Msg *msg);
	void processSlice();
	void sendDetectionComplete();
	void extractSliceData();
	void findTriplets();
	void addTriplet(dx::Pulse& p0, dx::Pulse& p1, dx::Pulse& p2);
	Polarization getTripletPol(Polarization pol0, Polarization pol1,
			Polarization pol2);
	void buildTrains();
	bool insideDriftCone(dx::Pulse& p0, dx::Pulse& p2);
	bool tripletCheck(dx::Pulse& p0, dx::Pulse& p1, dx::Pulse& p2);
	bool aboutEqual(PulseDiff& d0, PulseDiff& d1);
	void sendSliceComplete();
	void sendResolutionComplete();
	void displayPulseMap();

	// hidden
	PulseTask(string name_);

	// forbidden
	PulseTask(const PulseTask&);
	PulseTask& operator =(const PulseTask&);

};

}

#endif