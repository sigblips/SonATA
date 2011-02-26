/*******************************************************************************

 File:    NssMessage.cpp
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
#include "SseMsg.h"
#include "SseUtil.h"

using namespace std;

ostream& operator << (ostream &strm, const NssMessage &nssMessage)
{
    strm << " Code: " << nssMessage.code
	 << " Severity: " 
	 << SseMsg::nssMessageSeverityToString(nssMessage.severity) << endl
	 << "Text: " << nssMessage.description
	 << endl;

    return strm;
}

NssMessage::NssMessage(uint32_t theCode, NssMessageSeverity theSeverity, 
		       const char* theDescription)
    : code(theCode),
      severity(theSeverity)
{
    SseUtil::strMaxCpy(description, theDescription, MAX_TEXT_STRING);
}

NssMessage::NssMessage()
    : code(0),
      severity(SEVERITY_INFO)
{
    description[0] = '\0';
}

void NssMessage::marshall()
{
    HTONL(code);  // message code
    SseMsg::marshall(severity); // NssMessageSeverity

    // no need to marshall char array
    // description
}

void NssMessage::demarshall()
{
    marshall();
}