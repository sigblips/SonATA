/*******************************************************************************

 File:    IfcProxy.h
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


#ifndef ifc_proxy_h
#define ifc_proxy_h

#include "ace/Synch.h"
#include "SharedTclProxy.h"
#include "IfcIntrinsics.h"
#include "IfcStatus.h"
#include "ActivityId.h"
#include <string>

class ObserveActivity;

template<typename Tproxy> class NssComponentManager;

class IfcProxy : public SharedTclProxy
{
 public:

   IfcProxy(NssComponentManager<IfcProxy> *siteIfcManager);
   ~IfcProxy();

   IfcStatus getStatus() const;
   Polarization getStxPol() const;
   bool goodStxStatus();
   string getStxStatusString();
   IfcIntrinsics getIntrinsics() const;
   string getName();
   void setName(const string &name);

   // utilities
   void notifyInputConnected();
   void notifyInputDisconnected();

   void attachObserveActivity(ObserveActivity* act);
   void detachObserveActivity();

   // outgoing messages to Ifc

   void sendServerCommand(const string& command);
   void attn(int32_t attnDbLeft, int32_t attnDbRight);
   void off();

   void requestStatusUpdate();
   void requestIntrinsics();
   void requestReady();
   void reset();
   void selftest();
   void shutdown();

   void stxStart();
   void stxSetVariance(float64_t lcpVariance, float64_t rcpVariance,
		       float64_t tolerance, int32_t histogramLen);

   void ifSource(const string &source);

 protected:

   void logError(const string &errorText);
   virtual void processIndividualMessage(const string & message);

 private:

   // Disable copy construction & assignment.
   // Don't define these.
   IfcProxy(const IfcProxy& rhs);
   IfcProxy& operator=(const IfcProxy& rhs);

   string expectedInterfaceVersion();
   string receivedInterfaceVersion();

   void parseIntrinsics(const string& message);
   void parseStatus(const string& message);

   ActivityId_t getActivityId();
   void sendDisconnectErrorToActivity();
   void forwardErrorToActivity(NssMessage & nssMessage);
   void sendIfcReadyToActivity();
   void echoResponse(const string & message);

   IfcIntrinsics intrinsics_;
   IfcStatus status_;
   bool echoResponse_;
   NssComponentManager<IfcProxy>* siteIfcManager_;
   ObserveActivity* activity_;

   ACE_Recursive_Thread_Mutex activityPtrMutex_; 
   mutable ACE_Recursive_Thread_Mutex statusMutex_; 
   mutable ACE_Recursive_Thread_Mutex intrinsicsMutex_; 
   ACE_Recursive_Thread_Mutex echoResponseMutex_; 

};


#endif // ifc_proxy_h