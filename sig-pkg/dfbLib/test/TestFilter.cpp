/*******************************************************************************

 File:    TestFilter.cpp
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

// test the polyphase filter
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <sys/time.h>
#include "DfbTest.h"

using namespace std;

#undef FULL_BIN

/**
* Plot the power in the filter coefficients.
*
* Description:\n
*	Plots the power in each filter coefficient.
*
* @param: dfb the DFB object, which has already been initialized.
*/
void
DfbTest::plotFilterPower(Dfb& dfb)
{
	DfbInfo info;

	dfb.getInfo(&info);
	float coeff[info.nCoeff];
	dfb.getCoeff(coeff, info.nCoeff);
	complex<float> cCoeff[info.nCoeff];
	float sum = 0;
	for (int i = 0; i < info.nCoeff; ++i) {
		cCoeff[i] = complex<float> (coeff[i], coeff[i]);
		sum += coeff[i];
	}
	cout << "Sum of filter coefficients = " << sum << endl;
	writePower("Filter", cCoeff, info.nCoeff, info.fftLen, 0.0, false, false);
}

/**
* Test the filter response across a set of bins.
*
* Description:\n
*	Starts with a signal placed at the center of the subject bin and
*	iterates to a signal placed at the the center of some adjacent bin.
*	At each iteration, the signal power in the subject bin is computed.
*	This gives the total filter response.\n\n
* Notes:\n
*	The output is written to a file of the form "BinPowerdB_chans_freqY",
*	where chans is the number of channels and freqY is the frequency
*	(where 0 <= f <= chans).\n
*	The data is in a form plottable with gnuplot.

* @param	dfb the DFB object, which has already been initialized.
*/
void
DfbTest::testBinResp(Dfb& dfb)
{
	DfbInfo info;
	dfb.getInfo(&info);

	int points = ADJACENT_BINS * info.fftLen + 1;

	// use bin f = fftLen / 4;
	int f = info.fftLen / 4;
	int len = info.dataLen;
	complex<float> *tdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * len);
	DfbAssert(DFB_ALIGNED(tdData));
	complex<float> *fdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * info.fftLen);
	DfbAssert(DFB_ALIGNED(fdData));
//	complex<float> **fd = new complex<float> *[info.fftLen];
	complex<float> *fd[info.fftLen];
	double freq = f;
	for (int i = 0; i < info.fftLen; ++i)
		fd[i] = &fdData[i];

#ifdef FULL_BIN
	complex<float> binData[2*points];
#else
	complex<float> binData[2*points];
#endif
	int iterationsPerBin = info.fftLen;
//	int iterationsPerBin = 204;
	for (int i = 0; i < points; ++i) {
		// create a signal at some offset from the center of the bin
		generateSignal(freq + (double) i / iterationsPerBin, 1.0, info.fftLen,
				len, tdData);
		complex<float> *td = tdData;
		dfb.polyphase(td, fd, 0);
#ifdef FULL_BIN
		binData[points-i] = fdData[f];
		binData[points+i] = fdData[f];
#else
		binData[i] = fdData[f];
#endif
	}

#ifdef FULL_BIN
	writePower("Bin", binData, 2 * points, info.fftLen, f, true, false);
#else
	writePower("Bin", binData, points, info.fftLen, f, false, false);
#endif

	fftwf_free(tdData);
	fftwf_free(fdData);
//	delete [] fd;
}

