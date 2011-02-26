/*******************************************************************************

 File:    Spectra.cpp
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
// Spectrum library
//
#include <iostream>
#include "Spectra.h"

using std::cout;
using std::endl;

namespace spectra {

/**
* Spectrum analysis using simple FFT's, not spectrum synthesis.
*
*/

Spectra::Spectra(): debugLevel(0), resolutions(0),
		halfFrames(0), newHalfFrames(0), samplesPerHalfFrame(0), samples(0),
		hfData(0), hfData0(0), hfData1(0), hfDataN(0), spectra(0),
		spectra0(0), spectra1(0), spectraN(0)
{
}

Spectra::~Spectra()
{
	fftwf_free(hfData);
}

/**
* Perform setup of the spectrum synthesizer.
*
* Description:\n
*	Sets the initial transform to perform (the shortest FFT), the
*	number of resolutions to be built, and an array of overlap
*	flags, indicating whether the corresponding resolution uses
*	overlapping transforms.\n\n
* Notes:\n
*	Builds fftw plans for each resolution:\n
*	rank (dimensions) = 1.\n
*	fft length = fftLen >> resolution.\n
*	number of transforms = 1 << resolution.\n
*	input array = pointer to array of time domain samples.\n
*	inembed is NULL, since we are not computing from subarrays.\n
*	input stride (dist between input samples) = 1.\n
*	input distance between input data blocks = 2 * fft length, since we are
*	computing only half the fft's.\n
*	output array = pointer to array of fourier coefficients.\n
*	oembed is NULL, since we are not storing a subarray.\n
*	output stride (dist between output samples) = 1.\n
*	output distance between output fft's = 2 * fft length.\n
*	sign of transform is FFT_FORWARD.\n
*	flags is FFTW_MEASURE because we want to measure the fft to find
*	the best method of computing.
*
* @param	coarseRes_ resolution of the coarsest resolution to create.
* @param	minFftLen_ minimum FFT length to perform.  This will be the
*			coarsest resolution.
* @param	resolutions_ # of resolutions to create.
* @param	halfFrames_ # of half frames of data passed to the
*			computeSpectra call.  This will be at least 3, and computeSpectra
* 			will be called for every even half frame except the first.  The
* 			first call will be at half frame 2 with data for half frames
* 			0-2; the second call will be at half frame 4 with data for half
* 			frames 2-4 (note that hf 2 data is submitted twice).
* @param	samplesPerHalfFrame_ # of samples in each half frame.
* @param	overlap_ array of flags (one per resolution) indicating
*			whether a given resolution is overlapped or not.
*/
void
Spectra::setup(Resolution coarseRes_, int32_t minFftLen_,
		int32_t resolutions_,
		int32_t halfFrames_, int32_t samplesPerHalfFrame_,
		const bool *overlap_, ResInfo *resInfo)
{
	SpecAssert(resolutions_ >= 1);

	// build a resolution array of consecutive resolutions
	ResData r[MAX_RESOLUTIONS];
	// set up the initial resolution
	r[0].res = coarseRes_;
	r[0].fftLen = minFftLen_;
	r[0].overlap = overlap_[0];

	// do remaining resolutions
	for (int32_t i = 1; i < resolutions_; ++i) {
		r[i].res = (Resolution) ((int32_t) r[i-1].res - 1);
		r[i].fftLen = r[i-1].fftLen << 1;
		r[i].overlap = overlap_[i];
	}
	setup(r, resolutions_, halfFrames_, samplesPerHalfFrame_, resInfo);
}

/**
 * Perform setup using an array of (possibly) sparse resolutions
 *
 * Description:\n
 *
 * @param	res_ pointer to array of resolutions to be created.
 * @param	resolutions_ # of resolutions to create.
 * @param	halfFrames_ # of half frames of data passed to the
 *			computeSpectra call.  This will be at least 3, and computeSpectra
 * 			will be called for every even half frame except the first.  The
 * 			first call will be at half frame 2 with data for half frames
 * 			0-2; the second call will be at half frame 4 with data for half
 * 			frames 2-4 (note that hf 2 data is submitted twice).
* @param	samplesPerHalfFrame_ # of samples in each half frame.
* @param	overlap_ array of flags (one per resolution) indicating
*			whether a given resolution is overlapped or not.
 *
 */
