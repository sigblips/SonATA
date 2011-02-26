/*******************************************************************************

 File:    sseInterface.h
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

#ifndef _sse_interface_h
#define _sse_interface_h

#include "machine-dependent.h"

// forward declare ostream
#include <iosfwd>

using std::ostream;

const int MAX_NSS_MESSAGE_STRING = 512;
const int MAX_TEXT_STRING = 256;
const int SSE_MAX_HDR_ID_SIZE = 16;
const int NSS_NO_ACTIVITY_ID = -1;

typedef char8_t IpAddress[MAX_TEXT_STRING];

enum bool_t { 
   SSE_FALSE = 0, SSE_TRUE = 1 
   }; 

enum SiteId { 
    SITE_ID_UNINIT = 0, SITE_ID_ARECIBO = 5, SITE_ID_JODRELL_BANK = 7,
    SITE_ID_ATA = 8
   };

// Note: Polarization values are set this way for backwards
// compatibility with older data files.
enum Polarization { 
   POL_RIGHTCIRCULAR = 0,
   POL_LEFTCIRCULAR = 1, 
   POL_BOTH = 2, 
   POL_MIXED = 3,
   POL_UNINIT = 4, 
   POL_XLINEAR = 5,
   POL_YLINEAR = 6,
   POL_BOTHLINEAR = 7
};


struct NssDate          /* Equivalent of timeval struct */
{
  int32_t tv_sec;       /* Seconds since 1/1/70         */
  int32_t tv_usec;      /* Microseconds                 */

  NssDate();
  void marshall();
  void demarshall();
  friend ostream& operator << (ostream &strm, const NssDate &date);
};


#ifndef NSS_BC_TM_H
#define NSS_BC_TM_H
struct NSS_BC_TM
 {
   long tm_nsec;      /* nanoseconds */
   long tm_sec;       /* seconds */
   long tm_min;       /* minutes */
   long tm_hour;      /* hours */
   long tm_yday;      /* year day */
   long tm_year;	/* year   */
 };
#endif


// Assigned start of message code ranges
const int SSE_CODE_RANGE_START    = 10000;
const int RFC_CODE_RANGE_START    = 20000;
const int IFC_CODE_RANGE_START    = 30000;
const int DX_CODE_RANGE_START    = 40000;
const int TSCOPE_CODE_RANGE_START = 50000;
const int TSIG_CODE_RANGE_START   = 60000;
const int DX_ARCHIVER_CODE_RANGE_START   = 70000;
const int CHANNELIZER_CODE_RANGE_START   = 80000;

struct SseInterfaceHeader
{
  uint32_t	code;          // type of message in message body
  uint32_t      dataLength;    // message body length in bytes    
  uint32_t      messageNumber; // message number (1 - up increment by sender)
  int32_t	activityId;    // set to NSS_NO_ACTIVITY_ID if not relevant
  NssDate       timestamp;     
  char        	sender[SSE_MAX_HDR_ID_SIZE];        // ID in TBD format
  char        	receiver[SSE_MAX_HDR_ID_SIZE];      // ID in TBD format

  SseInterfaceHeader();
  void marshall();
  void demarshall();
  friend ostream& operator << (ostream &strm, const SseInterfaceHeader &hdr);
};

// severity level associated with NssMessage
enum NssMessageSeverity {
  SEVERITY_INFO = 0,
  SEVERITY_WARNING =  1,
  SEVERITY_ERROR = 2,
  SEVERITY_FATAL = 3,
};

// Notification message
struct NssMessage
{
  uint32_t 	code;                  // type of notification
  NssMessageSeverity severity;          
  char description[MAX_NSS_MESSAGE_STRING]; 

  NssMessage();
  NssMessage(uint32_t theCode, NssMessageSeverity theSeverity, 
	     const char* theDescription);

  void marshall();
  void demarshall();
  friend ostream& operator << (ostream &strm, const NssMessage &nssMessage);
};


#endif /*_sse_interface_h */