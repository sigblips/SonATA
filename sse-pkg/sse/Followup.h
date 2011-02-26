/*******************************************************************************

 File:    Followup.h
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

/*****************************************************************
 * Followup.h - declaration of functions defined in Followup.h
 * PURPOSE:  
 *****************************************************************/

#ifndef FOLLOWUP_H
#define FOLLOWUP_H

#include <ace/Synch.h>
#include <machine-dependent.h>
#include "DxList.h"
#include "ActivityId.h"
#include <list>
#include <string>
#include <map>


using std::string;
using std::list;
using std::map;

class NssParameters;

// This is a Singleton class that stores an activity
// for subsequent followup.  It also stores a 
// map of activity types that are followups for
// other activity types.

// key = orig act type, value = followup act type
typedef map <string, string> FollowupTypesMap;

class Followup 
{
public:
  static Followup * instance();

  virtual ~Followup();
  virtual void addActivityId(ActivityId_t actId);
  virtual bool followupIsPending() const;
  virtual void prepareParameters(NssParameters* nssParameters);
  virtual const string findFollowupActType(const string & origActType);

  // sets the followup activity for a given activity
  void setFollowupActType(const string & origActType,
			  const string & followupActType);

  void validateFollowupActivitySequences();

  string getStatus();
  void clearActivityIdList();

 protected:

  // only allow one instance
  Followup();

 private:

  typedef list<ActivityId_t> FollowupActIdList;

  static Followup * instance_;
  FollowupActIdList followupActIds_;
  mutable ACE_Recursive_Thread_Mutex objectMutex_;
  FollowupTypesMap followupTypesMap_;

};

#endif /* FOLLOWUP_H */