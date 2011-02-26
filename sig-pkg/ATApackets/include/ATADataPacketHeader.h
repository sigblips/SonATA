/*******************************************************************************

 File:    ATADataPacketHeader.h
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

#ifndef	ATADATAPACKETHEADER_H_
#define	ATADATAPACKETHEADER_H_

#include <stdint.h>
#include "basics.h"

//
// Class definition for ATADataPacketHeader
// Author:
// Date: 04/28/06
//

class ATADataPacketHeader
{
public:
	enum Misc
	{
		CORRECT_ENDIAN = 0xaabbccdd
		, UNDEFINED = 0xffffffff
		, DEFAULT_STREAMS = 1
		, DEFAULT_FREQ = 0
		, DEFAULT_BITS = 16
		, DEFAULT_LEN = 1024
		, DEFAULT_BINARY_POINT = 0
		, DEFAULT_CHANNEL = 0
		, DEFAULT_BEAM = 0
		, DEFAULT_CENTER_FREQUENCY = 1420
		, DEFAULT_SAMPLING_RATE = 0
#ifdef notdef
		// type field bit flags
		, INTEGER = 0x00
		, FLOAT = 0x01
		, UNSIGNED = 0x00
		, SIGNED = 0x02
		, REAL = 0x00
		, COMPLEX = 0x04
		, DEFAULT_TYPE = INTEGER | SIGNED | COMPLEX
		// the following are currently defined groups
		, ATA = 0
		// below are packet formats ('src' field values)
		, BEAM_104MHZ = 0
		, CHAN_400KHZ = 1
#endif

		, BEAM8_BITS = 8
		, BEAM8_SAMPLES = 2048
		, BEAM16_BITS = 16
		, BEAM16_SAMPLES = 1024
		, CHANNEL_BITS = DEFAULT_BITS
		, CHANNEL_SAMPLES = 1024
		, INITIAL_VERSION = 0x00
		, VERSION_1 = 0x01
		// this should always be set to the latest version of the header
		, CURRENT_VERSION = VERSION_1
	};
	enum Group {
		ATA = 0x00
	};
	enum Source {
		BEAM_104MHZ = 0,
		CHAN_400KHZ = 1

	};
	enum PolarizationCode
	{
		NONE = -1,
		LCIRC = 0,
		RCIRC,
		XLINEAR,
		YLINEAR,
		BOTH
	};
	enum MarshallMode
	{
		LAZY
		, FORCE_BIG_ENDIAN
		, FORCE_LITTLE_ENDIAN
	};
	enum Type {
		INTEGER = 0x00,
		FLOAT = 0x01,
		UNSIGNED = 0x00,
		SIGNED = 0x02,
		REAL = 0x00,
		COMPLEX = 0x04,
		DEFAULT_TYPE = INTEGER | SIGNED | COMPLEX
	};
	static const float DEFAULT_USABLE_FRACTION = 0.6875;
	static const uint64_t TIME_NOT_SET = 0xffffffffffffffffLL;
	enum Flags {
		DATA_VALID = 1
	};

	uint8_t group, version, bitsPerSample, binaryPoint;
	uint32_t order;
	uint8_t type, streams, polCode, hdrLen;
	uint32_t src;
	uint32_t chan;
	uint32_t seq;
	double freq;
	double sampleRate;
	float usableFraction;
	float reserved;
	uint64_t absTime;
	uint32_t flags;
	uint32_t len;

	ATADataPacketHeader(uint8_t group_ = (uint8_t) UNDEFINED
			, uint32_t src_ = UNDEFINED
			, uint32_t chan_ = UNDEFINED
			, uint32_t len_ = DEFAULT_LEN
			, uint8_t bitsPerSample_ = DEFAULT_BITS
		)
		: group(group_)
		, version((uint8_t) CURRENT_VERSION)
		, bitsPerSample(bitsPerSample_)
		, binaryPoint((uint8_t) DEFAULT_BINARY_POINT)
		, order(CORRECT_ENDIAN)
		, type((uint8_t) DEFAULT_TYPE)
		, streams((uint8_t) DEFAULT_STREAMS)
		, polCode((uint8_t) UNDEFINED)
		, hdrLen(sizeof(ATADataPacketHeader))
		, src(src_)
		, chan(chan_)
		, seq(UNDEFINED)
		, freq(DEFAULT_FREQ)
		, sampleRate(DEFAULT_SAMPLING_RATE)
		, usableFraction(DEFAULT_USABLE_FRACTION)
		, reserved(0)
		, absTime(TIME_NOT_SET)
		, flags(0)
		, len(len_)
	{}

	static timeval absTimeToTimeval(uint64_t absTime_);
	static uint64_t timevalToAbsTime(const timeval& tv);
	static uint64_t float96ToAbsTime(long double t);
	void printHeader();

private:
	static int16_t exchangeBytes16(uint16_t in)
	{
		const uint16_t mask = 0x00ff;
		const int shift = 8;
		return ( ((in & mask) << shift) | ((in & ~mask) >> shift) );
	}
	static uint32_t exchangeBytes32(uint32_t in)
	{
		const uint32_t mask = 0x00ff00ff;
		const int shift = 8;
		return ( ((in & mask) << shift) | ((in & ~mask) >> shift) );
	}
	static uint32_t exchangeHalfWords32(uint32_t in)
	{
		const uint32_t mask = 0x0000ffff;
		const int shift = 16;
		return ( ((in & mask) << shift) | ((in & ~mask) >> shift) );
	}
	static uint64_t exchangeBytes64(uint64_t in)
	{
		const uint64_t mask = 0x00ff00ff00ff00ffLL;
		const int shift = 8;
		return ( ((in & mask) << shift) | ((in & ~mask) >> shift) );
	}
	static uint64_t exchangeHalfWords64(uint64_t in)
	{
		const uint64_t mask = 0x0000ffff0000ffffLL;
		const int shift = 16;
		return ( ((in & mask) << shift) | ((in & ~mask) >> shift) );
	}
	static uint64_t exchangeWords64(uint64_t in)
	{
		const uint64_t mask = 0x00000000ffffffffLL;
		const int shift = 32;
		return ( ((in & mask) << shift) | ((in & ~mask) >> shift) );
	}
	void flipEndian()
	{
		flip32(order);
		flip32(src);
		flip32(chan);
		flip32(seq);
		flip64((uint64_t&) freq);
		flip64((uint64_t&) sampleRate);
		flip32((uint32_t &) usableFraction);
		flip32((uint32_t &) reserved);
		flip64(absTime);
		flip32(flags);
		flip32(len);
	}

public:
	static void flipHalfWords32(uint32_t &in)
	{
		in = exchangeBytes32(in);
	}
	static void flip16(uint16_t &in)
	{
		in = exchangeBytes16(in);
	}
	static void flip32(uint32_t &in)
	{
		in = exchangeHalfWords32(exchangeBytes32(in));
	}
	static void flip64(uint64_t &in)
	{
		in = exchangeWords64(exchangeHalfWords64(exchangeBytes64(in)));
	}
	bool marshall(MarshallMode mode = LAZY)
	{
		if (mode == LAZY)
			return false;
		if (mode == FORCE_BIG_ENDIAN)
		{
			if ( (*(uint8_t *)&order) == 0xaa)
				return false;
		}
		else
		{
			if ( (*(uint8_t *)&order) == 0xdd)
				return false;
		}
		flipEndian();
		return true;
	}
	bool demarshall()
	{
		if (order == CORRECT_ENDIAN) return false;
		flipEndian();
		ASSERT(order == CORRECT_ENDIAN);
		return true;
	}
};

#endif // ATADATAPACKETHEADER_H_