/*******************************************************************************

 File:    test.cpp
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

#include <fftw3.h>
#include <iostream>
#include <string.h>
#include "Dadd.h"

const int32_t TEST_ROWS = 128;
const int32_t PASSES = 6;					// log2(TEST_ROWS / 2)
const int32_t TEST_BINS = 589824;			// 768 * 768
const int32_t THRESHOLD = 200;				// arbitrary
const int32_t BAND_BINS = 768;
const int32_t BAD_BAND_LIMIT = 100;

using namespace dadd;
using std::cout;
using std::endl;

// forward declarations
void initArray(DaddAccum *data);
void reportHit(const DaddPath& path);
void reportBadBand(Polarization pol, int32_t badBandLimit,
		const DaddPath& path);

int
main(int argc, char **argv)
{
	DaddAccum *array;
	Dadd *dadd = new Dadd();
	dadd->setup(TEST_ROWS, TEST_BINS, TEST_BINS + TEST_ROWS, THRESHOLD,
			BAND_BINS, BAD_BAND_LIMIT, TDDadd, true);

	size_t size = TEST_ROWS * (TEST_BINS + TEST_ROWS) * sizeof(DaddAccum);
	array = (DaddAccum *) (fftwf_malloc(size));
//	memset(array, 0, size);

	initArray(array);
	uint64_t t0 = getticks();
	dadd->pairSum(0, TEST_BINS, array, array + TEST_BINS);
	uint64_t t1 = getticks();
	float dt = elapsed(t1, t0);
	cout << "drift 0, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(1, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 1, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(2, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 2, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(3, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 3, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(4, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 4, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(5, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 5, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(6, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 6, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(7, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 7, " << dt << endl;

	initArray(array);
	t0 = getticks();
	dadd->pairSum(8, TEST_BINS, array, array + TEST_BINS);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "drift 8, " << dt << endl;

	t0 = getticks();
	for (int32_t i = 0; i < PASSES / 2; ++i) {
		int32_t dr = 1 << i;
		for (int32_t j = 0; j < TEST_ROWS / 2; ++j) {
			dadd->pairSum(j, TEST_BINS, array + j * TEST_BINS,
					array + (j + dr) * TEST_BINS);
		}
	}
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << TEST_ROWS << " spectra, " << dt << endl;
	t0 = getticks();
	dadd->thresholdData(array);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "threshold " << TEST_ROWS << " spectra, " << dt << endl;

	// do the actual dadd
	initArray(array);
	t0 = getticks();
	dadd->execute(POL_RIGHTCIRCULAR, Positive, array, 0);
	dadd->reportBadBands(0);
	t1 = getticks();
	dt = elapsed(t1, t0);
	cout << "full DADD " << TEST_ROWS << " spectra, " << dt << endl;
	DaddStatistics stats = dadd->getStatistics();
	cout << stats;
	DaddTiming timing = dadd->getTiming();
	cout << timing;
}

void
initArray(DaddAccum *data)
{
	// initialize the array
	for (int32_t i = 0; i < TEST_ROWS; ++i) {
		DaddAccum *dp = data + i * (TEST_BINS + TEST_ROWS);
		int32_t j;
		for (j = 0; j < TEST_BINS; ++j)
			dp[j] = j & 1;
		memset(dp + j, 0, TEST_ROWS * sizeof(DaddAccum));
	}
}