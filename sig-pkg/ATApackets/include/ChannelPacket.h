/*******************************************************************************

 File:    ChannelPacket.h
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
#ifndef	CHANNELPACKET_H_
#define	CHANNELPACKET_H_

#include <string.h>
#include "ATAPacket.h"
#include "ChannelDataPacket.h"

/**
 * Channel packet class.
 *
 * Description:\n
 * 	This class, which is a subclass of ATAPacket, is a wrapper for the
 * 	actual beam data packet transmitted over the network, which is defined
 * 	in BeamDataPacket.h.  A wrapper is necessary because ATAPacket contains
 * 	virtual functions, and the vtable pointer changes the structure in a
 * 	non-portable way.\n\n
 * Notes:\n
 * 	Use of subclasses allows the beam packet format to be different from
 * 	the channel packet format without changing a great deal of application
 * 	code.
 */
class ChannelPacket: public ATAPacket
{
public:

	ChannelPacket(uint32_t src = ATADataPacketHeader::CHAN_400KHZ,
			uint32_t channel = ATADataPacketHeader::DEFAULT_CHANNEL):
			packet(src, channel) {}

	virtual void *getSamples() { return (&packet.data.samples); }
	virtual complex<float> getSample(int32_t i);
 	virtual void getSamples(complex<float> *data);
	virtual void putSample(int32_t i, const complex<float>& s);
	virtual void putSamples(const complex<float> *data);
	virtual int32_t getDataSize() { return (sizeof(packet.data)); }
	virtual int32_t getPacketSize() { return (sizeof(packet)); }
	virtual int32_t getSize() { return (sizeof(*this)); }
	virtual int32_t getLen() { return (packet.hdr.len); }
	virtual ATADataPacketHeader& getHeader() { return (packet.hdr); }
	virtual void putHeader(const ATADataPacketHeader& hdr) { packet.hdr = hdr; }
	virtual void *getData() { return (&packet.data); }
	virtual ATADataPacket *getPacket() { return (&packet); }
	virtual void getPacket(ATADataPacket *pkt) {
		memcpy(pkt, &packet, getPacketSize());
	}
	virtual void putPacket(const ATADataPacket *pkt) {
		memcpy(&packet, pkt, getPacketSize());
	}
	virtual bool marshall(ATADataPacketHeader::MarshallMode mode
			= ATADataPacketHeader::LAZY) {
		if (!packet.hdr.marshall(mode))
			return (false);
		flipEndian();
		return (true);
	}
	virtual bool demarshall()
	{
		if (!packet.hdr.demarshall())
			return false;
		flipEndian();
		return true;
	}

private:
	ChannelDataPacket packet;

protected:
	/*
	 * Description:\n
	 * 	Marshalls/demarshalls the samples in a packet.  This default is
	 * 	for the normal case of 16-bit complex samples.
	 */
	virtual void flipEndian() {
		for (uint32_t i = 0; i < ATADataPacketHeader::CHANNEL_SAMPLES; ++i)
			ATADataPacketHeader::flip32(packet.data.samples[i]);
	}
};

#endif // CHANNELPACKET_H_