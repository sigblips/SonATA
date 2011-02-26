/*******************************************************************************

 File:    CwConfirmationChannel.cpp
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
// CwConfirmationChannel: CWD confirmation channel class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwConfirmationChannel.cpp,v 1.5 2009/05/24 22:46:03 kes Exp $
//
#include "dedrift.h"
#include "CwConfirmationChannel.h"
#include "DxErr.h"
#include "Log.h"
#include "Signal.h"
#include "TransformWidth.h"

using std::cout;
using std::endl;

namespace dx {

CwConfirmationChannel::CwConfirmationChannel(): fftCwPlan(0)
{
}

CwConfirmationChannel::~CwConfirmationChannel()
{
	if (fftCwPlan)
		fftwf_destroy_plan(fftCwPlan);
}

/**
 * Create the signal channel from a set of subchannels.
 *
 * Description:\n
 * 	Performs an inverse FFT from several subchannels of confirmation
 * 	data to create a wider channel for coherent detection or secondary
 * 	confirmation.\n\n
 * Notes:\n
 * 	This channel is created and used in two distinct situations: (1)
 * 	when a signal is detected with DADD, a coherent detection is performed
 * 	using the DADD bin and drift to more accurately describe the signal's
 * 	starting bin, width and SNR.  (2) When a signal is detected in another
 * 	beam, the detector may be asked to do a restricted, coherent search
 * 	for the signal to determine whether or not it is visible.  This is
 * 	known as secondary or confirmation detection.\n
 * 	Synthesis of the channel is not simply an n-point inverse FFT to generate
 * 	n time samples of the wider channel.  This is because the output
 * 	subchannels are oversampled, and are thus wider than the spacing between
 * 	adjacent subchannels.  When we perform the inverse FFT, we must remove
 * 	the samples which are overlapped between spectra.\n
 *  As an example, if 1024 subchannels are created from a channel, a
 *  1024-point FFT is performed on 1024 channel time samples.  Assume an
 *  oversampling of 25%, so that the last 256 samples used to create
 *  spectrum n are used as the first 256 samples in creation of spectrum n+1.
 *  If we wanted to reverse this process and recreate the original time
 *  series, we would take a 1024-point inverse FFT of spectrum n, then
 *  a second 1024-point FFT of spectrum n+1.  BUT, since 256 of the
 *  time samples overlap between the spectra, we must use only the first
 *  768 time samples from spectrum n, the first 768 from spectrum n+1,
 *  and so on for following spectra.\n
 *  Thus, to synthesize a narrower channel from a subset of the subchannels,
 *  we will not use all the time samples created by the inverse FFT for
 *  each spectrum.  Again, an example: if we want to synthesize a
 *  confirmation channel from 16 subchannels, we would perform a 16-point
 *  inverse FFT from the 16-point subspectrum n containing those channels.
 *  This would create 16 time samples for each FFT performed, of which we
 *  would keep 12 from each spectrum processed.  Thus, where we would
 *  expect performing an n-point inverse FFT on m spectra to produce a
 *  total of n * m time-series points, we would actually produce only
 *  n + .75 * n * (m - 1) time samples.\n
 *  Note that this limits the choices of n for a given oversampling, because
 *  we must keep an integral number of samples for each inverse FFT.  For
 *  25% oversampling, n can be 4 (keep 3 samples), 8 (keep 6 samples),
 *  16 (keep 12 samples) , etc., while for 18.75% oversampling
 *  it can be 16 (keep 13 samples), 32 (keep 26), ..., and for 20.3125% it
 *  must be a minimum of 64 (keep 51 samples).
 */
