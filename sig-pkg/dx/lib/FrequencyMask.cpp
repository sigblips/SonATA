/*******************************************************************************

 File:    FrequencyMask.cpp
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
// DX frequency mask class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/FrequencyMask.cpp,v 1.4 2009/05/24 23:41:53 kes Exp $
//
#include "Err.h"
#include "FrequencyMask.h"

namespace dx {

FrequencyMask::FrequencyMask(int32_t nBands_, const FrequencyBand *mask_,
		const FrequencyBand& bandCovered_, const NssDate *versionDate_):
		useCount(1), current(-1), nBands(nBands_), bandCovered(bandCovered_),
		mask(0), mLock("fmask")
{
	int i;

	if (versionDate_)
		versionDate = *versionDate_;
	else
		memset(&versionDate, 0, sizeof(versionDate));
	// allocate and fill a new mask
	mask = new FrequencyBand[nBands];
	for (i = 0; i < nBands; i++) {
		mask[i] = mask_[i];
		Debug(DEBUG_FREQ_MASK, mask[i].centerFreq, "mask cf");
		Debug(DEBUG_FREQ_MASK, mask[i].bandwidth, "mask bw");
	}
}

FrequencyMask::~FrequencyMask()
{
	delete [] mask;
}

FrequencyBand *
FrequencyMask::getFirst()
{
	current = 0;
	return (&mask[0]);
}

FrequencyBand *
FrequencyMask::getNext()
{
	return (getBand(++current));
}

FrequencyBand *
FrequencyMask::getBand(int32_t cur_)
{
	FrequencyBand *band = 0;

	lock();
	if (cur_ >= 0 && cur_ < nBands)
		band = &mask[cur_];
	unlock();
	return band;
}

//
// Notes:
//		This function assumes that the mask is ordered by
//		ascending frequency, and that there are no overlaps.
//		A binary search would be much faster.
//
bool
FrequencyMask::isMasked(float64_t frequency_, float64_t width_)
{
	bool rval = false;
	int i;
	float64_t loFreq, hiFreq, loMask, hiMask;

	loFreq = frequency_ - width_ / 2;
	hiFreq = frequency_ + width_ / 2;
	Debug(DEBUG_FREQ_MASK, frequency_, "signal freq");
	Debug(DEBUG_FREQ_MASK, loFreq, "low signal freq");
	Debug(DEBUG_FREQ_MASK, hiFreq, "high signal freq");

	lock();
	for (i = 0; i < nBands; i++) {
		loMask = mask[i].centerFreq - mask[i].bandwidth / 2;
		hiMask = mask[i].centerFreq + mask[i].bandwidth / 2;
		Debug(DEBUG_FREQ_MASK, loMask, "low mask freq");
		Debug(DEBUG_FREQ_MASK, hiMask, "high mask freq");

		// if we've passed it in the list, it's not there
		if (hiFreq < loMask)
			break;
		if (loFreq <= hiMask && hiFreq >= loMask) {
			rval = true;
			break;
		}

	}
	unlock();
	return (rval);
}

int
FrequencyMask::incrementUseCount()
{
	lock();
	useCount++;
	unlock();
	return (useCount);
}

int
FrequencyMask::decrementUseCount()
{
	lock();
	useCount--;
	unlock();
	return (useCount);
}

}