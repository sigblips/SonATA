/*******************************************************************************

 File:    SCU.h
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

// Switch Control Unit control code
// Original code by L.R. McFarland

#ifndef SCU_H
#define SCU_H

#include <GPIBDevice.h>

using std::cerr;
using std::endl;

class SCU : public GPIBDevice
{

 public:

   enum IfSource { SOURCE_SKY, SOURCE_TEST, SOURCE_STATE_UNKNOWN };

   SCU(int bufferSize = DefaultBufferSize);
   SCU(int bus, int addr, int sz = DefaultBufferSize);
   virtual ~SCU();

   virtual void setAttnLeft(int level) = 0;
   virtual int getAttnLeft() = 0;

   virtual void setAttnRight(int level) = 0;
   virtual int getAttnRight() = 0;

   virtual void setIfSource(IfSource ifSource);
   virtual IfSource getIfSource();
   virtual string ifSourceToString(IfSource ifSource);

 protected:

   struct Attn
   {
      int32_t  val;
      int32_t  switch_id;
      Attn() : val(0), switch_id(0) {}
   };

 private:
   IfSource ifSource_;

};

class HP3488A : public SCU
{

 public:

   HP3488A(int bufferSize = DefaultBufferSize) : 
      SCU(bufferSize) 
      {
	 id("HP3488A");
      };
  
   HP3488A(int bus, int addr, int bufferSize = DefaultBufferSize)  : 
      SCU(bus, addr, bufferSize)
      {
	 id("HP3488A");
      };

   virtual ~HP3488A() {};

   void open(int32_t switch_id);
   void close(int32_t switch_id);

   string digitalRead(int32_t slotPort);
   void digitalWrite(int32_t slotPort, int32_t value);
   void displayText(const string& msg);
   void turnDisplayOn();

   // IEEE 488.2 common commands won't work with this machine

   virtual string getModelName() const;

   virtual void identify(); 
   virtual void reset() {/* no op */};
   virtual void selftest(string& result);

};

class AT34970 : public SCU
{

 public:

   AT34970(int bufferSize = DefaultBufferSize) : SCU(bufferSize) {};
   AT34970(int bus, int addr, int bufferSize = DefaultBufferSize) : SCU(bus, addr, bufferSize) {};

   virtual ~AT34970() {};

   void reset();

   void open(int32_t switchId);
   void close(int32_t switchId);
   bool isClosed(int32_t switchId);
   void strobe(int32_t switchId);

   virtual string getModelName() const;

 private:

   static const int32_t lowIndex = 101;
   static const int32_t highIndex = 121;



};


#endif // SCU_H