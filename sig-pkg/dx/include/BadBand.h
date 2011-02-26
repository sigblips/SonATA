/*******************************************************************************

 File:    BadBand.h
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
// Bad band structures
//
#ifndef _BadBandH
#define _BadBandH

#include <iostream>
#include <sseDxInterface.h>
#include <SseMsg.h>
#include "Dadd.h"
#include "Lock.h"

using namespace dadd;
using namespace sonata_lib;
using std::endl;

namespace dx {

#ifdef notdef
struct DaddPath {
	int32_t bin;
	int32_t drift;
	int32_t power;

	friend ostream& operator << (ostream& s, const DaddPath& daddPath);
};
#endif

struct BadBand {
    int32_t bin;                // starting bin of the band
	int32_t width;				// width of the band in bins
	Polarization pol;			// polarization of the band

	friend ostream& operator << (ostream& s, const BadBand& band);
};

struct CwBadBand {
	BadBand band;				// bad band description
	int32_t paths;				// total # of paths
	int32_t maxPaths;			// maximum # of paths
	DaddPath maxPath;			// maximum path

	friend ostream& operator << (ostream& s, const CwBadBand& cwBand);
};

struct PulseBadBand {
	BadBand band;
	Resolution res;				// resolution of the band
	int32_t pulses;
	int32_t triplets;
	int32_t maxPulses;
	int32_t maxTriplets;
	bool tooManyTriplets;
	friend ostream& operator << (ostream& s, const PulseBadBand& pulseBand);

};

#ifdef notdef
/**
 * Bad band list base class
 */
class BadBandList {
public:
	BadBandList();
	~BadBandList();

	void setObsParams(float64_t baseFreq_, float64_t binWidth_,
			int32_t spectra_)
	{
		baseFreq = baseFreq_;
		binWidth = binWidth_;
		spectra = spectra_;
		obsLength = (spectra / 2) / binWidth;
	}

	float64_t binsToMHz(int32_t bins);
	float64_t binsToHz(int32_t bins);
	float64_t binsToAbsoluteMHz(int32_t bins);

	bool isComplete() { return (complete); }
	int32_t getSpectra() { return (spectra); }
	float64_t getBinWidth() { return (binWidth); }
	float64_t getObsLength() { return (obsLength); }

	virtual void clear();
	virtual void done() = 0;
	virtual int32_t getSize() = 0;
	virtual void print() = 0;

protected:
	Lock llock;

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

private:
	bool complete;
	int32_t spectra;
	float64_t baseFreq;
	float64_t binWidth;
	float64_t obsLength;

	// forbidden
	BadBandList(const BadBandList&);
	BadBandList& operator=(BadBandList&);
};
#endif
}
#endif