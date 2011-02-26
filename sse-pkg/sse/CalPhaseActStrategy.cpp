/*******************************************************************************

 File:    CalPhaseActStrategy.cpp
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



#include "CalPhaseActStrategy.h" 
#include "SchedulerParameters.h"
#include "TscopeParameters.h"
#include <cmath>

CalPhaseActStrategy::CalPhaseActStrategy(
                               Scheduler *scheduler,
                               Site *site, 
                               const NssParameters &nssParameters,
                               int verboseLevel):
   CalActStrategy(scheduler, site, nssParameters, verboseLevel)
{
}

CalPhaseActStrategy::~CalPhaseActStrategy()
{
}

void CalPhaseActStrategy::loadCalFreqListMhz()
{
   double calFreqMhz(-1);

   if (getNssParameters().sched_->isAutoTuneRf())
   {
      /*
        Select appropriate freq from begin/end freq params.
        Use 1420 if it's available.
        TBD: Avoid perm rfi bands.
      */

      double beginFreqMhz(getNssParameters().sched_->getBeginObsFreqMhz());
      double endFreqMhz(getNssParameters().sched_->getEndObsFreqMhz());

      if (beginFreqMhz > endFreqMhz)
      {
         throw SseException(
            "CalPhase: scheduler parameter error: beginfreq > endfreq\n",
            __FILE__, __LINE__);
      }

      double preferredFreqMhz(1420);
      if (preferredFreqMhz >= beginFreqMhz && preferredFreqMhz <= endFreqMhz)
      {
         calFreqMhz = preferredFreqMhz;
      }
      else
      {
         // TBD check perm rfi mask for freqs to avoid

         calFreqMhz = (beginFreqMhz + endFreqMhz) / 2.0;
      }
   }
   else 
   {
      // User tune, use value already in params

      /*
        Assumes that both tuning C & D are in use,
        and enforces that they have the same freq.
        TBD: rework to use configuration data to determine
        which tunings were actually requested.
        
      */
      string tuningCName("tuningc");
      double tuningCFreqMhz(getNssParameters().tscope_->getTuningSkyFreqMhz(
                               tuningCName));
   
      string tuningDName("tuningd");
      double tuningDFreqMhz(getNssParameters().tscope_->getTuningSkyFreqMhz(
                               tuningDName));

      // check that both freqs are the same
      double freqTolMhz(0.001);
      if (fabs(tuningCFreqMhz - tuningDFreqMhz) > freqTolMhz)
      {
         throw SseException(string("CalPhase: tunings c & d must be set")
                            + " to the same frequency\n", __FILE__,__LINE__);
      }

      calFreqMhz = tuningCFreqMhz;

      /*
        The base class sets the NssParameters tuneOffsetMhz param to zero.
        However in this case the user explicitly chose the desired cal freq, so 
        adjust that freq by the original tune offset in order for it to
        match the tuning that will be used for later observing.
      */

      calFreqMhz += getOrigTuneOffsetMhz();
   }

   addCalFreqMhz(calFreqMhz);
}

string CalPhaseActStrategy::getCatalogName()
{
   return "calphase";
}

string CalPhaseActStrategy::getCalType()
{
   return "phase";
}

int CalPhaseActStrategy::getNumCalCycles()
{
   return 2;
}

int CalPhaseActStrategy::getCalTimeSecs(double obsFreqMhz,
                                        double targetFluxJy)
{
   return getPhaseCalTimeSecs(obsFreqMhz, targetFluxJy);
}