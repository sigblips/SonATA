/*******************************************************************************

 File:    TestSig.cpp
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

// Test Signal Generator Control

#include <TestSig.h>
#include <iostream>
#include <sstream>
#include <SseUtil.h>
#include "Assert.h"
#include "sseTestSigInterface.h"

using namespace std;

static const double HzPerMhz = 1.0e6;
static const char *At33250Name = "at33250";
static const char *At4400Name = "at4400";
static const char *SigGenInternalName = "internal";

TestSig::TestSig(): 
   name_("no name"),
   version_(SSE_TESTSIG_INTERFACE_VERSION), 
   simulated_(false),
   usingPulseGen_(false),
   siggen_(0),
   pulsegen_(0)
{
   // default devices
   setSigGenSource(At33250Name);
   setPulseGenSource(At33250Name);
}

TestSig::~TestSig()
{
}

// ===== accessors for tcl =====

const char* TestSig::name() const  
{
   return(name_.c_str());
}

const char* TestSig::name(const string& newName)
{
   name_ = newName; 
   return(name());
}

const char* TestSig::version() const
{
   return(version_.c_str());
}


bool TestSig::isSimulated() 
{
   return(simulated_);
}

bool TestSig::setSimulated(bool mode)
{
   simulated_ = mode;
   return(isSimulated());
}


// ===== GPIB configuration =====

double TestSig::setsiglimit(double limit)
{
   getSigGen()->safetyAmp(limit);
   return(getsiglimit());
}

double TestSig::getsiglimit()
{
   return(getSigGen()->safetyAmp());
}

double TestSig::setpulselimit(double limit)
{
   getPulseGen()->safetyAmp(limit);
   return(getpulselimit());
}

double TestSig::getpulselimit() 
{
   return(getPulseGen()->safetyAmp());
}


// Non-tcl accessors

SignalGenerator* TestSig::getSigGen() 
{
   Assert(siggen_);
   return(siggen_);
}

SignalGenerator* TestSig::getPulseGen() 
{
   Assert(pulsegen_);
   return(pulsegen_);
}

// ===== private methods =====

const char* TestSig::sendToSocket(const string& message)
{
   tclBuffer_ = message;
   return(tclBuffer_.c_str());
}

// ===== public methods =====

const char* TestSig::setSigGenSource(const char * sigGenSourceName) 
{
   if (SseUtil::strCaseEqual(sigGenSourceName, At4400Name))
   {
      sigGenSource_ = SigGenAt4400;
      delete siggen_;
      siggen_ = new AT4400;
      getSigGen()->simulated(isSimulated());
   }
   else if (SseUtil::strCaseEqual(sigGenSourceName, At33250Name))
   {
      sigGenSource_ = SigGenAt33250;
      delete siggen_;
      siggen_ = new AT33250;
      getSigGen()->simulated(isSimulated());
   }
   else 
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER 
	     << " TestSig::setSigGenSource() unsupported sig gen source: "
	     << sigGenSourceName << endl
	     << "must be one of: " << At4400Name << " " << At33250Name << endl;
      return(sendToSocket(errmsg.str()));
   }
   siggen_->function("Tone Generator");

   return(getSigGenSourceName());
}

const char* TestSig::getSigGenSourceName()
{
   if (sigGenSource_ == SigGenAt4400)
   {
      return(sendToSocket(At4400Name));
   }
   else if (sigGenSource_ == SigGenAt33250)
   {
      return(sendToSocket(At33250Name));
   }
  
   AssertMsg(0, "unknown sig gen source");

}

const char* TestSig::setPulseGenSource(const char* pulseGenSourceName) 
{
   if (SseUtil::strCaseEqual(pulseGenSourceName, At33250Name))
   {
      pulseGenSource_ = PulseGenAt33250;
      delete pulsegen_;
      pulsegen_ = new AT33250;
      pulsegen_->simulated(isSimulated());
      pulsegen_->function("Pulse Generator");
   }
   else if (SseUtil::strCaseEqual(pulseGenSourceName, At4400Name))
   {
      pulseGenSource_ = PulseGenAt4400;
      delete pulsegen_;
      pulsegen_ = new AT4400;
      pulsegen_->simulated(isSimulated());
      pulsegen_->function("Pulse Generator");
   }
   else if (SseUtil::strCaseEqual(pulseGenSourceName, SigGenInternalName))
   {
      // pulses are generated internally in the sig (tone) generator
      pulseGenSource_ = SigGenInternal;
   }
   else 
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER 
	     << " TestSig::setPulseGenSource() unsupported pulse gen source: "
	     << pulseGenSourceName << endl
	     << "must be one of: " << At33250Name << " " << At4400Name 
	     << " " << SigGenInternalName << endl;
      return(sendToSocket(errmsg.str()));
   }

   return(getPulseGenSourceName());
}

const char* TestSig::getPulseGenSourceName()
{
   if (pulseGenSource_ == PulseGenAt33250)
   {
      return(sendToSocket(At33250Name));
   }
   else if (pulseGenSource_ == PulseGenAt4400)
   {
      return(sendToSocket(At4400Name));
   }
   else if (pulseGenSource_ == SigGenInternal)
   {
      return(sendToSocket(SigGenInternalName));
   }
  
   AssertMsg(0, "unknown pulse gen source");

}

const char * TestSig::setsiggensim(const char *mode)
{
   return simulate(getSigGen(), mode);
}

const char * TestSig::setpulsegensim(const char *mode)
{
   return simulate(getPulseGen(), mode);
}

const char* TestSig::simulate(GPIBDevice *device, const char *mode)
{
   bool sim = false;
   if (SseUtil::strCaseEqual(mode, "on")) 
   {
      sim = true;
   }
   else if (SseUtil::strCaseEqual(mode, "off"))
   {
      sim = false;
   }
   else
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER
	     << " TestSig::simulate(device) unsupported simulate mode: " 
	     << mode << endl
	     << "use \"on\" or \"off\"";
      return(sendToSocket(errmsg.str()));
   }

   device->simulated(sim);

   return(mode);
}

const char* TestSig::simulate(const char* mode)
{
   bool sim = false;
   if (SseUtil::strCaseEqual(mode, "on")) 
   {
      sim = true;
   }
   else if (SseUtil::strCaseEqual(mode, "off"))
   {
      sim = false;
   }
   else
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER
	     << " TestSig::simulate() unsupported simulate mode: " 
	     << mode << endl
	     << "use \"on\" or \"off\"";
      return(sendToSocket(errmsg.str()));
   }

   getSigGen()->simulated(sim);
   setSimulated(sim);
   getPulseGen()->simulated(sim);
    
   return(mode);
}


// ===== GPIB configuration =====


const char* TestSig::setsiggenaddress(int bus, int addr)
{
   getSigGen()->bus(bus);
   getSigGen()->address(addr);
   return(getsiggeninfo());
}

const char* TestSig::getsiggeninfo() 
{
   stringstream msg;
   msg << "TestSig::getsiggen() " 
       << getSigGen()->getModelName() << ": "
       << "bus(" << getSigGen()->bus() 
       << "), address(" << getSigGen()->address() << ")" 
       << endl;
   return(sendToSocket(msg.str()));
}

const char* TestSig::setpulsegenaddress(int bus, int addr) 
{
   getPulseGen()->bus(bus);
   getPulseGen()->address(addr);
   return(getpulsegeninfo());
}

const char* TestSig::getpulsegeninfo() 
{
   stringstream msg;
   
   if (pulseGenSource_ == SigGenInternal)
   {
      msg << "PulseGenSource: " << getPulseGenSourceName()
	  << endl;
   }
   else
   {
      msg << "TestSig::getpulsegen() " 
	  << getPulseGen()->getModelName() << ": "
	  << "bus(" << getPulseGen()->bus() 
	  << "), address(" << getPulseGen()->address() << ")" 
	  << endl;
   }
   return(sendToSocket(msg.str()));
}


// ===== commands =====

const char* TestSig::gpib(const char* mode) 
{
   bool verbose = false;
   if (SseUtil::strCaseEqual(mode, "on"))
   {
      verbose = true;
   }
   else if (SseUtil::strCaseEqual(mode, "off"))
   {
      verbose = false;
   }
   else
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER
	     << " TestSig::gpib() unsupported gpib mode: " << mode << endl
	     << "use: \"on\" or \"off\"";
      return(sendToSocket(errmsg.str()));
   }

   getSigGen()->verbose(verbose);
   getPulseGen()->verbose(verbose);
  
   return(mode);
}

const char* TestSig::help() 
{
   stringstream msg;

   msg << endl
       << "---- general ----" << endl
    
       << "tsig getname" << endl
       << "tsig setname <name>" << endl
    
       << "tsig getversion" << endl
    
       << "tsig getpulsegensource" << endl
       << "tsig setpulsegensource <"
       << At33250Name << " | " << At4400Name << " | "
       << SigGenInternalName << ">" << endl
    
       << "tsig intrinsics " << endl
    
       << "---- GPIB ----" << endl
    
       << "gpib <on|off>  (note: toggles verbose mode)" << endl;
    
    
   msg << "tsig setsiggenaddress <bus> <address>" << endl
       << "tsig getsiggeninfo" << endl
       << "tsig setsiggensource <"
       << At33250Name << "|" << At4400Name << ">" << endl
       << "tsig getsiglimit" << endl
       << "tsig setsiglimit <limit dBm>" << endl
       << "tsig setsiggensim <on|off> " << endl    

       << "tsig setpulsegenaddress <bus> <address>" << endl
       << "tsig getpulsegeninfo" << endl
       << "tsig getpulselimit" << endl
       << "tsig setpulselimit <limit dBm>" << endl
       << "tsig setpulsegensim <on|off> " << endl;       

   msg << "---- commands ----" << endl
    
       << "exit" << endl
       << "help" << endl
       << "id" << endl
       << "tsig gpibcmd <cmd> <arg1> <arg2> ... " << endl
       << "[tsig] off" << endl
       << "[tsig] on" << endl
       << "tsig quiet" << endl
       << "requestready" << endl
       << "reset" << endl
       << "selftest" << endl
       << "simulate <on|off>" << endl
       << "status" << endl;

   msg << "tunesiggen <frequency MHz> <amplitude dBm> <rate Hz/sec> "
       << "<duration sec>" << endl
       << "tsig pulse <amplitude dBm> <period sec> <duration sec> " << endl
       << endl;
  
   return(sendToSocket(msg.str()));
}

const char* TestSig::id() 
{
   stringstream msg;

   try
   {
      // sig gen
      if (getSigGen()->simulated())
      {
	 getSigGen()->id("GPIB device simulated.");
      }
      else
      {
	 getSigGen()->identify();
      }

      // pulse gen
      if (getPulseGen()->simulated())
      {
	 getPulseGen()->id("GPIB device simulated.");
      }
      else if (pulseGenSource_ == SigGenInternal)
      {
	 getPulseGen()->id("siggen internal");
      } 
      else
      {
	 getPulseGen()->identify();
      }
   }
   catch (GPIBError gpib_err)
   {
      msg  << SSE_TESTSIG_ERROR_MSG_HEADER 
	   << " TestSig::id(): " << gpib_err;
      return(sendToSocket(msg.str()));
   }

   msg << "siggen: "
       << getSigGen()->getModelName() << ": " 
       << getSigGen()->id() << endl;

   msg << "pulsegen: ";
   if (pulseGenSource_ == SigGenInternal)
   {
      msg << getPulseGen()->id() << endl;
   }
   else
   {
      msg << getPulseGen()->getModelName() << ": " 
	  << getPulseGen()->id() << endl;
   }

   return(sendToSocket(msg.str()));
}


const char* TestSig::intrinsics()
{
   stringstream msg;
   msg << SSE_TESTSIG_INTRIN_MSG_HEADER << endl
       << "name = " << name() << endl
       << "version = " << version() << endl;
   return(sendToSocket(msg.str()));
}

const char* TestSig::on()
{
   stringstream msg;
   try
   {
      getSigGen()->RF(true);

      // Note: pulse gen is turned on in pulse() method
   }
   catch (GPIBError gpib_err)
   {
      msg << SSE_TESTSIG_ERROR_MSG_HEADER 
	  << " TestSig::on(): " << gpib_err;
      return(sendToSocket(msg.str()));
   }

   msg << "TestSig::on() complete.";
   return(sendToSocket(msg.str()));
}

const char* TestSig::off()
{
   stringstream msg;
   try
   {
      getSigGen()->RF(false);
      if (pulseGenSource_ != SigGenInternal)
      {
	 getPulseGen()->RF(false);
      }
   }
   catch (GPIBError gpib_err) 
   {
      msg << SSE_TESTSIG_ERROR_MSG_HEADER 
	  << " TestSig::off(): " << gpib_err;
      return(sendToSocket(msg.str()));
   }

   msg << "TestSig::off() complete.";
   return(sendToSocket(msg.str()));
}

const char* TestSig::requestready()
{
   stringstream msg;
   msg << SSE_TESTSIG_READY_MSG << endl;

   return(sendToSocket(msg.str()));
}



const char* TestSig::reset() 
{
   stringstream msg;
   try 
   {
      getSigGen()->reset();

      if (pulseGenSource_ != SigGenInternal)
      {
	 getPulseGen()->reset();
      }
   }
   catch (GPIBError gpib_err) 
   {
      msg << SSE_TESTSIG_ERROR_MSG_HEADER 
	  << " TestSig::reset: " << gpib_err;
      return(sendToSocket(msg.str()));
   }

   msg << "TestSig::reset() complete.";
   return(sendToSocket(msg.str()));
}

const char* TestSig::selftest() 
{
   stringstream msg;
   string resultSigGen;
   string resultPulseGen;

   try
   {
      getSigGen()->selftest(resultSigGen);
      msg << "sig gen: " << resultSigGen << endl;

      if (pulseGenSource_ != SigGenInternal)
      {
	 getPulseGen()->selftest(resultPulseGen);
	 msg << "pulse gen: " << resultPulseGen << endl;
      }
   }
   catch (GPIBError gpib_err)
   {
      msg  << SSE_TESTSIG_ERROR_MSG_HEADER 
	   << " TestSig::selftest: " << gpib_err;
      return(sendToSocket(msg.str()));
   }
   msg << "TestSig::selftest() complete.";
   return(sendToSocket(msg.str()));
}


const char* TestSig::status() 
{
   stringstream msg;
   stringstream errmsg;

   bool sweepState(false);
   double sigAmpdBm(0);
   double startFreqHz(0);
   double stopFreqHz(0);
   double sweepTime(0);
   bool cwOutputState(false);

   double pulseAmpdBm(0);
   double pulsePeriod(0);
   double pulseWidth(0);
   bool pulseOutputState(false);

   try
   {
      sigAmpdBm = getSigGen()->amplitude();
      sweepState = getSigGen()->sweepState();

      if (sweepState)
      {
	 startFreqHz = getSigGen()->startFrequency();
	 stopFreqHz = getSigGen()->stopFrequency();
	 sweepTime  = getSigGen()->sweepTime();
      }
      else 
      {
	 startFreqHz = getSigGen()->frequency();
      }

      cwOutputState = getSigGen()->RF();

      if (pulseGenSource_ == PulseGenAt33250 )
      {
	pulseAmpdBm = getPulseGen()->amplitude();
	pulsePeriod = getPulseGen()->period();
	pulseWidth = getPulseGen()->width();
	pulseOutputState = getPulseGen()->RF();
      }
      else if (pulseGenSource_ == SigGenInternal)
      {
	pulseAmpdBm = getSigGen()->amplitude();
	pulsePeriod = getSigGen()->period();
	pulseWidth = getSigGen()->width();
	pulseOutputState = usingPulseGen_;
      }
   }
   catch (GPIBError gpib_err)
   {
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER 
	     << " TestSig::status(): " << gpib_err;
      return(sendToSocket(errmsg.str()));
   }
   catch (SseException &except) 
   {
      return sendToSocket(except.descrip());
   }

   msg.precision(9);
   msg.setf(std::ios::fixed);

   msg << SSE_TESTSIG_STATUS_MSG_HEADER
       << " = " << name() << endl
       << "Signal Sweep State = " << sweepState << endl
       << "Signal Amplitude (dBm) = " << sigAmpdBm << endl
       << "Signal Start Frequency (MHz) = " << startFreqHz/HzPerMhz << endl
       << "Signal Stop Frequency (MHz) = " << stopFreqHz/HzPerMhz << endl
       << "Signal Sweep Time (sec) = " << sweepTime << endl
       << "Signal Output = " << cwOutputState << endl
       << "Pulse Source = " << getPulseGenSourceName() << endl
       << "Pulse Amplitude (dBm) = " << pulseAmpdBm << endl
       << "Pulse Period (sec) = " << pulsePeriod << endl
       << "Pulse Width (sec) = " << pulseWidth << endl
       << "Pulse Output = " << pulseOutputState << endl
       << "Simulated = " << isSimulated() << endl
       << "Sig Gen Simulated = " << getSigGen()->simulated() << endl
       << "Pulse Gen Simulated = " << getPulseGen()->simulated() << endl
#ifdef HAVE_LIBGPIB
       << "HAVE_LIBGPIB = yes " << endl
#else
       << "HAVE_LIBGPIB = no "       << endl
#endif
       << endl;
  
   return(sendToSocket(msg.str()));

}

/*
  Make as little rf output as possible, so that the 
  tsig output cannot be mistaken for any real signal; 
 */
