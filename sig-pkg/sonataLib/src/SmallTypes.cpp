/*******************************************************************************

 File:    SmallTypes.cpp
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
 * SmallTypes.cpp
 *
 *  Created on: Jun 9, 2009
 *      Author: kes
 */

#include "SmallTypes.h"

namespace sonata_lib {

/**
 * Microfloats (4 bits)
 */
float4Statistics float4_t::statistics;

float4_t::ff4 float4_t::fToF4[] = {
#if (FLOAT4_BIAS == 1)
	{ .25, 0x0 },
	{ .75, 0x1 },
	{ 1.25, 0x2 },
	{ 1.75, 0x3 },
	{ 2.5, 0x4 },
	{ 3.5, 0x5 },
	{ 5.0, 0x6 },
#else
	{ .125, 0x0 },
	{ .375, 0x1 },
	{ .625, 0x2 },
	{ .875, 0x3 },
	{ 1.25, 0x4 },
	{ 1.75, 0x5 },
	{ 2.5, 0x6 },
#endif
	{ INFINITY, 0x7 }
};

float32_t float4_t::f4ToF[] = {
#if (FLOAT4_BIAS == 1)
		0.0, 0.5, 1.0, 1.5, 2.0, 3.0, 4.0, 6.0,
		-0.0, -0.5, -1.0, -1.5, -2.0, -3.0, -4.0, -6.0
#else
		0.0, 0.25, 0.50, .75, 1.0, 1.5, 2.0, 3.0,
		-0.0, -0.25, -0.50, -0.75, -1.0, -1.5, -2.0, -3.0
#endif

};

float4_t::operator float32_t()
{
	return (f4ToF[val&0xf]);
}

float4_t::operator uint8_t()
{
	return (val&0xf);
}

uint8_t
float4_t::getfloat4(float32_t f)
{
	++statistics.total;
	float32_t a = fabs(f);
	if (a > f4ToF[7])
		++statistics.saturated;
	int32_t i = 0;
	while (a >= fToF4[i].fval)
		++i;
	uint8_t v = fToF4[i].f4val;
	++statistics.histogram[v];
	return ((v && f < 0) ? v | 0x8 : v);
}

const float4Statistics&
float4_t::getStatistics()
{
	return (statistics);
}

ComplexFloat4::operator ComplexFloat32() const
{
	ComplexFloat32 f(real(), imag());
	return (f);
}

ComplexFloat4::operator ComplexPair() const
{
	ComplexPair p;
	p.pair = val;
	return (p);
}

}
