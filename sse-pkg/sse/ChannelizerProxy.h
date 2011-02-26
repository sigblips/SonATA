/*******************************************************************************

 File:    ChannelizerProxy.h
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


#ifndef ChannelizerProxy_H
#define ChannelizerProxy_H

#include "sseChannelizerInterface.h"
#include "SharedProxy.h"
#include <string>

class ChannelizerProxyInternal;
template<typename Tproxy> class NssComponentManager;

class ChannelizerProxy : public SharedProxy
{

public:
    ChannelizerProxy(NssComponentManager<ChannelizerProxy> *siteChannelizerManager);
    ~ChannelizerProxy();

    // public utility methods
    // -----------------------
    void notifyInputConnected();
    void notifyInputDisconnected();

    ssechan::Intrinsics getIntrinsics();
    int32_t getOutputChannels();
    float64_t getMhzPerChannel();
    ssechan::Status getCachedStatus();
    string getName();
    float64_t getRequestedTuneFreq();
    void setRequestedTuneFreq(float64_t freq);

    // outgoing messages to Channelizer
    // --------------------------------
    void requestIntrinsics();
    void requestStatusUpdate();
    void start(const NssDate & startTime, double skyFreqMhz);
    void shutdown();
    void stop();

private:
    // Disable copy construction & assignment.
    // Don't define these.
    ChannelizerProxy(const ChannelizerProxy& rhs);
    ChannelizerProxy& operator=(const ChannelizerProxy& rhs);


    // ----- private utilities -----------------------
    void sendMsgNoId(int messageCode, int dataLength = 0,
		     const void *msgBody = 0);

    // from base class
    void sendMessage(int messageCode, int activityId = -1, int dataLength = 0,
		     const void *msgBody = 0);

    string expectedInterfaceVersion();
    string receivedInterfaceVersion();

    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff);

    ChannelizerProxyInternal *internal_;
    friend class ChannelizerProxyInternal;

};


#endif 