/*******************************************************************************

 File:    Ifc.cpp
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

// NSS IF Control

// Original code created by L. R. McFarland 2003

#include <Ifc.h>
#include <SseUtil.h>
#include <iostream>
#include <sstream>
#include <fcntl.h>
#include "sseIfcInterface.h"

using namespace std;

Ifc::Ifc(): 
   name_("no name"),
   version_(SSE_IFC_INTERFACE_VERSION), 
   simulated_(false),
   stx_(this)
{
   scu_.function("IF Attenuator SCU");
}

Ifc::~Ifc()
{}

// ===== accessors for tcl =====

const char* Ifc::name() const
{
   return(name_.c_str());
}

const char* Ifc::name(string& newName)
{
   name_ = newName;
   return(name());
}

const char* Ifc::name(const char* newName)
{
   name_ = newName;
   return(name());
}

const char* Ifc::version() const
{
   return(version_.c_str());
}


bool Ifc::simulated() 
{
   return(simulated_);
}

bool Ifc::simulated(bool mode)
{
   simulated_ = mode; 
   return(simulated());
}


// ===== private methods =====

const char* Ifc::sendToSocket(const string& message)
{
   tclBuffer_ = message;
   return(tclBuffer_.c_str());
}

// ===== public methods =====


const char* Ifc::simscu(const char* mode)
{
   return simulate(&scu_, mode);
}

const char* Ifc::simulate(GPIBDevice *device, const char *mode)
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
      errmsg << SSE_IFC_ERROR_MSG_HEADER 
	     << " Ifc::gpib(device) unsupported simulate mode: " 
	     << mode << endl
	     << "use \"on\" or \"off\"";
      return(sendToSocket(errmsg.str()));

   }

   device->simulated(sim);

   return(mode);
}


const char* Ifc::simulate(const char* mode)
{
   if (SseUtil::strCaseEqual(mode, "on"))
   {
      scu_.simulated(true);
      stx_.simulated(true);
      simulated(true);

   }
   else if (SseUtil::strCaseEqual(mode, "off"))
   {
      scu_.simulated(false);
      stx_.simulated(false);
      simulated(false);
   }
   else
   {
      stringstream errmsg;
      errmsg << SSE_IFC_ERROR_MSG_HEADER 
	     << " Ifc::gpib() unsupported simulate mode: " 
	     << mode << endl
	     << "use \"on\" or \"off\"";
      return(sendToSocket(errmsg.str()));
   }
   return(mode);
}


// ===== GPIB configuration =====


const char* Ifc::addressscu(int bus, int addr)
{
   scu_.bus(bus);
   scu_.address(addr);

   return(getscu());
}

const char* Ifc::getscu()
{
   stringstream msg;
   msg << "Ifc::getscu() " << scu_.function() << ": "
       << "bus(" << scu_.bus() 
       << "), address(" << scu_.address() << ")" 
       << endl;

   return(sendToSocket(msg.str()));
}

// ===== commands =====

const char* Ifc::gpib(const char* mode)
{
   if (SseUtil::strCaseEqual(mode, "on"))
   {
      scu_.verbose(true);
   }
   else if (SseUtil::strCaseEqual(mode, "off"))
   {
      scu_.verbose(false);
   }
   else 
   {
      stringstream errmsg;
      errmsg << SSE_IFC_ERROR_MSG_HEADER
	     << " Ifc::gpib() unsupported gpib mode: " << mode << endl
	     << "use \"on\" or \"off\"";
      return(sendToSocket(errmsg.str()));
   }
   return(mode);
}

const char* Ifc::help() 
{
   stringstream msg;

   msg << endl
       << "---- general ----" << endl
    
       << "ifc getname" << endl
       << "ifc setname <name>" << endl
    
       << "ifc getversion" << endl
    
       << "ifc intrinsics" << endl
    
       << "---- STX ----" << endl
    
       << "ifc getstxfilename" << endl
       << "ifc setstxfilename <stx filename, e.g. /dev/stx0>" << endl
    
       << "ifc gethistlen" << endl
       << "ifc sethistlen <stx histogram length>" << endl
    
       << "ifc gettol" << endl
       << "ifc settol <stx tolerance>" << endl
    
       << "ifc getmaxtries" << endl
       << "ifc setmaxtries <stx maximum tries>" << endl
    
       << "ifc stxstart" << endl
       << "ifc stxstatus" << endl

       << "ifc stxvariance <lcp var> <rcp var>" << endl

       << "ifc stxpol <'both'|'left'|'right'>" << endl
    
       << "---- GPIB ----" << endl
    
       << "gpib <on|off> (note: toggles verbose mode)" << endl

       << "ifc addressscu <bus> <address>" << endl
       << "ifc getscu" << endl
       << "ifc simscu <on|off>" << endl

       << "---- commands ----" << endl
    
       << "attn <left attn dB> <right attn dB>" << endl
       << "exit" << endl
       << "help" << endl
       << "ifc ifsource <sky|test>" << endl
       << "id" << endl
       << "off" << endl
       << "requestready" << endl
       << "reset" << endl
       << "selftest" << endl
       << "simulate <on|off>" << endl
       << "status" << endl
       << endl;

   return(sendToSocket(msg.str()));
}

const char* Ifc::id()
{
   stringstream msg;

   try
   {
      if (scu_.simulated())
      {
	 scu_.id("GPIB device simulated.");
      }
      else
      {
	 scu_.identify();
      }
   }
   catch (GPIBError & gpibError)
   {
      msg  << SSE_IFC_ERROR_MSG_HEADER 
	   << " Ifc::id(): " << gpibError;
      return(sendToSocket(msg.str()));
   }

   msg << "IF scu: " << scu_.function() << " " << scu_.id();

   return(sendToSocket(msg.str()));
}

// set the IF source to either test or sky
const char* Ifc::ifsource(const char* source)
{
   SCU::IfSource ifSource;
   stringstream msg;

   if (SseUtil::strCaseEqual(source, "test"))
   {
      ifSource = SCU::SOURCE_TEST;
   }
   else if (SseUtil::strCaseEqual(source, "sky"))
   {
      ifSource = SCU::SOURCE_SKY;
   }
   else 
   {
      stringstream errmsg;
      errmsg << SSE_IFC_ERROR_MSG_HEADER
	     << " Ifc::ifsource() unsupported mode: " << source << endl
	     << "use \"test\" or \"sky\"";
      return(sendToSocket(errmsg.str()));
   }


   try
   {
      scu_.setIfSource(ifSource);
   }
   catch (GPIBError & gpibError)
   {
      msg << SSE_IFC_ERROR_MSG_HEADER 
	  << " Ifc::ifsource(): " << gpibError;
      return(sendToSocket(msg.str()));
   }

   msg << "ifsource complete." << endl;

   return(sendToSocket(msg.str()));

}

// set the stx operating polarity (both, left, right)
const char* Ifc::setStxPol(const char* polStr)
{
   stringstream msg;
   Polarization pol;

   if (SseUtil::strCaseEqual(polStr, "both"))
   {
       pol = POL_BOTH;
   }
   else if (SseUtil::strCaseEqual(polStr, "left"))
   {
       pol = POL_LEFTCIRCULAR;
   }
   else if (SseUtil::strCaseEqual(polStr, "right"))
   {
       pol = POL_RIGHTCIRCULAR;
   }
   else 
   {
      stringstream errmsg;
      errmsg << SSE_IFC_ERROR_MSG_HEADER
	     << " Ifc::setStxPol() invalid pol: " << polStr << endl
	     << "Must be one of: \"both\" \"left\" or \"right\"";
      return(sendToSocket(errmsg.str()));
   }

   stx_.setPol(pol);

   msg << "set stx pol complete." << endl;

   return(sendToSocket(msg.str()));

}


const char* Ifc::intrinsics()
{
   stringstream msg;
   msg << SSE_IFC_INTRIN_MSG_HEADER << endl
       << "name = " << name() << endl
       << "version = " << version() << endl;

   return(sendToSocket(msg.str()));
}

const char* Ifc::off()
{
   // nothing to do

   stringstream msg;
   msg << "Ifc::off() complete.";
   return(sendToSocket(msg.str()));
}

const char* Ifc::requestready()
{
   stringstream msg;
   msg << SSE_IFC_READY_MSG;
   return(sendToSocket(msg.str()));
}


const char* Ifc::reset()
{
   stringstream msg;

   try
   {
      scu_.reset();
   }
   catch (GPIBError & gpibError)
   {
      msg << SSE_IFC_ERROR_MSG_HEADER 
	  << " Ifc::reset: " << gpibError;
      return(sendToSocket(msg.str()));
   }
   msg << "Ifc::reset() complete.";
   return(sendToSocket(msg.str()));
}

const char* Ifc::selftest()
{
   stringstream msg;
   string resultSCU;

   try
   {
      scu_.selftest(resultSCU);
      msg << "scu: " << resultSCU << endl;
   }

   catch (GPIBError & gpibError)
   {
      msg  << SSE_IFC_ERROR_MSG_HEADER 
	   << " Ifc::selftest: " << gpibError;
      return(sendToSocket(msg.str()));
   }

   msg << "Ifc::selftest() complete.";

   return(sendToSocket(msg.str()));
}

const char* Ifc::attn(int attnDbLeft, int attnDbRight)
{
   stringstream msg;

   // TBD get min/max from scu attn code

   const int minAttn(0);   // minimum attenuation
   const int maxAttn(11);  // maximum attenuation

   if (attnDbLeft < minAttn || attnDbRight < minAttn)
   {
      msg << SSE_IFC_ERROR_MSG_HEADER 
	  << " Ifc::attn() minimum attenuation (" 
	  << minAttn << " dB) exceeded: "
	  << attnDbLeft << " dB left, " << attnDbRight << " dB right";
      return(sendToSocket(msg.str()));
   }

   if (attnDbLeft > maxAttn || attnDbRight > maxAttn)
   {
      msg << SSE_IFC_ERROR_MSG_HEADER 
	  << " Ifc::attn() maximum attenuation (" 
	  << maxAttn << " dB) exceeded: "
	  << attnDbLeft << " dB left, " << attnDbRight << " dB right";
      return(sendToSocket(msg.str()));
   }

   try
   {
      scu_.setAttnLeft(attnDbLeft);
      scu_.setAttnRight(attnDbRight);
   }
   catch (GPIBError & gpibError)
   {
      msg  << SSE_IFC_ERROR_MSG_HEADER 
	   << " Ifc::attn():" << gpibError;
      return(sendToSocket(msg.str()));
   }

   msg << "Ifc::attn(" << attnDbLeft << ", " 
       << attnDbRight << ") complete." << endl;
   return(sendToSocket(msg.str()));
}

const char* Ifc::status()
{
   stringstream msg;
   stringstream errmsg;

   int attnDbLeft(0);
   int attnDbRight(0);
   SCU::IfSource ifSource(SCU::SOURCE_STATE_UNKNOWN);

   try
   {
      attnDbLeft = scu_.getAttnLeft();
      attnDbRight = scu_.getAttnRight();
      ifSource = scu_.getIfSource();
   }
   catch (GPIBError & gpibError)
   {
      errmsg << SSE_IFC_ERROR_MSG_HEADER 
	     << " Ifc::status(): " << gpibError;
      return(sendToSocket(errmsg.str()));
   }
  
   msg.precision(9);
   msg.setf(std::ios::fixed);

   msg << SSE_IFC_STATUS_MSG_HEADER 
       << " = " << name() << endl;

   msg << "Attn. Left (dB) = " << attnDbLeft << endl
       << "Attn. Right (dB) = " << attnDbRight << endl
       << "IF Source = " << scu_.ifSourceToString(ifSource) << endl
       << "Simulated = " << simulated() << endl
#ifdef HAVE_LIBGPIB
       << "HAVE_LIBGPIB = yes" << endl
#else
       << "HAVE_LIBGPIB = no" << endl
#endif
       << "SCU simulated = " << scu_.simulated() << endl
       << endl
       << stx_.status()
       << endl;

   return(sendToSocket(msg.str()));

}

Stx & Ifc::getStx()
{
   return stx_;
}

IfcAttnScu & Ifc::getAttnScu()
{
   return scu_;
}