/**
* Test the overall response of the filter to an impulse.
*
* Description:\n
*	Places an impulse (actually a brief square wave) near the beginning
*	of the input data, then computes a single DFB (filter/fft)
*	operation.\n\n
* Notes:\n
*	Several files in gnuplot format are generated: three for the
*	time domain input data, three for the output frequency domain data:\n
*	(1) ImpulseTDComplex: plots the complex input data (real part
*		only).\n
*	(2) ImpulseTDPower: plots the power in each sample of the input data.\n
*	(3) ImpulseTDPowerdB: plots the power in dB in each sample of the
*		input data.\n
*	(4) ImpulseFDComplex: plots the complex values of the output bins (real
*		part only).\n
*	(5) ImpulseFDPower: plots the power in each output bin.\n
*	(6) ImpulseFDPowerdB: plots the power in dB in each output bin.
*
* @aparam	dfb the DFB object, which has already been initialized.
*/
void
DfbTest::testOverallResp(Dfb& dfb)
{
	DfbInfo info;
	dfb.getInfo(&info);

	float power[info.fftLen];
	double amp = 10000.0;

	int tdLen = info.fftLen * info.foldings;
	complex<float> *tdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * tdLen);
	DfbAssert(DFB_ALIGNED(tdData));
	for (int i = 0; i < tdLen; ++i)
		tdData[i] = 0.0 + 0.0fi;
	complex<float> *fdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * info.fftLen);
	DfbAssert(DFB_ALIGNED(fdData));
//	complex<float> **fd = new complex<float> *[info.fftLen];
	complex<float> *fd[info.fftLen];
	for (int i = 0; i < info.fftLen; ++i)
		fd[i] = &fdData[i];

	// create a time-domain array containing a single square wave somewhere
	generateImpulse(0, info.foldings, amp, info.fftLen, info.foldings, tdData);

	writeComplex("ImpulseTD", tdData, tdLen, info.fftLen, 0.0);
	writePower("ImpulseTD", tdData, tdLen, info.fftLen, 0.0, false, false);
	writePower("ImpulseTD", tdData, tdLen, info.fftLen, 0.0, true, false);

	complex<float> *td = tdData;
	dfb.polyphase(td, fd, 0);

	for (int i = 0; i < info.fftLen; ++i)
		power[i] = std::norm(fdData[i]);

	writeComplex("ImpulseFD", fdData, info.fftLen, info.fftLen, 0.0);
	// write both dB and actual power forms of the data
	writePower("ImpulseFD", fdData, info.fftLen, info.fftLen, 0.0, true, true);
	writePower("ImpulseFD", fdData, info.fftLen, info.fftLen, 0.0, false, true);

	fftwf_free(tdData);
	fftwf_free(fdData);
//	delete [] fd;
}

/**
* Test the filter iteration, when multiple spectra are computed at once.
*
* Description:\n
*	Generates data at a specific frequency for enough samples to perform
*	a complete iteration, then calls iterate() to produce the filtered
*	output spectra.\n\n
* Notes:\n
*	Several files in gnuplot format are generated:\n
*	(1) IterTDComplex: plots the complex input data (real part only).\n
*	(2) IterTDPowerdB: plots the power in dB in each sample of the input
*		data.\n
*	(3) IterFDComplex: plots the complex bins, which consists of 512
*		(one half-frame) of samples for each bin.\n
*	(4) IterFDPower: plots the power in the output bins; for each bin,
*		one half-frame's worth of data are plotted.\n
*	(5) IterFDPower: plots the power in dB in the output bins; for each bin,
*		one half-frame's worth of data are plotted.
*
* @aparam	dfb the DFB object, which has already been initialized.
*/
void
DfbTest::testIteration(Dfb& dfb)
{
	double amp = .1;

	DfbInfo info;
	dfb.getInfo(&info);

	int frames = info.samplesPerChan;
	double freq = 13;

	int tdLen = info.dataLen;
	complex<float> *tdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * tdLen);
	int fdLen = info.fftLen * frames;
	complex<float> *fdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * fdLen);
