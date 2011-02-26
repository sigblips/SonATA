/*******************************************************************************

 File:    CalActStrategy.h
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

#ifndef CalActStrategy_H
#define CalActStrategy_H

#include "ActivityStrategy.h"
#include "CalTargets.h"
#include <deque>

class InterpolateLinear;

class CalActStrategy : public ActivityStrategy
{
 public:
   CalActStrategy(Scheduler *scheduler,
                       Site *site,
                       const NssParameters &nssParameters,
                       int verboseLevel);

   virtual ~CalActStrategy();
   
 protected:
   // base class overrides
   virtual void startInternalHook();
   virtual Activity *getNextActivity(NssComponentTree *nssComponentTree);
   virtual bool moreActivitiesToRun();
   virtual bool okToStartNewActivity();
   virtual void activityCompleteInternalHook(Activity *activity, bool failed);

   // subclass overrides
   virtual void loadCalFreqListMhz() = 0;
   virtual string getCatalogName() = 0;
   virtual string getCalType() = 0;
   virtual int getNumCalCycles() = 0;
   virtual int getCalTimeSecs(double obsFreqMhz, double targetFluxJy) = 0;
      
   // base class util methods (not to be overridden)
   NssParameters & getNssParameters();
   void addCalFreqMhz(double freqMhz);
   double getOrigTuneOffsetMhz();
   
   // util methods
   virtual void loadCalIntegrationTimes(const string &dbTableName, double obsFreqMhz,
                                        InterpolateLinear &interpCalTimeSecs);

   virtual int computeCalTimeSecs(const string & BfCalTimeTable, 
                                  double obsFreqMhz, double targetFluxJy);

   virtual int getPhaseCalTimeSecs(double obsFreqMhz, double targetFluxJy);

 private:

   void selectCalTarget(TargetId &targetId, double &targetFluxJy);
   void prepareParameters();
   double getNextCalFreqMhz();
   void setTuningFreqInParams(double freqMhz);
   void logRemainingCalFreqs();

   NssParameters nssParameters_;
   CalTargets calTargets_;
   std::deque<double> calFreqQueueMhz_;
   double origTuneOffsetMhz_;

   // Disable copy construction & assignment.
   // Don't define these.
   CalActStrategy(const CalActStrategy& rhs);
   CalActStrategy& operator=(const CalActStrategy& rhs);

};

#endif // CalActStrategy_H