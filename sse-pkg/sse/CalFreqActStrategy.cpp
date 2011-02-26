/*******************************************************************************

 File:    CalFreqActStrategy.cpp
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



#include "CalFreqActStrategy.h" 
#include "SchedulerParameters.h"
#include "TscopeParameters.h"
#include <cmath>

CalFreqActStrategy::CalFreqActStrategy(
                               Scheduler *scheduler,
                               Site *site, 
                               const NssParameters &nssParameters,
                               int verboseLevel):
   CalActStrategy(scheduler, site, nssParameters, verboseLevel)
{
}

CalFreqActStrategy::~CalFreqActStrategy()
{
}

void CalFreqActStrategy::addCalFreqMhzIfInRange(
   double freqMhz, double minFreqMhz, double maxFreqMhz)
{
   if (freqMhz > minFreqMhz && freqMhz < maxFreqMhz)
   {
      addCalFreqMhz(freqMhz);
   }
}

void CalFreqActStrategy::loadCalFreqListMhz()
{
   if (getNssParameters().sched_->isAutoTuneRf())
   {

      double beginFreqMhz(getNssParameters().sched_->getBeginObsFreqMhz());
      double endFreqMhz(getNssParameters().sched_->getEndObsFreqMhz());

      if (beginFreqMhz > endFreqMhz)
      {
         throw SseException(
            "CalPhase: scheduler parameter error: beginfreq > endfreq\n",
            __FILE__, __LINE__);
      }

      vector<double> calFreqListMhz;

      /*
        See if we're observing the waterhole.  If so, use the
        preselected freq series to avoid RFI regions.
      */

      double waterholeStartMhz(1410);
      double waterholeEndMhz(1730);
      double edgeTolMhz(5);

      if ((beginFreqMhz >= waterholeStartMhz-edgeTolMhz)
          && (beginFreqMhz <= waterholeStartMhz+edgeTolMhz)
          && (endFreqMhz >= waterholeEndMhz-edgeTolMhz)
          && (endFreqMhz <= waterholeEndMhz+edgeTolMhz))
      {
         // observing waterhole

         calFreqListMhz.push_back(1420);
         calFreqListMhz.push_back(1421);
         calFreqListMhz.push_back(1425);
         calFreqListMhz.push_back(1430);
         calFreqListMhz.push_back(1440);
         calFreqListMhz.push_back(1470);
         calFreqListMhz.push_back(1500);
         calFreqListMhz.push_back(1620);
         calFreqListMhz.push_back(1660);
         calFreqListMhz.push_back(1730);
      }
      else 
      {
         /*
           Select freqs at various offsets to characterize band.
           TBD: Avoid perm rfi bands.
         */
         
         double calFreqMhz(beginFreqMhz);
         calFreqMhz +=1;
         calFreqListMhz.push_back(calFreqMhz);

         calFreqMhz +=1;
         calFreqListMhz.push_back(calFreqMhz);

         calFreqMhz +=5;
         calFreqListMhz.push_back(calFreqMhz);

         calFreqMhz +=10;
         calFreqListMhz.push_back(calFreqMhz);

         calFreqMhz +=20;
         calFreqListMhz.push_back(calFreqMhz);

         while (calFreqMhz < endFreqMhz)
         {
            calFreqMhz += 50;
            calFreqListMhz.push_back(calFreqMhz);
         }
      }

      for (unsigned int i=0; i<calFreqListMhz.size(); ++i)
      {
         addCalFreqMhzIfInRange(calFreqListMhz[i], 
                                beginFreqMhz-edgeTolMhz, endFreqMhz+edgeTolMhz);
      }
   }
   else
   {
      // User tune - single cal freq.

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
         throw SseException(string("CalFreq: tunings c & d must be set")
                            + " to the same frequency\n", __FILE__,__LINE__);
      }

      addCalFreqMhz(tuningCFreqMhz);
      
   }
   
}

string CalFreqActStrategy::getCatalogName()
{
   return "calphase";
}

string CalFreqActStrategy::getCalType()
{
   return "freq";
}

int CalFreqActStrategy::getNumCalCycles()
{
   return 1;
}

int CalFreqActStrategy::getCalTimeSecs(double obsFreqMhz, double targetFluxJy)
{
    return getPhaseCalTimeSecs(obsFreqMhz, targetFluxJy);
}
