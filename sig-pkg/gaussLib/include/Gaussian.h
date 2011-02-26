/*******************************************************************************

 File:    Gaussian.h
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
 * Gaussian.h
 *
 *  Created on: Apr 22, 2009
 *      Author: kes
 *
 *  Gaussian generator uses the Mersenne Twister random number generator
 *  code by Takuji Nishimura and Makoto Matsumoto (Mersenne.cpp).
 */

#ifndef GAUSSIAN_H_
#define GAUSSIAN_H_

#include <vector>
#include <Sonata.h>
#include <Types.h>
#include "GaussVersion.h"
#include "Signals.h"
#include "SmallTypes.h"

using namespace sonata_lib;

namespace gauss {

/**
 * Signal list element.
 *
 * Description:\n
 * 	Describes a signal to be inserted into the sample stream.  Signals may be
 * 	either one of a set of predefined types or a signal defined by the caller.
 */
struct Sig {
	SigType type;						// signal type
	BasicSig *sig;						// signal description
	void (*genSignal)(BasicSig *, ComplexFloat64&, uint64_t);

	Sig(): type(NoSignal), sig(0), genSignal(0) {}
	Sig(SigType type_, BasicSig *sig_,
			void (*genSignal_)(BasicSig *, ComplexFloat64&, uint64_t)):
			type(type_), sig(sig_), genSignal(genSignal_) {}
};

typedef std::vector<Sig> SigList;

class Gaussian {
public:
	Gaussian(int32_t seed_ = 0);
	~Gaussian();

	unsigned int getVersion() { return (GAUSS_VERSION); }
	unsigned int getIfVersion() { return (GAUSS_IFVERSION); }
	void setup(int32_t seed_, float64_t bandwidthMHz_, float64_t avgPower_);
	void setSeed(int32_t seed_);
	int32_t getSeed() { return (seed); }
	void setBandwidth(float64_t bandwidthMHz_) { bandwidthMHz = bandwidthMHz_; }
	float64_t getBandwidth() { return (bandwidthMHz); }
	void setNoisePower(float64_t avgPower_) { avgPower = avgPower_; }
	float64_t getNoisePower() { return (avgPower); }
	void addCwSignal(float64_t freq_, float64_t drift_, float64_t snr_);
	void addPulseSignal(float64_t freq_, float64_t drift_, float64_t snr_,
			float64_t tStart_, float64_t tOn_, float64_t tOff_);
	void addUserSignal(BasicSig *basicSig,
			void (*genSig)(BasicSig *, ComplexFloat64&, uint64_t));
	void addSignal(float64_t freq_, float64_t drift_, float64_t snr_);
	int64_t getSampleCnt() { return (sampleCnt); }
	ComplexFloat64 &getPower() { return (power); }
	ComplexFloat64 &getSum() { return (sum); }

	void getSamples(ComplexPair *data, int32_t n);
	void getSamples(ComplexFloat4 *data, int32_t n);
	void getSamples(ComplexInt8 *data, int32_t n);
	void getSamples(ComplexInt16 *data, int32_t n);
	void getSamples(ComplexFloat32 *data, int32_t n);
	void getSamples(ComplexFloat64 *data, int32_t n);
	ComplexFloat64 getSample();

private:
	int32_t seed;						// seed
	int64_t sampleCnt;					// current sample #
	float64_t bandwidthMHz;				// total bandwidth (MHz)
	float64_t avgPower;					// average noise power
	ComplexFloat64 power;				// total power of all samples
	ComplexFloat64 sum;					// sum of all samples
	SigList signals;					// list of injected signals

	ComplexFloat64 dirtyGauss();		// generator of dirty Gaussian noise
};

}

#endif /* GAUSSIAN_H_ */