/*******************************************************************************

 File:    Scheduler.h
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
  Scheduler - runs a series of one or more ActivityStrategies
*/

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <ace/Atomic_Op.h>
#include <ace/Synch.h>
#include "Condition.h"
#include "ActivityId.h"
#include "DebugLog.h"  // keep this early in the headers for VERBOSE macros
#include "Timeout.h"
#include "MutexBool.h"
#include <map>
#include <list>
#include <vector>
#include <string>
#include <deque>

class Activity;
class Site;
class ActivityStrategy;
class NssParameters;
class NssComponentTree;
class Scheduler;

using std::string;
using std::map;
using std::list;
using std::deque;

class ActivityEntry
{
public:
  typedef Activity *(*NewActivityFunction)(
     ActivityId_t id,
     ActivityStrategy* activityStrategy,
     NssComponentTree *nssComponentTree,
     const NssParameters& nssParameters,
     int verboseLevel);

  enum TargetUse { UsesTargets, NoTargets };

  ActivityEntry(TargetUse targetUse, NewActivityFunction newActivity):
     targetUse_(targetUse),
     newActivity_(newActivity)
  {
  }
  TargetUse targetUse_;  
  NewActivityFunction newActivity_;

};


class ActStratEntry
{
public:
  typedef ActivityStrategy *(*NewActStratFunction)(
     Scheduler *scheduler,
     Site *site,
     const NssParameters& nssParameters,
     int verboseLevel);

  ActStratEntry(NewActStratFunction newActStratFunction):
     newActStrat_(newActStratFunction)
  {
  }
  NewActStratFunction newActStrat_;

};


class Scheduler
{
public:
  enum Status {UNINITIALIZED, INITIALIZED, FAILED, SHUTDOWN_STARTED,
	       SHUTDOWN
  };
  typedef BaseCondition<Status> StateCondition;

  static Scheduler *instance();

  virtual ~Scheduler();
  void strategyComplete(ActivityStrategy *strategy, bool failed);

  virtual int setVerboseLevel(int verboseLevel);
  virtual int getVerboseLevel() const;

  void addActivityType(
     const string & activityType,
     ActivityEntry::TargetUse targetUse,
     ActivityEntry::NewActivityFunction newActivity);

  Activity * getNewActivityFromFactory(
     ActivityId_t id,
     ActivityStrategy *schedulerStrategy,
     NssComponentTree *nssComponentTree,
     const NssParameters& nssParameters,
     int verboseLevel);

  ActivityEntry::TargetUse getActivityTypeTargetUse(
     const string & activityType);

  void addActStrategyType(
     const string & actStratType,
     ActStratEntry::NewActStratFunction newActStrat);

  ActivityStrategy * getNewActStrategyFromFactory(
     const string & actStratName,
     const NssParameters& nssParameters,
     int verboseLevel);

  bool isValidActStrategyType(const string & actStratName);
  vector<string> getValidStrategyNameList(const string &strategyNamesArg);

  virtual int waitForShutdown();
  virtual void wrapUp();
  virtual const char* actlist();
  virtual void stop();
  void quit();
  virtual const char *getStatus() const ;

  virtual void failed();
  virtual void setSite(Site *site);
  void runStrategies(NssParameters &nssParameters, const string &taskNames);

  void acquireStrategyActive();
  void releaseStrategyActive();
  bool isStrategyActive();

  void updateSystemStatusSummary();
  void updateSystemConfigStatusFile(const NssParameters &nssParameters);

private:

  // methods

  // This is a Singleton, so only allow one instance
  Scheduler();

  void addStrategy(ActivityStrategy* strategy);
  void addStrategyNamesToQueue(const vector<string> & strategyNames);
  void startNextStrategyOnQueue(NssParameters &nssParameters);
  void removeCompletedStrategies();
  virtual void finishShutdown();
  string getPendingStrategiesStatus() const;
  void clearStrategyQueue();
  bool isStrategyQueueEmpty();
  string getNextStrategyFromQueue();

  // data

  static Scheduler* instance_;
  Site* site_;

  typedef map<string, const ActivityEntry *> ActFactoryMap;
  ActFactoryMap actFactoryMap_;

  typedef map<string, const ActStratEntry *> ActStratFactoryMap;
  ActStratFactoryMap actStratFactoryMap_;

  string actTypesString_;
  string systemStatusString_;

  typedef list<ActivityStrategy*> StrategyList;
  StrategyList schedulerStrategies_;
  deque<string> actStrategyQueue_;

  mutable string schedulerStatus_;
  Scheduler::StateCondition state_;   // current state
  ACE_Atomic_Op < ACE_Mutex, int > verboseLevel_;
  mutable ACE_Recursive_Thread_Mutex actStrategyQueueMutex_;

  // This variable is acquired when strategy is running
  // and released when next strategy is free to run
  MutexBool strategyRunning_;

  Timeout<Scheduler> systemStatusRepeatTimer_;  
  NssParameters *nssParameters_;

};

#endif /* SCHEDULER_H */