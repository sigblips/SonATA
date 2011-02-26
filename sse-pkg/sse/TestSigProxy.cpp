/*******************************************************************************

 File:    TestSigProxy.cpp
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
#include "TestSigProxy.h"
#include "DebugLog.h" // keep this early in the headers for VERBOSE macros
#include "NssComponentManager.h"
#include "ObserveActivity.h"
#include "SseAstro.h"
#include "SseArchive.h"
#include "DebugLog.h"
#include "Assert.h"
#include "sseTestSigInterface.h"
#include "SseMsg.h"

#include <cmath> // Needed for Linux

using namespace std;

TestSigProxy::TestSigProxy(NssComponentManager<TestSigProxy> *siteTestSigManager)
   :
   echoResponse_(false),
   siteTestSigManager_(siteTestSigManager), 
   activity_(0)
{
   intrinsics_.name = "testsig-unknown";
}

TestSigProxy::~TestSigProxy()
{
   VERBOSE2(getVerboseLevel(), "TestSigProxy destructor for " << getName() 
	    << endl;);
}


string TestSigProxy::expectedInterfaceVersion()
{
   return SSE_TESTSIG_INTERFACE_VERSION;
}

string TestSigProxy::receivedInterfaceVersion()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return intrinsics_.interfaceVersion;
}

TestSigStatus TestSigProxy::getStatus() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   return status_;
}

TestSigIntrinsics TestSigProxy::getIntrinsics() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return intrinsics_;
}

string TestSigProxy::getName() 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   return intrinsics_.name;
}


void TestSigProxy::attachObserveActivity(ObserveActivity* act)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   activity_ = act;
}

void TestSigProxy::detachObserveActivity() 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   activity_ = 0;
}


void TestSigProxy::logError(const string& errorText)
{
   SseArchive::ErrorLog() << errorText << endl;
}

void TestSigProxy::requestIntrinsics() 
{
   send("tsig intrinsics\n");
}

void TestSigProxy::parseIntrinsics(const string& message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(intrinsicsMutex_);

   intrinsics_.name = parseElement(message, "name = ");
   intrinsics_.interfaceVersion = parseElement(message, "version = ");

}

void TestSigProxy::requestReady()
{
   send("tsig requestready\n");
}


void TestSigProxy::requestStatusUpdate()
{
   send("tsig status\n");
}

void TestSigProxy::parseStatus(const string& message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   status_.name = getName();
   status_.timeStamp = SseMsg::currentNssDate();

   status_.testSignal.driftTone.start.frequency =
      parseDouble(message, "Signal Start Frequency (MHz) = ");
   status_.testSignal.driftTone.start.freqUnits = Tone::MHz;

   status_.testSignal.driftTone.start.amplitude =
      parseDouble(message, "Signal Amplitude (dBm) = ");
   status_.testSignal.driftTone.start.ampUnits = Tone::dBm;

   string signalState = parseElement(message, "Signal Output = ");

   if (signalState == "1")
   {
      status_.testSignal.driftTone.start.output = true;
   }
   else if (signalState == "0")
   {
      status_.testSignal.driftTone.start.output = false;
   }
   else
   {
      throw ("unexpected drift tone output state");
   }

   string sweepState = parseElement(message, "Signal Sweep State = ");
   if (sweepState == "1")
   {
      status_.testSignal.driftTone.sweepState = true;
   }
   else if (sweepState == "0")
   {
      status_.testSignal.driftTone.sweepState = false;
   }
   else
   {
      throw ("unexpected sweep state");
   }
   
   status_.testSignal.driftTone.duration =
      parseDouble(message, "Signal Sweep Time (sec) = ");
   
   double stopFreq = parseDouble(message, "Signal Stop Frequency (MHz) = ");

   status_.testSignal.driftTone.driftRate = 0.0;
   if (status_.testSignal.driftTone.duration > 0.0)
   {
      status_.testSignal.driftTone.driftRate = 
	 fabs(stopFreq - status_.testSignal.driftTone.start.frequency) * 
	 SseAstro::HzPerMhz / status_.testSignal.driftTone.duration;
   }

   status_.testSignal.pulse.amplitude =
      parseDouble(message, "Pulse Amplitude (dBm) = ");

   status_.testSignal.pulse.period =
      parseDouble(message, "Pulse Period (sec) = ");

   status_.testSignal.pulse.duration =
      parseDouble(message, "Pulse Width (sec) = ");

   string pulseOutputState = parseElement(message, "Pulse Output = ");
   status_.testSignal.pulse.output = false;
   if (pulseOutputState == "1")
   {
      status_.testSignal.pulse.output = true;
   }
   else if (pulseOutputState == "0")
   {
      status_.testSignal.pulse.output = false;
   }
   else
   {
      throw("unexpected pulse output state");
   }

}


string TestSigProxy::getTestSignalType()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(statusMutex_);

   string sigtype("none");
   if (status_.testSignal.driftTone.start.output)
   {
      sigtype = "CW";

      // if pulse gen is also on, then it's a pulse signal
      if (status_.testSignal.pulse.output)
      {
	 sigtype = "Pulse";
      }
   }

   return sigtype;

}


void TestSigProxy::sendServerCommand(const string& command)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(echoResponseMutex_);

   echoResponse_ = true;

   stringstream msg;
   msg << command << endl;
   send(msg.str());
}

void TestSigProxy::shutdown()
{
   send("exit\n");
}

void TestSigProxy::off()
{
   send("tsig off\n");
}

void TestSigProxy::quiet()
{
   send("tsig quiet\n");
}

void TestSigProxy::reset()
{
   send("reset\n");
}

void TestSigProxy::selftest()
{
   send("selftest\n");
}

void TestSigProxy::cwTest(DriftingTone tone)
{
   VERBOSE1(getVerboseLevel(), "CW test:" << endl;);
   VERBOSE2(getVerboseLevel(), tone);

   // ASSUMES: amplitude in dBm, frequencies in MHz, drift rate Hz/sec,
   // ASSUMES: duration in sec

   stringstream msg;
   msg.precision(9);
   msg.setf(std::ios::fixed);

   msg << "tsig tunesiggen " << tone.start.frequency
       << " " << tone.start.amplitude
       << " " << tone.driftRate
       << " " << tone.duration
       << ";"
       << endl;


   send(msg.str());

}

void TestSigProxy::pulseTest(PulsedTone pulse)
{
   VERBOSE1(getVerboseLevel(), "Pulse test:" << endl;);
   VERBOSE2(getVerboseLevel(), pulse);
  
   stringstream msg;
   msg.precision(9);
   msg.setf(std::ios::fixed);

   msg << "tsig tunesiggen " << pulse.driftTone.start.frequency
       << " " << pulse.driftTone.start.amplitude
       << " " << pulse.driftTone.driftRate
       << " " << pulse.driftTone.duration 
       << ";"
       << endl;

   msg << "tsig pulse " << pulse.pulse.amplitude
       << " " << pulse.pulse.period
       << " " << pulse.pulse.duration
       << "; "
       << endl;

   send(msg.str());

}

void TestSigProxy::sigGenOn() 
{
   send("tsig on\n");
}

void TestSigProxy::sigGenOff()
{
   send("tsig off\n");
}

//-----------------------------
// ----- utility routines -----
//-----------------------------

void TestSigProxy::notifyInputConnected()
{
   // Register this device with the site.
   // Need to do this here instead of in the Proxy constructor
   // because we don't want to register the proxy until its
   // socket connection with the real device is established.

   siteTestSigManager_->registerProxy(this);
}


void TestSigProxy::sendDisconnectErrorToActivity()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   // notify the activity that we're gone
   if (activity_)
   {
      NssMessage nssMessage;
      nssMessage.code = 0; // TBD
      nssMessage.severity = SEVERITY_ERROR;
      SseUtil::strMaxCpy(nssMessage.description, 
			 "tsig (test sig gen) disconnected\n",
			 MAX_NSS_MESSAGE_STRING);

      activity_->testSigError(this, nssMessage);
   }
}

void TestSigProxy::notifyInputDisconnected()
{
   sendDisconnectErrorToActivity();

   siteTestSigManager_->unregisterProxy(this);
}


void TestSigProxy::forwardErrorToActivity(NssMessage & nssMessage)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   if (activity_)
   {
      activity_->testSigError(this, nssMessage);
   }
}

void TestSigProxy::sendTestSigReadyToActivity()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(activityPtrMutex_);

   if (activity_) 
   {
      activity_->testSigReady(this);
   }
}

void TestSigProxy::echoResponse(const string & message)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(echoResponseMutex_);

   if (echoResponse_)
   {
      cout << endl << getName() << ": " << endl << message << endl;
      echoResponse_ = false;
   }
}

void TestSigProxy::processIndividualMessage(const string & message)
{
   VERBOSE2(getVerboseLevel(),
	    "TestSigProxy::processIndividualMessage(): \n"
	    << getName() << ": "<< message << endl);

   try {

      echoResponse(message);

      if (message.find(SSE_TESTSIG_ERROR_MSG_HEADER) != string::npos)
      {
	 NssMessage nssMessage;
	 nssMessage.code = 0; // TBD
	 nssMessage.severity = SEVERITY_ERROR;
	 SseUtil::strMaxCpy(nssMessage.description, message.c_str(),
			    MAX_NSS_MESSAGE_STRING);

	 forwardErrorToActivity(nssMessage);

	 siteTestSigManager_->processNssMessage(this, nssMessage);

      }

      // ----- intrinsics -----

      if (message.find(SSE_TESTSIG_INTRIN_MSG_HEADER) != string::npos)
      {
	 parseIntrinsics(message);
	 siteTestSigManager_->receiveIntrinsics(this);
      }

      // ----- status -----
      if (message.find(SSE_TESTSIG_STATUS_MSG_HEADER) != string::npos)
      {
	 parseStatus(message);
	 siteTestSigManager_->notifyStatusChanged(this);
      }

      // ----- test signal ready -----
      if (message.find(SSE_TESTSIG_READY_MSG)
          != string::npos)
      {
         sendTestSigReadyToActivity();
      }


   }
   catch (const SseException &except)  
   {
      SseArchive::ErrorLog() << 
	 "caught TestSigProxy::handleIncomingMessage exception: " 
			     << except << endl;
   }
   catch (const string &error)  
   {
      SseArchive::ErrorLog() << 
	 "caught TestSigProxy::handleIncomingMessage error: " << error << endl;
   }
   catch (...)
   {
      SseArchive::ErrorLog() << 
	 "caught TestSigProxy::handleIncomingMessage unexpected error" << endl;
   }



}