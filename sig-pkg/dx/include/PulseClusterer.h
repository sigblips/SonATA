/*******************************************************************************

 File:    PulseClusterer.h
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
// PD Clustering object
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/PulseClusterer.h,v 1.1 2009/03/06 21:52:00 kes Exp $
//
#ifndef _DxPDClusterH
#define _DxPDClusterH

#include <map>
#include <vector>
#include <sseDxInterface.h>
#include "ChildClusterer.h"

using std::map;
using std::multimap;
using std::pair;
using std::vector;
using namespace sonata_lib;

namespace dx {

class SuperClusterer;

class PulseClusterer : public ChildClusterer {
public:
	// Create a Pulse clustering object
	PulseClusterer(SuperClusterer *parent, Resolution res);
	~PulseClusterer();

	void setClusterRange(int32_t bins) { clusterRange = bins; }
	void setResolution(Resolution res) { resolution = res; }
	void setBins(int32_t bins_) { bins = bins_; }
	void setSpectra(int32_t spectra_) {
		spectra = spectra_;
		setSpectraPerObs(spectra);
	}
	void setPulseThreshold(float32_t pulseThreshold_)
	{
		pulseThreshold = pulseThreshold_;
	}

	// Hit recording
	void recordTriplet(int spectrum1, int bin1, float32_t power1,
			Polarization pole1, int spectrum2, int bin2, float32_t power2,
			Polarization pole2, int spectrum3, int bin3, float32_t power3,
			Polarization pole3);
	void allHitsLoaded();
	int getTriplets();
	void clearHits();

	// cluster information access
	int getCount();
	float64_t getNominalFreq(int);
	bool isStronger(int myIndex, ClusterTag *);

	PulseClusterer *getPulse();
	PulseSignalHeader & getNth(int);

private:
	struct Pulse
	{
		int spectrum, bin;
		float32_t power;
		Polarization pole;
		// default ctor needed for Train::addPulse()
		Pulse()
			: spectrum(0)
			, bin(0)
			, power(0)
			, pole(POL_MIXED)
		{}
	};
	static const int TSZ = 3;
	struct Triplet
	{
		float32_t startBin;
		float32_t drift;
		Pulse pulses[TSZ];

		Triplet(): startBin(0), drift(0) {}
	};
	typedef multimap<float,Triplet> TripletList;
	struct Counter
	{
		int val;
		Counter()
			: val(0)
		{}
	};
	struct Train
	{
		float loBin, hiBin;
		map<int,Pulse> pulses;
		map<int,Counter> histogram;
		void addPulse(const Pulse &);
		Train(): loBin(0), hiBin(0) {}
	};
	typedef vector<PulseSignalHeader *> ClusterList;


	// disallow
	PulseClusterer(const PulseClusterer&);
	PulseClusterer& operator=(const PulseClusterer&);

	// service functions
	void initLR();
	void addPulseToLR(const Pulse &);
	bool getLRResult(float32_t *startBin, float32_t *drift);
	bool absorb(Train &cluster, const pair<float, Triplet>&i);
	void clusterDone(Train &);
	float32_t computePfa(int32_t n, float32_t power);

	// configuration
	Resolution resolution;
	int32_t bins, spectra;
	float32_t pulseThreshold;
	int clusterRange;

	// clustering
	int bogusTriplets;
	TripletList tripletList;
	ClusterList clusterList;

	// linear regression
	double sumx, sumxx, sumy, sumxy;
	int pulseCnt;
};

}

#endif