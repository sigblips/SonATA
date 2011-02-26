/*******************************************************************************

 File:    IfcProxy.cpp
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
#include "IfcProxy.h"
#include "NssComponentManager.h"
#include "ObserveActivity.h"
#include "sseIfcInterface.h"
#include "SseMsg.h"

using namespace std;

IfcProxy::IfcProxy(NssComponentManager<IfcProxy>* siteIfcManager)
   :
   echoResponse_(false),
   siteIfcManager_(siteIfcManager), 
   activity_(0) 
{ 
   intrinsics_.name = "ifc-unknown";
}

IfcProxy::~IfcProxy()
{
   VERBOSE2(getVerboseLevel(), "IfcProxy destructor for " << getName() 
	    << endl;);
}


string IfcProxy::expectedInterfaceVersion()
{
   return SSE_IFC_INTERFACE_VERSION;
} 

string IfcProxy::receivedInterfaceVersion()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return intrinsics_.interfaceVersion;
}


void IfcProxy::setName(const string & name)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   intrinsics_.name = name;
}

IfcStatus IfcProxy::getStatus() const 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   return status_;
}

Polarization IfcProxy::getStxPol() const 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   return SseMsg::stringToPolarization(status_.stxPol);
}



IfcIntrinsics IfcProxy::getIntrinsics() const 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return intrinsics_;
}

string IfcProxy::getName() 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return intrinsics_.name;
}


bool IfcProxy::goodStxStatus()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   return status_.goodStxStatus;
}

// convert stx status to a hex string
string IfcProxy::getStxStatusString()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   stringstream strm;

   strm << "0x";
   strm.fill('0');
   strm.width(3);
   strm << hex << status_.stxStatus << dec;

   return strm.str();
}


void IfcProxy::attachObserveActivity(ObserveActivity* act)
{ 
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);
 
   activity_ = act;
}

void IfcProxy::detachObserveActivity()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   activity_ = 0;
}

ActivityId_t IfcProxy::getActivityId()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   ActivityId_t actId = NSS_NO_ACTIVITY_ID;
   if (activity_)
   {
      actId = activity_->getId();
   }

   return actId;
}

void IfcProxy::logError(const string& errorText)
{
   stringstream strm;
   strm  << errorText ;

   SseMessage::log(getName(),
                   getActivityId(), SSE_MSG_IFC_PROXY_ERROR,
                   SEVERITY_ERROR, strm.str(),
                   __FILE__, __LINE__);
}

void IfcProxy::requestIntrinsics()
{
   send("ifc intrinsics\n");
}

void IfcProxy::requestReady()
{
   send("ifc requestready\n");
}

void IfcProxy::parseIntrinsics(const string& message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   intrinsics_.name = parseElement(message, "name = ");
   intrinsics_.interfaceVersion = parseElement(message, "version = ");
}

void IfcProxy::requestStatusUpdate()
{
   send("ifc status\n");
}

void IfcProxy::parseStatus(const string& message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   status_.name = getName();
   status_.timeStamp = SseMsg::currentNssDate();

   status_.attnDbLeft = parseInt(message, "Attn. Left (dB) = ");
   status_.attnDbRight = parseInt(message, "Attn. Right (dB) = ");

   // IF source (test or sky)
   status_.ifSource = parseElement(message, "IF Source = ");

   status_.stxPol = parseElement(message, "STX Pol = ");
   status_.stxStatus = parseInt(message, "Status Register = ");
   status_.goodStxStatus = parseInt(message, "GoodStatus = ");

   status_.stxCountLeft = parseInt(message, "LCP count = ");
   status_.stxMeanLeft = parseDouble(message, "LCP mean = ");
   status_.stxVarLeft = parseDouble(message, "LCP variance = ");

   status_.stxCountRight = parseInt(message, "RCP count = ");
   status_.stxMeanRight = parseDouble(message, "RCP mean = ");
   status_.stxVarRight = parseDouble(message, "RCP variance = ");

}

void IfcProxy::sendServerCommand(const string& command)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(echoResponseMutex_);

   echoResponse_ = true;

   stringstream msg;
   msg << command << endl;

   send(msg.str());
}

void IfcProxy::shutdown()
{
   send("exit\n");
}

void IfcProxy::off()
{
   send("ifc off\n");
}

void IfcProxy::reset()
{
   send("reset\n");
}

void IfcProxy::selftest()
{
   send("selftest\n");
}

void IfcProxy::attn(int32_t attnDbLeft, int32_t attnDbRight)
{
   VERBOSE1(getVerboseLevel(), getName() << " IFC Proxy attn(" 
	    << attnDbLeft << ", " << attnDbRight  << ")" << endl;);

   stringstream msg;
   msg << "ifc attn " << attnDbLeft << " " << attnDbRight << endl;
   send(msg.str());
}

void IfcProxy::stxStart() 
{
   send("ifc stxstart\n");
}

void IfcProxy::stxSetVariance(float64_t lcpVariance, float64_t rcpVariance,
			      float64_t tolerance, int32_t histogramLen)
{
   VERBOSE1(getVerboseLevel(), getName() << " Set STX variance:" << endl;);

   stringstream msg;
   msg.precision(9);
   msg.setf(std::ios::fixed);

   msg << "ifc sethistlen " << histogramLen << endl
       << "ifc settol " << tolerance    << endl
       << "ifc stxvariance " << lcpVariance << " " << rcpVariance << endl
       << endl;

   send(msg.str());

}

void IfcProxy::ifSource(const string &source)
{
   VERBOSE1(getVerboseLevel(), getName() << "ifSource" << endl;);

   Assert(source == "test" || source == "sky");

   stringstream msg;
   msg << "ifc ifsource " << source << endl;

   send(msg.str());

}




//-----------------------------
// ----- utility routines -----
//-----------------------------

void IfcProxy::notifyInputConnected()
{
   // Register this device with the site.
   // Need to do this here instead of in the Proxy constructor
   // because we don't want to register the proxy until its
   // socket connection with the real device is established.

   siteIfcManager_->registerProxy(this);
}

void IfcProxy::sendDisconnectErrorToActivity()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   // notify the activity that we're gone
   if (activity_)
   {
      NssMessage nssMessage;
      nssMessage.code = 0; // TBD
      nssMessage.severity = SEVERITY_ERROR;
      SseUtil::strMaxCpy(nssMessage.description,"ifc disconnected\n",
			 MAX_NSS_MESSAGE_STRING);

      activity_->ifcError(this, nssMessage);
   }
}

void IfcProxy::notifyInputDisconnected()
{
   sendDisconnectErrorToActivity();

   siteIfcManager_->unregisterProxy(this);
}


void IfcProxy::forwardErrorToActivity(NssMessage & nssMessage)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   if (activity_)
   {
      activity_->ifcError(this, nssMessage);
   }
}

void IfcProxy::sendIfcReadyToActivity()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   if (activity_)
   {
      activity_->ifcReady(this);
   }
}

void IfcProxy::echoResponse(const string & message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(echoResponseMutex_);

   if (echoResponse_)
   {
      cout << endl << getName() << ": " << endl << message << endl;
      echoResponse_ = false;
   }
}

void IfcProxy::processIndividualMessage(const string & message)
{
   VERBOSE2(getVerboseLevel(),
	    "IfcProxy::processIndividualMessage(): \n"
	    << getName() << ": "<< message << endl);
   try
   {
      echoResponse(message);

      if (message.find(SSE_IFC_ERROR_MSG_HEADER) != string::npos)
      {
	 NssMessage nssMessage;
	 nssMessage.code = 0; // TBD
	 nssMessage.severity = SEVERITY_ERROR;
	 SseUtil::strMaxCpy(nssMessage.description, message.c_str(),
			    MAX_NSS_MESSAGE_STRING);

	 forwardErrorToActivity(nssMessage);

	 siteIfcManager_->processNssMessage(this, nssMessage);

      }

      // ----- intrinsics -----
      if (message.find(SSE_IFC_INTRIN_MSG_HEADER) != string::npos)
      {
	 parseIntrinsics(message);

	 siteIfcManager_->receiveIntrinsics(this);
      }

      // ----- status -----
      if (message.find(SSE_IFC_STATUS_MSG_HEADER) != string::npos)
      {
	 parseStatus(message);
	 siteIfcManager_->notifyStatusChanged(this);
      }

      // ifc ready
      if (message.find(SSE_IFC_READY_MSG) != string::npos)
      {
	 sendIfcReadyToActivity();
      }

   }
   catch (const SseException &except)  
   {
      stringstream strm;
      strm <<  "IfcProxy::handleIncomingMessage parse error: " 
	   << except.descrip() << endl;
      SseMessage::log(getName(), getActivityId(), 
                      except.code(), except.severity(),
                      strm.str(),
                      except.sourceFilename(), except.lineNumber());
   }
   catch (const string &error)  
   {
      stringstream strm;
      strm <<  "IfcProxy::handleIncomingMessage parse error: " << error << endl;
      SseMessage::log(getName(),
                      getActivityId(), SSE_MSG_IFC_PROXY_ERROR,
                      SEVERITY_ERROR, strm.str(),
                      __FILE__, __LINE__);
   }
   catch (...)
   {
      stringstream strm;
      strm <<  "IfcProxy::handleIncomingMessage unexpected parse error" << endl;
      SseMessage::log(getName(),
                      getActivityId(), SSE_MSG_IFC_PROXY_ERROR,
                      SEVERITY_ERROR, strm.str(),
                      __FILE__, __LINE__);
   }

   // TBD case IFC_STX_SET_VARIANCE_COMPLETE:

}
