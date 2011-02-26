/*******************************************************************************

 File:    ActParameters.h
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


#ifndef ActParameters_H
#define ActParameters_H

#include "SeekerParameterGroup.h"
#include "RangeParameter.h"
#include "TargetId.h"
#include "ActivityId.h"
#include <string>
#include <mysql.h>

class ActParametersInternal;
class Scheduler;



class ActParameters : public SeekerParameterGroup
{

 public:

   enum CandidateArchiveOption
   {
      ARCHIVE_ALL_CANDIDATES, ARCHIVE_CONFIRMED_CANDIDATES,
      ARCHIVE_NO_CANDIDATES
   };
   
   enum FreqInvertOption
   {
      FREQ_INVERT_RF, FREQ_INVERT_IF, FREQ_INVERT_ALWAYS,
      FREQ_INVERT_NEVER
   };
   
   enum PrimaryBeamPositionType
   {
      PRIMARY_BEAM_POS_TARGET_ID, PRIMARY_BEAM_POS_COORDS
   };

   enum NullType
   {
      NULL_NONE, NULL_AXIAL, NULL_PROJECTION
   };

   ActParameters(string command);
   ActParameters(const ActParameters& rhs);
   ActParameters& operator=(const ActParameters& rhs);
   ~ActParameters();

   void setScheduler(Scheduler* scheduler);
   Scheduler* getScheduler();

   // immediate commands
   const char *status() const;  // return activity status
   void clearfollowuplist();
   void addfollowupactid(ActivityId_t actId);

   // retrieve parameter values
   int getStartDelaySecs() const;
   const string getActivityType() const;
   bool setActivityType(const string & actTypeName);

   TargetId getTargetIdForBeam(const string &beamName) const;
   void setTargetIdForBeam(const string &beamName, TargetId targetId) const;
   int getPrimaryTargetId() const;
   int setPrimaryTargetId(int targetId) const;

   const string getSiteName() const;
   double getDiffUtcUt1() const;
   const string getEarthEphemFilename() const;
   CandidateArchiveOption getCandidateArchiveOption() const;
   bool compareDxDataProducts() const;
   bool emailActStatus() const;
   const string getEmailActStatusAddressList() const;
   int getPreviousActivityId() const;
   void setPreviousActivityId(int activityId);
   bool useWatchdogTimers() const;
   int getSigDetWaitFactor() const;
   int getComponentReadyTimeoutSecs() const;
   int getTscopeReadyTimeoutSecs() const;
   FreqInvertOption getFreqInvertOption() const;

   bool checkVarianceErrorLimits();
   double varianceErrorLowerLimit();
   double varianceErrorUpperLimit();

   bool checkVarianceWarnLimits();
   double varianceWarnLowerLimit();
   double varianceWarnUpperLimit();

   double getDiskPercentFullWarningLimit();
   double getDiskPercentFullErrorLimit();

   double getRecentRfiAgeLimitDays();

   int getDataCollCompleteTimeoutOffsetSecs();
   double getDoneSendingCwCohSigsTimeoutFactorPercent();

   bool pointPrimaryBeam() const;
   void setPointPrimaryBeam(bool mode);

   PrimaryBeamPositionType getPrimaryBeamPositionType() const;
   void setPrimaryBeamPositionType(PrimaryBeamPositionType posType);

   double getPrimaryBeamRaHours() const;
   double getPrimaryBeamDecDeg() const;

   void setPrimaryBeamRaHours(double raHours);
   void setPrimaryBeamDecDeg(double decDeg);

   NullType getOffActNullType() const;
   bool useMultiTargetNulls() const;
   double getNullDepthDb() const;

   // utility methods
   void addActivityType(const string & actTypeName);

 protected:
   void addParameters();
   virtual void addAllImmedCmdHelp();

   RangeParameter<TargetId> * findTargetIdParamForBeamname(const string & beamName) const;

 private:
   ActParametersInternal *internal_;

};


#endif // ActParameters_H