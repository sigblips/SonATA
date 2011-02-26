/*******************************************************************************

 File:    TopDown.cpp
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
// DADD detection class
//
// $Header: /home/cvs/nss/sonata-pkg/daddLib/src/TopDown.cpp,v 1.1 2009/03/03 23:21:53 kes Exp $
//
#include <iostream>
#include "Dadd.h"

namespace dadd {

/**
 *
 * Compute DADD on an array of power bins.
 *
 * Description:\n
 * 	Computes DADD pathsums on a general (i.e., no-power-of-2) array.
 * 	The data is a two-dimensional array of rows spectra by bins
 * 	columns; the data elements are integer power sums.\n
 * Notes:\n
 * 	This version of DADD is not restricted to power-of-2 length
 * 	rows.  It splits the incoming data block into equal length
 * 	subblocks if rows is even, or nearly equal subblocks if rows
 * 	is odd, runs DADD on the subblocks, then sums the resulting
 * 	vectors.  The argument rows is the actual row count of the
 * 	data block, no log2 of the row count.\n
 * 	When the subblocks are unequal, the upper subblock is the
 * 	larger.
 *
 * @param	rows the number of spectra.
 * @param	bins the width of each spectrum; includes any overlap region.
 * @param	data the array (2D) containing the data.
 */
void
Dadd::topDown(int32_t rows, int32_t bins, DaddAccum *data)
{
	if (rows <= 1)
		return;

	// if blocks are unequal, upper one is bigger
	int32_t lowerRows = rows / 2;
	int32_t upperRows = rows - lowerRows;

	// run DADD on the two subblocks
	topDown(lowerRows, bins, data);
	topDown(upperRows, bins, data + lowerRows * bins);

	// now combine the paths from the subblocks
	if (upperRows > lowerRows) {
		// subbblocks are unequal; do leftover case first
		int32_t lower = (lowerRows - 1) * bins;
		int32_t upper = (rows - 1) * bins;
		singleSum(upperRows, bins, data + lower, data + upper);
	}

	// add the matched pairs of drifts from subblocks
	for (int32_t i = 0; i < lowerRows; ++i) {
		int32_t iBitRev1 = getPosition(i, lowerRows);
		int32_t iBitRev2 = getPosition(i, upperRows);
		int32_t lower = iBitRev1 * bins;
		int32_t upper = (lowerRows + iBitRev2) * bins;
		pairSum(i, bins, data + lower, data + upper);
	}
}

/**
 * Compute a row index given the row drift.
 *
 * Description:\n
 * 	When the DADD algorithm is run, the results appear not in their
 * 	natural order.  In the case of power of 2 DADD, the row containing
 * 	pathsum of drift k appears in row bitreverse(k).  When the number
 * 	of rows is not a power of two, the correspondence is more complicated.
 * 	This function generalizes the bit reversal calculation so that it
 * 	works for non-power of 2 spectra.\n\n
 * 	WHY THE ABOVE CALCULATION WORKS:  By induction, one can see that in
 * 	an m row block, the drift 0 row is always the bottom row, and the
 * 	drift m-1 row is always the top row.  Because of the way subblocks
 * 	are combined, even drifts will lie in the lower subblock, and odd
 * 	drifts will lie in the upper subblock, with the possible exception
 * 	the maximum drift row, which lies at the top of the upper subblock
 * 	independent of the parity of its index. Thus, if drift is even,
 * 	the row in question will be placed in the lower subblock, in the
 * 	position previously occupied by the row of drift = drift / 2 in
 * 	that subblock, and if drift is odd, then the row in question
 * 	will lie in the upper subblock in the position previously
 * 	occupied by the row of drift = drift / 2 in that subblock.
 */
int32_t
Dadd::getPosition(int32_t drift, int32_t blocksize)
{
	if (drift == 0 || drift == blocksize - 1)
		return (drift);

	int32_t middleRow = blocksize / 2;
	if (drift & 1)
		return (middleRow + getPosition(drift / 2, blocksize - middleRow));
	else
		return (getPosition(drift / 2, middleRow));
}

}
