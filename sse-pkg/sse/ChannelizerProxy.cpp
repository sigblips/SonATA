/*******************************************************************************

 File:    ChannelizerProxy.cpp
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

/*
  ChannelizerProxy.cpp
 */

#include <ace/OS.h>
#include "DebugLog.h" // keep this early in the headers for VERBOSE macros
#include "ChannelizerProxy.h"
#include "sseChannelizerInterface.h"
#include "SseArchive.h"
#include "SseMessage.h"
#include "NssComponentManager.h"
#include "SseMsg.h"
#include "Assert.h"
#include <algorithm>
#include <map>
#include <iostream>

using namespace ssechan;

struct ChannelizerProxyInternal
{
   ChannelizerProxyInternal(
      ChannelizerProxy *proxy,
      NssComponentManager<ChannelizerProxy> *siteChannelizerManager);

   ~ChannelizerProxyInternal();

   string getNameInternal();
   void setNameInternal(const string &name);

   ChannelizerProxy *proxy_;
   NssComponentManager<ChannelizerProxy> *siteChannelizerManager_;
   Intrinsics intrinsics_;
   Status status_;
   string name_;
   float64_t requestedTuneFrequency_;

   ACE_Recursive_Thread_Mutex nameMutex_;
   ACE_Recursive_Thread_Mutex statusMutex_;
   ACE_Recursive_Thread_Mutex intrinsicsMutex_;


   // --- private routines to process incoming messages from chanzer -----

   void sendNssMessage(NssMessage *nssMessage, int activityId);
   void sendIntrinsics(Intrinsics *intrinsics);  
   void sendStatus(Status *status); 
   void sendStarted(Started *started); 
   void setIntrinsics(const Intrinsics & intrinsics);

   int getVerboseLevel();
   void logBadMessageSize(SseInterfaceHeader *hdr);

   void updateChannelizerStatus(const Status &status);
};

ChannelizerProxyInternal::ChannelizerProxyInternal(
   ChannelizerProxy *proxy, 
   NssComponentManager<ChannelizerProxy> *siteChannelizerManager)
   :
   proxy_(proxy),
   siteChannelizerManager_(siteChannelizerManager),
   name_("channelizer-unknown"),
   requestedTuneFrequency_(1420.0)
{
   // TBD change to Channelizer
   // defaults
   SseUtil::strMaxCpy(intrinsics_.interfaceVersion,
		      SSE_CHANNELIZER_INTERFACE_VERSION, MAX_TEXT_STRING);

   // should this be stored?
   status_.timestamp = SseMsg::currentNssDate();

}

ChannelizerProxyInternal::~ChannelizerProxyInternal()
{

}

string ChannelizerProxyInternal::getNameInternal()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(nameMutex_);

   return name_;
}

void ChannelizerProxyInternal::setNameInternal(const string &name)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(nameMutex_);

   name_ = name;
}



// ---- private routines to handle incoming messages from Channelizer server ---
//------------------------------------------------------------------------

void ChannelizerProxyInternal::updateChannelizerStatus(
   const Status &status)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

      status_ = status;
   }

   // don't mutex protect this call, so that the
   // status can be read back without blocking

   siteChannelizerManager_->notifyStatusChanged(proxy_);
}





void ChannelizerProxyInternal::sendNssMessage(NssMessage *nssMessage,
					      int activityId)
{
   // forward error to the site.
   siteChannelizerManager_->processNssMessage(
      proxy_, *nssMessage, activityId,
      proxy_->getRemoteHostname());

}

void ChannelizerProxyInternal::setIntrinsics(
   const Intrinsics & intrinsics)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   intrinsics_ = intrinsics;

   setNameInternal(intrinsics_.name);

   VERBOSE2(getVerboseLevel(),  intrinsics_);
}


void ChannelizerProxyInternal::sendIntrinsics(Intrinsics *intrinsics)
{
   setIntrinsics(*intrinsics);

   siteChannelizerManager_->receiveIntrinsics(proxy_);
}


void ChannelizerProxyInternal::sendStatus(Status *stat)
{
   updateChannelizerStatus(*stat);
}

void ChannelizerProxyInternal::sendStarted(Started *started)
{
   proxy_->requestStatusUpdate();

   // TBD notify activity
}

