/*******************************************************************************

 File:    SseMessage.h
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


// ============================================================
// Filename:    SseMessage.h
// Description: message structures for SSE 
// Authors:     Jordan
// Created:     4 Oct 04
// ============================================================

#ifndef _sse_message
#define _sse_message

#include "sseInterface.h"
#include <string>

using std::string;

enum SseMsgCode 
{
    SSE_MSG_UNINIT = SSE_CODE_RANGE_START,
    SSE_MSG_INFO,		// Information only
    SSE_MSG_DBERR,		// Error from MySQL query or invalid results set
    SSE_MSG_INVALID_PARMS,	// Invalid parameter
    SSE_MSG_STX_INVALID_STAT,	// Invalid STX Status
    SSE_MSG_STX_RVAR_WUP,	// Rgt STX Variance exceeds upper Warning limit
    SSE_MSG_STX_LVAR_WUP,	// Left STX Variance exceeds upper Warning limit
    SSE_MSG_STX_RVAR_WLW,	// Rgt STX Variance exceeds lower Warning limit
    SSE_MSG_STX_LVAR_WLW,	// Left STX Variance exceeds lower Warning limit
    SSE_MSG_STX_RVAR_EUP,	// Rgt STX Variance exceeds upper Error limit
    SSE_MSG_STX_LVAR_EUP,	// Left STX Variance exceeds upper Error limit
    SSE_MSG_STX_RVAR_ELW,	// Rgt STX Variance exceeds lower Error limit
    SSE_MSG_STX_LVAR_ELW,	// Left STX Variance exceeds lower Error limit
    SSE_MSG_BASELINE_RW,	// Rgt Baseline statistic exceeds Warning limit
    SSE_MSG_BASELINE_LW,	// Left Baseline statistic exceeds Warning limit
    SSE_MSG_BASELINE_RE,	// Rgt Baseline statistic exceeds Error limit
    SSE_MSG_BASELINE_LE,	// Left Baseline statistic exceeds Error limit
    SSE_MSG_BAD_SIG_CLASS,	// Invalid Signal Classification for FollowUp
    SSE_MSG_DX_DISCONNECT,	// DX closed socket connection
    SSE_MSG_UNINIT_DB_ID,	// Uninit Database Id (Db Version mismatch)
    SSE_MSG_DX_TUNE_DIFF,	// Diff of Dx Tuned vs. Requested Freq too big
    SSE_MSG_DISK_FULL_ERR,     // Used space on Archive Disk exceeds Error Limit
    SSE_MSG_DISK_FULL_WARN,  // Used space on Archive Disk exceeds Warning Limit
    SSE_MSG_INVALID_SKY_FREQ, 	// Tuning Spread exceeds input bandwidth
    SSE_MSG_ACT_FAILED,		// Activity Failed
    SSE_MSG_RD_TARG_EPHEM,	// Error Reading Spacecraft Ephemeris File
    SSE_MSG_RD_EARTH_EPHEM,	// Error Reading Earth Ephemeris File
    SSE_MSG_SPACECRAFT_POS,	// Error Calculating Spacecraft Position
    SSE_MSG_OUT_OF_BAND_FREQ,	// Dx tune freq outside IF bandwidth
    SSE_MSG_MISSING_TSCOPE,	// No Telescopes available for actiivty
    SSE_MSG_MISSING_TSIG,	// No TSigs available for activity
    SSE_MSG_MISSING_IFC,	// No ifcs available for activity
    SSE_MSG_MISSING_DX,	// No Dxs available for activity
    SSE_MSG_MISSING_BEAM,       // No (or too few) beams available
    SSE_MSG_NO_DB,		// Database not enable or not available
    SSE_MSG_EXCEPTION,		// Unexpected exception
    SSE_MSG_NO_SEC_SIG,		// No Counterpart Signals available
    SSE_MSG_RFI_MASK_2LONG,	// # of FreqBands > 4096
    SSE_MSG_ACT_UNIT_FAILED,	// Activity Unit Failed, terminating ...
    SSE_MSG_AUTO_TARG_FAILED,	// Automatic Target Selection Failed
    SSE_MSG_ACT_STRAT_FAILED,	// Failure in the Startegy Active Object Code
    SSE_MSG_START_ACT_FAILED,	// Failure Starting Next Activity
    SSE_MSG_FILE_ERROR,		// Error Reading or Writing a disk file
    SSE_MSG_IFC_PROXY_ERROR,	// Errors received from IFC
    SSE_MSG_SCHED_FAILED,	// Scheduler failed to start
    SSE_MSG_2MANY_ACT,		// Too many dx activities
    SSE_MSG_INVALID_MSG,	// Invalid message received from dx
    SSE_MSG_ALREADY_ATTACH,	// Dx Proxy already attached to ActUnit
    SSE_MSG_ERROR_DETACH,	// Error from dxProxy detaching from ActUnit
    SSE_MSG_INTERFACE_MISMATCH,	// Mismatch tween sse-dx interface versions
    SSE_MSG_INVALID_PORT,	// Invalid Archiver Port
    SSE_MSG_PROXY_NOT_FOUND,	// Error releasing Proxy
    SSE_MSG_UNKNOWN_ERROR,	// Text Message received from subsystem
    SSE_MSG_PROXY_TIMEOUT,	// Timeout reading subsystem message
    SSE_MSG_CONNECT_CLOSED,	// Socket connection is closed
    SSE_MSG_SOCKET_RESET,	// Socket error necessitates closing socket
    SSE_MSG_START_OBS_ERROR,	// Dx shutdown after start obs error
    SSE_MSG_CAND_LOOKUP_FAILED,	// Database error looking up Followup Cands
    SSE_MSG_INVALID_TARGET      // Invalid target was chosen
};

// Sse Logged message
class SseMessage
{
 public:

  static void log(const string & sender, int activityId,
                  SseMsgCode code, NssMessageSeverity severity, 
                  const string & description,
                  const string & sourceFilename = "",
                  int lineNumber = -1);

 private:

  SseMessage();
  virtual ~SseMessage();

};


#endif /*_sse_message */