/*******************************************************************************

 File:    DxArchiverProxy.cpp
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


#include <ace/OS.h>
#include "DebugLog.h" // keep this early in the headers for VERBOSE macros
#include "DxArchiverProxy.h"
#include "SseDxMsg.h"
#include "sseDxArchiverInterface.h"
#include "SseMsg.h"
#include "Assert.h"
#include <iostream>
#include "SseArchive.h"
#include "SseMessage.h"
#include <algorithm>
#include <map>
#include "NssComponentManager.h"


struct DxArchiverProxyInternal
{
   DxArchiverProxyInternal(DxArchiverProxy *proxy,
			    NssComponentManager<DxArchiverProxy> *siteDxManager);
   ~DxArchiverProxyInternal();

   string getNameInternal();
   void setNameInternal(const string &name);

   DxArchiverProxy *proxy_;
   NssComponentManager<DxArchiverProxy> *siteDxArchiverManager_;
   DxArchiverIntrinsics intrinsics_;
   DxArchiverStatus status_;
   string name_;

   ACE_Recursive_Thread_Mutex nameMutex_;
   ACE_Recursive_Thread_Mutex statusMutex_;
   ACE_Recursive_Thread_Mutex intrinsicsMutex_;

   // --- private routines to process incoming messages from Dx -----

   void sendNssMessage(NssMessage *nssMessage, int activityId);
   void sendIntrinsics(DxArchiverIntrinsics *intrinsics);
   void sendStatus(DxArchiverStatus *status);
   void setIntrinsics(const DxArchiverIntrinsics & intrinsics);

   int getVerboseLevel();
   void logBadMessageSize(SseInterfaceHeader *hdr);

   void updateDxArchiverStatus(const DxArchiverStatus &status);
};

DxArchiverProxyInternal::DxArchiverProxyInternal(
   DxArchiverProxy *proxy, 
   NssComponentManager<DxArchiverProxy> *siteDxArchiverManager)
   :
   proxy_(proxy),
   siteDxArchiverManager_(siteDxArchiverManager),
   name_("dx-archiver-unknown")
{
   // TBD change to DxArchiver
   // defaults
   SseUtil::strMaxCpy(intrinsics_.interfaceVersionNumber,
		      SSE_DX_INTERFACE_VERSION, MAX_TEXT_STRING);

   // should this be stored?
   status_.timestamp = SseMsg::currentNssDate();

}

DxArchiverProxyInternal::~DxArchiverProxyInternal()
{

}

string DxArchiverProxyInternal::getNameInternal()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(nameMutex_);

   return name_;
}

void DxArchiverProxyInternal::setNameInternal(const string &name)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(nameMutex_);

   name_ = name;
}



// ---- private routines to handle incoming messages from DxArchiver server ---
//------------------------------------------------------------------------

void DxArchiverProxyInternal::updateDxArchiverStatus(const DxArchiverStatus &status)
{
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

      status_ = status;
   }

   // don't mutex protect this call, so that the
   // status can be read back without blocking

   siteDxArchiverManager_->notifyStatusChanged(proxy_);
}





void DxArchiverProxyInternal::sendNssMessage(NssMessage *nssMessage,
					      int activityId)
{
   // forward error to the site.
   siteDxArchiverManager_->processNssMessage(proxy_,
					      *nssMessage, activityId,
					      proxy_->getRemoteHostname());

}

void DxArchiverProxyInternal::setIntrinsics(const DxArchiverIntrinsics & intrinsics)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   intrinsics_ = intrinsics;

   setNameInternal(intrinsics_.name);

   VERBOSE2(getVerboseLevel(),  intrinsics_ );
}


void DxArchiverProxyInternal::sendIntrinsics(DxArchiverIntrinsics *intrinsics)
{
   setIntrinsics(*intrinsics);

   siteDxArchiverManager_->receiveIntrinsics(proxy_);
}


void DxArchiverProxyInternal::sendStatus(DxArchiverStatus *stat)
{
   updateDxArchiverStatus(*stat);
}


int DxArchiverProxyInternal::getVerboseLevel()
{
   return proxy_->getVerboseLevel();
}

void DxArchiverProxyInternal::logBadMessageSize(SseInterfaceHeader *hdr)
{
   stringstream strm;
   strm << "DxArchiverProxy Error:"
	<< " dataLength of " << hdr->dataLength
	<< " is invalid for message '"
	<< SseDxMsg::messageCodeToString(hdr->code)
	<< "' from dxArchiver: " << proxy_->getName() << endl;
   SseMessage::log(proxy_->getName(), hdr->activityId,
                   SSE_MSG_INVALID_MSG,
                   SEVERITY_ERROR, strm.str().c_str(),
                   __FILE__, __LINE__);
}



//  ------- end DxArchiverProxyInternal ---------
//  --------------------------------------
//  ------- begin DxArchiverProxy ---------------


DxArchiverProxy::DxArchiverProxy(
   NssComponentManager<DxArchiverProxy> *siteDxArchiverManager)
   :
   internal_(new DxArchiverProxyInternal(this, siteDxArchiverManager))
{
}

DxArchiverProxy::~DxArchiverProxy()
{
   delete internal_;

   VERBOSE2(getVerboseLevel(), 
	    "DxArchiverProxy destructor" << endl;);
}

// --------- DxArchiverProxy public utility methods ------------
// ------------------------------------------------------

void DxArchiverProxy::notifyInputConnected()
{

   // Register this dx with the site.
   // Need to do this here instead of in the DxArchiverProxy constructor
   // because we don't want to register the proxy until its
   // socket connection with the real dx is established.

   internal_->siteDxArchiverManager_->registerProxy(this);

}

void DxArchiverProxy::notifyInputDisconnected()
{
   // unregister with the site
   internal_->siteDxArchiverManager_->unregisterProxy(this);
}


DxArchiverIntrinsics DxArchiverProxy::getIntrinsics()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->intrinsicsMutex_);

   return internal_->intrinsics_;
}



// Return the currently stored (cached) status.
// Does NOT query the server for the latest status.
DxArchiverStatus DxArchiverProxy::getCachedStatus()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->statusMutex_);

   return internal_->status_;
}



// a unique identifier for this archiver
string DxArchiverProxy::getName()
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

// dispatch incoming dx message to appropriate
// sse classes & methods
void DxArchiverProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{

   // TBD. someday translate message codes to ascii strings

   VERBOSE2(getVerboseLevel(),
	    "Recv Msg from " << getName() <<  ": "
	    << hdr->code << "\n" << *hdr);

   DxArchiverIntrinsics *intrinsics;
   DxArchiverStatus *status;
   NssMessage *nssMessage;

   // dispatch here
   // process message body, if there is one
   switch (hdr->code)
   {
   case SEND_DX_ARCHIVER_INTRINSICS:

      // store intrinsics in this proxy
      VERBOSE2(getVerboseLevel(),
	       "DxArchiverProxy:store intrinsics  ... " << endl;);
 
      ValidateMsgLength(hdr, 
			hdr->dataLength == sizeof(DxArchiverIntrinsics));
      intrinsics = static_cast<DxArchiverIntrinsics *>(bodybuff);
      intrinsics->demarshall();
      internal_->sendIntrinsics(intrinsics);

      break;

   case SEND_DX_ARCHIVER_STATUS:
 
      // store status in this proxy
      VERBOSE2(getVerboseLevel(),
	       "store status  ... " << endl;);

      ValidateMsgLength(hdr, hdr->dataLength == sizeof(DxArchiverStatus));
      status = static_cast<DxArchiverStatus *>(bodybuff);
      status->demarshall();
      internal_->sendStatus(status);

      VERBOSE2(getVerboseLevel(), *status << endl);

      break;


   case SEND_NSS_MESSAGE:

      ValidateMsgLength(hdr, hdr->dataLength == sizeof(NssMessage));
      nssMessage = static_cast<NssMessage *>(bodybuff);
      nssMessage->demarshall();
      internal_->sendNssMessage(nssMessage, hdr->activityId);
      break;


   default: 
      stringstream strm;
      strm << "DxArchiverProxy::handleDxMessage: "
	   <<  "unexpected message code received:"
	   << hdr->code 
	   << " from dx " << getName() << endl;
      SseMessage::log(getName(), hdr->activityId,
                      SSE_MSG_INVALID_MSG,
                      SEVERITY_ERROR, strm.str().c_str(),
                      __FILE__, __LINE__);
      break;
   };  


}


// *** outgoing messages to dxArchive server *****
//--------------------------------------------

void DxArchiverProxy::requestIntrinsics()
{
   VERBOSE2(getVerboseLevel(), 
	    "DxArchiverProxy: sending requestIntrinsics msg to dxArchiver\n";);

   DxArchiverMessageCode code = REQUEST_DX_ARCHIVER_INTRINSICS;
   sendMsgNoId(code);
}


void DxArchiverProxy::requestStatusUpdate()
{
   VERBOSE2(getVerboseLevel(),     
	    "DxArchiverProxy: sending requestStatus msg to dxArchiver\n";);

   DxArchiverMessageCode code = REQUEST_DX_ARCHIVER_STATUS;
   sendMsgNoId(code);
}



void DxArchiverProxy::shutdown()
{ 
   VERBOSE2(getVerboseLevel(),     
	    "DxArchiverProxy: sending shutdown msg to dxArchiver\n";);

   DxArchiverMessageCode code = SHUTDOWN_DX_ARCHIVER;
   sendMsgNoId(code);
}




// *** DxArchiverProxy private utility routines ****
//---------------------------------------------

string DxArchiverProxy::expectedInterfaceVersion()
{
   return SSE_DX_ARCHIVER_INTERFACE_VERSION;
}

string DxArchiverProxy::receivedInterfaceVersion() 
{
   return getIntrinsics().interfaceVersionNumber;
}



void DxArchiverProxy::sendMsgNoId(int messageCode, int dataLength,
				   const void *msgBody)
{
   int activityId = NSS_NO_ACTIVITY_ID;
   sendMessage(messageCode, activityId, dataLength, msgBody);
}

// send a marshalled message
void DxArchiverProxy::sendMessage(int messageCode, int activityId, int dataLength,
				   const void *msgBody)
{
   VERBOSE2(getVerboseLevel(), 
	    "Send Msg to " << getName() <<  ": "
	    << " code: " << messageCode << "\n");

   // TBD translate messageCode to string

   // Assume this is mutex protected by NssProxy
   NssProxy::sendMessage(messageCode, activityId, dataLength, msgBody);


}
