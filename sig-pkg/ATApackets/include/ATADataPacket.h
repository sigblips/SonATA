/*******************************************************************************

 File:    ATADataPacket.h
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
// Class definition for ATA data packet base class
//
#ifndef	ATADATAPACKET_H_
#define	ATADATAPACKET_H_

#include "ATADataPacketHeader.h"

/**
 * ATA data packet structure.
 *
 * Description:\n
 * 	This structure is the base class for the channel and beam
 * 	packet structures.  It is not intended to be instantiated.
 */
struct ATADataPacket {
	ATADataPacketHeader hdr;

	ATADataPacket(): hdr() {}
	ATADataPacket(uint32_t grp, uint32_t src, uint32_t channel, uint32_t len,
			uint8_t bitsPerSample): hdr(grp, src, channel, len, bitsPerSample)
			{}
};

#endif // ATADATAPACKET_H