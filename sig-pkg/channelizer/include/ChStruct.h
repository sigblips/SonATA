/*******************************************************************************

 File:    ChStruct.h
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

// Structures

#ifndef CHSTRUCT_H_
#define CHSTRUCT_H_

#include "System.h"
#include "Struct.h"

namespace chan {

// beam specification
struct BeamSpec {
	uint32_t src;						// beam source
	::uint8_t pol;						// polarization
	float64_t freq;						// center frequency
	float64_t bandwidth;				// bandwidth
	float64_t oversampling;				// oversampling(%)

	BeamSpec(): src(ATADataPacketHeader::UNDEFINED),
			pol((::uint8_t) ATADataPacketHeader::UNDEFINED),
			freq(0.0), bandwidth(0.0), oversampling(0.0) {}
	BeamSpec(uint32_t src_,::uint8_t pol_, float64_t freq_,
			float64_t bandwidth_, float64_t oversampling_):
			src(src_), pol(pol_), freq(freq_), bandwidth(bandwidth_),
			oversampling(oversampling_) {}
	BeamSpec& operator=(const BeamSpec& rhs) {
		src = rhs.src;
		pol = rhs.pol;
		freq = rhs.freq;
		bandwidth = rhs.bandwidth;
		oversampling = rhs.oversampling;
		return (*this);
	}

	friend ostream& operator << (ostream& s, const BeamSpec& beamSpec);
};

// channel specification
struct ChannelSpec {
	int32_t total;						// total # of channels
	int32_t usable;						// # of usable channels (~total * .9)
	int32_t foldings;					// # of foldings in filter bank
	
	ChannelSpec(): total(DEFAULT_TOTAL_CHANNELS),
			usable(DEFAULT_USABLE_CHANNELS), foldings(DEFAULT_FOLDINGS) {}
	ChannelSpec(int32_t total_, int32_t usable_,
			int32_t foldings_ = DEFAULT_FOLDINGS): total(total_),
			usable(usable_), foldings(foldings_)
			{}
};

}

#endif /*CHSTRUCT_H_*/