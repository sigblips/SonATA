/*******************************************************************************

 File:    Scheduler.cpp
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
Scheduler controls activity strategies
*/

#include "Scheduler.h"
#include "ActivityStrategy.h"
#include "ObsActStrategy.h"
#include "CalDelayActStrategy.h"
#include "Site.h"
#include "ActParameters.h"
#include "SchedulerParameters.h"
#include "DbParameters.h"
#include "Followup.h"
#include "SseArchive.h"
#include "NssParameters.h"
#include "SseArchive.h"
#include <sstream>

using namespace std;

Scheduler* Scheduler::instance_ = 0;
static ACE_Recursive_Thread_Mutex singletonLock_;

Scheduler* Scheduler::instance()
{
   // Use "double-check locking optimization" design 
   // pattern to prevent initialization race condition.

   if (instance_ == 0)
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(singletonLock_);
      if (instance_ == 0)
      {
         instance_ = new Scheduler;
      }
   }
   return instance_;
}

Scheduler::Scheduler():
    site_(0),
    state_(INITIALIZED),
    verboseLevel_(0),
    systemStatusRepeatTimer_("status repeat timer"),
    nssParameters_(0)
{
   // update the system status every N seconds

   int initialWaitTimeSecs(10);
   int verboseLevel(0);

   systemStatusRepeatTimer_.setRepeatIntervalSecs(initialWaitTimeSecs);

   systemStatusRepeatTimer_.startTimer(
      initialWaitTimeSecs, this, &Scheduler::updateSystemStatusSummary,
      verboseLevel);
}

Scheduler::~Scheduler()
{
   delete nssParameters_;
}


int Scheduler::setVerboseLevel(int verboseLevel)
{
   verboseLevel_ = verboseLevel;

   if (site_ != 0)
   {
      site_->setVerbose(verboseLevel);
   }
  
   for (StrategyList::iterator index = schedulerStrategies_.begin();
	index != schedulerStrategies_.end();
	++index)
   {
      (*index)->setVerboseLevel(verboseLevel);
   }
  
   return verboseLevel_.value();
}

int Scheduler::getVerboseLevel() const
{
   return verboseLevel_.value();
}

void Scheduler::clearStrategyQueue()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(actStrategyQueueMutex_);

   actStrategyQueue_.clear();
}

bool Scheduler::isStrategyQueueEmpty()
{
  ACE_Guard<ACE_Recursive_Thread_Mutex> guard(actStrategyQueueMutex_);

  return actStrategyQueue_.empty();
}

string Scheduler::getNextStrategyFromQueue()
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(actStrategyQueueMutex_);
   
   Assert(! actStrategyQueue_.empty());

   string actStratName(actStrategyQueue_.front());
   actStrategyQueue_.pop_front();  // remove it
   
   return actStratName;
}

string Scheduler::getPendingStrategiesStatus() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(actStrategyQueueMutex_);

   stringstream strm;

   strm << "Tasks (strategies) pending: ";

   if (actStrategyQueue_.empty())
   {
      strm << "none";
   }
   else
   {
      for (unsigned int i=0; i<actStrategyQueue_.size(); ++i)
      {
         strm << actStrategyQueue_[i] << " ";
      }
   }
   strm << "\n";
   return strm.str();
}

const char* Scheduler::getStatus() const
{
   schedulerStatus_.erase();
   if (schedulerStrategies_.size() == 0)
   {
      schedulerStatus_ = "No activities\n";
   }
   else
   {
      for (StrategyList::const_iterator
	      index = schedulerStrategies_.begin();
	   index != schedulerStrategies_.end();
	   ++index)
      {
          string status((*index)->getActivityStatus());

          // ignore completed strategies
          if (status.find("completed") == string::npos)
          {
              schedulerStatus_ += status;
          }
      }
   }

   schedulerStatus_ += Followup::instance()->getStatus();

   schedulerStatus_ += getPendingStrategiesStatus();

   return schedulerStatus_.c_str();
}

