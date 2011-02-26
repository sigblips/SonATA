/*******************************************************************************

 File:    ChannelPacket.cpp
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

#include <xmmintrin.h>
#include "basics.h"
#include "ChannelPacket.h"

typedef short v8s __attribute__ ((vector_size(16)));
typedef int32_t v4w __attribute__ ((vector_size(16)));
typedef float v4sf __attribute__ ((vector_size(16)));

complex<float>
ChannelPacket::getSample(int32_t i)
{
	complex<int16_t> *p = reinterpret_cast<complex<int16_t> *>
			(&packet.data.samples[i]);
	int8_t re = p->real();
	int8_t im = p->imag();
	return (complex<float> (re, im));
}

void
ChannelPacket::getSamples(complex<float> *data)
{
	complex<int16_t> *p = reinterpret_cast<complex<int16_t> *>
			(packet.data.samples);
	for (uint32_t i = 0; i < packet.hdr.len; ++i)
		data[i] = complex<float>(p[i].real(), p[i].imag());
}

void
ChannelPacket::putSample(int32_t i, const complex<float>& s)
{
	complex<int16_t> *p = reinterpret_cast<complex<int16_t> *>
			(&packet.data.samples[i]);
	int32_t re = saturate(s.real());
	int32_t im = saturate(s.imag());
	*p = complex<int16_t>(re, im);
}

void
ChannelPacket::putSamples(const complex<float> *data)
{
#if defined(__x86_64__) || defined(__SSE2__)
	complex<int16_t> *p = (complex<int16_t> *) packet.data.samples;
	register __m128 d[2];
	register v4w w[2];
	for (uint32_t i = 0; i < packet.hdr.len; i += 4) {
		asm("movaps %1, %0" : "=x" (d[0]) : "m" (data[i]));
		asm("movaps %1, %0" : "=x" (d[1]) : "m" (data[i+2]));
		asm("cvtps2dq %1, %0" : "=x" (w[0]) : "x" (d[0]));
		asm("cvtps2dq %1, %0" : "=x" (w[1]) : "x" (d[1]));
		asm("packssdw %1, %0" : "+x" (w[0]) : "x" (w[1]));
		asm("movups %1, %0" : "=m" (p[i]) : "x" (w[0]));
	}
#else
	complex<int16_t> *samples = (complex<int16_t> *) packet.data.samples;
	for (uint32_t i = 0; i < packet.hdr.len; ++i) {
		samples[i] = complex<int8_t>(saturate(data->real()),
				saturate(data->imag()));
	}
#endif
}
