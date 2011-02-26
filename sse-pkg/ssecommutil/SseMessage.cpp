/*******************************************************************************

 File:    SseMessage.cpp
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
// Filename:    SseMessage.cpp
// Description: Output and Logging message structures for SSE
// Authors:     Jordan
// Created:     4 Oct 04
// ============================================================

#include "SseMessage.h"
#include "SseArchive.h"
#include "SseMsg.h"
#include <sstream>

using namespace std;

void SseMessage::log(
   const string & sender, int activityId,
   SseMsgCode code, NssMessageSeverity severity, 
   const string & description, const string & sourceFilename,
   int lineNumber)
{
    stringstream strm;
    strm << SseMsg::nssMessageSeverityToString(severity) << " "
    	 << code << ": " << sender << ": ";

    if (activityId != NSS_NO_ACTIVITY_ID)
    {
	strm << "Act " << activityId << ": ";
    }

    if (sourceFilename != "")
    {
	strm << " " << sourceFilename; 
    }
    if (lineNumber >= 0)
    {
	strm << "[" << lineNumber << "] ";
    }
    strm << description ;

    SseArchive::SystemLog() << strm.str();

    if (severity != SEVERITY_INFO)
    {
       SseArchive::ErrorLog() << strm.str();
    }
}