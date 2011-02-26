/*******************************************************************************

 File:    SseInterfaceHeader.cpp
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

#include "sseInterfaceLib.h"

#include <iostream>

using std::endl;

ostream& operator << (ostream &strm, const SseInterfaceHeader &hdr)
{
    strm << "Msg hdr: ";
    strm << "code: " << hdr.code;
    strm << " dlen: " << hdr.dataLength;
    strm << " num: " << hdr.messageNumber;
    strm << " actId: " << hdr.activityId;
    strm << " time: " << hdr.timestamp;  
    strm << " sender: " << hdr.sender;
    strm << " receiver: " << hdr.receiver;

    strm << endl;

    return strm;
}

SseInterfaceHeader::SseInterfaceHeader()
    :
    code(0),
    dataLength(0),
    messageNumber(0),
    activityId(NSS_NO_ACTIVITY_ID)
{
    sender[0] = '\0';
    receiver[0] = '\0';
}

void SseInterfaceHeader::marshall()
{
    NTOHL(code);  // MessageCode
    NTOHL(dataLength);
    NTOHL(messageNumber);
    NTOHL(activityId);
    timestamp.marshall();  // NssDate
    // sender - char array, no marshalling
    // receiver- char array, no marshalling

}

void SseInterfaceHeader::demarshall()
{
    marshall();
}