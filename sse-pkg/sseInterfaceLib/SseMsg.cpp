/*******************************************************************************

 File:    SseMsg.cpp
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



#include <iostream>
#include <string>
#include <iomanip>
#include "Assert.h"
#include "HostNetByteorder.h"
#include "SseMsg.h"
#include "ArrayLength.h"

using namespace std;

// ------------- utility arrays & routines

// polarizations
static const char * PolNames[] = 
{
    "right",
    "left",
    "both",
    "mixed",
    "uninit",
    "xlin",
    "ylin",
    "bothlin"
};

Polarization SseMsg::stringToPolarization(const string &polStr)
{
    for (int i = 0; i < ARRAY_LENGTH(PolNames); i++)
    {
        if (string(PolNames[i]) == polStr)
        {
            return static_cast<Polarization>(i);
        }
    }

    // pol string not found
    //TBD throw exception?

    return POL_UNINIT;

}

// Convert from polarization enum to string.
//
string SseMsg::polarizationToString(Polarization pol)
{
    if (pol >= 0 && pol < ARRAY_LENGTH(PolNames)) 
    { 
	return PolNames[pol];
    }

    return "SseMsg Error: invalid polarization";

    // throw 1;   // TBD use appropriate exception object

}

char SseMsg::polarizationToSingleUpperChar(Polarization pol)
{
    // polarization (first letter only, in uppercase)
    return toupper(SseMsg::polarizationToString(pol).at(0));
}

static const char *siteIdNames[] =
{
    "uninit",   // uninitialized 
    "?",
    "?",
    "?",
    "?",
    "Arecibo",  // ID_ARECIBO = 5
    "?",
    "Jodrell",  // ID_JODRELL = 7
    "ATA"       // ID_ATA = 8
};


string SseMsg::getSiteIdName(SiteId id)
{
    string siteIdName("?");
    
    if (id >= 0 && id < ARRAY_LENGTH(siteIdNames)) 
    { 
	siteIdName =  siteIdNames[id];
    }
    else
    {
	// error handling TBD
	//cerr << "Error: invalid siteid" << endl;
    }
    return siteIdName;
}


// ----- Message Severity --------
static const char *nssMessageSeverityNames[] =
{
    "Info",
    "Warning",
    "Error",
    "Fatal"
};


string SseMsg::nssMessageSeverityToString(NssMessageSeverity severity)
{
    string severityString("?");
    
    if (severity >= 0 && severity < ARRAY_LENGTH(nssMessageSeverityNames)) 
    { 
	severityString = nssMessageSeverityNames[severity];
    }
    else
    {
	// error handling TBD
	//cerr << "Error: invalid severity" << endl;
    }
    return severityString;
}
// --------------------------------

// marshalling stuff

void SseMsg::marshall(Polarization &pol)
{
    pol = static_cast<Polarization>(htonl(pol));
}

void SseMsg::demarshall(Polarization &pol)
{
    marshall(pol);
}

//---------------------------
void SseMsg::marshall(SiteId &id)
{
    id = static_cast<SiteId>(htonl(id));
}

void SseMsg::demarshall(SiteId &id)
{
    marshall(id);
}

//---------------------------
void SseMsg::marshall(bool_t &arg)
{
    arg = static_cast<bool_t>(htonl(arg));
}

void SseMsg::demarshall(bool_t &arg)
{
    marshall(arg);
}

//---------------------------
void SseMsg::marshall(NssMessageSeverity &arg)
{
    arg = static_cast<NssMessageSeverity>(htonl(arg));
}

void SseMsg::demarshall(NssMessageSeverity &arg)
{
    marshall(arg);
}
