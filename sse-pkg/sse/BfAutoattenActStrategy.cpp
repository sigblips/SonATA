/*******************************************************************************

 File:    BfAutoattenActStrategy.cpp
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


#include "BfAutoattenActStrategy.h" 
#include "ActivityWrappers.h"
#include "ActParameters.h"
#include "TscopeParameters.h"
#include "ArrayLength.h"

static const char * ActType = "bfautoatten";

BfAutoattenActStrategy::BfAutoattenActStrategy(Scheduler *scheduler,
                                     Site *site, 
                                     const NssParameters &nssParameters,
                                     int verboseLevel):
   BfActStrategy(scheduler, site, nssParameters, ActType, verboseLevel)
{
   const double autoattenSkyFreqMhz(1420);
   setTuningFreqInParams(autoattenSkyFreqMhz);
}

BfAutoattenActStrategy::~BfAutoattenActStrategy()
{
}

void BfAutoattenActStrategy::setTuningFreqInParams(double freqMhz)
{
   string methodName("BfAutoattenActStrategy::setTuningFreqInParams");

   // Set freq on all tunings (this should do no harm for tunings
   // that are not enabled).
   // TBD get tuning names from TscopeParameters

   const char *tunings[] = {"tuninga", "tuningb", "tuningc", "tuningd"};
   for (int i=0; i<ARRAY_LENGTH(tunings); ++i)
   {
      if (! getNssParameters().tscope_->setTuningSkyFreqMhz(
             tunings[i], freqMhz))
      {
         throw SseException("error trying to set skyfreq on tscope tuning: " 
                            + string(tunings[i]) + " in " + methodName + "\n",
                            __FILE__,__LINE__);
      }
   }
}


Activity * BfAutoattenActStrategy::createNewActivity(
   NssComponentTree *nssComponentTree)
{
   Activity *act = NewBeamformerAutoattenActWrapper(
      getNextActId(), this,
      nssComponentTree, getNssParameters(),
      getVerboseLevel());

   return act;
}