void
Spectra::setup(const ResData *res_, int32_t resolutions_,
		int32_t halfFrames_, int32_t samplesPerHalfFrame_, ResInfo *resInfo)
{
	size_t sSize = 0;

	// save the setup parameters
	resolutions = resolutions_;
	halfFrames = halfFrames_;
	newHalfFrames = halfFrames - 1;
	samplesPerHalfFrame = samplesPerHalfFrame_;
	samples = halfFrames * samplesPerHalfFrame;

	// basic validity checks
	SpecAssert(resolutions >= 1);
	SpecAssert(halfFrames >= 3);
	SpecAssert(newHalfFrames % 2 == 0);

	// allocate space for the time-domain data
	size_t tdLen = halfFrames * samplesPerHalfFrame;
	hfData = (complex<float> *) fftwf_malloc(tdLen * sizeof(complex<float>));
	SpecAssert(hfData);

	// initialize each resolution;
	for (int32_t i = 0; i < resolutions; ++i) {
		const ResData& r = res_[i];
		SpecRes& res = resolution[i];
		res.resolution = r.res;
		res.overlap = r.overlap;
		res.specLen = res.fftLen = r.fftLen;
		SpecAssert(res.fftLen);
		// frame length must be a multiple of spectrum length
		SpecAssert((2 * samplesPerHalfFrame) % res.specLen == 0);

		//////////////////////////////////////////////
		// compute parameters for call to computeSpectra, which processes
		// the specified number of half frames - 1
		//////////////////////////////////////////////
		// compute the number of spectra generated by each computeSpectra call
		int32_t computeSamples = newHalfFrames * samplesPerHalfFrame;
		if (res.overlap)
			computeSamples *= 2;
		res.nSpectra = computeSamples / res.specLen;
		SpecAssert(res.nSpectra > 0);

		// compute the total size of the spectrum buffer required for
		// this resolution, then allocate the buffer if it must be larger
		// than already allocated
		size_t size = res.nSpectra * res.specLen;
		if (sSize < size) {
			sSize = size;
			if (spectra)
				fftwf_free(spectra);
			spectra = (complex<float> *)
					fftwf_malloc(sSize * sizeof(complex<float>));
		}

		int32_t idist = res.fftLen;
		if (res.overlap)
			idist /= 2;
		int32_t odist = res.specLen;
		int32_t istride = 1;
		int32_t ostride = 1;

		// create the fft plans for this resolution
		int rank = 1;

		// create the plan used by computeSpectra.
		// the input will always be from the same place in the time-domain
		// data buffer, and output will always be to the same place in
		// the spectrum buffer for this resolution, so we can use the
		// actual addresses when creating the plan
		complex<float> *in = hfData;
		complex<float> *out = spectra;
		if (debugLevel > 0) {
			cout << "in " << in << endl;
			cout << "out " << out << " - " << out + size << endl;
		}
		res.plan = fftwf_plan_many_dft(rank, &res.fftLen, res.nSpectra,
				(fftwf_complex *) in, NULL, istride, idist,
				(fftwf_complex *) out, NULL, ostride, odist,
				FFTW_FORWARD, FFTW_MEASURE);
		SpecAssert(res.plan);
		resInfo[i].res = res.resolution;
		resInfo[i].nSpectra = res.nSpectra;
		resInfo[i].specLen = res.specLen;
	}
#ifdef notdef
	if (swapBuf && swapBufSize < maxFftLen) {
		fftwf_free(swapBuf);
		swapBuf = 0;
	}
	if (!swapBuf) {
		swapBufSize = maxFftLen;
		swapBuf = static_cast<complex<float> *>
				(fftwf_malloc(swapBufSize * sizeof(complex<float>)))
		SpecAssert(swapBuf);
	}
#endif
}

/**
* Compute spectra from time-domain data.
*
* Description:\n
*	From the raw time domain data, computes a full frame of spectra
* 	for each resolution specified during setup.  It then computes
* 	other spectra using half-length fft's as described above.\n
* Notes:\n
*	The output buffers must all be aligned on a 16-byte boundary,
*	to allow full use of the vector instructions.
*	The spectrum buffer already contains data from the previous half
*	frame, so the new data must be put into the buffer immediately
*	following it.
*
* @param	input time-domain input data, consisting of three consecutive
* 			half frames of data.
* @param	output pointer to array of output buffers, one for each
*			resolution to be produced.
*/
void
Spectra::computeSpectra(const complex<float> *input, complex<float> **output)
{
	// validate the output addresses
	for (int32_t i = 0; i < resolutions; ++i) {
		SpecAssert(output[i]);
		SpecAssert(SPECTRA_ALIGNED(output[i]));
	}

	// copy the input time domain data to the internal buffer
	memcpy(hfData, input, samples * sizeof(complex<float>));

	// compute the spectra
	computeSpectra(output);
}