const char * TestSig::quiet()
{
   stringstream strm;

   strm << "Going into quiet mode. " << endl;

   /*
     Make any residiual signal as inconspicuous as 
     possible: minim freq & amp, zero drift
   */

   double freqMhz(getSigGen()->minFrequency() / HzPerMhz);
   double ampDbm(getSigGen()->minAmplitude());
   double driftRateHzSec(0.0);
   double durationSecs(100);   // TBD get max duration from sig gen?

   strm << tunesiggen(freqMhz, ampDbm, driftRateHzSec, durationSecs);

   // Turn off output
   strm << off();

   return(sendToSocket(strm.str()));

}


const char* TestSig::tunesiggen(double frequencyMhz, double amplitudedBm,
				double driftRateHzSec, double durationSecs) 
{
   // call before pulse command if a drifting pulse is required.
   stringstream msg;
 
   try
   {
      usingPulseGen_ = false;
     
      if (pulseGenSource_ != SigGenInternal)
      {
      	getPulseGen()->RF(false);  // Turn off Pulse Gen Output
      }

      getSigGen()->sweep(amplitudedBm, frequencyMhz*HzPerMhz,
			 driftRateHzSec, durationSecs);
   }
   catch (GPIBError gpib_err)
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER 
	     << " TestSig::tunesiggen(): " << gpib_err;
      return(sendToSocket(errmsg.str()));
   }

   msg << "TestSig::tunesiggen() complete." << endl << status();

   return(sendToSocket(msg.str()));

}


