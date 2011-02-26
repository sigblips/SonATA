/*******************************************************************************

 File:    Struct.h
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

#ifndef STRUCT_H_
#define STRUCT_H_

#include "Sonata.h"

using std::norm;

namespace sonata_lib {

// network host specification data
struct HostSpec {
	IpAddress addr;						// name/address
	int32_t port;						// port
	in_addr inaddr;						// host IP address

	HostSpec(): port(DEFAULT_PORT) {
		memcpy(addr, DEFAULT_ADDR, sizeof(addr));
	}
	HostSpec(const IpAddress addr_, int32_t port_): port(port_) {
		memcpy(addr, addr_, sizeof(addr));
	}
	HostSpec& operator=(const HostSpec&rhs) {
		memcpy(addr, rhs.addr, sizeof(addr));
		port = rhs.port;
		inaddr = rhs.inaddr;
		return (*this);
	}
};

// statistics
struct NetStatistics {
	uint64_t total;						// total # of packets
	uint64_t wrong;						// # of wrong packets
	uint64_t missed;					// # of missing packets
	uint64_t late;						// # of late packets
	uint64_t invalid;					// # of packets with invalid data

	NetStatistics() { reset(); }
	void reset() { total = wrong = missed = late = invalid = 0; }
};

struct SampleStatistics {
	uint64_t samples;					// total # of samples
	float64_t sumSq;					// sum of squares
	ComplexFloat64 min;					// min of samples
	ComplexFloat64 max;					// max of samples
	ComplexFloat64 sum;					// sum of samples

	SampleStatistics() { reset(); }
	void reset() {
		samples = 0;
		sumSq = 0;
		min = max = sum = ComplexFloat64(0, 0);
	}
	ComplexFloat64 getMin() const { return (min); }
	ComplexFloat64 getMax() const { return (max); }
	ComplexFloat64 getMean() const {
		ComplexFloat64 avg(0,0);
		if (samples > 0)
			avg = sum / (float64_t) samples;
		return (avg);
	}
	float64_t getMinPower() const { return (norm(min)); }
	float64_t getMaxPower() const { return (norm(max)); }
	float64_t getMeanPower() const {
		float64_t mean = 0;
		if (samples > 0)
			mean = sumSq / samples;
		return (mean);
	}
	float64_t getSD() const {
		if (samples > 2) {
			float64_t avg = getMeanPower();
			float64_t var = (sumSq - avg * avg / samples) / (samples - 1);
			if (var < 0)
				var = 0;
			return (sqrt(var));
		}
		return (0);
	}
};

}

#endif /*STRUCT_H_*/