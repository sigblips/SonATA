/*******************************************************************************

 File:    IfcAttnScu.cpp
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



#include "IfcAttnScu.h" 
#include "Assert.h"
#include <sstream>

using namespace std;

static const int SkyTestSwitchIdLeft(109);
static const int SkyTestStrobeSwitchIdLeft(119);

static const int SkyTestSwitchIdRight(110);
static const int SkyTestStrobeSwitchIdRight(120);

const bool UseCommandedValuesForStatus = true;

IfcAttnScu::IfcAttnScu(int bufferSize) : 
   AT34970(bufferSize), 
   simAttnDbLeft_(-1), 
   simAttnDbRight_(-1),
   attnBankLeft_(*this),
   attnBankRight_(*this)
{
   initAttnBankLeft();
   initAttnBankRight();
}

IfcAttnScu::~IfcAttnScu() 
{
}

void IfcAttnScu::initAttnBankLeft()
{
   attnBankLeft_.addStepLevelDbAndSwitchId(4, 104);
   attnBankLeft_.addStepLevelDbAndSwitchId(4, 103);
   attnBankLeft_.addStepLevelDbAndSwitchId(2, 102);
   attnBankLeft_.addStepLevelDbAndSwitchId(1, 101);

   attnBankLeft_.addStrobeSwitchId(117);
}


void IfcAttnScu::setAttnLeft(int levelDb)
{
   simAttnDbLeft_ = levelDb;
   attnBankLeft_.setAttnLevelDb(levelDb);
}

int IfcAttnScu::getAttnLeft()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(simAttnDbLeft_);
   }

   return attnBankLeft_.getAttnLevelDb();

}


void IfcAttnScu::initAttnBankRight()
{
   attnBankRight_.addStepLevelDbAndSwitchId(4, 108);
   attnBankRight_.addStepLevelDbAndSwitchId(4, 107);
   attnBankRight_.addStepLevelDbAndSwitchId(2, 106);
   attnBankRight_.addStepLevelDbAndSwitchId(1, 105);

   attnBankRight_.addStrobeSwitchId(118);
}


void IfcAttnScu::setAttnRight(int levelDb)
{
   simAttnDbRight_ = levelDb;
   attnBankRight_.setAttnLevelDb(levelDb);
}

int IfcAttnScu::getAttnRight()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(simAttnDbRight_);
   }

   return attnBankRight_.getAttnLevelDb();

}


void IfcAttnScu::setIfSource(IfSource ifSource)
{
   if (ifSource == SOURCE_SKY)
   {
      // set the switches
      open(SkyTestSwitchIdLeft);
      open(SkyTestSwitchIdRight);

      // TBD display text on SCU display: "sky sky"
   }
   else if (ifSource == SOURCE_TEST)
   {
      // set the switches
      close(SkyTestSwitchIdLeft);
      close(SkyTestSwitchIdRight);

      // TBD display text on SCU display: "test test"
   }
   else
   {
      AssertMsg(0, "invalid IfSource value");
   }

   // strobe group supply pins to update scu 
   strobe(SkyTestStrobeSwitchIdLeft);
   strobe(SkyTestStrobeSwitchIdRight);

   // remember setting
   SCU::setIfSource(ifSource);

}


SCU::IfSource IfcAttnScu::getIfSource()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return SCU::getIfSource();
   }

   // Read the current sky/test state.
   // Left & right should be the same, so just use one.
   // TBD: validate that both switches are the same.

   IfSource ifSource(SOURCE_SKY);
   if (isClosed(SkyTestSwitchIdLeft))
   {
      ifSource = SOURCE_TEST;
   }

   return ifSource;
}