const char* TestSig::pulse(double amplitudedBm, double periodSec,
			   double widthSec)
{
   stringstream msg;
 
   try
   {
      if (pulseGenSource_ == SigGenInternal)
      {
	 if (sigGenSource_ == SigGenAt33250)
	 {
	    stringstream errMsg;
	    errMsg << SSE_TESTSIG_ERROR_MSG_HEADER 
		   << " TestSig::pulse(): "
		   << "internal pulse gen mode not supported "
                   << "for " << At33250Name;
	    
	    return(sendToSocket(errMsg.str()));
	 }

	 getSigGen()->pulse(amplitudedBm, periodSec, widthSec);
	 getSigGen()->RF(true);
      }
      else if (pulseGenSource_ == PulseGenAt33250 || 
	    pulseGenSource_ == PulseGenAt4400)
      {
	 getPulseGen()->pulse(amplitudedBm, periodSec, widthSec);
	 getPulseGen()->RF(true);
      }
      else
      {
	 AssertMsg(0, "invalid pulse gen type");
      }
   }
   catch (GPIBError gpib_err)
   {
      stringstream errmsg;
      errmsg << SSE_TESTSIG_ERROR_MSG_HEADER 
	     << " TestSig::pulse(): " << gpib_err;
      return(sendToSocket(errmsg.str()));
   }

   usingPulseGen_ = true;

   msg << "TestSig::pulse() complete." << endl << status();

   return(sendToSocket(msg.str()));

}

// forward gpib command to the device, with any arguments
const char* TestSig::gpibcmd(const char *cmd, const char *arg1,
	const char*arg2, const char *arg3, const char *arg4)
{
  stringstream strm;
  strm << cmd  <<  " " << arg1 <<  " " << arg2 <<  " " 
       << arg3 << " " << arg4;

  getSigGen()->send(strm.str());

  return sendToSocket("command sent");
}