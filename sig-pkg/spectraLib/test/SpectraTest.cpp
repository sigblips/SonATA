/*******************************************************************************

 File:    SpectraTest.cpp
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
// Spectra test code
//
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>
#include "SpectraTest.h"

using std::cout;
using std::endl;

using namespace spectra;

const bool overlap[] = {
//	false, false, false, false, false, false, false, false, false, false, false
	true, true, true, true, true, true, true, true, true, true, true
};

/**
* Test the Spectra library.
*
* Description:\n
*	Run a set of tests of the library.\n\n
* Notes:\n
*	Uses the Eastshore Design Group test facility.
*	Very rudimentary at this point.
*/
void
SpectraTest::test()
{
	testArgs test1 = { RES_512HZ, true, 2, 10, 3, 3, 512, 2.0 };
	doTest(test1, false, true, false);
	doTest(test1, false, true, false);

	testArgs test3 = { RES_64HZ, true, 16, 7, 3, 3, 512, 2.0 };
	cout << "contiguous input" << endl;
	doTest(test3, false, true, false);
	cout << "scattered input" << endl;
	doTest(test3, true, true, false);

	testArgs test6 = { RES_2HZ, true, 512, 2, 3, 3, 512, 2.0 };
	cout << "contiguous input" << endl;
	doTest(test6, false, false, true);
	cout << "scattered input" << endl;
	doTest(test6, true, true, false);

	// test varying overlap: just overlap the finest 3 spectra	
	bool overlap[10] = { false, false, false, false, false, false, false,
			true, true, true };
	testArgs test7 = { RES_512HZ, false, 2, 10, 3, 3, 512, 2.0 };
	cout << "varying overlap, contiguous input" << endl;
	doTest(test7, false, true, false, overlap);

	testArgs test9 = { RES_2HZ, false, 512, 1, 3, 3, 512, 2.0 };
	cout << "varying overlap, contiguous input" << endl;
	doTest(test9, false, true, false, overlap);
	cout << "varying overlap, scattered input" << endl;
	doTest(test9, true, false, true, overlap);

	testArgs test10 = { RES_1HZ, false, 1024, 1, 3, 3, 512, 2.0 };
	cout << "varying overlap, contiguous input" << endl;
	doTest(test10, false, true, false, overlap);
	cout << "varying overlap, scattered input" << endl;

	cout << "timing tests" << endl;
	testArgs test2 = { RES_512HZ, true, 2, 10, 3, 33, 512, 2.0 };
	doTest(test2, false, false, true);

	testArgs test4 = { RES_512HZ, false, 2, 10, 3, 33, 512, 2.0 };
	cout << "contiguous input" << endl;
	doTest(test4, false, false, true);
	cout << "scattered input" << endl;
	doTest(test4, true, false, true);

	testArgs test5 = { RES_64HZ, false, 16, 7, 3, 33, 512, 2.0 };
	cout << "contiguous input" << endl;
	doTest(test5, false, false, true);
	cout << "scattered input" << endl;
	doTest(test5, true, false, true);

	testArgs test8 = { RES_512HZ, true, 2, 10, 3, 33, 512, 2.0 };
	cout << "contiguous input" << endl;
	doTest(test8, false, false, true);
	cout << "scattered input" << endl;
	doTest(test8, true, false, true);

	cout << "varying overlap, contiguous input" << endl;
	doTest(test8, false, false, true, overlap);
	cout << "varying overlap, scattered input" << endl;
	doTest(test8, true, false, true, overlap);

	test10.totalHalfFrames = 33;
	doTest(test10, true, false, true, overlap);

	testArgs test11 = { RES_1HZ, true, 1024, 1, 3, 3, 512, 2.0 };
	doTest(test11, true, false, true);
}

