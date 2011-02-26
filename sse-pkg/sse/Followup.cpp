/*******************************************************************************

 File:    Followup.cpp
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

/*++ Followup.cpp - description of file contents
 * PURPOSE:  
 --*/

#include "Followup.h"
#include "NssParameters.h"
#include "ActParameters.h"
#include "DbParameters.h"
#include "SseException.h"
#include "Assert.h"
#include <sstream>
#include <map>

using namespace std;


// set up the Singleton

Followup * Followup::instance_ = 0;
static ACE_Recursive_Thread_Mutex singletonLock_;

Followup * Followup::instance()
{
   // Use "double-check locking optimization" design 
   // pattern to prevent initialization race condition.

   if (instance_ == 0)
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(singletonLock_);
      if (instance_ == 0)
      {
         instance_ = new Followup();
      }
   }
   
   return instance_;
}

Followup::Followup()
{
}

Followup::~Followup()
{
}

bool Followup::followupIsPending() const
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);
 
    return !followupActIds_.empty();
}

void Followup::prepareParameters(NssParameters* nssParameters)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

    Assert(! followupActIds_.empty());

    FollowupActIdList::iterator followupActIdPtr = followupActIds_.begin();
    ActivityId_t prevActId = *followupActIdPtr;
    
    // erase the id from the static list before the query, in case the
    // latter throws an error
    followupActIds_.erase(followupActIdPtr);
    
    nssParameters->restore(nssParameters->db_->getDb(),
			 prevActId);
    nssParameters->act_->setPreviousActivityId(prevActId);
    
    string followupActType = findFollowupActType(
       nssParameters->act_->getActivityType());
    if (! nssParameters->act_->setActivityType(followupActType))
    {
       throw SseException("tried to set invalid followup activity type: " 
			  + followupActType + "\n",__FILE__,__LINE__);
    }    
}

void Followup::setFollowupActType(const string & origActType,
				  const string & followupActType)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

    followupTypesMap_[origActType] = followupActType;
}

const string Followup::findFollowupActType(const string & origActType)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

    FollowupTypesMap::const_iterator index = 
	followupTypesMap_.find(origActType);
    if (index != followupTypesMap_.end())
    {
	return (*index).second;
    }
    else
    {
	return origActType;
    }
    
}

// Verify that every followup name is also listed
// as a key (ie, that the chain of followups is unbroken)
// Throws SseException if errors are found.

void Followup::validateFollowupActivitySequences()
{
   bool valid(true);
   stringstream errorStrm;
   
   errorStrm << "Followup::validateFollowupActSequences():\n";

   for(FollowupTypesMap::const_iterator index =
	  followupTypesMap_.begin(); 
       index != followupTypesMap_.end(); ++index)
   {
      const string & followupName = (*index).second;

      if (followupTypesMap_.find(followupName) ==
	  followupTypesMap_.end())
      {
	 errorStrm << " Error: followupType '" << followupName
		   << "' is not listed as a key in the followup table."
		   << endl;
	 valid = false;
      }
   }
    
   if (!valid)
   {
      throw SseException(errorStrm.str(),__FILE__,__LINE__);
   }

}

void Followup::addActivityId(ActivityId_t actId)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

    followupActIds_.push_back(actId);
}


void Followup::clearActivityIdList()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

    followupActIds_.clear();
}

string Followup::getStatus()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

    stringstream strm;
    
    strm <<"Followup Act Ids pending: ";

    if (followupActIds_.empty())
    {
	strm << "none";
    }
    else 
    {
	for (FollowupActIdList::iterator it = followupActIds_.begin();
	     it !=  followupActIds_.end(); ++it)
	{
	    ActivityId_t actId = *it;
	    strm << actId << " ";
	}
    }
    strm << endl;
    
    return strm.str();
}

