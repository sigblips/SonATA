/*******************************************************************************

 File:    BeamDataPacket.h
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
// Class definition for BeamPacket
// Author:
// Date: 08/30/06
//
#ifndef	BEAMDATAPACKET_H_
#define	BEAMDATAPACKET_H_

#include "ATADataPacket.h"

/**
 * Specify the size of the beam data.
 */
#define BEAM8	true
#if (BEAM8)
const int BEAM_BITS = ATADataPacketHeader::BEAM8_BITS;
const int BEAM_SAMPLES = ATADataPacketHeader::BEAM8_SAMPLES;
typedef uint16_t beamSample;
#else
const int BEAM_BITS = ATADataPacketHeader::BEAM16_BITS;
const int BEAM_SAMPLES = ATADataPacketHeader::BEAM16_SAMPLES;
typedef uint32_t beamSample;
#endif

struct BeamDataPacket: public ATADataPacket {
	struct {
		beamSample samples[BEAM_SAMPLES];
	} data;

	BeamDataPacket(uint32_t src = ATADataPacketHeader::BEAM_104MHZ,
			uint32_t channel = ATADataPacketHeader::UNDEFINED):
			ATADataPacket(ATADataPacketHeader::BEAM_104MHZ, src, channel,
			BEAM_SAMPLES, BEAM_BITS) {}
	int32_t getSize() { return (sizeof(BeamDataPacket)); }
};

#endif // BEAMDATAPACKET_H_