/*
  Write the system status into a file in the archive directory.
  Old contents of the file are overwritten.
*/
void Scheduler::updateSystemStatusSummary() 
{
   string statusFilename(SseArchive::getArchiveTemplogsDir() + 
		      "/system-status-summary.txt");

   // open & truncate the file.
   ofstream strm; 
   strm.open(statusFilename.c_str(), (ios::out | ios::trunc));
   if (!strm.is_open())
   {
      // cerr << "File Open failed on " << logFilename << endl;

      // TBD error handling.  

      return;
   }

   strm << SseUtil::currentIsoDateTime() << endl << endl;
   strm << getStatus() << endl;

   strm.close();

}

/*
  Write the system configuration status into a file in the archive directory.
  Old contents of the file are overwritten.

  File format:
  -----------------------
  <systemConfig>
   <dbHost>host</dbHost>
   <dbName>name</dbName>
  </systemConfig>
  ------------------------

*/
void Scheduler::updateSystemConfigStatusFile(const NssParameters &nssParameters) 
{
   string statusFilename(SseArchive::getArchiveTemplogsDir() + 
		      "/system-config.txt");

   // open & truncate the file.
   ofstream strm; 
   strm.open(statusFilename.c_str(), (ios::out | ios::trunc));
   if (!strm.is_open())
   {
      // cerr << "File Open failed on " << logFilename << endl;

      // TBD error handling.  

      return;
   }

   strm << "<systemConfig>\n"
        << "  <dbHost>" << nssParameters.db_->getDbHost() << "</dbHost>\n"
        << "  <dbName>" << nssParameters.db_->getDbName() << "</dbName>\n"
        << "</systemConfig>\n";

   strm.close();
}

const char* Scheduler::actlist()
{
   stringstream types;
   for(ActFactoryMap::iterator index =
	  actFactoryMap_.begin();
       index != actFactoryMap_.end();
       index++)
   {
      types << " " << index->first;
   }

   // save result to object storage so it sticks around for the UI to grab
   actTypesString_ = types.str();

   return actTypesString_.c_str();
}

// returns when Scheduler is shutdown
int Scheduler::waitForShutdown()
{
   removeCompletedStrategies();
   return state_.waitWhileNe(SHUTDOWN);
}

// stop all strategies, clear any queued up ones
// TBD: are mutexes needed for either list?
void Scheduler::stop()
{
   for (StrategyList::iterator index = schedulerStrategies_.begin();
	index != schedulerStrategies_.end();
	++index) 
   {
      (*index)->stop();
   }
   
   clearStrategyQueue();
}

// wrap up all strategies
void Scheduler::wrapUp()
{
   for (StrategyList::iterator index = schedulerStrategies_.begin();
	index != schedulerStrategies_.end();
	++index)
   {
      (*index)->wrapUp();
   }
  
   clearStrategyQueue();
}

// add new activity type to the list of activity types
void
Scheduler::
addActivityType(const string &activityType,
		ActivityEntry::TargetUse targetUse,
		ActivityEntry::NewActivityFunction newActivity)
{
   VERBOSE2(getVerboseLevel() , "Adding Activity to Map\n");

   ActivityEntry *factoryUnit =
      new ActivityEntry(targetUse, newActivity);
   actFactoryMap_[activityType] = factoryUnit;

   // add the new type to the UI
   paraGlobal.act_->addActivityType(activityType);
}

Activity * Scheduler::
getNewActivityFromFactory(ActivityId_t id,
                          ActivityStrategy *schedulerStrategy,
                          NssComponentTree *nssComponentTree,
                          const NssParameters& nssParameters,
                          int verboseLevel)
{
   VERBOSE2(getVerboseLevel() , "Scheduler::getNewActivityFromFactory\n");

   ActFactoryMap::iterator index =
      actFactoryMap_.find(nssParameters.act_->getActivityType());
   AssertMsg(index != actFactoryMap_.end(), "Activity type not found");

   Activity *activity =
      index->second->newActivity_(id,
				  schedulerStrategy,
				  nssComponentTree,
				  nssParameters,
				  getVerboseLevel());
   return activity;
}

/*
  Lookup whether the given activity type uses targets or not
 */