//	complex<float> **fd = new complex<float> *[DFB_FFTLEN];
	complex<float> *fd[DFB_FFTLEN];
	for (int i = 0; i < info.fftLen; ++i)
		fd[i] = &fdData[i*frames];

	// create a signal at some offset from the center of the bin
	generateSignal(freq, amp, info.fftLen, tdLen, tdData);
	writeComplex("IterTD", tdData, tdLen, info.fftLen, freq);
	writePower("IterTD", tdData, tdLen, info.fftLen, freq, true, false);

	const complex<float> *td = tdData;
	int n = dfb.iterate(&td, 1, tdLen, fd);

	std::stringstream s;
	s << info.fftLen << " channels, " << info.samplesPerChan;
	s << " samples per channel, " << info.overlap << " samples of overlap, ";
	s << info.foldings << " foldings" << endl;
	s << n << " samples consumed per iteration";
	OUTL(s.str());

	writeComplex("IterFD", fdData, fdLen, info.fftLen, freq);
	writePower("IterFD", fdData, fdLen, info.fftLen, freq, false, false);
	writePower("IterFD", fdData, fdLen, info.fftLen, freq, true, true);
	writeSpectra("Spectra", fdData, info.samplesPerChan, info.fftLen, freq,
			true, true);

	// test the data to make sure that each frequency bin has consistent
	// power data
	for (int i = 0; i < info.fftLen; ++i) {
		float min = std::norm(fd[i][0]);
		float max = std::norm(fd[i][0]);
		for (int j = 0; j < info.samplesPerChan; ++j) {
			float power = std::norm(fd[i][j]);
			if (power < min)
				min = power;
			if (power > max)
				max = power;
		}
		float diff = fabs(max - min);
		CONFIRM(diff < 1e-5);
	}
	int fBin = (int) freq;
	cout << endl << "signal at " << freq << ", power[" << fBin << "] = ";
	cout << std::norm(fd[fBin][0]) << ", power[" << fBin + 1 << "] = ";
	cout << std::norm(fd[fBin+1][0]) << endl;
	fftwf_free(tdData);
	fftwf_free(fdData);
//	delete [] fd;
}

/**
* Compute the time required for each DFB operation.
*
* Description:\n
*	Generates a block of input data for one iterate() call, then
*	repeatedly calls iterate() to generate a half-frame of output
*	data.  The total time, time per iterate(), and time per
*	DFB (polyphase()) are computed and printed on the console.\n\n
* Notes:\n
*	Generates several output files in gnuplot format:\n
*		(1) TimingFDComplex: complex bin output of the final iterate() call.\n
*		(2) TimingFDPower: bin power output of the final iterate() call.\n
*		(3) TimingFDPowerdB: bin power output in dB of the final iterate()
*			call.\n
*	The same input data is used over and over again, so this is not
*	a perfect test of the timing.\n
*	A number of threads should be spawned to test the operation in
*	a more realistic environment.
*
* @param	dfb the DFB object, which has already been initialized.
*/
void
DfbTest::testTiming(Dfb& dfb)
{
	double amp = 1.0;

	DfbInfo info;
	dfb.getInfo(&info);

	int frames = info.samplesPerChan;
	double freq = 13;

	int tdLen = info.dataLen;
	complex<float> *tdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * tdLen);
	int fdLen = info.fftLen * frames;
	complex<float> *fdData = (complex<float> *) fftwf_malloc(
			sizeof(complex<float>) * fdLen);
