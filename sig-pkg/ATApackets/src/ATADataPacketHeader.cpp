/*******************************************************************************

 File:    ATADataPacketHeader.cpp
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

#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include "basics.h"
#include "ATADataPacketHeader.h"

using namespace std;

/**
 * Convert a packet header absTime to a timeval struct.
 */
timeval
ATADataPacketHeader::absTimeToTimeval(uint64_t absTime_)
{
	timeval tv;
	tv.tv_sec = absTime_ >> 32;
	double f = (absTime_ & 0xffffffff) / exp2(32);
	tv.tv_usec = (uint32_t) (f * 1e6);
	return (tv);
}

/**
 * Convert a timeval struct to a packet header absTime value.
 */
uint64_t
ATADataPacketHeader::timevalToAbsTime(const timeval& tv)
{
	uint64_t t = ((uint64_t) tv.tv_sec) << 32;
	double fsec = tv.tv_usec / 1e6;
	uint32_t f = (uint32_t) (fsec * exp2(32));
	t |= f;
	return (t);
}

/**
 * Convert a long double (float96) to a packet header absTime value.
 */
uint64_t
ATADataPacketHeader::float96ToAbsTime(long double t)
{
	uint64_t s = (uint64_t) t;
	long double fsec = t - s;
	uint32_t f = fsec * exp2(32);
	s = (s << 32) | f;
	return (s);
}

void
ATADataPacketHeader::printHeader()
{
	cout << "ATADataPacketHeader" << endl;
	cout << hex;
	cout << " group " << (int32_t) this->group << endl;
	cout << " version " << (uint32_t) this->version << endl;
	cout << " bitsPerSample " << dec << (uint32_t) this->bitsPerSample << endl;
	cout << " binaryPoint " << (uint32_t) this->binaryPoint << endl;

	cout << " order " << hex << (uint32_t) this->order << endl;
	cout << " type " << (uint32_t) this->type << endl;
	cout << " streams " << (uint32_t) this->streams << endl;
	switch (this->polCode) {
	 case LCIRC:
		 cout << " polCode LCIRC" << endl;
		 break;
	 case RCIRC:
		 cout << " polCode RCIRC" << endl;
		 break;
	 case XLINEAR:
		 cout << " polCode XLINEAR" << endl;
		 break;
	 case YLINEAR:
		 cout << " polCode YLINEAR" << endl;
		 break;
	 default:
		 cout << "Unknown polCode " << hex << this->polCode << endl;
	}
	cout << " hdrLen " << dec << (uint32_t) this->hdrLen <<  endl;
	cout << " src " << dec << this->src << endl;
	cout << " chan " << this->chan << endl;
	cout << " seq " << this->seq << endl;
	cout << " freq " << std::setprecision(8) << this->freq << endl;
	cout << " sample rate " << std::setprecision(8) << this->sampleRate << endl;
	cout << " usable fraction " << this->usableFraction << endl;
	cout << " reserved " << this->reserved << endl;
	// convert the time to floating point
	uint32_t sec = (this->absTime >> 32);
	double fsec = (this->absTime & 0xffffffff);
	fsec /= exp2(32);
	long double t = sec + fsec;
	cout << " absTime " << std::setprecision(21) << t << endl;
	cout << " flags " << this->flags << endl;
	cout << " len " << this->len << endl;
}