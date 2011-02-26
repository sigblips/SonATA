/*******************************************************************************

 File:    DfbTest.cpp
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
// Dfb test code
//
#include "DfbTest.h"

/*
* Test the DFB library.
*
* Description:\n
*	Run a set of tests of the library.\n\n
* Notes:\n
*	Uses the Eastshore Design Group test facility.
*	Does not test enough failure modes, such as not enough input
*	samples to iterate(), foldings specified
*/
void
DfbTest::test()
{
	Dfb dfb;

	DfbInfo info;
	dfb.getInfo(&info);
	CONFIRM(info.fftLen == DFB_FFTLEN);
	CONFIRM(info.overlap == DFB_OVERLAP);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_DEF_SAMPLES);

	computeFilterFft(dfb, info.nCoeff);

//	test8(dfb);
//	plotFilterPower(dfb);
	test256(dfb);
	plotFilterPower(dfb);
//	test4(dfb);
//	plotFilterPower(dfb);
	test128(dfb);
	plotFilterPower(dfb);
	test512(dfb);
	plotFilterPower(dfb);
	test1024(dfb);
	plotFilterPower(dfb);

	// test response with the default 256 channels
	setup256(dfb);
	testOverallResp(dfb);
	testIteration(dfb);
	testTiming(dfb);

	setup512(dfb);
	testTiming(dfb);

	setup1024(dfb);
	testTiming(dfb);
}

void
DfbTest::test4(Dfb& dfb)
{
	OUTL("test 4");
	setup4(dfb);
	checkCoeff(dfb);
	testBinResp(dfb);
}

void
DfbTest::test8(Dfb& dfb)
{
	OUTL("test 8");
	setup8(dfb);
	checkCoeff(dfb);
	testBinResp(dfb);
}

void
DfbTest::test128(Dfb& dfb)
{
	OUTL("test 128");
	setup128(dfb);
	checkCoeff(dfb);
	testBinResp(dfb);
}

void
DfbTest::test256(Dfb& dfb)
{
	OUTL("test 256");
	setup256(dfb);
	checkCoeff(dfb);
	testBinResp(dfb);
}

void
DfbTest::test512(Dfb& dfb)
{
	OUTL("test 512");
	setup512(dfb);
	checkCoeff(dfb);
	testBinResp(dfb);
}

void
DfbTest::test1024(Dfb& dfb)
{
	OUTL("test 1024");
	setup1024(dfb);
	checkCoeff(dfb);
	testBinResp(dfb);
}

void
DfbTest::setup4(Dfb& dfb)
{
	//	test the coefficient array and set the filter coefficients
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	dfb.setup(FFTLEN_4, OVERLAP_4, DFB_FOLDINGS, DFB_SAMPLES_4);

	DfbInfo info;
	dfb.getInfo(&info);

	int n = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(n == info.nRawCoeff);
	CONFIRM(info.nCoeff == info.fftLen * info.foldings);

	CONFIRM(info.fftLen == FFTLEN_4);
	CONFIRM(info.overlap == OVERLAP_4);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_SAMPLES_4);
}

void
DfbTest::setup8(Dfb& dfb)
{
	//	test the coefficient array and set the filter coefficients
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	dfb.setup(FFTLEN_8, OVERLAP_8, DFB_FOLDINGS, DFB_SAMPLES_8);

	DfbInfo info;
	dfb.getInfo(&info);

	int n = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(n == info.nRawCoeff);
	CONFIRM(info.nCoeff == info.fftLen * info.foldings);

	CONFIRM(info.fftLen == FFTLEN_8);
	CONFIRM(info.overlap == OVERLAP_8);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_SAMPLES_8);
}

void
DfbTest::setup128(Dfb& dfb)
{
	// set the coefficient array, then do setup
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	dfb.setup(FFTLEN_128, OVERLAP_128, DFB_FOLDINGS, DFB_DEF_SAMPLES);

	DfbInfo info;
	dfb.getInfo(&info);

	int n = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(n == info.nRawCoeff);

	CONFIRM(info.nCoeff == info.fftLen * info.foldings);
	CONFIRM(info.fftLen == FFTLEN_128);
	CONFIRM(info.overlap == OVERLAP_128);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_DEF_SAMPLES);

	//	test the coefficient array and set the filter coefficients
}

void
DfbTest::setup256(Dfb& dfb)
{
	//	test the coefficient array and set the filter coefficients
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	dfb.setup(FFTLEN_256, OVERLAP_256, DFB_FOLDINGS, DFB_SAMPLES_256);

	DfbInfo info;
	dfb.getInfo(&info);

	int n = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(n == info.nRawCoeff);
	CONFIRM(info.nCoeff == info.fftLen * info.foldings);

	CONFIRM(info.fftLen == FFTLEN_256);
	CONFIRM(info.overlap == OVERLAP_256);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_SAMPLES_256);
}

