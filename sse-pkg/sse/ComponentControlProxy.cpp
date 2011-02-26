/*******************************************************************************

 File:    ComponentControlProxy.cpp
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
#include "ComponentControlProxy.h"
#include "NssComponentManager.h"
#include "Assert.h"
#include "SseArchive.h"
#include "SseMsg.h"
#include <iostream>
#include <sstream>

using namespace std;

const char *SSE_COMPONENT_CONTROL_INTERFACE_VERSION = "1.0";
const char *SSE_COMPONENT_CONTROL_INTRIN_MSG_HEADER = "Intrinsics:";
const char *INTRINSICS_NAME_KEYWORD = "Name: ";
const char *INTRINSICS_VERSION_KEYWORD = "InterfaceVersion: ";
const char *SSE_COMPONENT_CONTROL_STATUS_MSG_HEADER = "Status:";
const char *SSE_COMPONENT_CONTROL_ERROR_MSG_HEADER = "Error:";

ostream& operator << (ostream& strm, const ComponentControlStatus& status)
{
    strm << "Control status:\n"
	 << "  Name: " << status.name << endl
	 << "  Time: " << status.timeStamp << endl
	 << "  Text: " << status.text << endl
	 << endl;

    return strm;
}

ComponentControlProxy::ComponentControlProxy(NssComponentManager<ComponentControlProxy> *siteComponentControlManager)
   :
   siteComponentControlManager_(siteComponentControlManager) 
{
   name_ = "componentControl-unknown";
}

ComponentControlProxy::~ComponentControlProxy()
{
   VERBOSE2(getVerboseLevel(), "ComponentControlProxy destructor for " << getName() 
	    << endl;);
}


string ComponentControlProxy::expectedInterfaceVersion()
{
   return SSE_COMPONENT_CONTROL_INTERFACE_VERSION;
}

string ComponentControlProxy::receivedInterfaceVersion()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return interfaceVersion_;
}


ComponentControlStatus ComponentControlProxy::getStatus() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   return status_;
}

string ComponentControlProxy::getIntrinsics() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   stringstream strm;

   strm << "ComponentControl Intrinsics: " << endl
	<< "----------------------------" << endl
	<< INTRINSICS_VERSION_KEYWORD << interfaceVersion_ << endl
	<< INTRINSICS_NAME_KEYWORD << name_ << endl;

   return strm.str();
}



string ComponentControlProxy::getName() 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return name_;
}

void ComponentControlProxy::logError(const string& errorText)
{
   SseArchive::ErrorLog() << errorText << endl;
}

void ComponentControlProxy::requestIntrinsics() 
{
   send("intrinsics\n");
}

void ComponentControlProxy::parseIntrinsics(const string& message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   name_ = parseElement(message, INTRINSICS_NAME_KEYWORD);
   interfaceVersion_ = parseElement(message, INTRINSICS_VERSION_KEYWORD);

}

void ComponentControlProxy::requestStatusUpdate()
{
   VERBOSE1(getVerboseLevel(), "requestStatusUpdate:" << endl;);

   send("status\n");
}

void ComponentControlProxy::parseStatus(const string& message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   status_.name = getName();
   status_.timeStamp = SseMsg::currentNssDate();
   status_.text = message;
}



void ComponentControlProxy::sendCommand(const string& command)
{
   stringstream msg;
   msg << command << endl;
   send(msg.str());
}

void ComponentControlProxy::shutdown()
{
   send("exit\n");
}

void ComponentControlProxy::reset()
{
   send("reset\n");
}



//-----------------------------
// ----- utility routines -----
//-----------------------------

void ComponentControlProxy::notifyInputConnected()
{
   // Register this device with the site.
   // Need to do this here instead of in the Proxy constructor
   // because we don't want to register the proxy until its
   // socket connection with the real device is established.

   siteComponentControlManager_->registerProxy(this);
}


void ComponentControlProxy::notifyInputDisconnected()
{
   siteComponentControlManager_->unregisterProxy(this);
}


void ComponentControlProxy::processIndividualMessage(const string & message)
{
   VERBOSE2(getVerboseLevel(),
	    "ComponentControlProxy::processIndividualMessage(): \n"
	    << getName() << ": "<< message << endl);

   try {

      // --- error ---
      if (message.find(SSE_COMPONENT_CONTROL_ERROR_MSG_HEADER)
	  != string::npos)
      {
	 NssMessage nssMessage;
	 nssMessage.code = 0; // TBD
	 nssMessage.severity = SEVERITY_ERROR;
	 SseUtil::strMaxCpy(nssMessage.description, message.c_str(),
			    MAX_NSS_MESSAGE_STRING);

	 siteComponentControlManager_->processNssMessage(this, nssMessage);

	 return;
      }


      // ----- intrinsics -----
      if (message.find(SSE_COMPONENT_CONTROL_INTRIN_MSG_HEADER) != string::npos)
      {
	 parseIntrinsics(message);
	 siteComponentControlManager_->receiveIntrinsics(this);

	 return;
      }


      // ----- status -----
      if (message.find(SSE_COMPONENT_CONTROL_STATUS_MSG_HEADER) != string::npos)
      {
	 parseStatus(message);
	 siteComponentControlManager_->notifyStatusChanged(this);

	 return;
      }

      // TBD trap unexpected message types.
      // For now just log it.

      SseArchive::SystemLog() << getName() << ": " << message;
      SseArchive::ErrorLog() << getName() << ": " << message;

   }
   catch (const SseException &except)  
   {
      SseArchive::ErrorLog() << 
	 "caught ComponentControlProxy::handleIncomingMessage exception: " 
			     << except << endl;
   }
   catch (const string &error)  
   {
      SseArchive::ErrorLog() << 
	 "caught ComponentControlProxy::handleIncomingMessage error: " << error << endl;
   }
   catch (...)
   {
      SseArchive::ErrorLog() << 
	 "caught ComponentControlProxy::handleIncomingMessage unexpected error" << endl;
   }



}