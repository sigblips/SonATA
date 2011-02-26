/*******************************************************************************

 File:    PacketList.h
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

// Packet list: maintains a free list of packet class objects
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/PacketList.h,v 1.7 2009/05/24 23:15:24 kes Exp $
//
// Note: this is a singleton
//
#ifndef _PacketListH
#define _PacketListH

#include <list>
#include "Lock.h"
#include "Sonata.h"

using std::list;

namespace sonata_lib {
template<class T, int32_t n>
class PacketList
{
public:
	static PacketList *getInstance(int32_t packets = n)
	{
		static Lock l;
		l.lock();
		if (!instance)
			instance = new PacketList(packets);
		l.unlock();
		return (instance);
	}

	~PacketList() { delete [] pBase; }

	T *alloc()
	{
		T *p = 0;
		lock();
		if (!pktList.empty()) {
			p = pktList.front();
			pktList.pop_front();
			++allocs;
		}
		unlock();
		return (p);
	}

	void free(T *p)
	{
		lock();
		pktList.push_back(p);
		++frees;
		unlock();
	}

private:
	static PacketList *instance;
	int32_t allocs, frees;
	T *pBase;
	Lock pLock;
	std::list<T *> pktList;

	void lock() { pLock.lock(); }
	void unlock() { pLock.unlock(); }

	PacketList(int32_t packets): allocs(0), frees(0), pBase(0),
			pLock("packetlist")
	{
		pBase = new T[packets];
		for (int i = 0; i < packets; ++i)
			pktList.push_back(&pBase[i]);
	}
};

}

#endif