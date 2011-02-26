/*******************************************************************************

 File:    DaddSum.cpp
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
 * DaddSum.cpp
 *
 *  Created on: Feb 24, 2009
 *      Author: kes
 */
#include "Dadd.h"

namespace dadd {

/**
 * Add a pair of row vectors with shift.
 *
 * Description:\n
 * 	Adds a pair of rows of drift k from the upper and lower blocks.  The
 * 	drift 2k sum is stored in the lower block, while the drift 2k+1 sum
 *  is stored in the upper block, shifted by one bin.\n
 * Notes:\n
 * 	The adds are performed using vector operations, 4 bins at a time.\n
 *
 * @param	drift the number of bins of accumulated drift for the two rows.
 * @param	bins the number of bins in a row.
 * @param	lower the lower row of bins.
 * @param	upper the upper row of bins.
 */
void
Dadd::pairSum(int32_t drift, int32_t bins, DaddAccum *lower, DaddAccum *upper)
{
#if (DADD_TIMING)
	uint64_t t0 = getticks();
#endif
	int32_t rowLen = bins / VECTOR_LEN;
	int32_t usableLen = (bins - (drift + 1)) / VECTOR_LEN;

	// pointers to the row data
	v8wu *lrw = (v8wu *) lower;
	v8wu *uw = (v8wu *) upper;
	v8wu *ur = (v8wu*) (upper + drift);

	// load upper(n) before entering the loop
	register v8wu l, ul, uh, u1;
	ul = (v8wu) __builtin_ia32_loaddqu((const char *) &ur[0]);

	// main loop
	int32_t i;
	for (i = 0; i < usableLen; ++i) {
		// get lower[n]
		l = lrw[i];
		// get drift 2k
		lrw[i] = (v8wu) __builtin_ia32_paddusw128((v8wi) l, (v8wi) ul);

		u1 = uh = (v8wu) __builtin_ia32_loaddqu((const char *) &ur[i+1]);
		ul = (v8wu) __builtin_ia32_psrldqi128((v2di) ul, 2 * 8);
		u1 = (v8wu) __builtin_ia32_pslldqi128((v2di) u1, 14 * 8);
		ul = (v8wu) __builtin_ia32_paddusw128((v8wi) ul, (v8wi) u1);
		uw[i] = (v8wu) __builtin_ia32_paddusw128((v8wi) l, (v8wi) ul);

		// upper[n+1] becomes upper[n] for next iteration
		ul = uh;
	}
	// do last pair of adds; upper(n+1) = 0 since it lies outside the array.
	l = lrw[i];
	lrw[i] = (v8wu) __builtin_ia32_paddusw128((v8wi) l, (v8wi) ul);
	ul = (v8wu) __builtin_ia32_psrldqi128((v2di) ul, 2 * 8);
	uw[i] = (v8wu) __builtin_ia32_paddusw128((v8wi) l, (v8wi) ul);

	// now just copy the remainder of lower row to empty bins in
	// upper row
	while (++i < rowLen)
		uw[i] = lrw[i];
#if (DADD_TIMING)
	uint64_t t1 = getticks();
	++timing.sum.pairSums;
	timing.sum.total += elapsed(t1, t0);
#endif
}

/**
* Sum the maximum-drift paths from the upper and lower blocks.
 *
 * Description:\n
 *	Handles the eccentric case which arises when adding an even block
 *	of rows to an odd block of rows in the non-2^n DADD algorithm.
 *	It sums the maximum drift from the larger (upper) block with
 *	the maximum drift from the smaller (lower) block.\n\n
 * Notes:\n
 *	This is the only case paths of different total drift are
 *	combined.
 *
 * @param	drift the total drift of the upper block.
 * @param	bins the number of bins in a row.
 * @param	lowerRow a pointer to the first bin in the lower block.
 * @param	upperRow a pointer to the first bin in the upper block.
 * @see		pairSum
*/
void
Dadd::singleSum(int32_t drift, int32_t bins, DaddAccum *lower,
        DaddAccum *upper)
{
	--drift;
	int32_t rowLen = bins / VECTOR_LEN;
	int32_t usableLen = (bins - drift) / VECTOR_LEN;

	v8wu *lrw = (v8wu *) lower;
	v8wu *uw = (v8wu *) upper;
	v8wu *ur = (v8wu *) (upper + drift);

	// load upper [n] before entering loop
	register v8wu ul;
	ul = (v8wu) __builtin_ia32_loaddqu((const char *) &ur[0]);

	int32_t i;
	for (i = 0; i < usableLen; ++i)
		uw[i] = (v8wu) __builtin_ia32_paddusw128((v8wi) lrw[i], (v8wi) ur[i]);

	// now just copy the remainder of lower row to empty bins in
	// upper row
	while (++i < rowLen)
		uw[i] = lrw[i];
}

}