//	complex<float> **fd = new complex<float> *[info.fftLen];
	complex<float> *fd[info.fftLen];
	for (int i = 0; i < info.fftLen; ++i)
		fd[i] = &fdData[i*frames];

	// create a signal at some offset from the center of the bin
	generateSignal(freq, amp, info.fftLen, tdLen, tdData);

	OUTL("start timing test");
	std::stringstream s;
	s << info.fftLen << " channels, " << info.samplesPerChan;
	s << " samples per channel, " << info.overlap << " samples of overlap, ";
	s << info.foldings << " foldings";
	OUTL(s.str());
	timeval start, end;
	gettimeofday(&start, 0);
	for (int i = 0; i < TIMING_ITERATIONS; ++i) {
		const complex<float> *td = tdData;
		dfb.iterate(&td, 1, tdLen, fd);
	}
	gettimeofday(&end, 0);
	float sec = end.tv_sec - start.tv_sec;
	float usec = end.tv_usec - start.tv_usec;
	if (usec < 0) {
		usec += 1e6;
		--sec;
	}
	float fsec = sec + usec / 1e6;
	double dt = fsec / TIMING_ITERATIONS;
	s.str("");
	s << TIMING_ITERATIONS << " iterations " << fsec << " sec" << endl;
	s << "time per iterate = " << dt * 1e3 << " msec" <<  endl;
	s << "time per DFB = " << dt * 1e6 / info.samplesPerChan << " usec" << endl;
	OUTL(s.str());

	writeComplex("TimingFD", fdData, fdLen, info.fftLen, freq);
	writePower("TimingFD", fdData, fdLen, info.fftLen, freq, false, false);
	writePower("TimingFD", fdData, fdLen, info.fftLen, freq, true, false);

	fftwf_free(tdData);
	fftwf_free(fdData);
//	delete [] fd;
}

/**
* Generate a test signal consisting of a sine wave of a specified
* frequency and amplitude.
*
* Description:\n
*	Utility function to create a test signal for filter testing.\n\n
* Notes:\n
*	Would be more useful if it created input data containing Gaussian
*	noise.
*
* @param	freq signal frequency (baseband, so 0 < freq < fftLen).
* @param	amp amplitude of the signal.
* @param	fftLen length of the FFT.
* @param	len total # of samples to generate.
* @param	out pointer to sample buffer in which to store the output data.
*/
void
DfbTest::generateSignal(double freq, double amp, int fftLen, int len,
		complex<float> *out)
{
	double alpha = 2 * M_PI * freq / fftLen;
	for (int i = 0; i < len; ++i) {
		out[i] = amp * cos(i * alpha) + amp * 1.0fi * sin(i * alpha);
	}
}

/**
* Generate a test signal consisting of a short impulse to test the filter
* response.
*
* Description:\n
*	Generates an impulse (actually, a square wave) of specified
*	amplitude, beginning at a specified bin for the specified number of
*	samples.\n\n
* Notes:\n
*	I'm not quite sure how to generate the proper kind of signal for
*	testing an impulse on a DFB which consists of so many foldings. (kes)
*
* @param	t sample at which to begin the pulse.
* @param	dt number of samples of pulse duration.
* @param	amp amplitude of the pulse.
* @param	fftLen FFT length (number of channels).
* @param	len total number of samples to generate.
* @param	out pointer to output buffer in which to store the output data.
*/
void
DfbTest::generateImpulse(int t, int dt, double amp, int fftLen, int foldings,
		complex<float> *out)
{
	for (int i = 0; i < foldings; ++i) {
		for (int j = 0; j < fftLen; ++j) {
			if (i == foldings / 2 && j >= t && j < (t + dt))
				out[i*fftLen+j] = amp;
			else
				out[i*fftLen+j] = 0.0 + 0.0fi;
		}
	}
}

/**
* Write an array of complex floating point values to a gnuplot file.
*
* Description:\n
*	Given an array of complex floating point values, writes the
*	real part of each value to a text file which can be displayed by
*	gnuplot.\n\n
* Notes:\n
*	File name is of the form "prefixComplex_fftLen_freq".\n
*	The real part of the complex value is the y axis; the index of the
*	point in the array is the x value.\n
*	By uncommenting a line of code, the imaginary value can be plotted
*	as well.
*
* @param	prefix character string for the beginning of the filename, to
*			identify the type of file being written.
* @param	data pointer to the array of complex data.
* @param	points total number of values in the array.
* @param	fftLen length of the FFT; used only to qualify the filename.
* @param	freq frequency relative to baseband (0 < freq < fftLen); used
*			only to qualify the filename.
*/
void
DfbTest::writeComplex(const char *prefix, const complex<float> *data,
		int points, int fftLen, double freq)
{
	char name[128];

	sprintf(name, "%sComplex_%d_%.2f", prefix, fftLen, freq);
	ofstream file(name, ios::binary);

	for (int i = 0; i < points; ++i) {
		float re = data[i].real();
		float im = data[i].imag();
		file << i << " " << re << endl;
		file << i << " " << im << endl;
	}
	file.close();
}

