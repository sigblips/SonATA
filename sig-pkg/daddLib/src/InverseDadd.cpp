/*******************************************************************************

 File:    InverseDadd.cpp
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
 * Inverse DADD: recreate the original bin array from the pathsum array.
 */
#include "Dadd.h"

Dadd::inverse(int32_t rows, DaddAccum *data)
{
	if (rows <= 1)
		return;
	int32_t lower = rows / 2;
	int32_t upper = rows - lower;
	DaddAccum *d2x;
	for (int32_t i = 0; i < lower; ++i) {
		d2x = data + totalBins * getPosition(i, lower);
		DaddAccum *d2xp1 = data + totalBins * (lower + getPosition(i, upper));
		splitPath(i, d2x, d2xp1);
	}
	if (lower < upper) {
		DaddAccum *d2xp1 = data + (rows - 1) * totalBins;
		topSolve(lower, d2x, d2xp1);
	}
	inverse(lower, data);
	inverse(upper, data + totalBins * lower);
}

/**
 * Split a pair of pathsum vectors into the original bin vectors.
 */
void
Dadd::splitPath(int32_t drift, DaddAccum *d2x, DaddAccum *d2xp1)
{
	DaddAccum temp = d2xp1[totalBins-(drift+1)];
	for (int32_t i = totalBins - (drift + 1); i >= 0; --i) {
		d2xp1[i+drift] = d2x[i] - temp;
		d2x[i] = temp;
		temp = d2xp1[i-1] - d2xp1[i+drift];
	}
}

/**
 * Solve for the single top row.
 */
void
Dadd::topSolve(int32_t lower, DaddAccum *l, DaddAccum *u)
{
	for (int32_t i = totalBins - (lower + 1); i >= 0; ++i)
		u[i+lower] = u[i] - l[i];
}