/**
* Compute spectra from time-domain data.
*
* Description:\n
*	From the raw time domain data, computes the full-length fft
*	specified during setup.  It then computes other spectra using
*	half-length fft's as described above.\n
* Notes:\n
*	This version assumes the input data is in an array of pointers to
* 	half frame buffers, which is passed as input.\n
*	The output buffers must all be aligned on a 16-byte boundary,
*	to allow full use of the vector instructions.\n
*
* @param	input pointer to array of input buffers, with one
*			buffer per half frame.
* @param	output pointer to array of output buffers, one for each
*			resolution to be produced.
*/
void
Spectra::computeSpectra(complex<float> **input, complex<float> **output)
{
	// move the input data to the correct buffers
	for (int32_t i = 0; i < halfFrames; ++i) {
		memcpy(&hfData[i*samplesPerHalfFrame], input[i],
				samplesPerHalfFrame * sizeof(complex<float>));
	}

	// compute the spectra
	computeSpectra(output);
}

/**
* Compute spectra from data in the half frame buffers.
*
* Description:\n
*	Private method called by the two public computeSpectra methods.
*	Assumes the input data has already been moved into the half frame
*	buffers allocated by setup.\n
* Notes:\n
*	None.
*
* @param	output array of pointers to output buffers, one for each
*			resolution.
*/
void
Spectra::computeSpectra(complex<float> **output)
{
	// validate the output addresses
	for (int32_t i = 0; i < resolutions; ++i) {
		SpecAssert(output[i]);
		SpecAssert(SPECTRA_ALIGNED(output[i]));
	}

#if (SPECTRA_TIMING)
	uint64_t t0 = getticks();
#endif
	for (int32_t i = 0; i < resolutions; ++i) {
		SpecRes& res = resolution[i];
#if (SPECTRA_TIMING)
		uint64_t t1 = getticks();
#endif
		fftwf_execute_dft(res.plan, (fftwf_complex *) hfData,
				(fftwf_complex *) spectra);
		// swap the front and back half of the FFT output; this puts DC
		// in the middle of the spectrum.
#if (SPECTRA_TIMING)
		uint64_t t2 = getticks();
#endif
		int32_t h = res.specLen / 2;
		int32_t hLen = h * sizeof(complex<float>);
		complex<float> *src = spectra;
		complex<float> *dest = output[i];
		for (int32_t j = 0; j < res.nSpectra; ++j) {
			memcpy(dest + h, src, hLen);
			memcpy(dest, src + h, hLen);
			src += res.specLen;
			dest += res.specLen;
		}
#if (SPECTRA_TIMING)
		uint64_t t3 = getticks();
#endif
		int32_t count = res.nSpectra * res.specLen;
//		memcpy(output[i], spectra, count * sizeof(complex<float>));
		rescale(output[i], count, res.specLen);
#if (SPECTRA_TIMING)
		uint64_t t4 = getticks();
		++timing.res.resComputes;
		timing.res.fft += elapsed(t2, t1);
		timing.res.swap += elapsed(t3, t2);
		timing.res.rescale += elapsed(t4, t3);
		timing.res.total += elapsed(t4, t1);
#endif
	}
#if (SPECTRA_TIMING)
	uint64_t t5 = getticks();
	++timing.computes;
	timing.total += elapsed(t5, t0);
#endif
}


/**
* Rescale the spectrum to normalize the power.
*/
void
Spectra::rescale(complex<float> *data, int32_t len, float factor)
{
	scalar f[2] __attribute__ ((aligned (16)));

	f[0] = f[1] = 1.0 / sqrt(factor);

	// use vector operations, so halve the iteration count
	len /= 2;

	v4sf *d = (v4sf *) data;
	v4sf *s = (v4sf *) f;

	for (int32_t i = 0; i < len; ++i)
		d[i] *= s[0];
//		d[i] = d[i] * s[0];
}

}