void
SpectraTest::doTest(testArgs& args, bool useArray, bool printData,
		bool doTiming, const bool *ovrlap)
{
	cout << args.resolutions << " resolutions, ";
	if (ovrlap)
		cout << "mixed overlap, ";
	else
		cout << (args.overlap ? "" : "not ") <<  "overlapped, ";
	cout << args.totalHalfFrames << " half frames" << endl;
	cout << "using straight fft's, not refinement" << endl;

	print = printData;
	const int32_t maxFftLen = args.minFftLen << (args.resolutions - 1);
	const int32_t count = args.totalHalfFrames / args.halfFrames;
	const int32_t frames = args.totalHalfFrames / 2;

	cout << "minfftlen = " << args.minFftLen;
	cout << ", maxfftlen = " << maxFftLen << endl;

	Spectra spectra;
	ResInfo resInfo[args.resolutions];

	// set up overlap array
	// either set all overlap flags the same, or use array of flags
	bool overlap[args.resolutions];
	if (!ovrlap) {
		for (int32_t i = 0; i < args.resolutions; ++i)
			overlap[i] = args.overlap;
	}
	else {
		for (int32_t i = 0; i < args.resolutions; ++i)
			overlap[i] = ovrlap[i];
	}

	spectra.setup(args.res, args.minFftLen, args.resolutions, args.halfFrames,
			args.samplesPerHalfFrame, overlap, resInfo);
	spectra.setDebugLevel(printData);

	size_t tdLen = args.totalHalfFrames * args.samplesPerHalfFrame;
	complex<float> *tdData = (complex<float> *) fftwf_malloc(tdLen
			* sizeof(complex<float>));
	generateSignals(maxFftLen, args.totalHalfFrames, args.samplesPerHalfFrame,
			args.frequency, tdData);

	// allocate storage for the spectra
	complex<float> *spec[args.resolutions];
	complex<float> *tSpec[args.resolutions];
	for (int32_t i = 0; i < args.resolutions; ++i) {
		size_t size = count * resInfo[i].nSpectra * resInfo[i].specLen;
		tSpec[i] = spec[i] = (complex<float> *)
				fftwf_malloc(size * sizeof(complex<float>));
	}

	// maybe using an array of half frame buffers, so copy the data
	// there
	complex<float> *hfData[args.totalHalfFrames];
	for (int32_t i = 0; i < args.totalHalfFrames; ++i)
		hfData[i] = &tdData[i*args.samplesPerHalfFrame];

	timeval start, end;
	if (doTiming)
		gettimeofday(&start, 0);
	for (int32_t i = 0; i < args.totalHalfFrames - 1; i += args.halfFrames) {
		if (useArray)
			spectra.computeSpectra(&hfData[i], tSpec);
		else {
			complex<float> *data = &tdData[i*args.samplesPerHalfFrame];
			spectra.computeSpectra(data, tSpec);
		}
		// bump storage pointers
		for (int32_t j = 0; j < args.resolutions; ++j)
			tSpec[j] += resInfo[j].nSpectra * resInfo[j].specLen;
	}
	if (doTiming) {
		gettimeofday(&end, 0);
		float sec = end.tv_sec - start.tv_sec;
		float usec = end.tv_usec - start.tv_usec;
		if (usec < 0) {
			usec += 1e6;
			--sec;
		}
		float fsec = sec + usec / 1e6;
		double dt = fsec / frames;
		std::stringstream s;
		s.str("");
		s << args.totalHalfFrames / 2 << " frames,  " << args.resolutions;
		s << " resolutions " << fsec << " sec" << endl;
		s << "time per frame = " << dt * 1e3 << " msec" <<  endl;
		cout << s.str();
	}
	if (printData) {
		for (int32_t i = 0; i < args.resolutions; ++i) {
			size_t size = count * resInfo[i].specLen * resInfo[i].nSpectra;
			cout << "len " << resInfo[i].specLen << " ";
			printPower("power", spec[i], size);
			printArray("spectrum", spec[i], size);
		}
	}
	// free storage
	fftwf_free(tdData);
	for (int32_t i = 0; i < args.resolutions; ++i)
		fftwf_free(spec[i]);
}

void
SpectraTest::generateSignals(int32_t fftLen, int32_t halfFrames,
		int32_t samplesPerHalfFrame, float freq, complex<float> *data)
{
	float arg = 2 * M_PI * freq / fftLen;
	for (int32_t i = 0; i < halfFrames * samplesPerHalfFrame; ++i)
		data[i] = complex<float>(cos(arg * i), sin(arg * i));
	if (print) {
		printPower("time data array power", data,
				halfFrames * samplesPerHalfFrame);
		printArray("time data array", data, halfFrames * samplesPerHalfFrame);
	}
}

void
SpectraTest::printArray(const char *s, const complex<float> *data,
		int32_t count)
{
	cout << s << ", points = " << count << endl;
	for (int32_t i = 0; i < count; ++i) {
		cout << "[" << i << "] = (" << data[i].real() << ", ";
		cout << data[i].imag() << ")" << endl;
	}
}

void
SpectraTest::printPower(const char *s, const complex<float> *data,
		int32_t count)
{
	float totalPower = 0.0;

	cout << s << ", point = " << count << endl;
	for (int32_t i = 0; i < count; ++i) {
		float power = std::norm(data[i]);
		totalPower += power;
	}
	cout << "total power = " << totalPower << endl;
}