/**
* Write the power in an array of complex values to a gnuplot file.
*
* Description:\n
*	Given an array of complex floating point values, computes the power
*	each element and writes the data to a text file which can be displayed
*	by gnuplot.\n\n
* Notes:\n
*	If the power is being plotted in dB, 0 power is mapped to -100dB.
*
* @param	prefix character string for the beginning of the filename,
*			to identify the file.
* @param	data pointer to the array of complex data.
* @param	points total number of values in the array.
* @param	fftLen length of the FFT; used only to qualify the filename.
* @param	freq frequency relative to baseband (0 < freq < fftLen); used
*			only to qualify the filename.
* @param	dbFlag if true, plot power in dB; if false, plot raw power.
* @param	swap if true, swap front and back halves of data to place DC
*			in the middle of the plot.
*/
void
DfbTest::writePower(const char *prefix, const complex<float> *data,
		int points, int fftLen, double freq, bool dBFlag, bool swap)
{
	char name[128];

	// plot bin vs power
	sprintf(name, "%sPower%s_%d_%.2f", prefix, dBFlag ? "dB" : "",fftLen, freq);
	ofstream file(name, ios::binary);

	// maybe swap front and back half of data, writing into array
	complex<float> tmpData[points];
	if (swap) {
		int hLen = points / 2;
		for (int i = 0; i < hLen; ++i) {
			tmpData[i] = data[i+hLen];
			tmpData[i+hLen] = data[i];
		}
	}
	else {
		for (int i = 0; i < points; ++i)
			tmpData[i] = data[i];
	}
	float p0 = 0.0;
	for (int i = 0; i < points; ++i)
		p0 += std::norm(tmpData[i]);
//	p0 /= points;

	// get the power in bin 0
#ifdef FULL_BIN
	p0 = std::norm(data[points/2]);
#else
	p0 = std::norm(data[0]);
#endif

	// create the file
	for (int i = 0; i < points; ++i) {
		float p = std::norm(tmpData[i]);

		if (dBFlag)
			p = p ? 10.0 * log10(p/p0) : -100.0;

#ifdef FULL_BIN
		file << i - points / 2 << " " << p;
#else
		file << i << " " << p;
#endif
		file << std::endl;
	}
	file.close();
}

/**
* Write a set of spectra of power values to a gnuplot file.
*
* Description:\n
*	Given an array of complex floating point values in corner-turned
*	output order (samplesPerChan samples per bin), plot the individual
*	power spectra.  Each spectrum is plotted in bin order (i.e., spectrum
*	0, bins 0 - fftLen-1 is plotted in the first fftLen points, spectrum
*	1 in the second fftLen points, etc.).\n\n
*
* @param	prefix character string for the beginning of the filename, to
*			identify the type of file being written.
* @param	data pointer to the array of complex data.
* @param	spectra number of spectra.
* @param	fftLen length of the FFT; used only to qualify the filename.
* @param	freq frequency relative to baseband (0 < freq < fftLen); used
*			only to qualify the filename.
*/
void
DfbTest::writeSpectra(const char *prefix, const complex<float> *data,
		int spectra, int fftLen, double freq, bool dBFlag, bool swap)
{
	char name[128];

	// plot bin vs power
	sprintf(name, "%sPower%s_%d_%.2f", prefix, dBFlag ? "dB" : "",fftLen, freq);
	ofstream file(name, ios::binary);

	complex<float> dataPower[spectra*fftLen];
	for (int i = 0; i < spectra * fftLen; ++i)
		dataPower[i] = std::norm(data[i]);

	// maybe swap front and back half of data, writing into array
	complex<float> tmpData[spectra*fftLen];
	if (swap) {
		int hLen = (spectra * fftLen) / 2;
		for (int i = 0; i < hLen; ++i) {
			tmpData[i] = data[i+hLen];
			tmpData[i+hLen] = data[i];
		}
	}
	else {
		for (int i = 0; i < spectra * fftLen; ++i)
			tmpData[i] = data[i];
	}

	complex<float> tmpPower[spectra*fftLen];
	for (int i = 0; i < spectra * fftLen; ++i)
		tmpPower[i] = std::norm(data[i]);

	// get the power in bin 0
	float p0 = std::norm(data[0]);

	// create the file
	for (int i = 0; i < spectra; ++i) {
		for (int j = 0; j < fftLen; ++j) {
			float p = std::norm(tmpData[i+j*spectra]);

			if (dBFlag)
				p = p ? 10.0 * log10(p/p0) : -100.0;

			file << (i * fftLen + j) << " " << p;
			file << std::endl;
		}
	}
	file.close();
}

