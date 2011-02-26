/*******************************************************************************

 File:    ChannelPacketVector.cpp
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

// SonATA channelizer channel packet vector list

#include "ChannelPacketVector.h"
#include "ChErr.h"

namespace chan {

ChannelPacketVectorList *ChannelPacketVectorList::instance = 0;

ChannelPacketVectorList *
ChannelPacketVectorList::getInstance(int packets_, int vectors_)
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ChannelPacketVectorList(packets_, vectors_);
	l.unlock();
	return (instance);
}

ChannelPacketVectorList::ChannelPacketVectorList(int vectors_, int packets_):
		allocs(0), frees(0), maxAllocated(0), packets(packets_),
		vectors(vectors_), llock("vectorList")

{
	// initialize the list of ChannelPacketVectors
	for (int i = 0; i < vectors_; ++i) {
		ChannelPacketVector *vector = new ChannelPacketVector(packets);
		ChannelPacket p = (*vector)[0];
		vectorList.push_back(vector);
#ifdef notdef
		std::cout << "size = " << size << ", src = " << p.getHeader().src;
		std::cout << ", pktsize = " << p.getPacketSize() << std::endl;
		vectorList.push_back(vector);
		std::cout << vectorList.size() << " vectors" << std::endl;
#endif
	}
}

ChannelPacketVectorList::~ChannelPacketVectorList()
{
}

ChannelPacketVector *
ChannelPacketVectorList::alloc()
{
	ChannelPacketVector *vector;

#if CHANNEL_PACKET_VECTOR_TIMING
	uint64_t t0 = getticks();
#endif
	lock();
	if (vectorList.empty()) {
		unlock();
		Fatal(ERR_NVA);
	}

	vector = vectorList.front();
	vectorList.pop_front();

	Assert(vector);

	++allocs;
	if (allocs - frees > maxAllocated)
		maxAllocated = allocs - frees;
	unlock();
#if CHANNEL_PACKET_VECTOR_TIMING
	uint64_t t1 = getticks();
	++timing.alloc.n;
	timing.alloc.total += elapsed(t1, t0);
#endif
	return (vector);
}

//
// free: add the msg to the end of the free list
//
// Notes:
//		If the free list is empty, both head and tail are assigned
//		the message.
//
void
ChannelPacketVectorList::free(ChannelPacketVector *vector)
{
#if CHANNEL_PACKET_VECTOR_TIMING
	uint64_t t0 = getticks();
#endif
	lock();
	vectorList.push_back(vector);
	++frees;
	unlock();
#if CHANNEL_PACKET_VECTOR_TIMING
	uint64_t t1 = getticks();
	++timing.free.n;
	timing.free.total += elapsed(t1, t0);
#endif
}

}

