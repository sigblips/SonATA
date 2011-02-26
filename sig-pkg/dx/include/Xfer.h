/*******************************************************************************

 File:    Xfer.h
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
// Data transfer specification structure
//
// Within the DX, there are many transfers between
// memory spaces: DSP->host buffer, host buffer->disk
// drive, disk drive->processing buffer, etc.  And
// there are a number of different data types.  This
// file describes a set of generic structures which
// define the transfer between a pair of memory
// spaces.
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/Xfer.h,v 1.1 2008/01/17 19:26:21 kes Exp $
//
#ifndef _XferH
#define _XferH

namespace dx_lib {

//
// buffer specification structure for a transfer (either source
// or destination)
//
struct BufSpec {
	uint32_t blk;				// current block
	uint64_t base;				// base address of buffer
	loff_t ofs;					// offset into buffer
	uint32_t blks;				// total # of blocks in a pass through buffer
	int32_t stride;				// stride between blocks
	int32_t passStride;			// stride between passes
};

#ifdef notdef
//
// pass specification structure for a transfer.  Filling or reading a buffer
// may require more than one pass through it.
//
struct dxPassSpec {
	int32_t pass;				// current pass
	int32_t passes;				// total # of passes
};
#endif

//
// transfer specification structure
//
struct XferSpec {
	size_t blkLen;				// length of a block in bytes
	int32_t pass;				// current pass
	int32_t passes;				// total # of passes
	BufSpec src;				// source buffer information
	BufSpec dest;				// destination buffer information
};

}

#endif