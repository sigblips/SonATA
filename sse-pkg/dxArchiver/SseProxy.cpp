/*******************************************************************************

 File:    SseProxy.cpp
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
#include "SseProxy.h"
#include <iostream>
#include "SseDxMsg.h"
#include "DxArchiver.h"
#include "HostNetByteorder.h"
#include "Assert.h"
#include "Verbose.h"

using namespace std;

//#define TEST_SEND_BAD_MESSAGE_SIZE 1


SseProxy::SseProxy(ACE_SOCK_STREAM &stream)
    : NssProxy(stream),
      dxArchiver_(0)
{
    //NssProxy::setVerboseLevel(2);   // 2 = maximum verbose output
}

SseProxy::~SseProxy()
{
}

void SseProxy::setDxArchiver(DxArchiver *dxArchiver)
{
    dxArchiver_ = dxArchiver;    
}

string SseProxy::getName()
{
    return "sse";
}


// outgoing messages to sse

void SseProxy::sendNssMessage(const NssMessage &nssMessage, int activityId)
{
    VERBOSE2(getVerboseLevel(), "SseProxy: sendNssMessage\n");

    DxArchiverMessageCode code = SEND_NSS_MESSAGE;
    int dataLength = sizeof(nssMessage);

    NssMessage marshall = nssMessage;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void SseProxy::sendIntrinsics(const DxArchiverIntrinsics &intrinsics)
{
    VERBOSE2(getVerboseLevel(), "SseProxy: sendIntrinsics" << endl);

    DxArchiverMessageCode code = SEND_DX_ARCHIVER_INTRINSICS;
    int dataLength = sizeof(intrinsics);

    // marshall a copy of the intrinsics
    DxArchiverIntrinsics marshall = intrinsics;
    marshall.marshall();
    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(code, activityId, dataLength, &marshall);

}


void SseProxy::sendArchiverStatus(const DxArchiverStatus &status)
{
    VERBOSE2(getVerboseLevel(), "SseProxy: sendArchiverStatus" << endl);

    DxArchiverMessageCode code = SEND_DX_ARCHIVER_STATUS;
    int dataLength = sizeof(status);

    // marshall a copy of the status
    DxArchiverStatus marshall = status;
    marshall.marshall();

    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(code, activityId, dataLength, &marshall);

}


//--------------------------------------------------
// --- utilities -----
// send a marshalled message to the SSE
void SseProxy::sendMessage(int messageCode, int activityId, 
                         int dataLength, const void *msgBody)
{
    VERBOSE2(getVerboseLevel(), 
	     "Send: " << " code: " << messageCode << endl);

    NssProxy::sendMessage(messageCode, activityId, dataLength, msgBody);

}

// dispatch incoming sse message to appropriate
// dx method
void SseProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{

  VERBOSE2(getVerboseLevel(),
	   "Recv Msg from SseProxy:"
	   << " code: " << hdr->code << "\n" << *hdr);

  // TBD translate code to string

  // dispatch to archiver
  // process message body, if there is one
  switch (hdr->code)
  {
  case REQUEST_DX_ARCHIVER_INTRINSICS:
      dxArchiver_->requestArchiverIntrinsics();
      break;

  case REQUEST_DX_ARCHIVER_STATUS:
      dxArchiver_->requestArchiverStatus();
      break;

  case SHUTDOWN_DX_ARCHIVER:
      dxArchiver_->shutdownArchiver();
      break;

  default: 

      cerr << "SseProxy::handleSseMsg: "
	   << "unexpected message code received:" << hdr->code << endl;

      break;
  };  


}
