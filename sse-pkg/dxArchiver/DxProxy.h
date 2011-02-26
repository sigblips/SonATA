/*******************************************************************************

 File:    DxProxy.h
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


#ifndef _dx_proxy_h
#define _dx_proxy_h

#include "sseDxInterface.h"
#include "NssProxy.h"
#include <string>

template<typename Tproxy> class NssComponentManager;
class DxProxyInternal;

class DxProxy : public NssProxy
{

public:
    DxProxy(NssComponentManager<DxProxy> *dxManager);
    virtual ~DxProxy();


    // public utility methods
    // -----------------------
    void notifyInputConnected();
    void notifyInputDisconnected();

    const DxIntrinsics & getIntrinsics();
    const string & getDxId();
    string getName();

    // outgoing messages to Dx
    // -------------------------
    void requestIntrinsics();

    void setVerboseLevel(int level);

 protected:
    // override base class method
    void logError(const string &errorText);

private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxProxy(const DxProxy& rhs);
    DxProxy& operator=(const DxProxy& rhs);

    DxProxyInternal *internal_;

    friend class DxProxyInternal;

    // ----- private utilities -----------------------
    void sendMsgNoId(int messageCode, int dataLength = 0,
		     const void *msgBody = 0);

    // from base class
    void sendMessage(int messageCode, int activityId = -1, int dataLength = 0,
		     const void *msgBody = 0);

    string expectedInterfaceVersion() { return SSE_DX_INTERFACE_VERSION; }
    string receivedInterfaceVersion() { return getIntrinsics().interfaceVersionNumber; }

    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff);

};


#endif //_dx_proxy_h