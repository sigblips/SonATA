/*******************************************************************************

 File:    FrequencyMask.h
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
// Frequency mask class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/FrequencyMask.h,v 1.4 2009/05/24 22:36:31 kes Exp $
//
#ifndef _FrequencyMaskH
#define _FrequencyMaskH

#include <sseDxInterface.h>
#include "Lock.h"

using namespace sonata_lib;

namespace dx {

//
// This is a pure base class for DxPermRfIMask, DxBirdieMask and
// DxRecentRfIMask.
//
// Notes:
//		Because a given mask may be used by more than one activity, a
//		use count must be maintained so that we know when to delete a
// 		mask.
//		Because a birdie mask may be shared by more than one
//		activity, each activity must keep track of where it
//		is in processing the mask.

class FrequencyMask {
public:
	FrequencyMask(int32_t nBands_, const FrequencyBand *mask_,
			const FrequencyBand& bandCovered_, const NssDate *versionDate_ = 0);
	~FrequencyMask();

	NssDate &getDate() { return (versionDate); }

	int getUseCount() { return (useCount); }
	int32_t getNumberOfBands() { return (nBands); }
	FrequencyBand *getFirst();				// return first frequency band
	FrequencyBand *getNext();				// return next frequency band
	FrequencyBand *getBand(int32_t idx_);	// return frequency band at idx_
	bool isMasked(float64_t frequency_, float64_t width_);

	int incrementUseCount();
	int decrementUseCount();

private:
	int32_t useCount;					// # of activities using
	int32_t current;					// current band index
	int32_t nBands;						// number of bands in array
	FrequencyBand bandCovered;			// total coverage of the mask
	FrequencyBand *mask;				// array containing the mask
	NssDate versionDate;				// date of the mask
    Lock mLock;                      // mutual exclusion lock

	void lock() { mLock.lock(); }
	void unlock() { mLock.unlock(); }

	// forbidden
	FrequencyMask(const FrequencyMask&);
	FrequencyMask& operator=(const FrequencyMask&);
};

}

#endif