/*******************************************************************************

 File:    ObserveActivityStatus.cpp
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



#include "ObserveActivityStatus.h" 
#include "Assert.h"


ObserveActivityStatus::ObserveActivityStatus()
    : statusEnum_(ACTIVITY_CREATED)
{

}

ObserveActivityStatus::~ObserveActivityStatus()
{
}

static const char * StatusText[ObserveActivityStatus::ACTIVITY_STATUS_MAXIMUM] =
{
	"Act Created",
	"Act Started",
	"Pend Data Coll",
	"Data Coll",
        "Data Coll Done",
	"Pend Sig Det",
	"Sig Det",
	"Sig Det Done",
	"Act Aborted",
	"Act Failed",
	"Act Stopping",
	"Act Stopped",
	"Act Complete",
};

ObserveActivityStatus::StatusEnum ObserveActivityStatus::get() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);
   
   return statusEnum_;
}

string ObserveActivityStatus::getString() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   Assert (statusEnum_ >= 0 && statusEnum_ < ACTIVITY_STATUS_MAXIMUM);
   return StatusText[statusEnum_];
}

void ObserveActivityStatus::set(StatusEnum status)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   Assert (status >= 0 && status < ACTIVITY_STATUS_MAXIMUM);
   statusEnum_ = status;
}

