/*******************************************************************************

 File:    InputBuffer.h
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
* Input sample buffer class declaration
*
* Buffer class to hold input samples.  The samples at this stage have
* been converted to floating-point, circular polarization, and are
* being buffered here prior to the DFB.\n
*
*/
#ifndef _InputBuffer
#define _InputBuffer

#include <list>
#include <map>
#include "Sonata.h"
#include "Lock.h"
#include "RWLock.h"
#include "Semaphore.h"
#include "Types.h"

namespace sonata_lib {

class InputBuffer
{
public:
	/**
	 * Constructor.
	 *
	 * Description:\n
	 * 	Create an input buffer with size_ elements each of size dataSize_.
	 *
	 * @param	size_ total # of data elements.
	 * @param	dataSize_ size of each data element.
	 */
	InputBuffer(uint32_t size_, int32_t dataSize_);
	~InputBuffer();

	void reset();

	/**
	* Read/write lock.
	*
	* Notes:\n
	*	The read/write lock can be used to allow a single writer but
	*	multiple readers, in situations where more than one thread
	*	may be running the DFB on the data.  In the current implementation,
	*	this is not possible, because a single input processing thread is
	*	associated with each channel.  As a result, the standard lock()
	*	call is used instead.
	*/

#ifdef notdef
	/**
	* Lock the buffer for reading.
	*
	* Notes:\n
	*	More than one reader may hold the lock.
	*/
	void rdlock() { rwLock.rdlock(); }

	/**
	* Lock the buffer for writing.
	*
	* Notes:\n
	*	Only one writer may hold the lock.
	*/
	void wdlock() { rwLock.wrlock(); }

	/**
	* Unlock the read/write lock on the buffer.
	*
	* Notes:\n
	*	Used to release both read and write locks.
	*/
	void rwunlock() { rwLock.unlock(); }
#endif
	/**
	* Lock the buffer.
	*
	* Notes:\n
	*	This is a generic lock.  It allows recursive locking by the
	*	same task.
	*/
	void lock() { bLock.lock(); }

	/**
	* Unlock the buffer.
	*
	* Notes:\n
	*	Used to release both read and write locks.
	*/
	void unlock() { bLock.unlock(); }

#ifdef  notdef
	/**
	* Functions to return buffer pointers.
	*
	* Notes:\n
	*	rdlock() or wrlock() must be called before calling these functions.
	*/
	void rlock() { rwLock.rdlock(); }
	void wlock() { rwLock.wrlock(); }
	void runlock() { rwLock.unlock(); }
	void wunlock() { rwLock.unlock(); }
#endif

	/**
	* Return the head of the buffer.
	*/
	void *getBuf() { return (buf); }

	/**
	 * Compute the address and length of a block of samples.
	 */
	void *getSampleBlk(uint64_t sample, int32_t& len);

	/**
	* Return the read pointer.
	*
	* Notes:\n
	*	This is the first sample that has not yet been fully processed.
	*	As a result of overlap between transforms, some samples are
	*	used more than once.
	*/
	void *getSample(uint64_t sample = 0);
#ifdef notdef
	{
		char *addr = static_cast<char *> (buf);
		uint64_t ofs = sample ? sample - first : done - first;
		return (static_cast<void *> (addr + ofs * dataSize));
//		return (sample ? &buf[sample-first] : &buf[done-first]);
	}
#endif
	/**
	* Return the write pointer.
	*
	* Notes:\n
	*	This is the first free sample in the buffer.
	*/
	void *getWrite(int32_t len) {
		int32_t ofs = getIndex(last);
		Assert(size - ofs >= len);
		char *addr = static_cast<char *> (buf);
		return (static_cast<void *> (addr + ofs * dataSize));
	}

	/**
	* Return various sample numbers.
	*
	* Notes:\n
	*	The sample numbers are not indices into the buffer; instead, they
	*	are absolute sample numbers since the beginning of processing for
	*	this activity.
	*/
	uint64_t getFirst() { return (first.sample); }
	uint64_t getLast() { return (last); }
	uint64_t getDone() { return (done); }
	uint64_t getNext() { return (next); }

	void setDone(uint64_t done_) { lock();
		done = done_;
		first.index = getIndex(done);
		first.sample = done;
		++dones;
		unlock();
	}
	void setNext(uint64_t next_) { lock(); next = next_; unlock(); }

	/**
	* Return the number of actual samples in the buffer.
	*/
	int32_t getSamples() { return (last - next); }
	int32_t getSamples(uint64_t sample) { return (last - sample); }

	/**
	* Return the number of free (unused) samples in the buffer.
	*/
	uint32_t getFree() { return (size - (last - first.sample)); }

	/**
	* Set the sample number of the first unused sample in the buffer.
	*/
	void setLast(int32_t n) { lock(); last += n; unlock(); }

	/**
	* Consume the number of samples specified.
	*
	* Description:\n
	*	Logically removes the specified number of samples from the
	*	buffer by incrementing the next counter by the argument.
	*
	* @param	n the number of samples to consume from the buffer.
	*/
	void consume(int32_t n) {
		lock();
		done += n;
		first.index = getIndex(done);
		first.sample = done;
		unlock();
	}

	/**
	* Remove any samples from the buffer which are no longer needed.
	*
	* Notes:\n
	*	Flushing is performed by moving all unused samples to the
	*	head of the buffer.
	*/
	void flush();

private:
	int32_t dones;
	int32_t dataSize;					// data size in bytes
	int32_t size;						// size of the buffer in samples
	struct f {
		uint64_t sample;				// first sample in the buffer
		int32_t index;					// index of this sample in the buffer

		f(): sample(0), index(0) {}
	} first;
	uint64_t done;						// last sample completed (actually first
										// sample not completed)
	uint64_t next;						// next sample to be processed
	uint64_t last;						// last sample in buffer

	Lock bLock;							// general lock
	void *buf;

	int32_t getIndex(uint64_t sample);
};

/**
 * Input buffer list: list of input buffers
*/
typedef std::list<InputBuffer *> InputBufList;

class InputBufferList {
public:
	InputBufferList(string ibname_, int32_t nBuffers_, size_t size_,
			int32_t dataSize_);
	~InputBufferList();

	InputBuffer *alloc(bool wait_ = false);
	void free(InputBuffer *buf_);
	ssize_t getSize() { return (bufList.size()); }

private:
	string ibname;
	int32_t nBuffers;
	size_t size;
	int32_t dataSize;
	Lock llock;
	Semaphore sem;
	InputBufList bufList;

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

	// forbidden
	InputBufferList(const InputBufferList&);
	InputBufferList& operator=(const InputBufferList&);
};

}

#endif
