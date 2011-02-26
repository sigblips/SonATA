/*******************************************************************************

 File:    TestSigProxy.h
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


#ifndef _tsig_proxy_h
#define _tsig_proxy_h

#include "ace/Synch.h"
#include "SharedTclProxy.h"
#include "TestSigStatus.h"
#include "TestSigIntrinsics.h"
#include <string>

class ObserveActivity;

template<typename Tproxy> class NssComponentManager;

class TestSigProxy : public SharedTclProxy
{

 public:

   TestSigProxy(NssComponentManager<TestSigProxy> *siteTestSigManager);
   ~TestSigProxy();

   TestSigStatus getStatus() const;
   TestSigIntrinsics getIntrinsics() const;
   string getName();

   string getTestSignalType();

   // utilities
   void notifyInputConnected();
   void notifyInputDisconnected();

   void attachObserveActivity(ObserveActivity* act);
   void detachObserveActivity();

   // outgoing messages to TestSig

   void sendServerCommand(const string& command);

   void off();
   void quiet();
   void requestStatusUpdate();
   void requestIntrinsics();
   void requestReady();
   void reset();
   void selftest();

   void cwTest(DriftingTone tone);
   void pulseTest(PulsedTone pulse);
   void sigGenOn();
   void sigGenOff();

   void shutdown();

 protected:

   void logError(const string &errorText);
   virtual void processIndividualMessage(const string & message);

 private:

   // Disable copy construction & assignment.
   // Don't define these.
   TestSigProxy(const TestSigProxy& rhs);
   TestSigProxy& operator=(const TestSigProxy& rhs);

   string expectedInterfaceVersion();
   string receivedInterfaceVersion();

   void parseIntrinsics(const string& message);
   void parseStatus(const string& message);

   void sendDisconnectErrorToActivity();
   void forwardErrorToActivity(NssMessage & nssMessage);
   void sendTestSigReadyToActivity();
   void echoResponse(const string & message);


   TestSigIntrinsics intrinsics_;
   TestSigStatus status_;
   bool echoResponse_;
   NssComponentManager<TestSigProxy>* siteTestSigManager_;
   ObserveActivity* activity_;

   ACE_Recursive_Thread_Mutex activityPtrMutex_; 
   mutable ACE_Recursive_Thread_Mutex statusMutex_; 
   mutable ACE_Recursive_Thread_Mutex intrinsicsMutex_; 
   ACE_Recursive_Thread_Mutex echoResponseMutex_; 



};



#endif