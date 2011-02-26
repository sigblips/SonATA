/*******************************************************************************

 File:    NssProxy.h
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


#ifndef NssProxy_H
#define NssProxy_H

// This is the base class that acts as a proxy for
// Nss components connecting via sockets.

#include "sseInterface.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ComponentProxy.h"

#include <string>


using std::string;

class NssProxyInternal;

class NssProxy : public ComponentProxy
{
 public:
    NssProxy();   // expects to be connected via an ACE Acceptor
    NssProxy(ACE_SOCK_STREAM &stream);  // connect via stream
    virtual ~NssProxy();

    // methods for ACE_Event_Handler
    virtual int handle_input(ACE_HANDLE);
    virtual ACE_HANDLE get_handle() const;
    virtual ACE_SOCK_Stream & getAceSockStream();

 protected:
    virtual void sendMessage(int messageCode, int activityId = -1, 
			     int dataLength = 0, const void *msgBody = 0);

    // subclasses need to define this:
    virtual void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff) = 0;

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    NssProxy(const NssProxy& rhs);
    NssProxy& operator=(const NssProxy& rhs);

    NssProxyInternal *internal_;

    friend class NssProxyInternal;
};

#endif // NssProxy_H