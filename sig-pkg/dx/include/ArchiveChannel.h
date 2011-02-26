/*******************************************************************************

 File:    ArchiveChannel.h
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

/*
 * ArchiveChannel class
 */

#ifndef _ArchiveChannelH
#define _ArchiveChannelH

#include <fftw3.h>
#include <sseInterface.h>
#include "System.h"
#include "DxStruct.h"

namespace dx {

class ArchiveChannel {
public:
	ArchiveChannel();
	~ArchiveChannel();

	Error setup(int32_t nSubchan_, int32_t hf_, int32_t samplesPerHf_,
			int32_t stride_, float32_t oversampling_, float64_t freqMHz_,
			float64_t subchanWidthMHz_);
	int32_t getSamples() { return (ac.samples); }
	size_t getSize() { return (ac.samples * sizeof(ComplexFloat32)); }
	size_t getCdSize() { return (ac.samples * sizeof(ComplexPair)); }
	float64_t getWidth() { return (ac.widthMHz); }
	float64_t getFreq() { return (freqMHz); }
	float32_t getBaseline(ComplexFloat32 *data, int32_t n) {
		return (computeBaseline(data, n));
	}
	float32_t create(const ComplexPair *scData, bool baseline = true);
	float32_t create(const ComplexFloat32 *scData, bool baseline = true);
	float32_t getTdData(ComplexFloat32 *acData);
	void convert(const ComplexFloat32 *acData, ComplexPair *cdData,
			int32_t n = 0);
	void convert(const ComplexPair *cdData, ComplexFloat32 *acData,
			int32_t n = 0);
	int32_t getSamplesPerSignalSample(float64_t widthHz);
	int32_t getSignalSamples(float64_t widthHz);
	void extractSignalChannel(ComplexFloat32 *scData,
			float64_t fMHz, float64_t driftHz, float64_t& wHz,
			int32_t samples);
	int32_t getSignalSpectra(int32_t fftLen, int32_t samples, bool overlap);
	ComplexFloat32 extractBin(float64_t freq, float64_t wHz, int32_t spectrum,
			bool overlap);
	void createSignalSpectra(const ComplexFloat32 *tdData,
			ComplexFloat32 *fdData, int32_t fftLen, int32_t samples,
			bool overlap);
	void dedrift(ComplexFloat32 *iData, ComplexFloat32 *oData,
			float64_t fMHz, float64_t driftHz);
	void rescale(ComplexFloat32 *td, int32_t fftLen, int32_t nSamples);
private:
	struct s {
		int32_t fftLen;					// length of FFT
		int32_t spectra;				// # of spectra
		int32_t samples;				// total # of samples
		int32_t stride;					// stride
		float64_t widthMHz;				// width of the (sub)channel in MHz
		ComplexFloat32 *data;			// data buffer
		fftwf_plan plan;				// FFTW plan

		s(): fftLen(0), spectra(0), samples(0), stride(0), widthMHz(0), data(0),
				plan(0) {}
		void reset() {
			fftLen = spectra = samples = stride = 0;
			widthMHz = 0;
			if (data)
				fftwf_free(data);
			data = 0;
			if (plan)
				fftwf_destroy_plan(plan);
			plan = 0;
		}
	};
	s in;								// input subchannel data (oversampled)
	s ac;								// archive channel data
	s ss;								// signal spectra
	s bin;								// single bin (pulse) extraction
	int32_t nSubchan;					// number of subchannels
	int32_t hf;							// # of half frames
	int32_t samplesPerHf;				// # of samples per half frame
										// subchannel spectrum
	float32_t oversampling;				// percentage of subchannel oversampling
	float64_t freqMHz;					// center frequency of the channel
	ComplexFloat32 *spectrum;			// subchannel spectrum buffer
	ComplexFloat32 *driftBuf; 			// dedrift buffer

	float32_t createChannel(bool baseline);
	void baselineChannel(float32_t base);
	void assembleSpectrum(const ComplexPair *scData);
	void assembleSpectrum(const ComplexFloat32 *scData);
	void createTimeSamples(ComplexFloat32 *ad);
	void createPlan(fftwf_plan& plan, ComplexFloat32 *spectrum, int32_t n,
			int32_t dir);
	void removeOversampling(int32_t subch);
	void copyUsableBins(int32_t subch);
	float32_t computeBaseline(ComplexFloat32 *td, int32_t samples);
};

}

#endif /* _ArchiveChannelH */