ActivityEntry::TargetUse Scheduler::getActivityTypeTargetUse(
   const string & activityType)
{
   ActFactoryMap::iterator index = actFactoryMap_.find(activityType);
   AssertMsg(index != actFactoryMap_.end(), "Activity type not found");

   return index->second->targetUse_;
}

void Scheduler::addActStrategyType(
   const string & actStratType,
   ActStratEntry::NewActStratFunction newActStratFunc)
{
   VERBOSE2(getVerboseLevel() , "Adding ActStratType to Map\n");

   ActStratEntry *factoryUnit =
      new ActStratEntry(newActStratFunc);
   actStratFactoryMap_[actStratType] = factoryUnit;

   // add the new type to the UI
   paraGlobal.sched_->addTaskType(actStratType);
}


ActivityStrategy * Scheduler::
getNewActStrategyFromFactory(
     const string & actStratName,
     const NssParameters& nssParameters,
     int verboseLevel)
{
   VERBOSE2(getVerboseLevel() , "Scheduler::getNewActStrategyFromFactory\n");

   ActStratFactoryMap::iterator index =
      actStratFactoryMap_.find(actStratName);
   if (index == actStratFactoryMap_.end())
   {
      throw SseException("ActStrategy type not found: " + actStratName + "\n");
   }

   ActivityStrategy *actStrat =
      index->second->newActStrat_(this,
				  site_,
				  nssParameters,
				  getVerboseLevel());
   return actStrat;
}

bool Scheduler::
isValidActStrategyType(const string & actStratName)
{
   ActStratFactoryMap::iterator index =
      actStratFactoryMap_.find(actStratName);
   return (index != actStratFactoryMap_.end());
}

void Scheduler::strategyComplete(ActivityStrategy *strategy, bool failed)
{
   StrategyList::iterator index = find(schedulerStrategies_.begin(),
				       schedulerStrategies_.end(),
				       strategy);
   if (index == schedulerStrategies_.end())
   {
      index = schedulerStrategies_.begin();

      // TBD better error handling
      throw 1;
   }
   else
   {
      if (state_.value() == SHUTDOWN_STARTED)
      {
	 schedulerStrategies_.erase(index);
	 if (schedulerStrategies_.size() == 0)
	 {
	    finishShutdown();
	 }
      }
      else
      {
         if (failed)
         {
            SseMessage::log("scheduler", NSS_NO_ACTIVITY_ID,
                            SSE_MSG_ACT_STRAT_FAILED, SEVERITY_ERROR,
                            "strategy (task) failed\n",
                            __FILE__, __LINE__);
            
            Assert(nssParameters_);
            if (nssParameters_->sched_->stopOnStrategyFailure())
            {
               SseMessage::log(
                  "scheduler", NSS_NO_ACTIVITY_ID,
                  SSE_MSG_ACT_STRAT_FAILED, SEVERITY_ERROR,
                  "issuing stop: will not run any subsequent strategies (tasks)\n",
                  __FILE__, __LINE__);

               stop();

               return;
            }
         }

         if (! isStrategyQueueEmpty())
         {
            startNextStrategyOnQueue(*nssParameters_);
         }
      }

   }
}


void Scheduler::quit()
{
   static ACE_Atomic_Op <ACE_Mutex, bool> already_quit = false;
   if (already_quit == true)
   {
      return;
   }

   state_ = SHUTDOWN_STARTED;
   already_quit = true;
   stop();

   removeCompletedStrategies();

   if (schedulerStrategies_.size() == 0)
   {
      finishShutdown();
   }
}

void Scheduler::finishShutdown()
{
   state_ = SHUTDOWN;
}

void Scheduler::addStrategy(ActivityStrategy* strategy)
{
   schedulerStrategies_.push_back(strategy);
}

// remove the completed strategies from the schedulerStrategies_ list
void Scheduler::removeCompletedStrategies()
{
   if (schedulerStrategies_.size() != 0)
   {
      // Check for completed Strategies
      StrategyList::iterator index = schedulerStrategies_.begin();
      while (index != schedulerStrategies_.end())
      {
	 if((*index)->isStrategyComplete())
	 {
	    delete (*index);
	    index = schedulerStrategies_.erase(index);
	    if (schedulerStrategies_.size() == 0)
	    {
	       break;
	    }
	 }
	 else
	 {
	    index++;
	 }
      }
   }
}

