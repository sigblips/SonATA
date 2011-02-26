/*******************************************************************************

 File:    DfbTest.h
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

// Dfb test fixture
#ifndef _DfbTestH
#define _DfbTest

#include "basics.h"
#include "Dfb.h"
#include "DfbCoeff.h"
#include "testVal.h"

using namespace dfb;

class DfbTest {
public:
	DfbTest() {}
	~DfbTest() {};

	void test();

protected:
	void test4(Dfb& dfb);
	void test8(Dfb& dfb);
	void test128(Dfb& dfb);
	void test256(Dfb& dfb);
	void test512(Dfb& dfb);
	void test1024(Dfb& dfb);
	void setup4(Dfb& dfb);
	void setup8(Dfb& dfb);
	void setup128(Dfb& dfb);
	void setup256(Dfb& dfb);
	void setup512(Dfb& dfb);
	void setup1024(Dfb& dfb);

private:
	void checkCoeff(Dfb& dfb);
	void getCanonicalCoeff(Dfb& dfb, const DfbInfo& info, float *coeff);

	// test functions
	void plotFilterPower(Dfb& dfb);
	void testBinResp(Dfb& dfb);
	void testOverallResp(Dfb& dfb);
	void testIteration(Dfb& dfb);
	void testTiming(Dfb& dfb);

	// utility functions
	void generateSignal(double freq, double amp, int fftLen, int len,
		complex<float> *out);
	void generateImpulse(int t, int dt, double amp, int fftLen, int foldings,
		complex<float> *out);
	void writeComplex(const char *prefix, const complex<float> *data,
		int points, int fftLen, double freq);
	void writePower(const char *prefix, const complex<float> *data,
		int points, int fftLen, double freq, bool dBFlag, bool swap);
	void writeSpectra(const char *prefix, const complex<float> *data,
		int spectra, int fftLen, double freq, bool dBFlag, bool swap);
	void printStatistics(const float *power, int pointsPerBin, int points);
	void computeFilterFft(Dfb& dfb, int nCoeff);
};

#endif