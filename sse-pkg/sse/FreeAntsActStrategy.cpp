/*******************************************************************************

 File:    FreeAntsActStrategy.cpp
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



#include "FreeAntsActStrategy.h" 
#include "ActivityWrappers.h"
#include "ActParameters.h"

FreeAntsActStrategy::FreeAntsActStrategy(
                               Scheduler *scheduler,
                               Site *site, 
                               const NssParameters &nssParameters,
                               int verboseLevel):
   ActivityStrategy(scheduler, site, nssParameters, verboseLevel),
   nssParameters_(nssParameters)
{
}

FreeAntsActStrategy::~FreeAntsActStrategy()
{
}

void FreeAntsActStrategy::startInternalHook()
{
}



Activity * FreeAntsActStrategy::getNextActivity(
   NssComponentTree *nssComponentTree)
{
   string methodName("FreeAntsActStrategy::getNextActivity");

   // set activity type (for logging purposes)
   string actType("freeants");
   if (! nssParameters_.act_->setActivityType(actType))
   {
      throw SseException("tried to set invalid activity type: " + actType
                         + " in " + methodName + "\n", __FILE__, __LINE__);
   }    

   Activity *act = NewFreeAntsActWrapper(
      getNextActId(), this,
      nssComponentTree, nssParameters_,
      getVerboseLevel());

   return act;
}


bool FreeAntsActStrategy::moreActivitiesToRun()
{
   // Only running 1 activity
   
   return false;
}

bool FreeAntsActStrategy::okToStartNewActivity()
{
   // This needs to be true for the strategy to wrap up

   return true;
}


