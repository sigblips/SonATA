/*******************************************************************************

 File:    Channelizer.h
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


#ifndef CHANNELIZER_H
#define CHANNELIZER_H

#include "sseChannelizerInterface.h"
#include "Timeout.h"
#include <vector>

using std::string;
class SseProxy;

using namespace ssechan;

class Channelizer {

public:
   Channelizer(SseProxy *sseProxy, const string &name,
               int totalChans, int outputChans, double outBwMhz);
    virtual ~Channelizer();

    // --- incoming messages from SSE ------
    virtual void requestIntrinsics();
    virtual void requestStatus();
    virtual void start(Start *start);
    virtual void stop();
    virtual void shutdown();

    // ----- public utility methods ------------------
    virtual void printIntrinsics();
    virtual void printStatus();

    virtual void setVerboseLevel(int level);
    virtual int getVerboseLevel();

    virtual void sendStatus();

    // test methods
    virtual void sendErrorMsgToSse(const string &text);

protected:
    virtual SseProxy *getSseProxy();

    virtual void initIntrinsics(const string &name,
                                int totalChans, int outputChans,
                                double outBwMhz);

    virtual void sendStarted();

private:
    
    void cancelTimer();

    SseProxy *sseProxy_;
    Intrinsics intrinsics_;
    Status status_;
    int verboseLevel_;
    Timeout<Channelizer> * startedTimeoutPtr_;

    // Disable copy construction & assignment.
    // Don't define these.
    Channelizer(const Channelizer& rhs);
    Channelizer& operator=(const Channelizer& rhs);

};



#endif