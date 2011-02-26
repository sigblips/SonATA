/*******************************************************************************

 File:    Types.h
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
// Typedefs and enums
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Types.h,v 1.8 2009/04/25 03:12:23 kes Exp $
//
#ifndef _TypesH
#define _TypesH

#include <complex>
#include <stdint.h>
#include <sseDxInterface.h>

namespace sonata_lib {

typedef int	Error;
typedef long double float96_t;

typedef std::complex<int8_t> ComplexInt8;
typedef std::complex<int16_t> ComplexInt16;
typedef std::complex<int32_t> ComplexInt32;
typedef std::complex<int64_t> ComplexInt64;
typedef std::complex<float32_t> ComplexFloat32;
typedef std::complex<float64_t> ComplexFloat64;
typedef std::complex<float96_t> ComplexFloat96;

enum LockType {
	NormalLock,
	RecursiveLock
};

enum TaskExitType {
	NormalExit = 0,
	TaskKilled
};

enum Unit {
	UnitNone = 0
};

enum ConnectionType {
	ActiveTcpConnection = 0,
	PassiveTcpConnection,
	UdpConnection,
	SerialConnection,
	KeyboardConnection,
	DisplayConnection,
	ConsoleConnection,
	FileConnection
};

// memory allocation type for a block: either fixed-length block
// using the DxPartitionSet facility, or a simple call to new
enum MsgDataType {
	FIXED_BLOCK,					// partition
	NEW,							// allocated with new
	NEW_ARRAY,						// allocated with new[]
	STATIC,							// static memory
	IMMEDIATE,						// value <= pointer size is stored in data
	USER							// user-managed data
};

}

#endif