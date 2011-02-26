/*******************************************************************************

 File:    CwUnpacker.h
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
// CWD Unpacker class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwUnpacker.h,v 1.5 2009/03/06 22:10:38 kes Exp $
//
#ifndef _CwUnpackerH
#define _CwUnpackerH

#include "System.h"
#include "Args.h"
#include "Dadd.h"
#include "DxTypes.h"

using namespace dadd;
using namespace sonata_lib;

namespace dx {

class CwUnpacker {
public:
	CwUnpacker();
	virtual ~CwUnpacker();

	void unpack(DaddSlope slope, uint8_t *packed, uint64_t *unpacked,
			int32_t packedOfs, int32_t unpackedOfs, int32_t spectra,
			int32_t bins, int32_t packedStride, int32_t unpackedStride);

private:
	uint64_t *xlatPos, *xlatNeg;

	Args *cmdArgs;

	uint64_t *buildUnpackArray(DaddSlope slope);
	void unpackPos(uint8_t *packed, uint64_t *unpacked, int32_t spectra,
			int32_t bins, int32_t packedStride, int32_t unpackedStride);
	void unpackNeg(uint8_t *packed, uint64_t *unpacked, int32_t spectra,
			int32_t bins, int32_t packedStride, int32_t unpackedStride);

	// forbidden
	CwUnpacker(const CwUnpacker&);
	CwUnpacker& operator=(const CwUnpacker&);
};

}

#endif