Error
CwConfirmationChannel::createSignalChannel(ComplexPair *cdData,
		ComplexFloat32 *sigData, Resolution res, float32_t *basePower)
{
	int32_t spectrum, bufSamples = 0, sigSample = 0;
	ComplexFloat32 confData[subchannels];

	// get the length of the primary dedrift block; this determines
	// the bandwidth of the signal channel
	TransformWidth *transform = TransformWidth::getInstance();
	confSamplesPerSigSample = static_cast<int32_t>
			(subchannels * transform->getSigChanWidth(res));
	ComplexFloat32 confTDData[confSamplesPerSigSample];

	// get the number of samples in a signal spectrum; this determines
	// the actual bandwidth of the signal bins
	sigSamplesPerSpectrum = transform->getSigSamplesPerSpectrum(res);

	if (confSamplesPerSigSample <= 0) {
		LogFatal(ERR_ICW, activity->getActivityId(),
				"subchannels = %d, sig chan width = %.3f",
				subchannels, transform->getSigChanWidth(res));
		Fatal(ERR_ICW);
	}

	sigSamples = samples / confSamplesPerSigSample;
	sigSpectra = sigSamples / sigSamplesPerSpectrum;

	Debug(DEBUG_CWD_CONFIRM, 0, "compute dedrift params");

	computeDedriftParams();

	// the total number of samples is the number of spectra times
	// the number of subchannels per spectrum
	Debug(DEBUG_CWD_CONFIRM, confSamplesPerSigSample, "confSamplesPerSig");
	for (spectrum = 0; spectrum < subchannelSpectra; spectrum++) {
		Debug(DEBUG_CWD_CONFIRM, spectrum, "assemble");
		assembleConfData(cdData, confData, spectrum);
		Debug(DEBUG_CWD_CONFIRM, spectrum, "synthesize");
		synthesizeConfChannel(confData, &confTDData[bufSamples], basePower);
		Debug(DEBUG_CWD_CONFIRM, bufSamples, "bufSamples");
		bufSamples += subchannels;
		Debug(DEBUG_CWD_CONFIRM, bufSamples, "bufSamples");
		while (bufSamples >= confSamplesPerSigSample) {
			Debug(DEBUG_CWD_CONFIRM, bufSamples, "extract");
			extractSignalChannel(confTDData, &sigData[sigSample],
					sigSample * confSamplesPerSigSample, basePower);
			sigSample++;
			bufSamples -= confSamplesPerSigSample;
			// if we have leftover samples, shift them up in the buffer
			if (bufSamples > 0) {
				memcpy(confTDData,
						&confTDData[bufSamples+confSamplesPerSigSample],
						bufSamples * sizeof(ComplexFloat32));
			}
		}
	}
	Debug(DEBUG_CWD_CONFIRM, 0, "wide channel created");
	return (0);
}

//
// extractSignalChannel: dedrift and heterodyne the confirmation channel
//		to remove all frequency and drift from the nominal signal, then
//		perform an FFT to extract the signal.
//
// Notes:
//		The width of the signal channel is dependent on the bandwidth of
//		the nominal signal.  For CWD, which has a limited bandwidth
//		range of 1 - 4 Hz, the signal channel is ~21Hz wide; this is
//		obtained by performing a 512-point FFT on the dedrifted and
//		heterodyned data.
//
Error
CwConfirmationChannel::extractSignalChannel(ComplexFloat32 *confData,
		ComplexFloat32 *sigData, int32_t firstSample, float32_t *basePower)
{
	// compute a signal spectra as a function of the signal channel width;
	// there is one signal channel sample produced by each signal channel
	// spectrum
	DedriftFTPlane(confData, sigData, 1, confSamplesPerSigSample, firstSample,
			fShiftPerSample, dShiftPerSample, NULL);

	return (0);
}

