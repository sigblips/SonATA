/*******************************************************************************

 File:    Spectra.h
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
// SonATA spectrum library header file
//
#ifndef _SpectraH
#define _SpectraH

#include <assert.h>
#include <complex>
#include <fftw3.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <cycle.h>
#include <sseDxInterface.h>
#include "SpVersion.h"

using std::ostream;
using std::complex;

namespace spectra {

#define SPECTRA_TIMING		(true)

#define SpecAssert(x)		assert(x)
#define SPECTRA_ALIGNED(addr)	((((unsigned long long) addr) & 0xf) == 0)

/**
* vector of four single-precision floating point values or two complex
* single-precision floating point values; allows compiler-generated
* vector instructions.
*
*
*/
typedef float v4sf __attribute__ ((vector_size(16)));

/**
* Scalar structure for multiplying complex values by a constant.
*
* An array of these structures contains the FB filter coefficients
* for vector operations. \n
* Initialized by setup().
*
* Notes:
*	Re and im will always be the same.
*/
struct scalar {
	float re;
	float im;

	/**
	*	Default constructor.
	*/
	scalar(): re(0.0), im(0.0) {}
	/**
	*	Constructor; argument initializes both re & im parts.
	*/
	scalar(float f): re(f), im(f) {}
	/**
	*	Assignment operator;  copies the re & im values from the
	*	rvalue to the lvalue.
	*/
	scalar& operator=(const scalar& s) {
		re = s.re;
		im = s.im;
		return (*this);
	}
	/**
	*	Multiplication operator; multiplies re and im parts of
	*	scalar value by factor, resulting in a new scalar value.
	*/
	scalar& operator*(float f) {
		re *= f;
		im *= f;
		return (*this);
	}
};

// resolution data
struct ResData {
	Resolution res;					// resolution
	int32_t fftLen;					// length of spectrum
	bool overlap;					// overlap flag

	ResData(): res(RES_UNINIT), fftLen(0), overlap(false) {}

	friend ostream& operator << (ostream& s, const ResData& resData);
};

// resolution information; returned by setup call
struct ResInfo {
	Resolution res;					// resolution
	int32_t specLen;				// length of the spectrum
	int32_t nSpectra;				// number of spectra returned by
									// computeSpectra
	ResInfo(): res(RES_UNINIT), specLen(0), nSpectra(0) {}

	friend ostream& operator << (ostream& s, const ResInfo& resInfo);
};

struct SpecRes {
	SpecRes(): resolution(RES_UNINIT), overlap(false), specLen(0), fftLen(0),
			nSpectra(0), scale(0), plan(0) {}
	~SpecRes() {
		if (plan)
			fftwf_destroy_plan(plan);
	}

	Resolution resolution;				// resolution
	bool overlap;						// whether resolution is overlapped
	int32_t specLen;					// total length of spectrum
	int32_t fftLen;						// length of fft to compute
	int32_t nSpectra;					// # of spectra created by computeSpectra
	float scale;						// normalization scale factor
	fftwf_plan plan;					// plan

	friend ostream& operator << (ostream& s, const SpecRes& specRes);
};

/**
 * Spectrum timing structure.
 */
struct SpectraTiming {
	uint64_t computes;
	float32_t total;
	struct r {
		uint64_t resComputes;
		float32_t fft;
		float32_t swap;
		float32_t rescale;
		float32_t total;

		r(): resComputes(0), fft(0), swap(0), rescale(0), total(0) {}

		friend ostream& operator << (ostream& s, const r& res);
	} res;

	SpectraTiming(): computes(0), total(0) {}

	friend ostream& operator << (ostream& s, const SpectraTiming& timing);
};

class Spectra {
public:
	Spectra();
	~Spectra();

	void setup(Resolution coarseRes_, int32_t minFftLen_,
			int32_t resolutions_,
			int32_t halfFrames_, int32_t samplesPerHalfFrame_,
			const bool *overlap_, ResInfo *resInfo);
	void setup(const ResData *res_, int32_t resolutions_,
			int32_t halfFrames_, int32_t samplesPerHalfFrame_, ResInfo *resInfo);
	uint32_t getVersion() { return (SPECTRA_VERSION); }
	uint32_t getIfVersion() { return (SPECTRA_IFVERSION); }
	void computeSpectra(const complex<float> *input, complex<float> **output);
	void computeSpectra(complex<float> **input, complex<float> **output);
	void setDebugLevel(int32_t level) { debugLevel = level; }
	const SpectraTiming& getTiming() { return (timing); }

private:
	int32_t debugLevel;					// debug print level
	int32_t resolutions;				// # of resolutions to create
	int32_t halfFrames;					// # of half frames of input data
	int32_t newHalfFrames;				// # of new half frames each computeSpectra
	int32_t samplesPerHalfFrame;		// # of samples per half frame
	int32_t samples;					// total # of td input samples
	complex<float> *hfData;				// time domain data
	complex<float> *hfData0;			// time domain data (hf 0)
	complex<float> *hfData1;			// time domain data (hf 1)
	complex<float> *hfDataN;			// time domain data (hf n-1)

	complex<float> *spectra;			// spectra for all resolutions
	complex<float> *spectra0;			// spectra from half frame 0
	complex<float> *spectra1;			// spectra from half frame 1
	complex<float> *spectraN;			// spectra from last half frame
	SpectraTiming timing;
	SpecRes resolution[MAX_RESOLUTIONS];

	void computeSpectra(complex<float> **output);
	void computeSpectraFft(complex<float> **output);
	void rescale(complex<float> *data, int32_t len, float factor);
};

}
#endif