int ChannelizerProxyInternal::getVerboseLevel()
{
   return proxy_->getVerboseLevel();
}

void ChannelizerProxyInternal::logBadMessageSize(SseInterfaceHeader *hdr)
{
   stringstream strm;
   strm << "ChannelizerProxy Error:" 
	<< " dataLength of " << hdr->dataLength
	<< " is invalid for message '"

      // TBD print message code string
      //<< SseDxMsg::messageCodeToString(hdr->code)
        << hdr->code

	<< "' from channelizer: " << proxy_->getName() << endl;

   SseMessage::log(proxy_->getName(), hdr->activityId,
                   SSE_MSG_INVALID_MSG,
                   SEVERITY_ERROR, strm.str().c_str(),
                   __FILE__, __LINE__);
}



//  ------- end ChannelizerProxyInternal ---------
//  --------------------------------------
//  ------- begin ChannelizerProxy ---------------


ChannelizerProxy::ChannelizerProxy(
   NssComponentManager<ChannelizerProxy> *siteChannelizerManager)
   :
   internal_(new ChannelizerProxyInternal(this, siteChannelizerManager))
{
}

ChannelizerProxy::~ChannelizerProxy()
{
   delete internal_;

   VERBOSE2(getVerboseLevel(), 
	    "ChannelizerProxy destructor" << endl;);
}

// --------- ChannelizerProxy public utility methods ------------
// ------------------------------------------------------

void ChannelizerProxy::notifyInputConnected()
{
   // Register this channelizer with the site.
   // Need to do this here instead of in the ChannelizerProxy constructor
   // because we don't want to register the proxy until its
   // socket connection with the real channelizer is established.

   internal_->siteChannelizerManager_->registerProxy(this);
}

void ChannelizerProxy::notifyInputDisconnected()
{
   // unregister with the site
   internal_->siteChannelizerManager_->unregisterProxy(this);
}

Intrinsics ChannelizerProxy::getIntrinsics()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->intrinsicsMutex_);

   return internal_->intrinsics_;
}

float64_t ChannelizerProxy::getRequestedTuneFreq()
{
    return internal_->requestedTuneFrequency_;
}
void ChannelizerProxy::setRequestedTuneFreq(float64_t freq)
{
    internal_->requestedTuneFrequency_ = freq;
}

int32_t ChannelizerProxy::getOutputChannels()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->intrinsicsMutex_);

   return internal_->intrinsics_.getOutputChannels();
}

float64_t ChannelizerProxy::getMhzPerChannel()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->intrinsicsMutex_);

   return internal_->intrinsics_.getMhzPerChannel();
}

/*
   Return the currently stored (cached) status.
   Does NOT query the server for the latest status.
*/
Status ChannelizerProxy::getCachedStatus()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->statusMutex_);

   return internal_->status_;
}

// a unique identifier for this channelizer
string ChannelizerProxy::getName()
{
   return internal_->getNameInternal();
}


// define a macro to validate the dataLength of an incoming message
#define ValidateMsgLength(hdr, validLengthTest) \
      if (!(validLengthTest)) \
      { \
	  internal_->logBadMessageSize(hdr); \
	  return; \
      }

