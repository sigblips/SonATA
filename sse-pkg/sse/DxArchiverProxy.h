/*******************************************************************************

 File:    DxArchiverProxy.h
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


#ifndef _dx_archiver_proxy_h
#define _dx_archiver_proxy_h


#include "sseDxArchiverInterface.h"
#include "SharedProxy.h"
#include <string>

class DxArchiverProxyInternal;
template<typename Tproxy> class NssComponentManager;

class DxArchiverProxy : public SharedProxy
{

public:
    DxArchiverProxy(NssComponentManager<DxArchiverProxy> *siteDxArchiverManager);
    ~DxArchiverProxy();

    // public utility methods
    // -----------------------
    void notifyInputConnected();
    void notifyInputDisconnected();

    DxArchiverIntrinsics getIntrinsics();
    DxArchiverStatus getCachedStatus();
    string getName();


    // outgoing messages to DxArchiver
    // --------------------------------
    void requestIntrinsics();
    void requestStatusUpdate();
    void shutdown();

private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxArchiverProxy(const DxArchiverProxy& rhs);
    DxArchiverProxy& operator=(const DxArchiverProxy& rhs);


    // ----- private utilities -----------------------
    void sendMsgNoId(int messageCode, int dataLength = 0,
		     const void *msgBody = 0);

    // from base class
    void sendMessage(int messageCode, int activityId = -1, int dataLength = 0,
		     const void *msgBody = 0);

    string expectedInterfaceVersion();
    string receivedInterfaceVersion();

    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff);

    DxArchiverProxyInternal *internal_;
    friend class DxArchiverProxyInternal;

};


#endif 