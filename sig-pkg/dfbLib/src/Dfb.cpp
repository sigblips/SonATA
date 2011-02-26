/*******************************************************************************

 File:    Dfb.cpp
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
//	SonATA DFB library class definition.
//

#include "cycle.h"
#include "Dfb.h"

namespace dfb {

#undef INTERPOLATE_SMALLER_FILTER

Dfb::Dfb(): rawFftLen(DFB_FFTLEN), fftLen(DFB_FFTLEN), overlap(DFB_OVERLAP),
		start(0), samplesPerChan(DFB_DEF_SAMPLES), dataLen(0),
		blks(DFB_FOLDINGS), nRawCoeff(0), nCoeff(0), rawCoeff(0), coeff(0),
		work(0), fftIn(0), fftOut(0), plan(0)
{
	setCoeff(dfbCoeff, DFB_FFTLEN, DFB_FOLDINGS);
	setup(DFB_FFTLEN, DFB_OVERLAP, DFB_FOLDINGS, DFB_DEF_SAMPLES);
}

Dfb::~Dfb()
{
	if (plan)
		fftwf_destroy_plan(plan);
	if (rawCoeff)
		delete [] rawCoeff;
	if (coeff)
		fftwf_free(coeff);
	if (fftIn)
		fftwf_free(fftIn);
	if (fftOut)
		fftwf_free(fftOut);
	if (work)
		fftwf_free(work);
}

/**
 * Compute the threshold required before a DFB iterate can be called.
 *
 * Notes:\n
 * 	This is a static function because it is passed the information
 * 	required in the parameter list.
 */
int
Dfb::getThreshold(int fftLen_, int foldings_, int overlap_, int samples_)
{
	return (foldings_ * fftLen_ + (fftLen_ - overlap_) * (samples_ - 1));
}

/**
* Set the filter coefficients.
*
* Description:\n
* This method:\n
*	(1) Assigns the supplied raw coefficients as the canonical set
*	of coefficients for the filter.\n
*	(2) Sets the raw FFT length.  The actual FFT length specified by
*		setup() must be a multiple or factor of this value.\n
*	(3) Sets the number of foldings (blocks) in the filter.\n\n
* Notes:\n
*	If this function is called to reset the filter coefficients,
*	setup() must be called afterward to reset the setup values.\n
*
* @param	rawCoeff_ a pointer to the array of raw filter coefficients.
* @param	nChan_ the number of channels.  Must be a power of 2.
* @param	foldings_ the number of foldings in the filter.
*/
void
Dfb::setCoeff(const float *rawCoeff_, int nChan_, int foldings_)
{
	DfbAssert(rawCoeff_);
	DfbAssert(nChan_ * foldings_);

	fftLen = rawFftLen = nChan_;
	blks = foldings_;
	overlap = -1;						// setup must be called

	if (rawCoeff)
		delete [] rawCoeff;

	// allocate space for the raw coefficients
	nRawCoeff = fftLen * blks;
	rawCoeff = new float[nRawCoeff];
	DfbAssert(rawCoeff);
	for (int i = 0; i < nRawCoeff; ++i)
		rawCoeff[i] = rawCoeff_[i];

	// now make the filter coefficients
	makeCoeff();
}

