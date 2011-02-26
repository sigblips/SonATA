/*******************************************************************************

 File:    InputBuffer.cpp
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

/**
* Input sample buffer class definition
*/

#include <fftw3.h>
#include "InputBuffer.h"
#include "Err.h"

namespace sonata_lib {

/**
* Create an input buffer
*
* Description:\n
*	Allocates an aligned array for storage of complex float input data.
*/
InputBuffer::InputBuffer(uint32_t size_, int32_t dataSize_): dones(0),
		dataSize(dataSize_), size(size_), done(0), next(0), last(0),
		bLock("InputBuf"), buf(0)
{
	first.sample = 0;
	first.index = 0;

	buf = fftwf_malloc(size * dataSize);
//	for (int32_t i = 0; i < size; ++i)
//		buf[i] = (i, i);
	Assert(buf);
	memset(buf, 0, size * dataSize);
}

/**
* Release an input buffer
*
* Description:\n
*	Frees the buffer array.
*/
InputBuffer::~InputBuffer()
{
	fftwf_free(buf);
}

/**
 * Reset the buffer.
 *
 * Description:\n
 * 	This operation must be performed before each activity, to ensure that
 * 	the buffer is empty.
 */
void
InputBuffer::reset()
{
	first.sample = done = next = last = dones = 0;
	first.index = 0;
}

/**
* Remove completely processed data samples from the buffer.
*
* Description:\n
*	Removes data samples which have been completed used, by shifting the
*	remaining samples up in the buffer.\n
* Notes:\n
*	IMPORTANT: must acquire the write lock before calling this function.
*/
void
InputBuffer::flush()
{
	if (done != first.sample) {
		first.index = getIndex(done);
		first.sample = done;
	}
}

/**
 * Get the address of a block of samples of the specified length.
 *
 * Description:\n
 * 	Computes the starting address of a block beginning at the specified
 * 	sample; if the end of the buffer occurs before the specified number
 * 	of samples, the total number of samples available in the block is
 * 	written into the length.
 */
void *
InputBuffer::getSampleBlk(uint64_t sample, int32_t& len)
{
	int32_t index = getIndex(sample);
	if (size - index < len)
		len = size - index;
	char *addr = static_cast<char *> (buf);
	return (static_cast<void *> (addr + index * dataSize));
}

/**
* Get the address of the specified sample.
*
* Description:\n
* 	Computes the buffer address of the specified sample, taking into account
* 	wrapping of the circular buffer.  If the sample number is 0, then
* 	the address returned is the address of the first sample which has
* 	not been completely processed;
*/
void *
InputBuffer::getSample(uint64_t sample)
{
	int32_t index;
	if (sample)
		index = getIndex(sample);
	else
		index = getIndex(done);
	char *addr = static_cast<char *> (buf);
	return (static_cast<void *> (addr + index * dataSize));
}
/**
 * Find the buffer index of the specified sample.
 *
 * Description:\n
 * 	Finds the buffer index of the specified sample.  The index of the
 * 	first sample is used to compute the offset to the specified sample;
 * 	if the buffer has wrapped, the sample index will be less than the
 * 	first sample index;
 */
int32_t
InputBuffer::getIndex(uint64_t sample)
{
	int32_t ofs = sample - first.sample;
	Assert(ofs < size);
	int32_t index = first.index + ofs;
	if (index >= size)
		index -= size;
	return (index);
}

/**
 * Input buffer list
*/

InputBufferList::InputBufferList(string ibname_, int32_t nBuffers_,
		size_t size_, int32_t dataSize_): ibname(ibname_), nBuffers(nBuffers_),
		size(size_), dataSize(dataSize_), llock("lIB" + ibname_),
		sem("sIB" + ibname_, nBuffers_)
{
	// create the specified number of buffer pairs
	for (int i = 0; i < nBuffers; i++) {
		InputBuffer *buf = new InputBuffer(size, dataSize);
		Assert(buf);
		bufList.push_back(buf);
	}
}

InputBufferList::~InputBufferList()
{
	InputBufList::iterator pos;

	for (pos = bufList.begin(); pos != bufList.end(); ++pos)
		delete (*pos);
	bufList.clear();
}

InputBuffer *
InputBufferList::alloc(bool wait_)
{
	InputBuffer *buf = 0;

	// see if we have a buffer available
	if (!sem.wait(wait_ ? -1 : 0)) {
		lock();
		// do a sanity check
		if (bufList.empty()) {
			unlock();
			Fatal(ERR_NBA);
		}

		buf = bufList.front();
		bufList.pop_front();
		unlock();
	}
	return (buf);
}

void
InputBufferList::free(InputBuffer *buf_)
{
	lock();
	bufList.push_back(buf_);
	unlock();
	sem.signal();

}

}
