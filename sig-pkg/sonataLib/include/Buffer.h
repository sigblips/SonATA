/*******************************************************************************

 File:    Buffer.h
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
// Buffer class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Buffer.h,v 1.4 2009/05/24 23:12:06 kes Exp $
//
#ifndef _BufferH
#define _BufferH

#include <list>
#include <ATADataPacketHeader.h>
#include "Lock.h"
#include "Semaphore.h"
#include "Types.h"

namespace sonata_lib {

class Buffer {
public:
	Buffer();
	Buffer(string bName_, size_t size_, bool init_);
	~Buffer();

	void setName(string name_) { bname = name_; }
	void alloc(size_t size_);
//	void setNext(Buffer *next_) { next = next_; }
//	Buffer *getNext() { return (next); }
	void initialize(uint8_t value = 0);
	void *getData() { return (static_cast<void *> (data)); }
	string getName() { return (bname); }
	size_t getSize() {return (size); }
	void setFull() { full = true; }
	void setEmpty() { full = false; }
	bool isFull() { return (full); }
	bool isEmpty() { return (!full); }

private:
	bool full;							// full/empty flag
	string bname;						// buffer name
	uint8_t *data;						// ptr to aligned data buffer
	uint32_t entryCount;				// count of entries in buffer
	size_t size;						// size of the buffer
	Error err;							// current error

	// forbidden
	Buffer(const Buffer&);
	Buffer& operator=(const Buffer&);
};

/**
 * Buffer pairs: used for left & right polarizations
*/
class BufPairList;

class BufPair {
public:
	BufPair(string bpname_, size_t size_, bool init_, BufPairList *parent_);
	~BufPair();

	void initialize();
	void free();
	void release();
	Buffer *getBuf(ATADataPacketHeader::PolarizationCode pol);
	Buffer *getLBuf() { return (lBuf); }
	Buffer *getRBuf() { return (rBuf); }
	void *getBufData(ATADataPacketHeader::PolarizationCode pol);

private:
	BufPairList *parent;
	Buffer *lBuf;
	Buffer *rBuf;
};

/**
 * Buffer pair list: list of left/right buffer pairs
*/
typedef std::list<BufPair *> BufPrList;

class BufPairList {
public:
	BufPairList(string bpname_, int32_t nBufPairs_, size_t size_,
			bool init_ = false);
	~BufPairList();

	BufPair *alloc(bool wait_ = true);
	void free(BufPair *bufPair_);
	ssize_t getSize() { return (bufPairList.size()); }

private:
	string bpname;
	int32_t nBufPairs;
	Lock llock;
	Semaphore sem;
	BufPrList bufPairList;

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

	// forbidden
	BufPairList(const BufPairList&);
	BufPairList& operator=(const BufPairList&);
};

}

#endif