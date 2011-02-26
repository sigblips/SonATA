/*******************************************************************************

 File:    PsStruct.h
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

// Structures

#ifndef PSSTRUCT_H_
#define PSSTRUCT_H_

#include "System.h"

namespace sonata_packetsend {

// network host specification data
struct HostSpec {
	IpAddress addr;						// name/address
	int32_t port;						// port
	in_addr inaddr;						// host IP address

	HostSpec(): port(DEFAULT_OUTPUT_PORT) {
		memcpy(addr, DEFAULT_OUTPUT_ADDR, sizeof(addr));
	}
	HostSpec(const IpAddress addr_, int32_t port_): port(port_) {
		memcpy(addr, addr_, sizeof(addr));
	}
};

/**
 * Reader input buffer.
 *
 * Description:\n
 * 	Buffers the input data from the file, reading an integral number
 * 	of complete packets.  Packets may be either beam packets or channel
 * 	packets, but the format of the buffer may change as a result.
 */
struct ReadBuffer {
	int32_t packets;					// # of packets in the buffer
	int32_t capacity;					// max # of packets in the buffer
	std::streamsize size;				// size of the buffer
	ATADataPacket *buf;					// pointer to buffer

	ReadBuffer(int32_t pktSize_ = sizeof(BeamDataPacket),
			int32_t packets_ = INPUT_COUNT): packets(0), capacity(packets_),
			size(0), buf(0) {
		size = capacity * pktSize_;
		buf = (ATADataPacket *) fftwf_malloc(size);
		Assert(buf);
	}

	~ReadBuffer()
	{
		fftwf_free(buf);
	}
};

}

#endif /*PSSTRUCT_H_*/