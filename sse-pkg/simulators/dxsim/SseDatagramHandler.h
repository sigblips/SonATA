/*******************************************************************************

 File:    SseDatagramHandler.h
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


#ifndef sse_datagram_handler_h
#define  sse_datagram_handler_h

#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"
#include "ace/Reactor.h"
#include <string>

using std::string;

class SseDatagramHandler : public ACE_Event_Handler
{
    // = TITLE
    //     Handle dgram events.
public:
    // = Initialization and termination methods.
    SseDatagramHandler (const char *multicastAddr,
			u_short multicastPort,
			ACE_Reactor &reactor);
    // Constructor.

    ~SseDatagramHandler (void);
    // Destructor.

    void contact(u_short *ssePort, string *sseIpAddrString);

    // Event demuxer hooks.
    virtual int handle_input (ACE_HANDLE);
    virtual int handle_close (ACE_HANDLE,
			      ACE_Reactor_Mask);
    virtual ACE_HANDLE get_handle (void) const;
    virtual int getSsePort();
    virtual string getSseIpAddrString(); 

private:
    ACE_INET_Addr dgramReceiveAddr_;  // use system assigned address, port
    ACE_SOCK_Dgram dgram_;
    bool thereYouAreReceived_;  // 

    friend class HereIAmTimerHandler;

    int ssePort_;
    string sseIpAddrString_;
    const char *multicastAddr_;
    int multicastPort_;
};



#endif
