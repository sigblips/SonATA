/*******************************************************************************

 File:    getHostIpAddr.cpp
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


#include "ace/OS.h"
#include "SseCommUtil.h"
#include <iostream>

using namespace std;

// Get host IP address in a thread-safe manner.
// Returns first one it finds or empty string on error.
//
string SseCommUtil::getHostIpAddr(const string &hostname)
{
    string ipAddr("");

    // get host info
    struct hostent hostinfoBuff;       
    struct hostent *hostinfo = &hostinfoBuff;       
    ACE_HOSTENT_DATA buffer;
    int h_errnop;

    if ((hostinfo = ACE_OS::gethostbyname_r (hostname.c_str(), hostinfo,
					     buffer, &h_errnop)) == 0)
    {
	cerr << "error in gethostbyname_r: h_errnop=" << h_errnop << endl;
	
	// tbd error handling -- throw an error?
    }
    else
    {
	// get first IP address
	char **addrs = hostinfo->h_addr_list;
	while (*addrs)
	{
	    char *ipStr = inet_ntoa(*(struct in_addr *) *addrs);

	    ipAddr = ipStr;
	    break;   // stop after the first one, for now

	    ++addrs;  
	}
    }

    return ipAddr; 
}