/**
* Set the operating parameters for the library
*
* Description:\n
*	Records the operating parameters for the DFB, computes the
*	actual filter coefficients to be used, and creates a plan
*	for the FFT.\n\n
* Notes:\n
*	This function must be called after a setCoeff() call, since
*	the latter resets the number of channels and foldings.\n
*	The number of channels must be a multiple or factor of
*	the raw number of channels.\n
*	The number of foldings must be the same as the raw number of foldings.
*
* @param	nChan_ the number of channels to generate; this is the
*			fft length.  Must be a multiple or factor of the raw number
*			of channels.
* @param	foldings_ the number of blocks of length fft length to use
*			in the filter.  Must be the same as the raw number of foldings
*			specified in setCoeff().
* @param	overlap_ # of samples to overlap each filter iteration.  Must
*			be < nChan_.  Normally 20-25% of nChan_.
* @param	samplesPerChan_ # of samples to generate per iterate() call;
*			this is the number of filter/fft cycles that will be
*			performed.  Normally a full half-frame (512 samples).
* @see		setCoeff
*/
void
Dfb::setup(int nChan_, int overlap_, int foldings_, int samplesPerChan_)
{
	fftLen = nChan_;
	blks = foldings_;
	overlap = overlap_;
	samplesPerChan = samplesPerChan_;
	start = 0;
	DfbAssert(powerOf2(fftLen));
	DfbAssert(overlap >= 0);
	DfbAssert(overlap % 1 == 0);
	DfbAssert(fftLen > overlap);

	// compute the length of data required (in samples)
	dataLen = Dfb::getThreshold(nChan_, foldings_, overlap_, samplesPerChan_);

	// create the set of coefficients
	if (rawCoeff)
		makeCoeff();

	// create a plan for the fft based on the length and the output buffer
	// description
	createPlan();
	DfbAssert(plan);
}

/**
* Return the current configuration of the DFB.
*
* Description:\n
*	Fills a user-supplied structure with the current configuration
*	information (FFT length, filter characteristics, etc.).
*
* @param	info pointer to structure in which to store the configuration info.
* @see		DfbInfo
*/
void
Dfb::getInfo(DfbInfo *info)
{
	if (info) {
		info->rawFftLen = rawFftLen;
		info->fftLen = fftLen;
		info->foldings = blks;
		info->overlap = overlap;
		info->samplesPerChan = samplesPerChan;
		info->dataLen = dataLen;
		info->nRawCoeff = nRawCoeff;
		info->nCoeff = nCoeff;
	}
}

/**
* Get the filter coefficients.
*
* Description:\n
*	Copies the filter coefficients into the user-supplied
*	array.\n\n
* Notes:\n
*	The nCoeff_ argument must match the current actual number
*	of filter coefficients.  To determine this number and allocate
*	the correct number of array elements, getInfo should be called
*	before calling this function.
*
* @param	coeff_ pointer to the array in which to store the coefficients.
* @param	nCoeff_ number of coefficients in the array; must match the actual
*			number of coefficients (fftLen * foldings).
* @see		getCoeff
*/
void
Dfb::getCoeff(float *coeff_, int nCoeff_)
{
	DfbAssert(nCoeff_ == nCoeff);
	for (int i = 0; i < nCoeff; ++i)
		coeff_[i] = coeff[i].re;
}

/**
* Get the raw filter coefficients.
*
* Description:\n
*	Copies the raw filter coefficients into the user-supplied
*	array.\n\n
* Notes:\n
*	The nRawCoeff_ argument must match the number of raw coefficients
*	specified with the setCoeff call (or the default, if setCoeff
*	has not been called).  To determine the correct number of
*	array elements, getInfo should be called before calling this function.
*	The number of raw coefficients and the number of actual coefficients
*	are not necessarily the same, since the number of channels can
*	be different.
*
* @param	rCoeff_ pointer to the array in which to store the coefficients.
* @param	nRawCoeff_ number of coefficients in the array; must match
*			the actual number of raw coefficients (rawFftLen * foldings).
*/
void
Dfb::getRawCoeff(float *rCoeff_, int nRawCoeff_)
{
	DfbAssert(nRawCoeff_ == nRawCoeff_);
	for (int i = 0; i < nRawCoeff; ++i)
		rCoeff_[i] = rawCoeff[i];
}

