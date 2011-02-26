/*******************************************************************************

 File:    ChannelPacketVector.h
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

// Packet list: maintains a free list of channe packet structures
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/include/ChannelPacketVector.h,v 1.3 2009/02/13 03:02:28 kes Exp $
//
// Note: this is a singleton
//
#ifndef _ChannelPacketVectorH
#define _ChannelPacketVectorH

#include <deque>
#include <vector>
#include "System.h"
#include "Lock.h"
#include "ChannelPacket.h"

using namespace sonata_lib;

namespace chan {

typedef std::vector<ChannelPacket> ChannelPacketVector;

struct AFTiming {
	uint64_t n;
	float total;

	AFTiming(): n(0), total(0) {}
};

struct CPVLTiming {
	AFTiming alloc;
	AFTiming free;
};

class ChannelPacketVectorList
{
public:
	static ChannelPacketVectorList *getInstance( int vectors_ = DEFAULT_VECTORS,
			int packets_ = DEFAULT_USABLE_CHANNELS);

	~ChannelPacketVectorList();

	ChannelPacketVector *alloc();
	void free(ChannelPacketVector *packet_);
	int getFreeVectors() { return (vectorList.size()); }
	int32_t getAllocs() { return (allocs); }
	int32_t getFrees() { return (frees); }
	int32_t getMaxAllocated() { return (maxAllocated); }

private:
	static ChannelPacketVectorList *instance;

	int allocs, frees;
	int maxAllocated;
	int packets;
	int vectors;
	CPVLTiming timing;
	
	std::deque<ChannelPacketVector *> vectorList;

	Lock llock;

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

	// hidden
	ChannelPacketVectorList(int vectors_, int packets_);

	// forbidden
	ChannelPacketVectorList(const ChannelPacketVectorList&);
	ChannelPacketVectorList& operator=(const ChannelPacketVectorList&);
};

}
#endif