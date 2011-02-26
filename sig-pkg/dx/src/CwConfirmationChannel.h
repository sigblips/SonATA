/*******************************************************************************

 File:    CwConfirmationChannel.h
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwConfirmationChannel.h,v 1.4 2009/02/22 04:48:37 kes Exp $
//
#ifndef _CwConfirmationChannelH
#define _CwConfirmationChannelH

#include <fftw3.h>
#include "ConfirmationChannel.h"
#include "TransformWidth.h"

using namespace sonata_lib;

namespace dx {

//
// This class is derived from the ConfirmationChannel class,
// and is used to compute the confirmation signal channel for
// coherent CW detection
//
class CwConfirmationChannel: public ConfirmationChannel {
public:
	CwConfirmationChannel();
	~CwConfirmationChannel();

	int32_t getSignalSpectra() { return (sigSpectra); }
	int32_t getSignalBinsPerSpectrum() { return (sigSamplesPerSpectrum); }

	Error createSignalChannel(ComplexPair *cdData, ComplexFloat32 *sigData,
			Resolution res, float32_t *basePower);
	Error createSignalSpectra(ComplexFloat32 *sigData,
			ComplexFloat32 *spectrumData, float32_t basePower);
	Error createPowerSpectra(ComplexFloat32 *sigData, float32_t *powerData,
			float32_t basePower);
	Error dumpSignalChannel(Buffer *buf, string *diskFile = 0);
	Error dumpSignalSpectra(Buffer *buf, string *diskFile = 0);
	Error dumpPowerSpectra(Buffer *buf, string *diskFile = 0);

private:
	int32_t confSamplesPerSigSample;
	int32_t sigSamples;
	int32_t sigSamplesPerSpectrum;
	int32_t prevSamplesPerSpectrum;
	int32_t sigSpectra;
	float64_t fShiftPerSample;
	float64_t dShiftPerSample;
	fftwf_plan fftCwPlan;

	Error extractSignalChannel(ComplexFloat32 *confData,
			ComplexFloat32 *sigData, int32_t firstSample, float32_t *basePower);
	void computeDedriftParams();
};

}

#endif	