/**
* Filter and transform a set of input data to produce output channels.
*
* Description:\n
*	Filters and transforms the input data to produce an entire
*	half-frame of output data.  The output data is corner-turned
*	into channelized output buffers.  Overlap between transforms
*	is as specified by setup().  Returns the number of samples
*	completely used by the iteration.\n\n
* Notes:\n
*	The iteration is performed according to the setup parameters.
*	The interface provides for a set of non-contiguous input
*	buffers, but for simplicity and performance the current
*	version allows only a single contiguous input buffer
*	containing at least enough input samples to produce the
*	number of channel-samples specified.
*	This function has no way of determining whether or not the
*	output buffer is full, so that is the responsibility of the
*	caller.
*
* @param	inBuf_ array of pointers to input sample buffers.  Currently, only
*			the first pointer is used.
* @param	nBufs_ total number of input buffers.  Currently, must be 1.
* @param	inLen_ total length in samples of each input buffer.  Currently,
*			must be large enough to generate the number of samples
*			in a half-frame.
* @param	outBuf_ array of pointers to output channel buffers.  Each pointer
*			represents a single channel, and each buffer must be large
*			enough to hold an entire half-frame of samples.
* @see		setup
*/
int
Dfb::iterate(const complex<float> **inBuf_, int nBufs_, int inLen_,
			complex<float> **outBuf_)
{
	const complex<float> *in = inBuf_[0];
	complex<float> **out = outBuf_;

	DfbAssert(nBufs_ == 1);
	int istride = fftLen - overlap;
	DfbAssert(inLen_ >= dataLen);
	int ofs = 0;
	// iterate to generate a buffer's worth of samples for each
	// channel
#if (DFB_TIMING)
	uint64_t t0 = getticks();
#endif
#ifndef NO_POLYPHASE
	for (int i = 0; i < samplesPerChan; ++i, in += istride, ++ofs)
		polyphase(in, out, ofs);
#endif
#if (DFB_TIMING)
	uint64_t t1 = getticks();
	++timing.iterations;
	timing.iterate += elapsed(t1, t0);
#endif
	// record the new input pointer
	inBuf_[0] = in;
	return (istride * samplesPerChan);
}

/**
* Create an FFTW plan for the specified transform.
*
* Description:\n
*	Destroys any existing plan and buffers, then creates a new
*	plan and allocates working buffers for the specified number
*	of channels (FFT length).\n\n
* Notes:\n
*	Current optimization is FFTW_PATIENT, but if we know that the
*	plan will be created very rarely we could use FFTW_EXHAUSTIVE.
*/
void
Dfb::createPlan()
{
	if (plan)
		fftwf_destroy_plan(plan);
	if (fftIn)
		fftwf_free(fftIn);
	if (fftOut)
		fftwf_free(fftOut);
	if (work)
		fftwf_free(work);
	fftIn = (complex<float> *) fftwf_malloc(fftLen * sizeof(complex<float>));
	DfbAssert(DFB_ALIGNED(fftIn));
	fftOut = (complex<float> *) fftwf_malloc(fftLen * sizeof(complex<float>));
	DfbAssert(DFB_ALIGNED(fftOut));
	work = (complex<float> *) fftwf_malloc(fftLen * sizeof(complex<float>));
	DfbAssert(DFB_ALIGNED(work));

	plan = fftwf_plan_dft_1d(fftLen, (fftwf_complex *) work,
			(fftwf_complex *) fftOut, FFTW_FORWARD, FFTW_PATIENT);
}

