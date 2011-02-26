/*******************************************************************************

 File:    Buffer.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Buffer.cpp,v 1.5 2009/05/24 23:36:22 kes Exp $
//
#include <fftw3.h>
#include "Sonata.h"
#include "Buffer.h"
#include "Err.h"

namespace sonata_lib {

Buffer::Buffer(): full(false), bname(""), data(0), entryCount(0), size(0),
		err(0)
{
}

Buffer::Buffer(string bName_, size_t size_, bool init_):
		full(false), bname(bName_), data(0), entryCount(0), size(0), err(0)
{
	alloc(size_);
	if (init_)
		initialize();
}

Buffer::~Buffer()
{
	if (data)
		fftwf_free(data);
//	if (unalignedData)
//		delete [] unalignedData;
}

void
Buffer::alloc(size_t size_)
{
	size = size_;
	data = (uint8_t *) fftwf_malloc(size_);
//	// allocate a data buffer, which must be aligned to a boundary
//	unalignedData = new uint8_t[size_+(BUF_PAGE_SIZE-1)];
//	data = (uint8_t *) ALIGN_BUF(unalignedData);
}

void
Buffer::initialize(uint8_t value)
{
	setEmpty();
	Assert(data);
	memset(data, value, size);
}

/**
 * Buffer pair for dual polarization
*/

BufPair::BufPair(string bpname_, size_t size_, bool init_,
		BufPairList *parent_): parent(parent_), lBuf(0), rBuf(0)
{

	lBuf = new Buffer("l" + bpname_, size_, init_);
	rBuf = new Buffer("r" + bpname_, size_, init_);
}

BufPair::~BufPair()
{
	delete lBuf;
	delete rBuf;
}

void
BufPair::initialize()
{
	lBuf->initialize();
	rBuf->initialize();
}

void
BufPair::free()
{
	release();
}

void
BufPair::release()
{
	// must have a parent list
	if (!parent)
		FatalStr(ERR_NBP, "BufPair::release");

	getLBuf()->setEmpty();
	getRBuf()->setEmpty();
	parent->free(this);
}

Buffer *
BufPair::getBuf(ATADataPacketHeader::PolarizationCode pol)
{
	Buffer *buf = 0;
	switch (pol) {
	case ATADataPacketHeader::LCIRC:
	case ATADataPacketHeader::XLINEAR:
		buf = getLBuf();
		break;
	case ATADataPacketHeader::RCIRC:
	case ATADataPacketHeader::YLINEAR:
		buf = getRBuf();
		break;
	default:
		Fatal(ERR_IPT);
		break;
	}
	return (buf);
}

void *
BufPair::getBufData(ATADataPacketHeader::PolarizationCode pol)
{
	Buffer *buf = getBuf(pol);
	return (buf->getData());
}

/**
 * Buffer pair list
*/

BufPairList::BufPairList(string bpname_, int32_t nBufPairs_, size_t size_,
		bool init_): bpname(bpname_), nBufPairs(nBufPairs_),
		llock("lBP" + bpname_), sem("sBP" + bpname_, nBufPairs_)
{
	BufPair *bufPair;

	// create the specified number of buffer pairs
	for (int i = 0; i < nBufPairs; i++) {
		bufPair = new BufPair(bpname, size_, init_, this);
		bufPairList.push_back(bufPair);
	}
}

BufPairList::~BufPairList()
{
	BufPrList::iterator pos;

	for (pos = bufPairList.begin(); pos != bufPairList.end(); ++pos)
		delete (*pos);
	bufPairList.clear();
}

BufPair *
BufPairList::alloc(bool wait_)
{
	BufPair *bufPair = 0;

	// see if we have a buffer available
	if (!sem.wait(wait_ ? -1 : 0)) {
		lock();
		// do a sanity check
		if (bufPairList.empty()) {
			unlock();
			Fatal(ERR_NBA);
		}

		bufPair = bufPairList.front();
		bufPairList.pop_front();
		unlock();
	}
	return (bufPair);
}

void
BufPairList::free(BufPair *bufPair_)
{
	lock();
	bufPairList.push_back(bufPair_);
	unlock();
	sem.signal();

}

}