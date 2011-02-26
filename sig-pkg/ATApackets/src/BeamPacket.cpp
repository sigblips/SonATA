/*******************************************************************************

 File:    BeamPacket.cpp
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
#include "BeamPacket.h"

typedef char v16s __attribute__ ((vector_size(16)));
typedef short v8s __attribute__ ((vector_size(16)));
typedef int32_t v4w __attribute__ ((vector_size(16)));
typedef float v4sf __attribute__ ((vector_size(16)));

complex<float>
BeamPacket::getSample(int32_t i)
{
#if (BEAM8)
	complex<int8_t> *p = reinterpret_cast<complex<int8_t> *>
			(&packet.data.samples[i]);
#else
	complex<int16_t> *p = reinterpret_cast<complex<int16_t> *>
			(&packet.data.samples[i]);
#endif
	int32_t re = p->real();
	int32_t im = p->imag();
	return (complex<float> (re, im));
}

void
BeamPacket::getSamples(complex<float> *data)
{
#if (BEAM8)
	complex<int8_t> *p = reinterpret_cast<complex<int8_t> *>
			(packet.data.samples);
#else
	complex<int16_t> *p = reinterpret_cast<complex<int16_t> *>
			(packet.data.samples);
#endif
	for (uint32_t i = 0; i < packet.hdr.len; ++i)
		data[i] = complex<float>(p[i].real(), p[i].imag());
}

void
BeamPacket::putSample(int32_t i, const complex<float>& s)
{
#if (BEAM8)
	complex<int8_t> *p = reinterpret_cast<complex<int8_t> *>
			(&packet.data.samples[i]);
	*p = complex<int8_t>(saturate(s.real()), saturate(s.imag()));
#else
	complex<int16_t> *p = reinterpret_cast<complex<int16_t> *>
			(&packet.data.samples[i]);
	*p = complex<int16_t>(saturate(s.real()), saturate(s.imag()));
#endif
}

void
BeamPacket::putSamples(const complex<float> *data)
{
#if (BEAM8)
#if defined(__x86_64__) || defined(__SSE2__)
	complex<int8_t> *p = (complex<int8_t> *) packet.data.samples;
	register __m128 d[2];
	register v4w w;
	register v8s s[2];
	for (uint32_t i = 0; i < packet.hdr.len; i += 8) {
		asm("movaps %1, %0" : "=x" (d[0]) : "m" (data[i]));
		asm("movaps %1, %0" : "=x" (d[1]) : "m" (data[i+2]));
		asm("cvtps2dq %1, %0" : "=x" (s[0]) : "x" (d[0]));
		asm("cvtps2dq %1, %0" : "=x" (w) : "x" (d[1]));
		asm("packssdw %1, %0" : "+x" (s[0]) : "x" (w));

		asm("movaps %1, %0" : "=x" (d[0]) : "m" (data[i+4]));
		asm("movaps %1, %0" : "=x" (d[1]) : "m" (data[i+6]));
		asm("cvtps2dq %1, %0" : "=x" (s[1]) : "x" (d[0]));
		asm("cvtps2dq %1, %0" : "=x" (w) : "x" (d[1]));
		asm("packssdw %1, %0" : "+x" (s[1]) : "x" (w));
		asm("packsswb %1, %0" : "+x" (s[0]) : "x" (s[1]));

		asm("movups %1, %0" : "=m" (p[i]) : "x" (s[0]));
	}
#else
	complex<int8_t> *samples = (complex<int8_t> *) packet.data.samples;
	for (uint32_t i = 0; i < packet.hdr.len; ++i) {
		samples[i] = complex<int8_t>(saturate(data->real()),
				saturate(data->imag()));
	}
#endif

#else // 16-bit samples
#if defined(__x86_64__) || defined(__SSE2__)
	complex<int16_t> *p = (complex<int16_t> *) packet.data.samples;
	register __m128 d[2];
	register v4w w[2];
	for (int32_t i = 0; i < packet.hdr.len; i += 4) {
		asm("movaps %1, %0" : "=x" (d[0]) : "m" (data[i]));
		asm("movaps %1, %0" : "=x" (d[1]) : "m" (data[i+2]));
		asm("cvtps2dq %1, %0" : "=x" (w[0]) : "x" (d[0]));
		asm("cvtps2dq %1, %0" : "=x" (w[1]) : "x" (d[1]));
		asm("packssdw %1, %0" : "+x" (w[0]) : "x" (w[1]));
		asm("movups %1, %0" : "=m" (p[i]) : "x" (w[0]));
	}
#else
	for (int32_t i = 0; i < packet.hdr.len; ++i) {
		samples[i] = complex<int8_t>(saturate(data->real()),
				saturate(data->imag()));
	}
#endif
#endif
}