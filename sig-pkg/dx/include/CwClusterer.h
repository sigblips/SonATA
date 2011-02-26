/*******************************************************************************

 File:    CwClusterer.h
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
// Cw Clustering object
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/CwClusterer.h,v 1.2 2009/05/24 22:33:39 kes Exp $
//
#ifndef _CwClustererH
#define _CwClustererH

#include <map>
#include <vector>
#include <sseDxInterface.h>
#include "ChildClusterer.h"

using std::multimap;
using std::pair;
using std::vector;
using namespace sonata_lib;

namespace dx {

// forward declaraton
class SuperClusterer;

class CwClusterer : public ChildClusterer {
public:

	// Create a Cw clustering object
	CwClusterer(SuperClusterer *parent, Polarization pole);
	~CwClusterer();

	// control
	void setClusterRange(int32_t bins);

	// Hit recording
	void recordHit(int startBin, int drift, int power);
	void allHitsLoaded();
	int getHits();
	void clearHits();

	// cluster information access
	int getCount();
	float64_t getNominalFreq(int);
	bool isStronger(int myIndex, ClusterTag *);

	CwClusterer *getCw();
	CwPowerSignal& getNth(int);

private:

	struct HitReport
	{
		int startBin, drift, power;

		HitReport(): startBin(0), drift(0), power(0) {}
		HitReport(int startBin_, int drift_, int power_): startBin(startBin_),
				drift(drift_), power(power_) {}
	};
	typedef multimap<float,HitReport> HitList;
	struct Cluster
	{
		HitReport hr;
		float loBin, hiBin;

		Cluster(): loBin(0), hiBin(0) {}
	};
	typedef vector<CwPowerSignal> ClusterList;

	// disallow
	CwClusterer(CwClusterer&);
	CwClusterer& operator=(CwClusterer&);

	// service routines
	bool absorb(Cluster &, const pair<float,HitReport>&);
	void clusterDone(Cluster &);

	// clustering configuration data
	int clusterRange;	// in bins

	// observation data
	Polarization pole;

	// hits to be clustered
	HitList hitList;

	// results
	ClusterList clusterList;
};

}

#endif