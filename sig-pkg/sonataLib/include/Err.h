/*******************************************************************************

 File:    Err.h
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
// Error codes
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Err.h,v 1.4 2009/02/13 18:08:45 kes Exp $
//
#ifndef _ErrH
#define _ErrH

#include <bitset>
#include <errno.h>
#include "Types.h"

namespace sonata_lib {

// debug mask bits
enum DebugMaskBits {
	DEBUG_NEVER = 0,
	DEBUG_ALWAYS,
	DEBUG_ARCHIVE,
	DEBUG_CONTROL,
	DEBUG_CONFIRM,
	DEBUG_DETECT,
	DEBUG_COLLECT,
	DEBUG_CMD,
	DEBUG_CWD,
	DEBUG_PD,
	DEBUG_CWD_CONFIRM,
	DEBUG_MSGLIST,
	DEBUG_QTASK,
	DEBUG_SUBCHANNEL,
	DEBUG_BIRDIE_MASK,
	DEBUG_PERM_RFI_MASK,
	DEBUG_TEST_MASK,
	DEBUG_FUDD_MASK,
	DEBUG_SIGNAL_CLASS,
	DEBUG_BASELINE,
	DEBUG_CONF_CHAN,
	DEBUG_DADD,
	DEBUG_TASK,
	DEBUG_SIGNAL,
	DEBUG_DISK,
	DEBUG_STATE,
	DEBUG_FREQ_MASK,
	DEBUG_PD_INPUT,
	DEBUG_SPECTROMETRY,
	
	DEBUG_MASK_BITS
};

typedef std::bitset<DEBUG_MASK_BITS> DebugMask;

// internal error codes

const int32_t ERR_BASE = (0);

enum ErrCode {
	ERR_NE = 0,

	ERR_DLK = ERR_BASE + 1,			// thread deadlock
	ERR_ILT,						// invalid lock type
	ERR_LNO,						// thread doesn't own lock
	ERR_CHS,						// can't handle signal
	ERR_TNR,						// task is not running
	ERR_CDT,						// can't detach thread
	ERR_TAR,						// thread is already running
	ERR_NC,							// no connection
	ERR_ICT,						// invalid connection type
	ERR_IHN,						// invalid host name
	ERR_HNF,						// host not found
	ERR_NS,							// no socket
	ERR_MAF,						// memory allocation failed
	ERR_NMA,						// no message available
	ERR_NDA,						// no data available
	ERR_IMT,						// invalid message type
	ERR_IDL,						// invalid data length
	ERR_BAA,						// buffer already allocated
	ERR_NBA,						// no buffer available
	ERR_QSE,						// queue send error
	ERR_BWE,						// buffer write error
	ERR_NDC,						// no data collection
	ERR_CAMP,						// can't allocate message pool
	ERR_IU,							// invalid unit
	ERR_IMBT,						// invalid memory buffer type
	ERR_SE,							// send error
	ERR_IRT,						// invalid science data request type
	ERR_VNM,						// version number mismatch
	ERR_ICL,						// invalid command line
	ERR_IPT,						// invalid pol type
	ERR_CCS,						// Can't create socket
	ERR_CBS,						// Can't bind socket
	ERR_NBP,						// no buffer pair
	ERR_INT,						// internal error
	ERR_LHTL,						// lock held too long
	ERR_DC,							// duplicate channel
	ERR_LMP,						// late or missing packets
	ERR_LNI,						// log notinitialized
	ERR_IFH,						// invalid filter file header
	ERR_IFC,						// invalid filter file coefficients
	ERR_COF,						// can't open filter file
	ERR_IAT,						// invalid alarm time
	
#ifdef notdef
	// Channelizer error codes
	ERR_CCC,						// can't create connection
	ERR_ECL,						// empty channel list
	ERR_ICN,						// invalid channel number
#endif

	ERR_END							// end of message
};

// function prototypes
void FatalMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str = 0);
void ErrorMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str = 0);
void WarningMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str = 0);
void InfoMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str = 0);
void DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, Error err, const char *str = 0);
void DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, float64_t val, const char *str = 0);
void DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, void *p, const char *str = 0);
void DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, ComplexFloat32 *val, const char *str = 0);
void SetDebugMask(int32_t maskBit);
void ResetDebugMask(int32_t maskBit);

}

// macros
#define Fatal(err)				sonata_lib::FatalMsg(__FILE__, __FUNCTION__, \
		__LINE__, err, 0);
#define FatalStr(err, arg)		sonata_lib::FatalMsg(__FILE__, __FUNCTION__, \
		__LINE__, err, arg);
#define Err(err)				sonata_lib::ErrorMsg(__FILE__, __FUNCTION__, \
		__LINE__, err, 0);
#define ErrStr(err, arg)		sonata_lib::ErrorMsg(__FILE__, __FUNCTION__, \
		__LINE__, err, arg);
#define Warning(err, arg)		sonata_lib::WarningMsg(__FILE__, __FUNCTION__, \
		__LINE__, err, arg);
#define Debug0(err, arg)		sonata_lib::DebugMsg(__FILE__, __FUNCTION__, \
		__LINE__, 0, err, 0);
#define Debug(mask, err, arg)	sonata_lib::DebugMsg(__FILE__, __FUNCTION__, \
		__LINE__, mask, err, arg);

#endif
