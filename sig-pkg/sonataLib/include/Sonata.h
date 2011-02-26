/*******************************************************************************

 File:    Sonata.h
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
// SonATA primary header file
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Sonata.h,v 1.12 2009/06/05 04:21:04 kes Exp $
//
#ifndef _SonataH
#define _SonataH

#include <assert.h>
#include <complex>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <values.h>
#include <fftw3.h>
#include <cycle.h>
#include "Types.h"

using std::string;

namespace sonata_lib {

const char *const CODE_VERSION = "$Name:  $";

// Timing code definitions
#define LOCK_TIMING						(false)
#define QUEUE_TIMING					(false)
#define QTASK_TIMING					(false)

// macros
#define Assert(x)						assert(x)

#define ALIGN_BUF(buf)					((((loff_t) buf) + BUF_PAGE_SIZE - 1) \
										& BUF_MASK)

#define _MIN(a, b)						(a < b ? a : b)
#define _MAX(a, b)						(a > b ? a : b)

#define GHZ_TO_MHZ(f)					((f) * MHZ_PER_GHZ)
#define MHZ_TO_GHZ(f)					((f) / MHZ_PER_GHZ)
#define MHZ_TO_HZ(f)					((f) * HZ_PER_MHZ)
#define HZ_TO_MHZ(f)					((f) / HZ_PER_MHZ)
#define MHZ_TO_KHZ(f)					((f) * KHZ_PER_MHZ)
#define KHZ_TO_MHZ(f)					((f) / KHZ_PER_MHZ)
#define KHZ_TO_HZ(f)					((f) * HZ_PER_KHZ)
#define HZ_TO_KHZ(f)					((f) / HZ_PER_KHZ)

// eternal constants
const int32_t MSEC_PER_SEC = 1000;
const int32_t USEC_PER_MSEC = 1000;
const int32_t NSEC_PER_USEC = 1000;
const int32_t USEC_PER_SEC = 1000000;
const int32_t BITS_PER_BYTE = 8;
const float64_t HZ_PER_MHZ = 1e6;
const float64_t KHZ_PER_MHZ = 1e3;
const float64_t HZ_PER_KHZ = 1e3;
const float64_t MHZ_PER_GHZ = 1e3;
const float64_t MIN_EXP = -700;
const float64_t MAX_EXP = 700;
const float64_t MAX_EXP10 = 307.0;
const float64_t HALFLOG2PI = 0.918938533204672;

// system definitions
const int32_t DEFAULT_ID = 1;
const IpAddress DEFAULT_ADDR = "default";
const int32_t DEFAULT_PORT = 0;
const int32_t DEFAULT_MESSAGES = 100;
const int32_t DEFAULT_QSLOTS = 50;
const int32_t DEFAULT_PACKETS = 10000;
const int32_t DEFAULT_BEAM_PACKETS = 100000;
const int32_t DEFAULT_CHANNEL_PACKETS = 100000;
const int32_t MAX_STR_LEN = 50;

#ifdef notdef
#ifndef NTOHL
#define NTOHS(x)						((x) = ntohs(x))
#define NTOHL(x)						((x) = ntohl(x))
#endif
#endif

// task priorities
const int32_t MAX_PRIO = 30;

const int32_t LOG_PRIO = 1;
}

#endif
