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

/*
  SseProxy.cpp
  Handle communications with the SSE.
*/

#include <ace/OS.h>
#include "SseProxy.h"
#include "Channelizer.h"
#include "Assert.h"
#include "Verbose.h"
#include <iostream>

using namespace std;

SseProxy::SseProxy(ACE_SOCK_STREAM &stream)
   : NssProxy(stream),
     chanzer_(0)
{
   //NssProxy::setVerboseLevel(2);   // 2 = maximum verbose output
}

SseProxy::~SseProxy()
{
}

void SseProxy::setChannelizer(Channelizer *chanzer)
{
   chanzer_ = chanzer;    
}

string SseProxy::getName()
{
   return "sse";
}


// outgoing messages to sse

void SseProxy::sendNssMessage(const NssMessage &nssMessage, int activityId)
{
   VERBOSE2(getVerboseLevel(), "SseProxy: sendNssMessage\n");

   ChannelizerMessageCode code = SEND_MESSAGE;
   int dataLength = sizeof(nssMessage);

   NssMessage marshall = nssMessage;
   marshall.marshall();
   sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendIntrinsics(const Intrinsics &intrinsics)
{
   VERBOSE2(getVerboseLevel(), "SseProxy: sendIntrinsics" << endl);

   ChannelizerMessageCode code = SEND_INTRINSICS;
   int dataLength = sizeof(intrinsics);

   // marshall a copy of the intrinsics
   Intrinsics marshall = intrinsics;
   marshall.marshall();
   int activityId = NSS_NO_ACTIVITY_ID;
   sendMessage(code, activityId, dataLength, &marshall);
}


void SseProxy::sendStatus(const Status &status)
{
   VERBOSE2(getVerboseLevel(), "SseProxy: sendStatus" << endl);

   ChannelizerMessageCode code = SEND_STATUS;
   int dataLength = sizeof(status);

   // marshall a copy of the status
   Status marshall = status;
   marshall.marshall();

   int activityId = NSS_NO_ACTIVITY_ID;
   sendMessage(code, activityId, dataLength, &marshall);
}

void SseProxy::sendStarted(const Started &started)
{
   VERBOSE2(getVerboseLevel(), "SseProxy: sendStarted" << endl);

   ChannelizerMessageCode code = STARTED;
   int dataLength = sizeof(started);

   // marshall a copy of the status
   Started marshall = started;
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

/*
  Dispatch incoming sse message to appropriate channelizer method.
*/
void SseProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{
   VERBOSE2(getVerboseLevel(),
            "Recv Msg from SseProxy:"
            << " code: " << hdr->code << "\n" << *hdr);

   // TBD translate code to string

   /*
     Dispatch to channelizer.
     Process message body, if there is one.
   */
   Start *startBody;

   switch (hdr->code)
   {
   case REQUEST_INTRINSICS:
      chanzer_->requestIntrinsics();
      break;

   case REQUEST_STATUS:
      chanzer_->requestStatus();
      break;

   case START:
      Assert(hdr->dataLength == sizeof(Start));
      startBody = static_cast<Start *>(bodybuff);
      startBody->demarshall();
      chanzer_->start(startBody);
      break;

   case STOP:
      chanzer_->stop();
      break;

   case SHUTDOWN:
      chanzer_->shutdown();
      break;

   default: 

      cerr << "SseProxy::handleSseMsg: "
	   << "unexpected message code received:" << hdr->code << endl;

      break;
   };  

}
