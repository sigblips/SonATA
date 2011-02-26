/*******************************************************************************

 File:    ChannelDataPacket.h
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
// Class definition for ChannelPacket
// Author:
// Date: 08/30/06
//
#ifndef	CHANNELDATAPACKET_H_
#define	CHANNELDATAPACKET_H_

#include "ATADataPacket.h"

struct ChannelDataPacket: public ATADataPacket {
	struct {
		uint32_t samples[ATADataPacketHeader::CHANNEL_SAMPLES];
	} data;

	ChannelDataPacket(uint32_t src = ATADataPacketHeader::CHAN_400KHZ,
			uint32_t channel = ATADataPacketHeader::UNDEFINED):
			ATADataPacket(ATADataPacketHeader::ATA, src, channel,
			ATADataPacketHeader::CHANNEL_SAMPLES,
			ATADataPacketHeader::CHANNEL_BITS) {}
	int32_t getSize() { return (sizeof(ChannelDataPacket)); }
};

#endif // CHANNELDATAPACKET_H_