// dispatch incoming message to appropriate
// sse classes & methods
void ChannelizerProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{

   // TBD. someday translate message codes to ascii strings

   VERBOSE2(getVerboseLevel(),
	    "Recv Msg from " << getName() <<  ": "
	    << hdr->code << "\n" << *hdr);

   Intrinsics *intrinsics;
   Status *status;
   NssMessage *nssMessage;
   Started *started;

   // dispatch here
   // process message body, if there is one
   switch (hdr->code)
   {
   case SEND_INTRINSICS:

      // store intrinsics in this proxy
      VERBOSE2(getVerboseLevel(),
	       "ChannelizerProxy:store intrinsics  ... " << endl;);
 
      ValidateMsgLength(hdr, 
			hdr->dataLength == sizeof(Intrinsics));
      intrinsics = static_cast<Intrinsics *>(bodybuff);
      intrinsics->demarshall();
      internal_->sendIntrinsics(intrinsics);

      break;

   case SEND_STATUS:
 
      // store status in this proxy
      VERBOSE2(getVerboseLevel(),
	       "store status  ... " << endl;);

      ValidateMsgLength(hdr, hdr->dataLength == sizeof(Status));
      status = static_cast<Status *>(bodybuff);
      status->demarshall();
      internal_->sendStatus(status);

      VERBOSE2(getVerboseLevel(), *status << endl);

      break;

   case STARTED:
 
      ValidateMsgLength(hdr, hdr->dataLength == sizeof(Started));
      started = static_cast<Started *>(bodybuff);
      started->demarshall();
      internal_->sendStarted(started);

      VERBOSE2(getVerboseLevel(), *started << endl);

      break;

   case SEND_MESSAGE:

      ValidateMsgLength(hdr, hdr->dataLength == sizeof(NssMessage));
      nssMessage = static_cast<NssMessage *>(bodybuff);
      nssMessage->demarshall();
      internal_->sendNssMessage(nssMessage, hdr->activityId);
      break;

   default: 
      stringstream strm;
      strm << "ChannelizerProxy::handleIncomingMessage: " 
	   <<  "unexpected message code received:"
	   << hdr->code 
	   << " from chanzer " << getName() << endl;
      SseMessage::log(getName(), hdr->activityId,
                      SSE_MSG_INVALID_MSG,
                      SEVERITY_ERROR, strm.str().c_str(),
                      __FILE__, __LINE__);
      break;
   };  


}


// *** outgoing messages to channelizer server *****
//--------------------------------------------

void ChannelizerProxy::requestIntrinsics()
{
   VERBOSE2(getVerboseLevel(), 
	    "ChannelizerProxy: sending requestIntrinsics msg to channelizer\n";);

   ChannelizerMessageCode code = REQUEST_INTRINSICS;
   sendMsgNoId(code);
}


void ChannelizerProxy::requestStatusUpdate()
{
   VERBOSE2(getVerboseLevel(),     
	    "ChannelizerProxy: sending requestStatus msg to channelizer\n";);

   ChannelizerMessageCode code = REQUEST_STATUS;
   sendMsgNoId(code);
}



void ChannelizerProxy::shutdown()
{ 
   VERBOSE2(getVerboseLevel(),     
	    "ChannelizerProxy: sending shutdown msg to channelizer\n";);

   ChannelizerMessageCode code = SHUTDOWN;
   sendMsgNoId(code);
}

void ChannelizerProxy::start(const NssDate & startTime, double skyFreqMhz)
{
   VERBOSE2(getVerboseLevel(),     
	    "ChannelizerProxy: sending start msg to channelizer\n";);
   
   ChannelizerMessageCode code = START;
   Start msgBody;
   msgBody.startTime.tv_sec = startTime.tv_sec;
   msgBody.centerSkyFreqMhz = skyFreqMhz;

   int dataLength = sizeof(msgBody);
   msgBody.marshall();
   sendMsgNoId(code, dataLength, &msgBody);
   setRequestedTuneFreq(skyFreqMhz);
}

void ChannelizerProxy::stop()
{
   VERBOSE2(getVerboseLevel(), 
	    "ChannelizerProxy: sending stop to channelizer\n";);

   ChannelizerMessageCode code = STOP;
   sendMsgNoId(code);
}


// *** ChannelizerProxy private utility routines ****
//---------------------------------------------

string ChannelizerProxy::expectedInterfaceVersion()
{
   return SSE_CHANNELIZER_INTERFACE_VERSION;
}

string ChannelizerProxy::receivedInterfaceVersion() 
{
   return getIntrinsics().interfaceVersion;
}

void ChannelizerProxy::sendMsgNoId(int messageCode, int dataLength,
				   const void *msgBody)
{
   int activityId = NSS_NO_ACTIVITY_ID;
   sendMessage(messageCode, activityId, dataLength, msgBody);
}

// send a marshalled message
void ChannelizerProxy::sendMessage(int messageCode, int activityId, int dataLength,
				   const void *msgBody)
{
   VERBOSE2(getVerboseLevel(), 
	    "Send Msg to " << getName() <<  ": "
	    << " code: " << messageCode << "\n");

   // TBD translate messageCode to string

   // Assume this is mutex protected by NssProxy
   NssProxy::sendMessage(messageCode, activityId, dataLength, msgBody);


}
