/*******************************************************************************

 File:    Signals.h
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

/**
 * Signals.h
 *
 *  Created on: May 28, 2009
 *      Author: kes
 */

#ifndef SIGNALS_H_
#define SIGNALS_H_

#include <Sonata.h>
#include <Types.h>

using namespace sonata_lib;

namespace gauss {

const float64_t DEFAULT_BANDWIDTH = 104.8576;
const float64_t DEFAULT_BEAM_BANDWIDTH = 104.8576;
const float64_t DEFAULT_CHAN_BANDWIDTH =  0.5461333333;
const float64_t DEFAULT_POWER = 1.0;

enum SigType {
	NoSignal,
	CwSignal,
	PulseSignal,
	UserSignal
};

struct BasicSig {
	float64_t bandwidth;				// total bandwidth (Hz)
	float64_t freq;						// frequency (MHz)
	float64_t drift;					// drift (Hz/sec)
	float64_t snr;						// SNR

	ComplexFloat96 vector;				// current signal vector
	ComplexFloat96 omega;				// current rotation rate
	ComplexFloat96 dOmega;				// change of rotation rate
	float64_t norm;						// normalization factor for power

	BasicSig(): vector(1, 0), omega(0, 0), dOmega(0, 0), norm(1) {}
	BasicSig(float64_t bw_, float64_t pwr_, float64_t freq_, float64_t drift_,
			float64_t snr_);
	void update() { vector *= omega; omega *= dOmega; }
};

struct CwSig: public BasicSig {
	CwSig(): BasicSig() {}
	CwSig(float64_t bw_, float64_t pwr_, float64_t freq_, float64_t drift_,
			float64_t snr_): BasicSig(bw_, pwr_, freq_, drift_, snr_) {}
};

struct PulseSig: public BasicSig {
	float64_t tStart;					// start time of first pulse
	float64_t secPerSample;				// sec per sample
	float64_t period;					// period
	float64_t duty;						// duty cycle

	PulseSig(float64_t bw_, float64_t pwr_, float64_t freq_, float64_t drift_,
			float64_t snr_, float64_t tStart_, float64_t tOn_, float64_t tOff_);
};

// internally supported signal types
void genCwSignal(CwSig *sig, ComplexFloat64& sample, uint64_t sampleNum);
void genPulseSignal(PulseSig *sig, ComplexFloat64& sample, uint64_t sampleNum);

}

#endif /* SIGNALS_H_ */