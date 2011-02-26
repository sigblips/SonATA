/*******************************************************************************

 File:    Signals.cpp
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
 * Signals.cpp: generic signal types, supported internally.
 *
 *  Created on: May 28, 2009
 *      Author: kes
 */

#include "Signals.h"

namespace gauss {

/**
 * Basic signal functions.
 *
 * Notes:\n
 * 	If pwr_ (average noise power) is greater than zero, the snr_ represents
 * 	the power of the signal relative to the noise power.
 *  If pwr_ is less than or equal to zero (i.e., there is no noise), then
 *  snr_ is the average power of the signal in a 1Hz channel.  This allows us to
 *  generate pure sinusoids without noise, which makes certain kinds of
 *  testing much easier.
 */
BasicSig::BasicSig(float64_t bw_, float64_t pwr_, float64_t freq_,
		float64_t drift_, float64_t snr_): bandwidth(MHZ_TO_HZ(bw_)),
		freq(freq_), drift(drift_), snr(snr_), vector(1, 0), omega(0, 0),
		dOmega(0, 0), norm(1)
{
	float64_t dTheta = 2 * M_PI * MHZ_TO_HZ(freq) / bandwidth;
	float64_t d2Theta = M_PI * drift / (bandwidth * bandwidth);

	vector = ComplexFloat96(1, 0);
	omega = ComplexFloat96(cosl(dTheta + d2Theta), sinl(dTheta + d2Theta));
	dOmega = ComplexFloat96(cosl(2 * d2Theta), sinl(2 * d2Theta));
	norm = sqrt(snr / bandwidth);
	if (pwr_ > 0)
		norm *= sqrt(pwr_);
#ifdef notdef
	else
		norm = sqrt(snr);
#endif
}

PulseSig::PulseSig(float64_t bw_, float64_t pwr_, float64_t freq_,
		float64_t drift_, float64_t snr_, float64_t tStart_, float64_t tOn_,
		float64_t tOff_): BasicSig(bw_, pwr_, freq_, drift_, snr_),
		tStart(tStart_), secPerSample(0), period(0), duty(0)
{
	secPerSample = 1 / bandwidth;
	period = tOn_ + tOff_;
	duty = tOn_ / period;
}

/*************************************************************************
 * Predefined signal implementations.
 */

/**
 * Insert a CW signal into the sample.
 *
 * Description:\n
 * 	Computes the value for a CW signal and adds it to the sample.
 */
void
genCwSignal(CwSig *sig, ComplexFloat64& sample, uint64_t sampleNum)
{
	ComplexFloat96 signal = sig->vector;
	signal *= sig->norm;
	sample += signal;

	// update the angle and rotation rate
	sig->update();
}

/**
 * Insert a pulse signal into the sample.
 *
 * Description:\n
 * 	Computes the value for a pulse signal and adds it to the sample.\n\n
 * Notes:\n
 * 	The pulse train is created by simply turning a sinusoidal CW signal
 * 	on and off according to the period and duty cycle.\n
 * 	The start time is not used, but implementing it should be very simple.
 */
void
genPulseSignal(PulseSig *sig, ComplexFloat64& sample, uint64_t sampleNum)
{
	float64_t t = sampleNum * sig->secPerSample;
	float64_t cycle = t / sig->period;
	float64_t phase = modf(cycle, &cycle);
	if (phase < sig->duty) {
		ComplexFloat96 signal = sig->vector;
		signal *= sig->norm;
		sample += signal;
	}
	// update the angle and rotation rate
	sig->update();
}

}