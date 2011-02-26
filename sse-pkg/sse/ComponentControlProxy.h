/*******************************************************************************

 File:    ComponentControlProxy.h
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


#ifndef _component_control_proxy_h
#define _component_control_proxy_h

#include "ace/Synch.h"
#include "SharedTclProxy.h"
#include "sseInterface.h"
#include <string>
template<typename Tproxy> class NssComponentManager;

struct ComponentControlStatus
{
   string name;
   NssDate timeStamp;
   string text;

   friend ostream& operator << (ostream& strm, const ComponentControlStatus& status);

};

class ComponentControlProxy : public SharedTclProxy
{

 public:

   ComponentControlProxy(NssComponentManager<ComponentControlProxy> *siteComponentControlManager);
   ~ComponentControlProxy();

   ComponentControlStatus getStatus() const;
   string getIntrinsics() const;
   string getName();

   // utilities
   void notifyInputConnected();
   void notifyInputDisconnected();

   // outgoing messages to ComponentControl

   void sendCommand(const string& command);
   void requestStatusUpdate();
   void requestIntrinsics();
   void reset();
   void shutdown();

 protected:

   void logError(const string &errorText);
   virtual void processIndividualMessage(const string & message);

 private:

   // Disable copy construction & assignment.
   // Don't define these.
   ComponentControlProxy(const ComponentControlProxy& rhs);
   ComponentControlProxy& operator=(const ComponentControlProxy& rhs);

   string expectedInterfaceVersion();
   string receivedInterfaceVersion();

   void parseIntrinsics(const string& message);
   void parseStatus(const string& message);

   // intrinsics:
   string name_;
   string interfaceVersion_;

   ComponentControlStatus status_;
   NssComponentManager<ComponentControlProxy>* siteComponentControlManager_;

   mutable ACE_Recursive_Thread_Mutex statusMutex_; 
   mutable ACE_Recursive_Thread_Mutex intrinsicsMutex_; 

};



#endif