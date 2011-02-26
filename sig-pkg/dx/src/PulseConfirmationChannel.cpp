/*******************************************************************************

 File:    PulseConfirmationChannel.cpp
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
// PulseConfirmationChannel: Pulse confirmation channel class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseConfirmationChannel.cpp,v 1.5 2009/05/24 22:57:58 kes Exp $
//
#include "PulseConfirmationChannel.h"

namespace dx {

PulseConfirmationChannel::PulseConfirmationChannel()
	: fftPulsePlan(0)
	, fftPulsePlanSize(0)
{
}

PulseConfirmationChannel::~PulseConfirmationChannel()
{
	if (fftPulsePlan)
		fftwf_destroy_plan(fftPulsePlan);
}

//
// extractBin: extract a single bin from the confirmation data
//
// Notes:
//		The bin is returned in complexFloat32 form.
//		The appropriate block of confirmation data is synthesized into
//		a wide confirmation channel.
//		The wide channel is dedrifted to the doppler between the primary
//		and secondary sites.
//		An FFT is performed to create the spectrum containing the bin
//		being extracted.
//		The bin number (relative to the center bin of the channel) is
//		computed from the relative frequencies.
//		Assumes that dedrift parameters have already been computed
//		by a prior call to computeDedriftParams.
//
Error
PulseConfirmationChannel::extractBin(ComplexPair *cdData,
		ComplexFloat32 *binVal, int32_t spectrum, float64_t freq,
		Resolution res)
{
	// the spectrum specified is for the final bin resolution; it must
	// be converted to the equivalent spectrum in the confirmation
	// data.  This depends upon the resolution of the detection.
	// Since we are overlapping spectra, the CD spectrum is
	// divided by two.
	int32_t cdSamplesPerSpectrum =
		activity->getUsableBinsPerSpectrum(res) / activity->getUsableSubchannels();
	int32_t totalSamplesOrBins = cdSamplesPerSpectrum * subchannels;
	int32_t cdStartSpectrum = spectrum * cdSamplesPerSpectrum / 2;

	// build the time sequence for a single spectrum
	ComplexFloat32 confData[subchannels];
	ComplexFloat32 confTDData[totalSamplesOrBins];
	for (int32_t i = 0; i < cdSamplesPerSpectrum; ++i) {
		assembleConfData(cdData, confData, cdStartSpectrum+i);
		synthesizeConfChannel(confData, &confTDData[i*subchannels], 0);
	}

	// we now have a block of wide channel data; transform it to
	// produce the bins requested.  This transform is done in place
	// and returns the data in the same
	if (!fftPulsePlan || fftPulsePlanSize != totalSamplesOrBins) {
		if (fftPulsePlan)
			fftwf_destroy_plan(fftPulsePlan);
		fftPulsePlanSize = totalSamplesOrBins;
		ComplexFloat32 in[fftPulsePlanSize];
		fftPulsePlan = fftwf_plan_dft_1d(fftPulsePlanSize, (fftwf_complex *) in,
				(fftwf_complex *) in,  FFTW_FORWARD, FFTW_ESTIMATE);
	}
	fftwf_execute_dft(fftPulsePlan, (fftwf_complex *) confTDData,
			(fftwf_complex *) confTDData);


	// compute the bin number relative to the center of the channel
	float64_t f = freq - centerFreq;
	int32_t bin = (int32_t) (f / activity->getBinWidthMHz(res));
	if (bin < 0)
		bin += totalSamplesOrBins;

	*binVal = confTDData[bin];
	*binVal /= sqrt(subchannels*fftPulsePlanSize); // remove FFT scaling
	return (0);
}

}