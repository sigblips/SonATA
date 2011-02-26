/*******************************************************************************

 File:    AutoselectAntsActStrategy.cpp
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



#include "AutoselectAntsActStrategy.h" 
#include "ActivityWrappers.h"
#include "ActParameters.h"
#include "SchedulerParameters.h"

AutoselectAntsActStrategy::AutoselectAntsActStrategy(
                               Scheduler *scheduler,
                               Site *site, 
                               const NssParameters &nssParameters,
                               int verboseLevel):
   ActivityStrategy(scheduler, site, nssParameters, verboseLevel),
   nssParameters_(nssParameters)
{
}

AutoselectAntsActStrategy::~AutoselectAntsActStrategy()
{
}

void AutoselectAntsActStrategy::startInternalHook()
{
}



Activity * AutoselectAntsActStrategy::getNextActivity(
   NssComponentTree *nssComponentTree)
{
   string methodName("AutoselectAntsActStrategy::getNextActivity");

   /* 
      Using a relatively high obs frequency (> 1420 MHz)
      when selecting ants by SEFD value eliminates almost all
      of them, so for now use a low fixed frequency to
      get a usable set of ants.
      Once the SEFD lookup works at higher freqs, use the actual
      obs freq instead.
   */
#ifdef SEFD_HIGH_FREQ_WORKING
   setMaxObsFreqInParams(nssParameters_);
#else
   const double sefdFreqMhz(1420);
   nssParameters_.sched_->setEndObsFreqMhz(sefdFreqMhz);
#endif

   // set activity type (for logging purposes)
   string actType("autoselectants");
   if (! nssParameters_.act_->setActivityType(actType))
   {
      throw SseException("tried to set invalid activity type: " + actType
                         + " in " + methodName + "\n", __FILE__, __LINE__);
   }    

   Activity *act = NewAutoselectAntsActWrapper(
      getNextActId(), this,
      nssComponentTree, nssParameters_,
      getVerboseLevel());

   return act;
}


bool AutoselectAntsActStrategy::moreActivitiesToRun()
{
   // Only running 1 activity
   
   return false;
}

bool AutoselectAntsActStrategy::okToStartNewActivity()
{
   // This needs to be true for the strategy to wrap up

   return true;
}


