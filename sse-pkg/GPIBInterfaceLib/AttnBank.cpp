/*******************************************************************************

 File:    AttnBank.cpp
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



#include "AttnBank.h" 
#include "SCU.h"
#include "Assert.h"
#include <sstream>
#include <algorithm>

using namespace std;

bool attnStepHigherDbValue(const AttnBank::AttnStep & attnStep1,
			   const AttnBank::AttnStep & attnStep2)
{
   return attnStep1.valueDb_ > attnStep2.valueDb_;
}

AttnBank::AttnStep::AttnStep(int valueDb, int switchId)
   :
   valueDb_(valueDb),
   switchId_(switchId)
{
}

AttnBank::AttnBank(AT34970 & scu)
   :
   scu_(scu)
{
}

AttnBank::~AttnBank()
{
}

// initialize bank
void AttnBank::addStepLevelDbAndSwitchId(int stepDb, int switchId)
{
   attnStepContainer_.push_back(AttnStep(stepDb, switchId));

   // keep sorted from high to low stepDb value as assumed
   // by setAttnLevelDb method

   sort(attnStepContainer_.begin(), attnStepContainer_.end(),
	attnStepHigherDbValue);

}

void AttnBank::addStrobeSwitchId(int switchId)
{
   strobeSwitchIds_.push_back(switchId);
}

// set SCU switches according to desired attn level
void AttnBank::setAttnLevelDb(int levelDb)
{
   AssertMsg(attnStepContainer_.size() > 0, "attn steps not initialized");

   int localLevelDb = levelDb;
   for (vector<AttnStep>::iterator it = attnStepContainer_.begin();
	it != attnStepContainer_.end(); ++it)
   {
      AttnStep & step = *it;

      if (localLevelDb >= step.valueDb_)
      {
	 scu_.close(step.switchId_);
	 localLevelDb -= step.valueDb_;
      }
      else
      {
	 scu_.open(step.switchId_);
      }
   }

   // strobe the controller to force the atten changes
   // to take place
   strobe();

   // send error if settings didn't work
   if (localLevelDb != 0) 
   {
      stringstream strm;
      strm << scu_.function()
	   << " unable to set attenuator level "
	   << levelDb << ", unset attn amount: " << localLevelDb
	   << " dB";
      throw GPIBError(strm.str());
   }

}

// query the SCU switches to read the attn level
int AttnBank::getAttnLevelDb()
{
   AssertMsg(attnStepContainer_.size() > 0, "attn steps not initialized");

   int levelDb(0);
   for (vector<AttnStep>::iterator it = attnStepContainer_.begin();
	it != attnStepContainer_.end(); ++it)
   {
      AttnStep & step = *it;

      if (scu_.isClosed(step.switchId_))
      {
	 levelDb += step.valueDb_;
      }
   }

   return levelDb;

}

// strobe group supply pin(s) to make attn control setting
// changes take effect

void AttnBank::strobe()
{
   AssertMsg(strobeSwitchIds_.size() > 0, "strobe switch ids not initialized");

   for (vector<int>::iterator it = strobeSwitchIds_.begin();
	it != strobeSwitchIds_.end(); ++it)
   {
      int strobeSwitchId = *it;
      scu_.strobe(strobeSwitchId);
   }
}