//
// createSignalSpectra: compute the set of spectra from the signal channel data
//
Error
CwConfirmationChannel::createSignalSpectra(ComplexFloat32 *sigData,
		ComplexFloat32 *spectrumData, float32_t basePower)
{
	int32_t spectrum, len, dLen;
	ComplexFloat32 *sigSpectrum, temp[sigSamplesPerSpectrum/2];

	// create a new signal channel plan, if necessary
	if (sigSamplesPerSpectrum != prevSamplesPerSpectrum) {
		if (fftCwPlan)
			fftwf_destroy_plan(fftCwPlan);
		ComplexFloat32 in[sigSpectra*sigSamplesPerSpectrum],
				out[sigSpectra*sigSamplesPerSpectrum];
		int n = sigSamplesPerSpectrum;
		fftCwPlan = fftwf_plan_many_dft(1, &n, sigSpectra,
				(fftwf_complex *) in, &n, 1, sigSamplesPerSpectrum,
				(fftwf_complex *) out, &n, 1, sigSamplesPerSpectrum,
				FFTW_FORWARD, FFTW_MEASURE);
	}
	prevSamplesPerSpectrum = sigSamplesPerSpectrum;

	fftwf_execute_dft(fftCwPlan, (fftwf_complex *) sigData,
			(fftwf_complex *) spectrumData);

	// rearrange the data for detection
	sigSpectrum = spectrumData;
	len = sigSamplesPerSpectrum;
	dLen = len * sizeof(ComplexFloat32) / 2;
	for (spectrum = 0; spectrum < sigSpectra; spectrum++, sigSpectrum += len) {
		memcpy(&temp[0], &sigSpectrum[0], dLen);
		memcpy(&sigSpectrum[0], &sigSpectrum[len/2], dLen);
		memcpy(&sigSpectrum[len/2], &temp[0], dLen);
	}
	return (0);
}

//
// createPowerSpectra: convert the signal spectra to power
//
Error
CwConfirmationChannel::createPowerSpectra(ComplexFloat32 *spectrumData,
		float32_t *powerData, float32_t basePower)
{
	int32_t spectrum, bin;
	ComplexFloat32 *sData = reinterpret_cast<ComplexFloat32 *> (spectrumData);

	// normalize the power by the number of spectra
	basePower /= sigSpectra;

	for (spectrum = 0; spectrum < sigSpectra; spectrum++) {
		for (bin = 0; bin < sigSamplesPerSpectrum; bin++, powerData++, sData++)
			*powerData = norm(*sData) / basePower;
	}
	return (0);
}

//
// computeDedriftParams: compute the parameters for dedrifting and
//		and heterodyning the nominal signal to baseband
//
void
CwConfirmationChannel::computeDedriftParams()
{
	float64_t confSamplesPerSec, confChanFreq, driftPerSample;

	// the number of samples per second in the channel is the same
	// as the channel width, since we are not overlapping samples
	confSamplesPerSec = samples / activity->getDataCollectionTime();
//	cout << "confSamplesPerSec " << confSamplesPerSec << endl;

	// compute the signal frequency relative to the center of the
	// confirmation channel
	confChanFreq = MHZ_TO_HZ(sig->path.rfFreq - centerFreq);
//	cout << "rfFreq " << sig->path.rfFreq << endl;
//	cout << "centerFreq " << centerFreq << endl;
//	cout << "confChanFreq " << confChanFreq << endl;

	// compute the phase shift per sample due to frequency offset
	fShiftPerSample = confChanFreq / confSamplesPerSec;
//	cout << "fShiftPerSample " << fShiftPerSample << endl;

	// compute the total drift of the signal (in Hz) over the observation
	driftPerSample = sig->path.drift / confSamplesPerSec;
//	cout << "driftPerSample " << driftPerSample << endl;

	// compute the phase shift per sample due to drift
	dShiftPerSample = driftPerSample / confSamplesPerSec;
//	cout << "dsShiftPerSample " << dShiftPerSample << endl;

	LogInfo(0, activity->getActivityId(),
			"f = %.8lf, d = %.8lf, fShift = %.8le, dShift = %.8le",
			confChanFreq, sig->path.drift, fShiftPerSample, dShiftPerSample);
}

}
