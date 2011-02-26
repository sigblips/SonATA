/*******************************************************************************

 File:    ActivityStrategy.h
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

/*
  Strategy for controlling activities.
  This is a base class.
*/

#ifndef ACTIVITYSTRATEGY_H
#define ACTIVITYSTRATEGY_H

#include <ace/Task_T.h>
#include <ace/Synch.h>
#include <ace/Token.h>
#include <ace/Atomic_Op_T.h>

#include "Id.h"
#include "Activity.h"
#include "TargetId.h"
#include "TargetIdSet.h"
#include "NssParameters.h"
#include "DebugLog.h" // keep this early in the headers for VERBOSE macros
#include "Timeout.h"
#include "TscopeList.h"
#include <string>
#include <vector>

using std::vector;
using std::string;

class WorkStrategySetVerbose;
class WorkStart;
class WorkShutdown;
class WorkStrategyStop;
class WorkStrategyStopStrategy;
class WorkDataCollectionComplete;
class WorkActivityComplete;
class WorkFoundConfirmedCandidates;

class Scheduler;
class Site;
class NssComponentTree;
class NssParameters;
class SchedulerParameters;
class SiteView;

class ActivityStrategy : public ACE_Task < ACE_MT_SYNCH >
{
 public:
   ActivityStrategy(Scheduler *scheduler,
		    Site *site,
		    const NssParameters &nssParameters,
                    int verboseLevel);
   virtual ~ActivityStrategy();

   virtual string getActivityStatus() const;
   virtual bool isStrategyComplete() const;
   virtual int setVerboseLevel(int verboseLevel);
   
   virtual int svc(void);
   virtual int start();
   virtual void shutdown();
   virtual void stop();
   virtual void wrapUp();  // complete current acts & any followups
   virtual void dataCollectionComplete(Activity *activity);
   virtual void foundConfirmedCandidates(Activity *activity);
   virtual void activityComplete(Activity *activity, bool failed);

   friend class WorkStrategySetVerbose;
   friend class WorkStart;
   friend class WorkShutdown;
   friend class WorkStrategyStop;
   friend class WorkStrategyWrapUp;
   friend class WorkDataCollectionComplete;
   friend class WorkActivityComplete;
   friend class WorkFoundConfirmedCandidates;
   friend class WorkAttemptToStartNextActivity;
   friend class WorkContinueWithAnyMoreActivities;

 protected:
   // Subclasses must define these
   virtual Activity *getNextActivity(NssComponentTree *nssComponentTree) = 0;
   virtual bool moreActivitiesToRun() = 0;
   virtual bool okToStartNewActivity() = 0;

   // Hook methods (do nothing in this class, subclasses may override)
   virtual void startInternalHook();
   virtual void setVerboseLevelInternalHook(int verboseLevel);
   virtual void dataCollectionCompleteInternalHook(Activity *activity);
   virtual void activityCompleteInternalHook(Activity *activity, bool failed);
   virtual void startNextActivityHook(Activity *activity);
   virtual void cleanUpActivityHook(Activity *activity);
   virtual void repeatStrategyHook();
   virtual void strategyCleanupHook();

   virtual int getVerboseLevel() const;
   virtual void setStrategyComplete(bool complete);
   virtual bool isStopRequested() const;
   virtual bool isWrapUpRequested() const;
   virtual bool isRestartPauseActive() const;
   virtual void continueWithAnyMoreActivities();
   virtual void attemptToStartNextActivity();
   virtual bool startNextActivity();
   virtual void strategyCleanup(bool hasFailed);
   virtual void strategyFailed();
   virtual void strategySuccessful();
   virtual void repeatStrategy();
   virtual void restartComponents();

   /*
     "Internal" methods, i.e., these are 
       used to process work messages.
    */
   virtual void startInternal();
   virtual int setVerboseLevelInternal(int verboseLevel);
   virtual void shutdownInternal();
   virtual void stopInternal();
   virtual void wrapUpInternal();
   virtual void dataCollectionCompleteInternal(Activity *activity);
   virtual void foundConfirmedCandidatesInternal(Activity *activity);
   virtual void activityCompleteInternal(Activity *activity, bool failed);

   virtual void cleanUpActivity(Activity *activity);

   virtual void validateChosenTargets(const NssParameters & nssParams,
				      bool *allTargetsAvailable, 
                                      bool *allTargetsInPrimaryFov);

   virtual bool areAllTargetsAvailable(vector<TargetId> targetIds);

   virtual void throwTargetIdsNotFoundException(const vector<TargetId> & 
                                                targetIdsNotFound);

   virtual void logUnavailableTargets(
      const vector<TargetId> & targetsNotVisible, 
      const vector<TargetId> & targetsTooCloseToSetting);

   virtual void printTargetIdsToStream(ostream &strm, const string & title,
				      const vector<TargetId> & targetIds);

   virtual bool isTargetTooCloseToSetting(time_t currentTime, time_t setTime);

   virtual bool isTargetVisible(
      double ra2000Rads, double dec2000Rads, double pmRaMasYr, double pmDecMasYr,
      double parallaxMas, time_t currentTime, double lmstRads,
      time_t *riseTime, time_t *setTime);

   virtual void logRiseSetTimes(TargetId targetId, time_t riseTime, time_t setTime);

   virtual void getTscopePrimaryBeamCoords(
      TscopeList &tscopeList,
      double *ra2000Hours, double *dec2000Deg);

   virtual string getMostRecentFailedActivityInfoFromDb();
   virtual void sendErrorEmail(const string & msg);
   virtual void getRunningActivityIds(ActivityIdList & activityIdList);
   virtual int numberOfRunningActivities();
   virtual void stopAllActivities();
   virtual void insertActivity(Activity *activity);
   virtual void eraseActivity(Activity *activity);
   virtual void sendWorkAttemptToStartNextActivityMsg();
   virtual void sendWorkContinueWithAnyMoreActivitiesMsg();

   virtual ActivityId_t getNextActId();

   virtual void setMaxObsFreqInParams(NssParameters &nssParams);

 private:

   void sendWorkStartMsg();

   typedef std::list<Activity*> ActivityList;
   int numberActivitiesStarted_;
   bool strategyComplete_;
   // true when strategyActive is released to allow next strategy to run
   bool schedulerReleased_;
   NssParameters nssParameters_;
   ACE_Atomic_Op < ACE_Mutex, int > verboseLevel_;

   // set to true when strategy has stopped
   ACE_Atomic_Op<ACE_Token, bool> stop_;
   ACE_Atomic_Op<ACE_Token, bool> wrapUp_;
   ACE_Atomic_Op < ACE_Mutex, int > messageCount_;
   int numberThreads_;
   ACE_Barrier barrier_;
   Scheduler *scheduler_;
   Site *site_;
   ActivityList activities_;
   int strategyRepeatCount_;
   int sequentialFailedActivitiesCount_;
   int maxSequentialFailedActivities_;
   int restartPauseSecs_;
   int failedStartCount_;
   bool waitingForTscopeReady_;
   int tscopeReadyWaitIntervalSecs_;
   int tscopeReadyMaxFailures_;
   Timeout<ActivityStrategy> restartTimeout_;
   bool finalCleanupAlreadyRun_;
   SiteView *siteView_;
   mutable ACE_Recursive_Thread_Mutex stratComplMutex_;
   mutable ACE_Recursive_Thread_Mutex activitiesMutex_;


   // disable copy construction & assignment.
   // don't define these
   ActivityStrategy(const ActivityStrategy& rhs);
   ActivityStrategy& operator=(const ActivityStrategy& rhs);
  
};

#endif