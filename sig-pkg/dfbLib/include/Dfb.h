/*******************************************************************************

 File:    Dfb.h
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

// SonATA DFB core library header file

#ifndef _DfbCoreLibH
#define _DfbCoreLibH

#include <assert.h>
#include <complex>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <fftw3.h>
#include "DfbCoeff.h"
#include "DfbVersion.h"

using std::ostream;
using std::complex;

namespace dfb {

#undef NO_POLYPHASE
#define DFB_TIMING			(false)

// macros
#define DfbAssert(exp)		assert(exp)
#define DFB_ALIGNED(addr)	((((unsigned long long) addr) & 0xf) == 0)

// default values
const int DFB_OVERLAP = (DFB_FFTLEN / 4);
const int DFB_DEF_SAMPLES = 512;

/**
* vector of four single-precision floating point values or two complex
* single-precision floating point values; allows compiler-generated
* vector instructions, used by polyphase().
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

/**
* Information structure describing the current setup of the DFB.
*
* The method getInfo() will return one of these structures, providing
* the application with detailed information about the current DFB
*setup.
*/
struct DfbInfo {
	int rawFftLen;						// # of channel in raw array
	int fftLen;							// # of channels
	int overlap;						// overlapping samples between spectra
	int foldings;						// # of foldings in the DFB
	int samplesPerChan;					// # of output samples per iterate()
	int dataLen;						// # of input samples required for
										// an iterate() call
	int nRawCoeff;						// # of raw coefficients
	int nCoeff;							// # of table coefficients

	DfbInfo(): rawFftLen(0), fftLen(0), overlap(0), foldings(0),
			samplesPerChan(0), dataLen(0), nRawCoeff(0), nCoeff(0) {}

	friend ostream& operator << (ostream& s, const DfbInfo& dfbInfo);
};

struct DfbTiming {
	uint64_t iterations;				// total # of iterations performed
	uint64_t dfbs;						// total # of dfb's performed
	float iterate;						// total time spent in iterate
	float wola;							// total time spent in WOLA
	float fft;							// total time spent in FFT
	float store;						// total time spent storing
	float dfb;							// total time spent in polyphase

	DfbTiming(): iterations(0), dfbs(0), iterate(0), wola(0), fft(0), store(0),
			dfb(0) {}

	friend ostream& operator << (ostream& s, const DfbTiming& dfbTiming);
};

class Dfb {
public:
	Dfb();
	~Dfb();

	static int getThreshold(int fftLen_, int foldings_, int overlap_,
			int samples_);
	void setCoeff(const float *coeff_, int nChan_, int foldings_);
	void setup(int nChan_, int overlap_, int foldings_, int samplesPerChan_);
	void getInfo(DfbInfo *dfbInfo);
	void getCoeff(float *coeff_, int nCoeff_);
	void getRawCoeff(float *rCoeff, int nRawCoeff_);
	/** Return the library version number in form 0xmmnn (major, minor). */
	unsigned int getVersion() { return (DFB_VERSION); }
	/** Return the library interface version number 0xmmnn (major, minor). */
	unsigned int getIfVersion() { return (DFB_IFVERSION); }
	int iterate(const complex<float> **inBuf_, int nBufs_, int inLen_,
			complex<float> **outBuf_);
	void polyphase(const complex<float> *in, complex<float> **out, int ofs);
	const DfbTiming& getTiming() { return (timing); }

private:
	int rawFftLen;						// # of channels in raw coeff array
	int fftLen;							// # of channels
	int overlap;						// # of samples of overlap
	int start;							// first sample in WOLA buffer
	int samplesPerChan;					// # of samples per iterate call
	int dataLen;						// total # of samples required to
										// perform iterate
	int blks;							// # of foldings in the filter
	int nRawCoeff;						// total number of coefficients
	int nCoeff;							// total number of used coefficients
	float *rawCoeff;					// raw filter coefficients
	scalar *coeff;						// actual filter coefficients
	complex<float> *work;				// WOLA working data array
	complex<float> *fftIn;				// FFT input data array
	complex<float> *fftOut;				// FFT output data array
	DfbTiming timing;					// timing statistics

	fftwf_plan plan;					// FFTW plan

	void createPlan();
	void makeCoeff();
	bool powerOf2(int val);

	void wola(const complex<float> *in, complex<float> *out);
	void rotate(complex<float> *src, complex<float> *dest);
};

}

#endif