/**
* Make the actual filter coefficients.
*
* Description:\n
*	Given the raw coefficient array and the number of channels,
*	creates an array of actual filter coefficients.\n\n
* Notes:\n
*	Coefficients will be removed by decimation or added by linear
*	interpolation if the number of channels is not the same as
*	the number of channels specified for the raw coefficient array.
*	Computes the set of actual coefficients based on the length
*	of the target FFT specified in setup():\n
*		(a) If the number of actual coefficients is the same as the
*		numberof raw coefficients, the raw coefficient array is
*		used as is.\n
*		(b) If the number of actual coefficients is smaller than the
*		number of raw coefficients, coefficients of the raw array
*		are combined to create the actual array.
*		(c) If the number of actual coefficients is larger than the
*		number of raw coefficients, coefficients are interpolated
*		between coefficients of the raw array to create the actual
*		array.\n\n
*	Interpolation and combination of coefficients must be done
*	carefully: given N raw coefficients, because while there
*	are N - 1 intervals for N coefficients, there are N/2 - 1
*	intervals for N/2 coefficients and 2N - 1 intervals for
*	2N coefficients.  So simply summing or averaging  adjacent
*	coefficients will not work.  Instead, a new array is created
*	with the first and last coefficients fixed, but all others
*	recomputed for the slightly different spacing.
*/
void
Dfb::makeCoeff()
{
	// allocate the coefficient array
	if (coeff)
		fftwf_free(coeff);
	coeff = static_cast<scalar *>
			(fftwf_malloc(fftLen * blks * sizeof(scalar)));
	DfbAssert(DFB_ALIGNED(coeff));
	nCoeff = fftLen * blks;

	float sum = 0.0;
	// three cases: correct # of coefficients, too few, too many
	if (nCoeff == nRawCoeff) {
		for (int i = 0; i < fftLen * blks; ++i) {
			coeff[i] = scalar(rawCoeff[i]);
			sum += rawCoeff[i];
		}
	}
	else {
		if (nCoeff < nRawCoeff) {
			DfbAssert(nRawCoeff % nCoeff == 0);
#ifdef INTERPOLATE_SMALLER_FILTER
			// fewer than the number of raw coefficients.
			// interpolate each coefficient between the two corresponding
			// coefficients in the raw array
			// the number of raw coefficients must be a multiple of the
			// number of actual coefficients.
			double d = (double) (nRawCoeff - 1) / (nCoeff - 1);
			for (int i = 0; i < nCoeff; ++i) {
				int idx = (int) (i * d);
				double dv = i * d - idx;
				dv *= (rawCoeff[idx+1] - rawCoeff[idx]);
				float v = rawCoeff[idx] + dv;
				coeff[i] = scalar(v);
				sum += v;
			}
#else
			// sum the sets of coefficients
			int stride = nRawCoeff / nCoeff;
			for (int i = 0; i < nCoeff; ++i) {
				float v = 0.0;
				for (int j = 0; j < stride; ++j)
					v += rawCoeff[i*stride+j];
				v /= stride;
				coeff[i] = scalar(v);
				sum += v;
			}
#endif
		}
		else {
			// linear interpolate to produce coefficients which were
			// not included in the raw coefficients.  Note that we must
			// recompute all the original points; since we are creating
			// 2n coefficients, simple interpolation between points (which
			// will produce an odd number of coefficients, cannot be used.
			DfbAssert(nCoeff % nRawCoeff == 0);
			// compute the number of points between the end points, and
			// the amount to advance each coefficient
			double d = (double) (nRawCoeff - 1) / (nCoeff - 1);
			// iterate across the set of raw coefficients
			for (int i = 0; i < nCoeff; ++i) {
				int idx = (int) (i * d);
				double dv = i * d - idx;
				dv *= (rawCoeff[idx+1] - rawCoeff[idx]);
				float v = rawCoeff[idx] + dv;
				coeff[i] = scalar(v);
				sum += v;
			}
		}
	}
#ifndef notdef
//	float factor = 1.0 / sum;
	float factor = sqrt(fftLen) / sum;
	for (int i = 0; i < nCoeff; ++i)
		coeff[i] = coeff[i] * factor;
#endif
}

/**
* Determine whether an integer is a power of 2.
*
* @param	val value
* @return	true if val is a power of 2, false otherwise
*/
bool
Dfb::powerOf2(int val)
{
	while (val > 1) {
		if (val & 1)
			return (false);
		val >>= 1;
	}
	return (true);
}

}