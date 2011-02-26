/*******************************************************************************

 File:    DxStruct.h
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
// Internal structures
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/DxStruct.h,v 1.4 2009/05/24 22:35:32 kes Exp $
//
#ifndef _DxStructH
#define _DxStructH

#include <bitset>
#include <iostream>
#include <map>
#include <sseInterface.h>
#include <sseDxInterface.h>
#include "Lock.h"
#include "Struct.h"

namespace dx {

#ifdef notdef
typedef uint64_t PulseKey;
typedef std::map<PulseKey, Pulse> PulseMap;


#define PULSE_KEY(p)		((PulseKey) ((((PulseKey) p.res) << 48) \
							| (((PulseKey) p.bin) << 24) \
							| ((PulseKey) p.spectrum)))
#endif

struct Bin {
	Polarization pol;
	Resolution res;
	float64_t freq;
	float32_t width;
	ComplexFloat32 bin;

	Bin() { reset(); }
	Bin(Polarization pol_, Resolution res_, float64_t freq_, float32_t width_):
			pol(pol_), res(res_), freq(freq_), width(width_), bin(0,0) {}
	void reset() {
		pol = POL_UNINIT;
		res = RES_UNINIT;
		freq = 0;
		width = 0;
		bin = ComplexFloat32(0, 0);
	}
};

struct Pulse {
	Polarization pol;
	Resolution res;
	int32_t bin;
	int32_t spectrum;
	float32_t power;
	float64_t freq;

	Pulse(): pol(POL_UNINIT), res(RES_UNINIT), bin(0), spectrum(0),
			power(0), freq(0) {}

	Pulse(Resolution res_, int32_t bin_, int32_t spectrum_,
			Polarization pol_, float32_t power_ = 0.0, float64_t freq_ = 0.0):
			pol(pol_), res(res_), bin(bin_), spectrum(spectrum_), power(power_),
			freq(freq_) {}

	Pulse(const Pulse& p): pol(p.pol), res(p.res), bin(p.bin), \
			spectrum(p.spectrum), power(p.power), freq(p.freq) {}

	Pulse& operator=(const Pulse& rhs) {
		pol = rhs.pol;
		res = rhs.res;
		bin = rhs.bin;
		spectrum = rhs.spectrum;
		power = rhs.power;
		freq = rhs.freq;
		return (*this);
	}

	friend ostream& operator << (ostream& s, const Pulse& p) {
	    s << "res = " << (std::dec) << p.res << ", pol = " << p.pol;
	    s << ", spectrum = " << p.spectrum << ", bin = " << p.bin;
	    s << ", power = " << p.power << std::endl;
	    return (s);
	}
};

struct PulseDiff {
	int32_t bins;
	int32_t spectra;

	PulseDiff(): bins(0), spectra(0) {}
	PulseDiff(const Pulse& p0, const Pulse& p1) {
		bins = p1.bin - p0.bin;
		spectra = p1.spectrum - p0.spectrum;
	}

	friend ostream& operator << (ostream& s, const PulseDiff& p) {
		s << (std::dec) << "spectra = " << p.spectra << ", bins = ";
		s << p.bins << std::endl;
		return (s);
	}
};

#ifdef notdef
enum ip_src {
	channelizer,
	sw_pattern
};
#endif

enum op_src {
	normal,
	tagged_data,
	by_pass     /* don't generate any output data */
};

enum ObsOps {
	ZERO_DC_BIN,
//	INVERT_FREQ,
	MAX_OBS_OPS
};

typedef std::bitset<MAX_OBS_OPS> OpsMask;

struct ObsData {
	uint32_t maxPulsesPerHalfFrame;
	uint32_t maxPulsesPerSubchannelPerHalfFrame;
	float64_t centerFreq;			// Center frequency for tuning
	Resolution cwResolution;		// which res gets CW processing
	uint32_t observationId;			// arbitrary # identifying observation
	bool resetBaseline;				// false starts from previous obs baseline
	uint32_t baselineDelay;			// # baseline halfFrames
	float32_t baselineWeighting;	// newBL = oldBL(weight) + curBL(1-weight)
	float32_t pulseThreshold;
	op_src cwOutputOption;
	op_src cdOutputOption;
	op_src blOutputOption;
	op_src pulseOutputOption;
	OpsMask ops;					// operations mask

	ObsData(): maxPulsesPerHalfFrame(0), maxPulsesPerSubchannelPerHalfFrame(0),
			centerFreq(0), cwResolution(RES_UNINIT), observationId(-1),
			resetBaseline(true), baselineDelay(0),
			baselineWeighting(1), pulseThreshold(0),
			cwOutputOption(normal),
			cdOutputOption(normal), blOutputOption(normal),
			pulseOutputOption(normal) {}
};

}

#endif
