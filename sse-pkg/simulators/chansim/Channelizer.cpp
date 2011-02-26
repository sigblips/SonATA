/*******************************************************************************

 File:    Channelizer.cpp
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


#include <ace/Timer_Queue.h>
#include <ace/Reactor.h>
#include "Channelizer.h"
#include "SseProxy.h"
#include "SseMsg.h"
#include "Assert.h"
#include "SseUtil.h"
#include "Verbose.h"
#include <list>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

class TestProxy : public NssProxy
{
 public:
    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff) {};
    string getName() { return "testProxy"; }
};

Channelizer::Channelizer(SseProxy *sseProxy, const string &name,
                         int totalChans, int outputChans, double outBwMhz) 
    : sseProxy_(sseProxy),
      verboseLevel_(0),
      startedTimeoutPtr_(0)
{
    sseProxy_->setChannelizer(this);
    initIntrinsics(name, totalChans, outputChans, outBwMhz);
    sseProxy_->setModuleName(name);
}

Channelizer::~Channelizer()
{
   cancelTimer();
   delete startedTimeoutPtr_;
}

void Channelizer::cancelTimer()
{
   if (startedTimeoutPtr_)
   {
      startedTimeoutPtr_->cancelTimer();  // harmless if not active
   }
}

SseProxy * Channelizer::getSseProxy()
{
    return (sseProxy_);
}

void Channelizer::setVerboseLevel(int level)
{
    verboseLevel_ = level;

    sseProxy_->setVerboseLevel(level);

}

int Channelizer::getVerboseLevel()
{
    return verboseLevel_;
}

void Channelizer::requestIntrinsics()
{
    VERBOSE2(getVerboseLevel(), "Channelizer::requestIntrinsics\n");

    sseProxy_->sendIntrinsics(intrinsics_);   

#ifdef DEBUG_NSSMSG
    // sendMessage debug test
    NssMessage nssMessage;
    nssMessage.code = 0;
    nssMessage.severity = SEVERITY_INFO;
    SseUtil::strMaxCpy(nssMessage.description,
                       "test of message, sent after intrinsics",
                       MAX_NSS_MESSAGE_STRING);
    sseProxy_->sendNssMessage(nssMessage);

#endif	

}

void Channelizer::requestStatus()
{
    sendStatus();
}

void Channelizer::sendStatus()
{ 
    status_.timestamp = SseMsg::currentNssDate();

    sseProxy_->sendStatus(status_);   
}

void Channelizer::sendErrorMsgToSse(const string &text)
{
    NssMessage nssMessage;
    nssMessage.code = 0;   // TBD add arg to set code
    nssMessage.severity = SEVERITY_ERROR;

    // Don't overrun the destination buffer.
    Assert (static_cast<signed int>(text.length())
	    < MAX_NSS_MESSAGE_STRING); 

    SseUtil::strMaxCpy(nssMessage.description, text.c_str(),
		       MAX_NSS_MESSAGE_STRING); 

    int activityId = NSS_NO_ACTIVITY_ID;
    sseProxy_->sendNssMessage(nssMessage, activityId);
}

void Channelizer::start(Start *start)
{
   status_.state = STATE_PENDING;
   status_.startTime = start->startTime;
   status_.centerSkyFreqMhz = start->centerSkyFreqMhz;

   /*
     Setup timer to call sendStarted.
    */
   cancelTimer();  // in case one is pending already

   delete startedTimeoutPtr_;
   startedTimeoutPtr_ = new Timeout<Channelizer>("start timeout");

   int waitDurationSecs = start->startTime.tv_sec - 
      SseMsg::currentNssDate().tv_sec;

   startedTimeoutPtr_->startTimer(
      waitDurationSecs, this,
      &Channelizer::sendStarted, getVerboseLevel());
}

void Channelizer::sendStarted()
{
   status_.state = STATE_RUNNING;

   Started started;
   started.startTime = status_.startTime;
   sseProxy_->sendStarted(started);
}

void Channelizer::stop()
{
   cancelTimer();
   status_.state = STATE_IDLE;
}

void Channelizer::shutdown()
{
    ACE_Reactor::end_event_loop();
}


//---- private  utilities -------------


void Channelizer::printIntrinsics()
{
    cout << intrinsics_;
}

void Channelizer::printStatus()
{
    cout << status_ << endl;
}


// initialize the intrinsics information
void Channelizer::initIntrinsics(
   const string &name, int totalChans, int outputChans, double outBwMhz) 
{
   // use fill_n to zero out all the char arrays 
   // (to appease the memory checking tools)

   fill_n(intrinsics_.interfaceVersion, MAX_TEXT_STRING, '\0');
   SseUtil::strMaxCpy(intrinsics_.interfaceVersion,
                      SSE_CHANNELIZER_INTERFACE_VERSION, MAX_TEXT_STRING);

   // debug
   // strcpy(intrinsics_.interfaceVersion, "bad version number test");

   // host
   fill_n(intrinsics_.host, MAX_TEXT_STRING, '\0'); 
   SseUtil::strMaxCpy(intrinsics_.host, SseUtil::getHostname().c_str(),
                      MAX_TEXT_STRING);

   // name
   fill_n(intrinsics_.name, MAX_TEXT_STRING, '\0'); 
   SseUtil::strMaxCpy(intrinsics_.name, name.c_str(), MAX_TEXT_STRING);

   intrinsics_.totalChannels = totalChans;
   intrinsics_.outputChannels = outputChans;
   intrinsics_.mhzPerChannel = outBwMhz / outputChans;

   intrinsics_.oversampling = 1.25;

}