/**
* Print filter statistics (not currently used).
*
* Description:\n
*	Prints a summary of filter statistics for a single bin.\n\n
* Notes:\n
*	Computes the maximum attenuation (minimum response) of any signal
*	in the bin of interest (relative to a signal in the exact center
*	of the bin).\n
*	Computes the attenuation at the bin edge.
*
* @param	power pointer to an array of power values.
* @param	pointsPerBin the number of points per bin.
* @param	points total number of points in the array.
*/
void
DfbTest::printStatistics(const float *power, int pointsPerBin, int points)
{
	int edge = pointsPerBin / 2 + 1;
	float response = 0.0;
	int attenBin = edge;
	float attenuation = power[edge];

	for (int i = 0; i < edge; ++i) {
		if (power[i] < response)
			response = power[i];
	}
	for (int i = attenBin + 1; i < points; ++i) {
		if (attenuation < power[i]) {
			attenuation = power[i];
			attenBin = i;
		}
	}
	cout << endl << "maximum attenuation inside bin = " << response;
	cout << endl;
	cout << "attenuation at bin edge = " << power[edge] << endl;
}


/**
 * Compute the FFT of the filter coefficients, then compute the power per bin
 * 	in the usable bins versus the power discarded.
 */
void
DfbTest::computeFilterFft(Dfb& dfb, int nCoeff)
{
	size_t size = nCoeff * sizeof(complex<float>);
	// allocate space for the output data
	complex<float> *filter = static_cast<complex<float> *> (fftwf_malloc(size));
	complex<float> *fft = static_cast<complex<float> *> (fftwf_malloc(size));
	fftwf_plan plan = fftwf_plan_dft_1d(nCoeff, (fftwf_complex *) filter,
			(fftwf_complex *) fft, FFTW_BACKWARD, FFTW_ESTIMATE);

	float *coeff = static_cast<float *> (fftwf_malloc(nCoeff * sizeof(float)));
	dfb.getCoeff(coeff, nCoeff);
	for (int i = 0; i < nCoeff; ++i)
		filter[i] = complex<float>(coeff[i], coeff[i]);
	fftwf_execute_dft(plan, (fftwf_complex *) filter, (fftwf_complex *) fft);
	writePower("FilterFFT", fft, nCoeff, nCoeff, 0, false, true);
	DfbInfo info;
	dfb.getInfo(&info);
	size = info.fftLen * sizeof(complex<float>);
	complex<float> *cSum = static_cast<complex<float> *> (fftwf_malloc(size));
	for (int i = 0; i < info.fftLen; ++i) {
		cSum[i] = 0;
		for (int j = 0; j < info.foldings; ++j) {
			cSum[i] += filter[i+info.fftLen*j];
		}
	}
	writePower("CoeffSum", cSum, info.fftLen, info.fftLen, 0, true, false);
}