void Scheduler::startNextStrategyOnQueue(NssParameters &nssParameters)
{
   if (isStrategyQueueEmpty())
   {
      throw SseException("strategy queue is empty, no strategies to run");
   }
   
   string actStratName(getNextStrategyFromQueue());

   VERBOSE2(getVerboseLevel(), "######## Creating new activityStrategy: "
            + actStratName + " ######\n");

   SseArchive::SystemLog() << "Starting new strategy: " << actStratName << endl;

   ActivityStrategy *activityStrategy =
      getNewActStrategyFromFactory(actStratName,
                                   nssParameters,
                                   getVerboseLevel());
   addStrategy(activityStrategy);
   
   activityStrategy->start();
}

vector<string> Scheduler::getValidStrategyNameList(const string &strategyNameList)
{
   /* parse & validate the strategy names*/
   string delimiter(",");
   vector<string> strategyNames(SseUtil::tokenize(strategyNameList, delimiter));

   // accumulate all errors and report them together
   stringstream badNamesStrm;
   for (unsigned int i=0; i<strategyNames.size(); ++i)
   {
      if (!isValidActStrategyType(strategyNames[i]))
      {
         badNamesStrm << strategyNames[i] << " ";
      }
   }

   if (badNamesStrm.str() != "")
   {
       stringstream strm;
       strm << "Error, can't start unknown strategy type(s): "
            << badNamesStrm.str() << "\n";

       cerr << strm.str();
       throw SseException(strm.str());
   }

   return strategyNames;
}

void Scheduler::addStrategyNamesToQueue(const vector<string> & strategyNames)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(actStrategyQueueMutex_);

   actStrategyQueue_.clear();
   for (unsigned int i=0; i<strategyNames.size(); ++i)
   {
      actStrategyQueue_.push_back(strategyNames[i]);
   }
}

/*
  Run the comma separated list of strategies in sequential order.
*/
void Scheduler::runStrategies(NssParameters &nssParameters,
                              const string &taskNameList)
{
   try {
      removeCompletedStrategies();
      
      if (strategyRunning_.get())
      {
         stringstream strm;
         
         strm << "Can't start new strategy, one is currently running\n";
         SseMessage::log("scheduler", NSS_NO_ACTIVITY_ID,
                         SSE_MSG_ACT_STRAT_FAILED, SEVERITY_ERROR,
                         strm.str(), __FILE__, __LINE__);

         cerr << strm.str();
         
         return;
      }
      
      strategyRunning_.set(true);

      string strategyNamesList(taskNameList);
      if (strategyNamesList == "tasks")
      {
         strategyNamesList = nssParameters.sched_->getTasks();
      }
      addStrategyNamesToQueue(getValidStrategyNameList(strategyNamesList));

      SseArchive::SystemLog() << "Requested strategies: " 
                              << strategyNamesList << endl;

      delete nssParameters_;
      nssParameters_ = new NssParameters(nssParameters);

      startNextStrategyOnQueue(*nssParameters_);

      VERBOSE2(getVerboseLevel(), "updateSystemConfigStatusFile\n");
      updateSystemConfigStatusFile(nssParameters);
   }
   catch (SseException &except)
   {
      stringstream strm;
      
      strm << "strategy start() failed: " << except.descrip();
      SseMessage::log("Scheduler", NSS_NO_ACTIVITY_ID,
                      except.code(), except.severity(), strm.str(),
                      except.sourceFilename(), except.lineNumber());

      strategyRunning_.set(false);
   }
}

void Scheduler::failed()
{
   state_ = FAILED;
}

// set the site_ variable
// site_ is protected, so it can be set directly

void Scheduler::setSite(Site *site)
{
   site_ = site;
}

void Scheduler::acquireStrategyActive()
{
   // do nothing
}

void Scheduler::releaseStrategyActive()
{
   strategyRunning_.set(false);
}

bool Scheduler::isStrategyActive()
{
   return strategyRunning_.get();
}

