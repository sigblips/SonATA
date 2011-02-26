/*******************************************************************************

 File:    SCU.cpp
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

// Original code by L.R. McFarland

#include "SCU.h"
#include "Assert.h"
#include <unistd.h>

using namespace std;

SCU::SCU(int bufferSize) : 
   GPIBDevice(bufferSize), ifSource_(SOURCE_STATE_UNKNOWN)
{
}

SCU::SCU(int bus, int addr, int bufferSize) : 
   GPIBDevice(bus, addr, bufferSize), ifSource_(SOURCE_STATE_UNKNOWN)
{
}

SCU::~SCU()
{
}

void SCU::setIfSource(IfSource ifSource)
{
   ifSource_ = ifSource;
}

SCU::IfSource SCU::getIfSource()
{
   return ifSource_;
}

string SCU::ifSourceToString(IfSource ifSource)
{
   switch (ifSource)
   {
   case SOURCE_SKY:
      return "sky";
      break;
   case SOURCE_TEST:
      return "test";
      break;
   case SOURCE_STATE_UNKNOWN:
      return "???";
      break;
   default:
      AssertMsg(0, "invalid if source");
      break;
   }

}


// -------------------
// ----- HP3488A -----
// -------------------

string HP3488A::getModelName() const
{
   return "HP3488A";  
}


void HP3488A::open(int32_t switchId)
{
   stringstream strm;

   strm << "OPEN " << switchId;
   send(strm.str());
}

void HP3488A::close(int32_t switchId)
{
   stringstream strm;

   strm << "CLOSE " << switchId;
   send(strm.str());
}

string HP3488A::digitalRead(int32_t slotPort)
{
/*
  Readback only seems to work correctly if digital mode 2 is set.
  mode 2 = "static, read inputs".  See 3488a switch/control
  unit operating, prog, & config manual, p.220.
  If mode =1 is used, then the first time the data is read back
  it's correct, but for subsequent reads it's always 255;
*/

   const int staticReadOutputsMode = 2;

   stringstream dmode;
   dmode << "DMODE " << slotPort << "," << staticReadOutputsMode;
   send(dmode.str());

  // read back the slot/port

   stringstream strm;

   strm << "DREAD " << slotPort;
   send(strm.str());

   string result;
   recv(result);

   return result;
}

void HP3488A::digitalWrite(int32_t slotPort, int32_t value) 
{
   stringstream strm;

   strm << "DWRITE " << slotPort << ", " << value;
   send(strm.str());
}

// Write msg to the device display.
// Msg can be up to 127 chars long.
// Chars not allowed:  LF CR ; : #

void HP3488A::displayText(const string& msg)
{
   stringstream strm;

   strm << "DISP " << msg;
   send(strm.str());
}

void HP3488A::turnDisplayOn()
{
   send("DON");
}


void HP3488A::selftest(string& result)
{
   send("TEST");
   if (simulated())
   {
      result = "0";  // Test succeeded
   }
   else
   {
      sleep(10); // TBD self test sleep time
      recv(result);
   }

};


void HP3488A::identify()
{
   string result;
   send("ID?");
   if (!simulated()) recv(result);
   id(result.c_str());
};

// -------------------
// ----- AT34970 -----
// -------------------

string AT34970::getModelName() const
{
   return "AT34970";  
}


void AT34970::reset()
{
   send("*RST");
   send("*CLS");
}

void AT34970::open(int32_t switchId)
{
   stringstream strm;

   // TBD check for valid switchId range
   if (switchId < lowIndex)
   {
      strm << function() << getModelName() << " switchId out of range: " 
	   << switchId;
      throw GPIBError(strm.str());
   }
   if (switchId > highIndex)
   {
      strm << function() << getModelName() << " switchId out of range: " 
	   << switchId;
      throw GPIBError(strm.str());
   }

   strm << "ROUTE:OPEN (@" << switchId << ")";
   send(strm.str());
}

void AT34970::close(int32_t switchId) 
{
   stringstream strm;

   // TBD check for valid switchId range
   if (switchId < lowIndex)
   {
      strm << function() << getModelName() << " switchId out of range: " << switchId;
      throw GPIBError(strm.str());
   }

   if (switchId > highIndex)
   {
      strm << function() << getModelName() << " switchId out of range: " << switchId;
      throw GPIBError(strm.str());
   }

   strm << "ROUTE:CLOSE (@" << switchId << ")";
   send(strm.str());
}

bool AT34970::isClosed(int32_t switchId)
{
   stringstream strm;
   string buffer;

   strm << "ROUTE:CLOSE? (@" << switchId << ")";
   send(strm.str());

   recv(buffer);
   if (buffer == "1")
   {
      return true;
   }
   else
   {
      return false;
   }

   // TBD verify buffer value is valid
}

// strobe the switchId to make changes take effect
void AT34970::strobe(int32_t switchId)
{
   close(switchId);
   open(switchId);
}