void
DfbTest::setup512(Dfb& dfb)
{
	//	test the coefficient array and set the filter coefficients
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	dfb.setup(FFTLEN_512, OVERLAP_512, DFB_FOLDINGS, DFB_DEF_SAMPLES);

	DfbInfo info;
	dfb.getInfo(&info);

	int n = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(n == info.nRawCoeff);
	CONFIRM(info.nCoeff == info.fftLen * info.foldings);

	CONFIRM(info.fftLen == FFTLEN_512);
	CONFIRM(info.overlap == OVERLAP_512);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_DEF_SAMPLES);

}

void
DfbTest::setup1024(Dfb& dfb)
{
	//	test the coefficient array and set the filter coefficients
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	dfb.setup(FFTLEN_1024, OVERLAP_1024, DFB_FOLDINGS, DFB_DEF_SAMPLES);

	DfbInfo info;
	dfb.getInfo(&info);

	int n = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(n == info.nRawCoeff);
	CONFIRM(info.nCoeff == info.fftLen * info.foldings);

	CONFIRM(info.fftLen == FFTLEN_1024);
	CONFIRM(info.overlap == OVERLAP_1024);
	CONFIRM(info.foldings == DFB_FOLDINGS);
	CONFIRM(info.samplesPerChan == DFB_DEF_SAMPLES);

}

/**
* Check setCoeff().
*/
void
DfbTest::checkCoeff(Dfb& dfb)
{
	DfbInfo info;
	dfb.getInfo(&info);

	return;

#ifdef notdef
	// test the coefficient array and use it to set the filter
	// coefficients
	int nRaw = sizeof(dfbCoeff) / sizeof(float);
	CONFIRM(nRaw == DFB_FFTLEN * DFB_FOLDINGS);
	dfb.setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
#endif

	// create an array to hold the filter coefficients
	int n = info.fftLen * info.foldings;
	float rCoeff[n];
	getCanonicalCoeff(dfb, info, rCoeff);

	float *coeff = new float[n];
	dfb.getCoeff(coeff, n);

	for (int i = 0; i < n; ++i) {
		if (fabs(coeff[i] - rCoeff[i]) > 1e-5) {
			char msg[128];
			sprintf(msg, "i = %d, n = %d, c = %f, r = %f", i, n,
					coeff[i], rCoeff[i]);
			FAIL(msg);
		}
	}
}

/**
* Get the gold-standard values for the coefficients for this transform size
* and number of foldings.
*
* Notes:\n
*	For the best results, a separate set of coefficients should be
*	computed for each transform size/# of foldings in use, and setCoeff()
*	used to set the coefficients directly.
*/
void
DfbTest::getCanonicalCoeff(Dfb& dfb, const DfbInfo& info, float *coeff)
{
	int n = info.fftLen * info.foldings;
	int nRaw = info.nRawCoeff;
	float rCoeff[nRaw];

	// get the raw filter coefficients
	dfb.getRawCoeff(rCoeff, nRaw);

	// test # of coefficients to see if same as raw
	if (n == nRaw) {
		for (int i = 0; i < n; ++i)
			coeff[i] = rCoeff[i];
	}
	else {
		float sum = 0.0;
		if (n < nRaw) {
			// working array has fewer coefficients than raw array; combine
			// coefficients
			int stride = nRaw / n;
			CONFIRM(n * stride == nRaw);
			for (int i = 0; i < n; ++i) {
				float v = 0.0;
				for (int j = 0; j < stride; ++j)
					v += rCoeff[i*stride+j];
				coeff[i] = v;
				sum += v;
			}
		}
		else {
			// working array has more coefficients than raw array; interpolate
			// coefficients
			CONFIRM(n % nRaw == 0);
			int insert = n / nRaw;
			CONFIRM(nRaw * insert == n);
			double d = (double) (nRaw - 1) / (n - 1);
			for (int i = 0; i < n; ++i) {
				int k = (int) (i * d);
				double dk = i * d - k;
				double dv = (rCoeff[k+1] - rCoeff[k]) * dk;
				float v = rCoeff[k] + dv;
				coeff[i] = v;
				sum += v;
			}
		}

		// compute the sum of the raw coefficients
		float cSum = 0.0;
		for (int i = 0; i < nRaw; ++i)
			cSum += rCoeff[i];

		float factor = cSum / sum;
		for (int i = 0; i < n; ++i)
			coeff[i] *= factor;
	}
}