/*******************************************************************************

 File:    Activity.h
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


// SSE Activity abstract base class

#ifndef _activity_h
#define _activity_h

#include <ace/OS.h>
#include <machine-dependent.h>
#include <string>
#include "ActivityId.h"

class ActivityStrategy;

using std::string;

class Activity
{

 public:
    Activity(ActivityId_t id,
	     ActivityStrategy* activityStrategy);

  // start the activity
  virtual bool start() = 0;
  virtual void stop();
  virtual void destroy() = 0;
  virtual ~Activity();
  virtual ActivityId_t getId() const;
  virtual const string getStatus() const;

 protected:

  ActivityStrategy* getActivityStrategy();

 private:

  const ActivityId_t id_;
  ActivityStrategy* activityStrategy_;

  // disable copy construction & assignment.
  // don't define these
  Activity(const Activity& rhs);
  Activity& operator=(const Activity& rhs);
};
 

#endif