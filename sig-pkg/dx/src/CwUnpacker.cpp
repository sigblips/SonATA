/*******************************************************************************

 File:    CwUnpacker.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwUnpacker.cpp,v 1.4 2009/03/06 22:10:38 kes Exp $
//
#include <fftw3.h>
#include "CwUnpacker.h"

namespace dx {

CwUnpacker::CwUnpacker()
{
	cmdArgs = Args::getInstance();
	Assert(cmdArgs);

	xlatPos = buildUnpackArray(Positive);
	xlatNeg = buildUnpackArray(Negative);
}

CwUnpacker::~CwUnpacker()
{
	delete xlatPos;
	delete xlatNeg;
}

void
CwUnpacker::unpack(DaddSlope slope, uint8_t *packed, uint64_t *unpacked,
		int32_t packedOfs, int32_t unpackedOfs, int32_t spectra,
		int32_t bins, int32_t packedStride, int32_t unpackedStride)
{
	int32_t binsPerXfer, xfersPerSpectrum;

	binsPerXfer = (int32_t) (sizeof(*packed) / CWD_BYTES_PER_BIN);
	xfersPerSpectrum = bins / binsPerXfer;

	packedOfs /= binsPerXfer;
	unpackedOfs /= binsPerXfer;
	packedStride /= binsPerXfer;
	unpackedStride /= binsPerXfer;

	// adjust data pointers by offset
	packed += packedOfs;
	unpacked += unpackedOfs;

	if (slope == Positive) {
		unpackPos(packed, unpacked, spectra, xfersPerSpectrum, packedStride,
				unpackedStride);
	}
	else {
		unpackNeg(packed, unpacked, spectra, xfersPerSpectrum, packedStride,
				unpackedStride);
	}
}

/**
 * Build a translation array to optimize unpacking of packed DADD buffers.
 *
 * Description:\n
 * 	Builds an array of DaddAccum-sized values which correspond to the
 * 	expanded equivalent of a block of
 */
uint64_t *
CwUnpacker::buildUnpackArray(DaddSlope slope)
{
	DaddAccum val[4];
	uint64_t *xlat;

	// allocate the array
	size_t size = CWD_XLAT_SIZE * sizeof(uint64_t);
	xlat = static_cast<uint64_t *> (fftwf_malloc(size));

	// initialize the array
	if (slope == Positive) {
		for (int32_t i = 0; i < CWD_XLAT_SIZE; ++i) {
			val[0] = i & 3;
			val[1] = (i >> 2) & 3;
			val[2] = (i >> 4) & 3;
			val[3] = (i >> 6) & 3;
			xlat[i] = *((uint64_t *) val);
		}
	}
	else {
		for (int32_t i = 0; i < CWD_XLAT_SIZE; ++i) {
			val[3] = i & 3;
			val[2] = (i >> 2) & 3;
			val[1] = (i >> 4) & 3;
			val[0] = (i >> 6) & 3;
			xlat[i] = *((uint64_t *) val);
		}
	}
	return (xlat);
}

void
CwUnpacker::unpackPos(uint8_t *packed, uint64_t *unpacked,
		int32_t spectra, int32_t xfersPerSpectrum, int32_t packedStride,
		int32_t unpackedStride)
{
	if (!cmdArgs->loadSliceNum()) {
		for (int32_t i = 0; i < spectra; i++) {
			for (int32_t j = 0; j < xfersPerSpectrum; j += 2) {
				unpacked[j] = xlatPos[packed[j]];
				unpacked[j+1] = xlatPos[packed[j+1]];
			}
			packed += packedStride;
			unpacked += unpackedStride;
		}
	}
	else {
		// don't really unpack
		for (int i = 0; i < spectra; i++) {
			for (int j = 0; j < xfersPerSpectrum; j++)
				unpacked[j] = packed[j];
			packed += packedStride;
			unpacked += unpackedStride;
		}
	}
}

void
CwUnpacker::unpackNeg(uint8_t *packed, uint64_t *unpacked,
		int32_t spectra, int32_t xfersPerSpectrum, int32_t packedStride,
		int32_t unpackedStride)
{
	// adjust the unpacked array pointer
	unpacked--;
	if (!cmdArgs->loadSliceNum()) {
		for (int i = 0; i < spectra; i++) {
			for (int j = 0; j < xfersPerSpectrum; j += 2) {
				unpacked[-j] = xlatNeg[packed[j]];
				unpacked[-j-1] = xlatNeg[packed[j+1]];
			}
			packed += packedStride;
			unpacked += unpackedStride;
		}
	}
	else {
		for (int i = 0; i < spectra; i++) {
			for (int j = 0; j < xfersPerSpectrum; j++)
				unpacked[-j] = packed[j];
			packed += packedStride;
			unpacked += unpackedStride;
		}
	}
}

}
