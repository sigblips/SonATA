/*******************************************************************************

 File:    DxUtil.cpp
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
// Utility functions
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/DxUtil.cpp,v 1.2 2009/02/22 04:41:41 kes Exp $
//
#include <unistd.h>
#include "DxUtil.h"
#include "Err.h"

using namespace sonata_lib;

namespace dx {

//
// BuildDaddArray: build the dadd short->long long translation array
//
#define XLSIZE			(1 << sizeof(short))

void
BuildDaddArray(uint64_t **xlat)
{
	int i;
	uint8_t val[8];
	uint64_t *xp;

	// allocate the array
	*xlat = new uint64_t[XLSIZE];

	// build the translation array
	xp = (uint64_t *) val;
	for (i = 0; i < XLSIZE; i++) {
		val[0] = i & 3;
		val[1] = (i >> 2) & 3;
		val[2] = (i >> 4) & 3;
		val[3] = (i >> 6) & 3;
		val[4] = (i >> 8) & 3;
		val[5] = (i >> 10) & 3;
		val[6] = (i >> 12) & 3;
		val[7] = (i >> 14) & 3;
		(*xlat)[i] = *xp;
	}
}

//
// powerOfTwo: return whether the specified number is a power of two
//
bool
IsPow2(uint32_t value)
{
	int i = 0;

	while (value) {
		if (value & 1)
			i++;
		value >>= 1;
	}
	return (i == 1);
}

//
// Log2: compute log base 2 of the value
//
//
uint32_t
Log2(uint32_t value)
{
	int32_t log;

	for (log = 0; value > 1; log++)
		value /= 2;
	return (log);
}

//
// Pow2: compute the nearest power of two less than
//		or equal to the specified number
//
uint32_t
Pow2(uint32_t value)
{
	uint32_t powerOfTwo;

	for (powerOfTwo = 1; powerOfTwo <= value; powerOfTwo <<= 1)
		if (!powerOfTwo)
			FatalStr(ERR_INT, "Internal error. Frames should not be 0.");
		;
	powerOfTwo >>= 1;
	return (powerOfTwo);
}

}