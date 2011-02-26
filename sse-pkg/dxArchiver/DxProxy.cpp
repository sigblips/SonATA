/*******************************************************************************

 File:    DxProxy.cpp
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
#include "DxProxy.h"
#include "SseDxMsg.h"
#include "SseMsg.h"
#include "Assert.h"
#include "Verbose.h"
#include <iostream>
#include "SignalArchiver.h"
#include "NssComponentManager.h"

//#include "SseArchive.h"


struct DxProxyInternal
{
    DxProxyInternal(DxProxy *proxy,
		     NssComponentManager<DxProxy> *dxManager);
    ~DxProxyInternal();

    DxProxy *proxy_;
    NssComponentManager<DxProxy> *dxManager_;
    DxIntrinsics intrinsics_;
    string dxId_;
    SignalArchiver signalArchiver_;

    // --- private routines to process incoming messages from Dx -----

    void sendDxMessage(NssMessage *nssMessage, int activityId);
    void sendIntrinsics(DxIntrinsics *intrinsics);  
    void sendArchiveComplexAmplitudes(void *msgBody);


    // utilities
    int getVerboseLevel();
    void logBadMessageSize(SseInterfaceHeader *hdr);

};

DxProxyInternal::DxProxyInternal(DxProxy *proxy, 
				   NssComponentManager<DxProxy> *dxManager)
    : proxy_(proxy),
      dxManager_(dxManager),
      dxId_("dx-unknown")
{
    // defaults
    SseUtil::strMaxCpy(intrinsics_.interfaceVersionNumber,
		       SSE_DX_INTERFACE_VERSION, MAX_TEXT_STRING);
    
    // should this be stored?
    //status_.timestamp = SseMsg::currentNssDate();
    //status_.numberOfActivities = 0;

}

DxProxyInternal::~DxProxyInternal()
{

}


// ---- private routines to handle incoming messages from physical dx ---
//------------------------------------------------------------------------


void DxProxyInternal::sendDxMessage(NssMessage *nssMessage, int activityId)
{
    // forward error to the dxManager
    dxManager_->processNssMessage(proxy_, *nssMessage, activityId,
				       proxy_->getRemoteHostname());

}

void DxProxyInternal::sendIntrinsics(DxIntrinsics *intrinsics)
{
    intrinsics_ = *intrinsics;

    // use the hostname as the dx name (dx identifier)
    dxId_ = intrinsics_.name;

    signalArchiver_.setDxHostname(intrinsics_.name);

    //TBD. Do something with intrinsics_.serialNumber;

    VERBOSE2(getVerboseLevel(),  intrinsics_ );

    dxManager_->receiveIntrinsics(proxy_);

}



void DxProxyInternal::sendArchiveComplexAmplitudes(void *msgBody)
{
    ComplexAmplitudeHeader *hdr;
    SubchannelCoef1KHz *subchannelArray;
    SseDxMsg::extractComplexAmplitudesFromMsg(msgBody, &hdr, &subchannelArray);

    signalArchiver_.sendArchiveComplexAmplitudes(*hdr, subchannelArray);
}




// -- handle archive & wrapup ------

// utilities


int DxProxyInternal::getVerboseLevel()
{
    return proxy_->getVerboseLevel();
}


void DxProxyInternal::logBadMessageSize(SseInterfaceHeader *hdr)
{
//    SseArchive::ErrorLog()

    cerr <<  "DxProxy Error:"
			   << " dataLength of " << hdr->dataLength
	                   << " is invalid for message '"
			   << SseDxMsg::messageCodeToString(hdr->code)
			   << "' from dx: " << proxy_->getDxId() << endl;
}



//  ------- end DxProxyInternal ---------
//  --------------------------------------
//  ------- begin DxProxy ---------------


DxProxy::DxProxy(NssComponentManager<DxProxy> *dxManager)
    : internal_(new DxProxyInternal(this, dxManager))
{
}

DxProxy::~DxProxy()
{
    delete internal_;

    VERBOSE2(getVerboseLevel(), 
	      "DxProxy destructor" << endl;);
}

// --------- DxProxy public utility methods ------------
// ------------------------------------------------------

void DxProxy::notifyInputConnected()
{
    // Register this dx with the dxManager.
    // Need to do this here instead of in the DxProxy constructor
    // because we don't want to register the proxy until its
    // socket connection with the real dx is established.

    internal_->dxManager_->registerProxy(this);

}

void DxProxy::notifyInputDisconnected()
{
    // unregister with the dxManager
    internal_->dxManager_->unregisterProxy(this);

}


const DxIntrinsics & DxProxy::getIntrinsics()
{
    return internal_->intrinsics_;
}

#if 0

// Return the currently stored (cached) status.
// Does NOT query the dx for the latest status.
const DxStatus & DxProxy::getCachedDxStatus()
{
    return internal_->status_;
}
#endif

// a unique identifier for this dx
const string &DxProxy::getDxId()
{
    return internal_->dxId_;
}

// a unique identifier for this dx (alias for getDxId)
string DxProxy::getName()
{
    return internal_->dxId_;
}



// define a macro to validate the dataLength of an incoming message
#define ValidateMsgLength(hdr, validLengthTest) \
      if (!(validLengthTest)) \
      { \
	  internal_->logBadMessageSize(hdr); \
	  return; \
      }

// dispatch incoming dx message to appropriate
// classes & methods
void DxProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{

//    cout << "DxProxy(handleDxMessage from " << getDxId() << "): " << endl;

  VERBOSE2(getVerboseLevel(),
	   "Recv Msg from " << getDxId() <<  ": "
	   << SseDxMsg::messageCodeToString(hdr->code) << "\n" << *hdr);



  DxIntrinsics *intrinsics;
  Count *count;
  NssMessage *nssMessage;
  ArchiveDataHeader *archiveDataHeader;

  // dispatch here
  // process message body, if there is one
  switch (hdr->code)
  {
  case SEND_INTRINSICS:

      // store intrinsics in this proxy
      VERBOSE2(getVerboseLevel(),
	       "DxProxy:store intrinsics  ... " << endl;);
 
      ValidateMsgLength(hdr, hdr->dataLength == sizeof(DxIntrinsics));
      intrinsics = static_cast<DxIntrinsics *>(bodybuff);
      intrinsics->demarshall();
      internal_->sendIntrinsics(intrinsics);

      break;


  case SEND_DX_MESSAGE:

      ValidateMsgLength(hdr, hdr->dataLength == sizeof(NssMessage));
      nssMessage = static_cast<NssMessage *>(bodybuff);
      nssMessage->demarshall();
      internal_->sendDxMessage(nssMessage, hdr->activityId);
      break;

  case ARCHIVE_SIGNAL:
      ValidateMsgLength(hdr, hdr->dataLength == sizeof(ArchiveDataHeader));
      archiveDataHeader = static_cast<ArchiveDataHeader *>(bodybuff);
      archiveDataHeader->demarshall();
      internal_->signalArchiver_.archiveSignal(*archiveDataHeader);
      break;

  case BEGIN_SENDING_ARCHIVE_COMPLEX_AMPLITUDES:
      ValidateMsgLength(hdr, hdr->dataLength == sizeof(Count));
      count = static_cast<Count *>(bodybuff);
      count->demarshall();
      internal_->signalArchiver_.beginSendingArchiveComplexAmplitudes(*count);
      break;

  case SEND_ARCHIVE_COMPLEX_AMPLITUDES:
      ValidateMsgLength(hdr, hdr->dataLength > 0);
      internal_->sendArchiveComplexAmplitudes(bodybuff);
      break;

  case DONE_SENDING_ARCHIVE_COMPLEX_AMPLITUDES:
      internal_->signalArchiver_.doneSendingArchiveComplexAmplitudes();
      break;

  default: 
      //SseArchive::ErrorLog() 
      cerr << "DxProxy::handleDxMessage: " 
			     <<  "unexpected message code received:"
			     << hdr->code 
			     << " from dx " << getName() << endl;
      break;
  };  


}


// *** outgoing messages to physical dx *****
//--------------------------------------------

void DxProxy::requestIntrinsics()
{
    VERBOSE2(getVerboseLevel(), 
	      "DxProxy: sending requestIntrinsics msg to dx\n";);

    DxMessageCode code = REQUEST_INTRINSICS;
    sendMsgNoId(code);
}

// *** dxProxy private utility routines ****
//---------------------------------------------

void DxProxy::sendMsgNoId(int messageCode, int dataLength, const void *msgBody)
{
    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(messageCode, activityId, dataLength, msgBody);
}

// send a marshalled message to the DX
void DxProxy::sendMessage(int messageCode, int activityId, int dataLength,
			   const void *msgBody)
{
    VERBOSE2(getVerboseLevel(), 
	     "Send Msg to " << getDxId() <<  ": "
	     << SseDxMsg::messageCodeToString(messageCode) << "\n");

    NssProxy::sendMessage(messageCode, activityId, dataLength, msgBody);


}

// subclasses can override this as needed
void DxProxy::logError(const string &errorText)
{
    cerr << errorText << endl;
    //SseArchive::ErrorLog() << errorText << endl;
}


void DxProxy::setVerboseLevel(int level)
{
    NssProxy::setVerboseLevel(level);
    
    internal_->signalArchiver_.setVerboseLevel(level);
}