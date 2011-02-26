/*******************************************************************************

 File:    sseDxArchiverInterface.h
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


#ifndef SSE_DX_ARCHIVER_INTERFACE_H
#define SSE_DX_ARCHIVER_INTERFACE_H


#include <config.h>
#include "machine-dependent.h"
#include "sseInterface.h"

// forward declare ostream
#include <iosfwd>  

const char *const SSE_DX_ARCHIVER_INTERFACE_VERSION =
   "SSE-DxArchiver Interface $Revision: 1.4 $ $Date: 2002/08/12 21:32:02 $";

typedef char8_t sseDxArchiverInterfaceVersionNumber[MAX_TEXT_STRING];


// message codes (message type identifier)
enum DxArchiverMessageCode {
    DX_ARCHIVER_MESSAGE_CODE_UNINIT = DX_ARCHIVER_CODE_RANGE_START,
    REQUEST_DX_ARCHIVER_INTRINSICS,
    SEND_DX_ARCHIVER_INTRINSICS,
    REQUEST_DX_ARCHIVER_STATUS,
    SEND_DX_ARCHIVER_STATUS,
    SEND_NSS_MESSAGE,
    SHUTDOWN_DX_ARCHIVER,
    DX_ARCHIVER_MESSAGE_CODE_END   // keep this as the last enum value
};



struct DxArchiverIntrinsics
{
    // public data
    sseDxArchiverInterfaceVersionNumber interfaceVersionNumber;
    char8_t hostname[MAX_TEXT_STRING];
    int32_t port;
    char8_t name[MAX_TEXT_STRING];

    // methods
    DxArchiverIntrinsics();
    void marshall();
    void demarshall();
    friend ostream& operator << (ostream &strm, 
				 const DxArchiverIntrinsics &intrinsics);
};


//    int32_t alignPad;         // aligment padding for marshalling



struct DxArchiverStatus
{
    NssDate timestamp;
    int32_t numberOfConnectedDxs;
    char8_t namesOfConnectedDxs[MAX_TEXT_STRING];

    DxArchiverStatus();
    void marshall();
    void demarshall();
    friend ostream& operator << (ostream &strm, const DxArchiverStatus &status);
};

#endif
