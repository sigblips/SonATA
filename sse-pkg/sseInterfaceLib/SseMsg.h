/*******************************************************************************

 File:    SseMsg.h
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


#ifndef _sse_msg_h
#define _sse_msg_h

#include "sseInterface.h"
#include <string>

using std::string;

class SseMsg
{
 public:
    
    static string polarizationToString(Polarization pol);
    static char polarizationToSingleUpperChar(Polarization pol);
    static Polarization stringToPolarization(const string &polStr);

    static string getSiteIdName(SiteId id);
    static string nssMessageSeverityToString(NssMessageSeverity severity);

    static void marshall(bool_t &arg);
    static void demarshall(bool_t &arg);

    static void marshall(Polarization &pol);
    static void demarshall(Polarization &pol);

    static void marshall(SiteId &id);
    static void demarshall(SiteId &id);

    static void marshall(NssMessageSeverity &arg);
    static void demarshall(NssMessageSeverity &arg);

    static NssDate currentNssDate();
    static string isoDateTime(const NssDate &nssDate);


 private:
    SseMsg();
    ~SseMsg();
};


#endif