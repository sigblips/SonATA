/*******************************************************************************

 File:    Partition.h
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
 * Classes to handle sets of fixed-size memory blocks
 * 
 * Description:\n
 * 	A set of fixed-size memory partitions is used to provide very fast
 * 	allocation and release of memory.  For a given allocation request,
 * 	the smallest partition with a large enough block to satisfy the
 * 	request is used; if it is empty, the next larger size is used until
 * 	the request can be filled.
 */
#ifndef _PartitionH
#define _PartitionH

#include <deque>
#include <map>
#include "ErrMsg.h"
#include "Lock.h"

namespace sonata_lib {

// forward declarations
class Partition;

/**
 * Fixed-size memory block.
 * 
 * Description:\n
 * 	Each partition contains a list of free memory blocks; a block is either
 * 	free or completely allocated, regardless of whether it is bigger than
 * 	the required allocation.  Thus, memory is not fragmented.\n
 * Notes:
 * 	Each memory block contains a pointer to its parent partition, so a
 * 	block can be freed without knowing which partition it came from.
 */
class MemBlk {
public:
	MemBlk(): data(0), parent(0) {}
	~MemBlk() {}

	void init(Partition *parent_, void *data_) {
		parent = parent_;
		data = data_;
	}
	void *getData() { return (data); }
	size_t getBlkSize();
//	Partition *getParent() { return (parent); }

	void free();

private:
	void *data;							// pointer to data
	Partition *parent;					// parent partition
	
	// forbidden
	MemBlk(const MemBlk&);
	MemBlk& operator=(const MemBlk&);
};

/**
 * Partition of memory blocks.
 * 
 * Description:\n
 * 	A partition is a list of fixed-size memory blocks which are available
 * 	for extremely fast allocation and release.  This is critical in a
 * 	system which must allocate and free thousands of data structures per
 * 	second.
 */
typedef std::deque<MemBlk *>		MemList;

class Partition {
public:
	Partition(size_t size_, uint32_t nBlks_);
	~Partition();

	MemBlk *alloc();
	void free(MemBlk *blk_);
	size_t getBlkSize() { return (size); }
	int32_t getFreeBlks() { return (list.size()); }

private:
	uint8_t *data;						// address of start of data
	int32_t blocks;						// total # of blocks
	int32_t allocs, frees;				// keep track of total usage
	size_t size;						// size of blocks in this list
	size_t smallest;					// smallest block allocated
	size_t largest;						// largest block allocated
	MemBlk *blks;						// pointer to memory blocks
	MemList list;						// free list
	Lock llock;							// serialization lock

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

	// forbidden
	Partition(const Partition&);
	Partition& operator=(const Partition&);
};

/**
 * Partition set of different-sized partitions (singleton).
 * 
 * Description:\n
 * 	A collection of partitions of different-sized memory blocks.  When
 * 	memory must be allocated, the singleton PartitionSet is called to
 * 	do the allocation.  The caller need not worry about which actual
 * 	partition to use, since the PartitionSet will automatically allocate
 * 	from the smallest partition which is both large enough and contains
 * 	free blocks.
 */
typedef map<size_t, Partition *>	PartitionList;

class PartitionSet {
public:
	static PartitionSet *getInstance();
	
	~PartitionSet();

	void addPartition(Partition *partition_);
	MemBlk *alloc(size_t size);

private:
	static PartitionSet *instance;
	size_t largest;

	Lock plock;
	PartitionList list;

    ErrMsg *errList;

    void lock() { plock.lock(); }
	void unlock() { plock.unlock(); }

	PartitionSet();

	// forbidden
	PartitionSet(const PartitionSet&);
	PartitionSet& operator=(const PartitionSet&);
};

}

#endif