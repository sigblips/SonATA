/*******************************************************************************

 File:    SpectraTest.h
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

// Spectra test fixture
#ifndef _SpectraTestH
#define _SpectraTestH

#include "basics.h"
#include "Spectra.h"

using namespace spectra;

struct testArgs {
	Resolution res;						// initial resolution
	bool overlap;						// overlap spectra
	int32_t minFftLen;					// minimum fft length
	int32_t resolutions;				// # of resolutions
	int32_t halfFrames;					// # of half frames per computeSpectra
	int32_t totalHalfFrames;			// total # of half frames to process
	int32_t samplesPerHalfFrame;		// samples per half frame
	float frequency;					// signal frequency

//	testArgs(): res(RES_UNINIT), overlap(true), minFftLen(0), resolutions(0),
//			halfFrames(0), samplesPerHalfFrame(0), frequency(0.0) {}
};
	
class SpectraTest {
public:
	SpectraTest() {}
	~SpectraTest() {}

	void test();

private:
	bool print;

	void doTest(testArgs& args, bool useArray, bool printData, bool doTiming,
			const bool *overlap = 0);
	void generateSignals(int32_t fftLen, int32_t halfFrames,
			int32_t samplesPerHalfFrame, float freq, complex<float> *tdData);
	void printArray(const char *s, const complex<float> *data, int32_t count);
	void printPower(const char *s, const complex<float> *data, int32_t count);
};


#endif
