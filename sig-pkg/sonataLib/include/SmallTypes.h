/*******************************************************************************

 File:    SmallTypes.h
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
 * SmallTypes.h: small data types used for data storage and archiving
 *
 *  Created on: Jun 9, 2009
 *      Author: kes
 */

#ifndef SMALLTYPES_H_
#define SMALLTYPES_H_

#include "Sonata.h"

namespace sonata_lib {

// define this to 1 to get normal microfloats (exponent bias = 1), to 2
// to get microfloats with half the dynamic range but twice the resolution
// (exponent bias = 2)
#define FLOAT4_BIAS		(2)

/**
 * Microfloats.
 *
 * Description:\n
 * 	Microfloats are tiny 4-bit floating-point values which can be used to store
 * 	data which is restricted in range to 0 - 6.5.  Specifically, they can be
 * 	used for storage of complex amplitude data, which consist of complex samples
 *  from subchannels which have been baselined to a mean sample power of one.
 *  By using floating-point values instead of integers, we can record a more
 * 	accurate set of samples.  Because the data has been baselined, and the
 * 	real and imaginary components have a Gaussian distribution, few or no
 * 	samples exceed the dynamic range.  Conversion from float32_t to
 * 	microfloat is performed by a simple table scan; conversion from
 * 	microfloat to float32_t is done by indexing into a 16-entry table.\n\n
 * Notes:\n
 * 	This is a storage format only; no arithmetic operations are supported.
 * 	Instead, functions are provided to convert between normal float32_t values
 * 	and the microfloat representation.\n
 * 	The encoding is similar to that of IEEE 794, but with changes to allow
 * 	maximum dynamic range.  The format is s ee f, where s is the sign bit (1 =
 * 	negative), ee is the 2-bit exponent with a bias of 1, and f is the single-
 * 	bit fraction.  For normalized values, the fraction bit is preceded by
 * 	an implicit 1. Zero, normalized values and denormalized numbers are as in
 * 	IEEE 794, but the infinities and NaN representations have been replaced
 * 	by normal processing to increase the dynamic range.\n
 * 	Value representation:\n
 * 		0 00 0 = 0\n
 * 		0 00 1 = 2^(0) * 0.1 = 1 * 0.5 = 0.5\n
 * 		0 01 0 = 2^(1-1) * 1.0 = 2^(0) * 1.0 = 1 * 1.0 = 1.0\n
 * 		0 01 1 = 2^(1-1) * 1.1 = 2^(0) * 1.1 = 1 * 1.5 = 1.5\n
 * 		0 10 0 = 2^(2-1) * 1.0 = 2^(1) * 1.0 = 2 * 1.0 = 2.0\n
 * 		0 10 1 = 2^(2-1) * 1.1 = 2^(1) * 1.1 = 2 * 1.5 = 3.0\n
 * 		0 11 0 = 2^(3-1) * 1.0 = 2^(2) * 1.0 = 4 * 1.0 = 4.0\n
 * 		0 11 1 = 2^(3-1) * 1.1 = 2^(2) * 1.1 = 4 * 1.5 = 6.0\n
 * 	Negative values have the same representation with the sign bit set to 1.\n
 * 	Note that resolution is increased at the low end of the range at the
 * 	expense of a representation for 5.  This is an acceptable tradeoff, for
 * 	far more values will lie at the low end of the dynamic range than at the
 * 	high end.\n\n
 * 	Note: it may be possible to improve low-end resolution even more by using
 * 	an exponent bias of 2, rather than 1.  This decreases the dynamic range to
 * 	0 - 3.5, but this may not be a problem in practice.  The representable
 * 	magnitudes are then 0, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0 and 3.0:
 * 		0 00 0 = 0\n
 * 		0 00 1 = 2^(-1) * 0.1 = 2^(-1) * 0.5 = 0.25\n
 * 		0 01 0 = 2^(1-2) * 1.0 = 2^(-1) * 1.0 = 0.5\n
 * 		0 01 1 = 2^(1-2) * 1.1 = 2^(-1) * 1.5 = 0.75\n
 * 		0 10 0 = 2^(2-2) * 1.0 = 2^(0) * 1.0 = 1.0\n
 * 		0 10 1 = 2^(2-2) * 1.1 = 2^(0) * 1.5 = 1.5\n
 * 		0 11 0 = 2^(3-2) * 1.0 = 2^(1) * 1.0 = 2.0\n
 * 		0 11 1 = 2^(3-2) * 1.1 = 2^(1) * 1.5 = 3.0\n
 */

/**
 * Statistics for conversion to float4_t.
 *
 * Description:\n
 * 	Keeps track of the total number of float32_t's converted to float4_t
 */
struct float4Statistics {
	uint64_t total;					// total number of values converted
	uint64_t saturated;				// # of values which saturated
	uint64_t histogram[8];			// histogram of converted values

	float4Statistics(): total(0), saturated(0) {
		for (int32_t i = 0; i < 8; ++i)
			histogram[i] = 0;
	}
};

class float4_t {
public:
	float4_t(): val(0) {}
	float4_t(float32_t f) { val = getfloat4(f); }
	float4_t(uint8_t f4): val((uint8_t) (f4 & 0xf)) {}

	operator float32_t();
	operator uint8_t();

	static uint8_t getfloat4(float32_t f);
	static const float4Statistics& getStatistics();

private:
	static float4Statistics statistics;
	uint8_t val;

	struct ff4 {
		float32_t fval;
		uint8_t f4val;
	};
	static ff4 fToF4[];
	static float32_t f4ToF[];
};

/**
 * Complex microfloats.
 *
 * Description:\n
 * 	A complex microfloat is a complex number in which the real and imaginary
 * 	components are microfloats.  For 4-bit microfloats, the two components
 * 	are packed into a single unsigned 8-bit integer, with the real part in
 * 	the upper 4 bits.\n\n
 * Notes:\n
 * 	This is a storage format only; no arithmetic operations are supported.
 * 	Functions are provided to convert between microfloat and standard
 * 	float32_t format.
 */
class ComplexFloat4 {
public:
	ComplexFloat4(): val(0) {}
	ComplexFloat4(const ComplexFloat64& f) {
		val = (float4_t::getfloat4(f.real()) << 4)
				| (float4_t::getfloat4(f.imag()));
	}
	ComplexFloat4(const ComplexFloat32& f) {
		val = (float4_t::getfloat4(f.real()) << 4)
				| (float4_t::getfloat4(f.imag()));
	}
	ComplexFloat4(const ComplexFloat4& f4) {
		val = (f4.real() << 4) | f4.imag();
	}
	// NOTE: the args should be const, but generates compiler error
	ComplexFloat4(float4_t& re, float4_t& im) {
		val = ((uint8_t) re << 4) | ((uint8_t) im);
	}
	ComplexFloat4(float32_t re, float32_t im) {
		val = (float4_t::getfloat4(re) << 4) | (float4_t::getfloat4(im));
	}
	ComplexFloat4(uint8_t re_, uint8_t im_) {
		val = ((re_ & 0xf) << 4) | (im_ & 0xf);
	}

	float4_t real() const { return (float4_t((uint8_t) ((val & 0xf0) >> 4))); }
	float4_t imag() const { return (float4_t((uint8_t) (val & 0xf))); }

	operator uint8_t() const { return (val); }
	operator ComplexFloat32() const;
	operator ComplexPair() const;

private:
	uint8_t val;
};

}

#endif /* SMALLTYPES_H_ */