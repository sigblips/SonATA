/*******************************************************************************

 File:    Threshold.cpp
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

/*
 * Threshold.cpp
 *
 *  Created on: Feb 27, 2009
 *      Author: kes
 */

#include "Dadd.h"

namespace dadd {

/**
 * Threshold the data to find hits.
 *
 * Description:\n
 * 	Subtracts the threshold from the entire array of row/drift
 * 	path sums, using an unsigned saturated subtract.  The result
 * 	is an array in which each element is either zero or the amount
 * 	over threshold.  The latter are hits.
 *
 */
void
Dadd::thresholdData(DaddAccum *data)
{
	uint16_t thr = (uint16_t) threshold;
	v8wu tv = (v8wu) { thr, thr, thr, thr, thr, thr, thr, thr};
	DaddAssert(!(spectrumBins % VECTOR_LEN));
	DaddAssert(!(totalBins % VECTOR_LEN));
	int32_t rowLen = totalBins / VECTOR_LEN;
	int32_t usableLen = spectrumBins / VECTOR_LEN;
	v8wu *dp = (v8wu *) data;
	for (int32_t i = 0; i < spectra; ++i, dp += rowLen) {
		for (int32_t j = 0; j < usableLen; ++j)
			dp[j] = (v8wu) __builtin_ia32_psubusw128((v8wi) dp[j], (v8wi) tv);
	}
}

}