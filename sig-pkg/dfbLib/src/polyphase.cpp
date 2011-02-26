/*******************************************************************************

 File:    polyphase.cpp
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

// SonATA DFB core library: polyphase filter

#include "cycle.h"
#include "Dfb.h"

namespace dfb {

#undef OUTPUT_ONLY						// just transfer input to output
#undef FFT_ONLY							// just do fft, no wola
#undef NO_OUTPUT						// don't output results
#undef ROTATE_INPUT						// rotate input data for phase error (?)
#undef COMPUTE_POWER					// TEST ONLY with gdb!!!!
// NOTE: the following value MUST be defined
#define ROTATE_DATA						// rotate data for phase error (?)

/**
* Perform a polyphase filter/channelization operation.
*
* Description:\n
*	Performs a single iteration of the DFB/channelization.  The block
*	of input samples is filtered, then an FFT is performed to produce
*	individual channels.  Each output channel is written into a separate
*	buffer.  Filter and FFT parameters are as specified by setup().\n\n
* Notes:\n
*	Actually not a "polyphase" filter bank, but a digital filter bank (DFB).
*	First argument should really be an array of pointers to input
*	buffers, to match the call syntax of iterate(), but this would
*	vastly complicate the operation.
*
* @param	in pointer to input sample array.
* @param	out array of pointers to output buffers.
* @param	ofs offset into each output buffer.
*/
void
Dfb::polyphase(const complex<float> *in, complex<float> **out, int ofs)
{
#ifndef OUTPUT_ONLY
#if (DFB_TIMING)
	uint64_t t0 = getticks();
#endif
#ifdef ROTATE_DATA
	wola(in, work);
	rotate(work, fftIn);
#else
	wola(in, fftIn);
#endif
#if (DFB_TIMING)
	uint64_t t1 = getticks();
#endif
	fftwf_execute_dft(plan, (fftwf_complex *) fftIn, (fftwf_complex *) fftOut);
#if (DFB_TIMING)
	uint64_t t2 = getticks();
#endif

#ifdef notdef
	// normalize the output to maintain the average bin power.
	// Note: this can be done more efficiently in makeCoeff.
	float scale = 1.0 / sqrt(fftLen);
	scalar factor(scale);
	for (int i = 0; i < fftLen; ++i)
		fftOut[i] *= scale;
#endif

	// the filtered, transformed data is now in an internal buffer -
	// transfer the individual bins to the output corner turn arrays.
	// NOTE: if normalization must be done, it should be done here;
	// similarly, any statistics gathering should be performed here.
#ifndef NO_OUTPUT
	for (int chan = 0; chan < fftLen; ++chan)
		out[chan][ofs] = fftOut[chan];
#endif
#if (DFB_TIMING)
	uint64_t t3 = getticks();
	++timing.dfbs;
	timing.wola += elapsed(t1, t0);
	timing.fft += elapsed(t2, t1);
	timing.store += elapsed(t3, t2);
	timing.dfb += elapsed(t3, t0);
#endif
#else		// OUTPUT_ONLY
	uint64_t t0 = getticks();
	for (int chan = 0; chan < fftLen; ++chan)
		out[chan][ofs] = in[chan];
	uint64_t t1 = getticks();
	++timing.dfbs;
	timing.store += elapsed(t1, t0);
#endif

	// if we are running tests, compute the power in the td data and
	// the output spectrum
#ifdef COMPUTE_POWER
	// compute the power in the input
	volatile float tdPower = 0;
	for (int chan = 0; chan < fftLen; ++chan)
		tdPower += std::norm(fftIn[chan]);

	// compute the power in the spectrum
	volatile float fdPower = 0;
	for (int chan = 0; chan < fftLen; ++chan)
		fdPower += std::norm(fftOut[chan]);
	fdPower /= fftLen;
#endif
}

/**
 * Weighted overlap and add.
 *
 * Description:\n
 * 	Performs a WOLA on a block of input data to produce filtered output
 * 	data which can be processed with a DFT.  The input data may be rotated
 * 	before the filter is applied to preserve proper phase, which is
 * 	modified by the overlap.\n\n
 */
void
Dfb::wola(const complex<float> *in, complex<float> *out)
{
#ifdef FFT_ONLY
	// test code: no polyphase, just straight fft
	float f = sqrt(1.0 / fftLen);
	for (int i = 0; i < fftLen; ++i)
		out[i] = in[i] * f;
#else
	// initialize the output data array
	memset(out, 0, fftLen * sizeof(complex<float>));
	// create the filtered data block to be transformed
//	int k = 0;
//	int l = 0;
	v4sf *id = (v4sf *) in;
	v4sf *od = (v4sf *) out;
	v4sf *cd = (v4sf *) coeff;
	int hLen = fftLen / 2;
#ifdef ROTATE_INPUT
	start %= fftLen;
#ifdef notdef
	int fBlks = blks;
	// if we're shifting data, do blks-1 full blocks
	if (start)
		--fBlks;
	// start at the beginning of a full block
	id = (v4sf *) (in + start);
	// apply the filter coefficients to all the full blocks of data
	for (int blk = 0; blk < fBlks; ++blk, id += hLen, cd += hLen) {
		for (int j = 0; j < hLen; ++j)
			od[j] += id[j] * cd[j];
	}
	// the final block may now be split between the end and the beginning
	// of the buffer data, so we have to handle it as two separate blocks.
	if (fBlks < blks) {
		int32_t end = (fftLen - start) / 2;
		for (int j = 0; j < end; ++j)
			od[j] += id[j] * cd[j];
		// do the final elements in the final block; these lie at the beginning
		// of the input buffer;
		id = (v4sf *) in;
		int i = 0;
		for (int j = end; j < hLen; ++j, ++i)
			od[j] += id[i] * cd[j];
	}
#else
	complex<float> tmp[blks*fftLen];
	int end = blks * fftLen - start;
	memcpy(&tmp[0], &in[start], end * sizeof(complex<float>));
	memcpy(&tmp[end], &in[0], start * sizeof(complex<float>));
	id = (v4sf *) tmp;
	for (int blk = 0; blk < blks; ++blk, id += hLen, cd += hLen) {
		for (int j = 0; j < hLen; ++j)
			od[j] += id[j] * cd[j];
	}
#endif
	start += overlap;
#else
	for (int blk = 0; blk < blks; ++blk, id += hLen, cd += hLen) {
		for (int j = 0; j < hLen; ++j)
			od[j] += id[j] * cd[j];
	}
#endif
#endif
}

/**
 * Rotate the DFT input to eliminate the phase shift.
 *
 * Description:\n
 * 	Rotates the output of the WOLA filter to align the first sample
 * 	with the beginning of an FFT.  The rotation is done from the
 * 	work buffer into a second FFT input buffer for efficiency.
 */
void
Dfb::rotate(complex<float> *src, complex<float> *dest)
{
#ifdef ROTATE_DATA
	start %= fftLen;
	int end = fftLen - start;
	memcpy(&dest[0], &src[start], end * sizeof(complex<float>));
	memcpy(&dest[end], &src[0], start * sizeof(complex<float>));
	start += overlap;
#endif
}

}