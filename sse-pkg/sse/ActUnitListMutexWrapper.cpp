/*******************************************************************************

 File:    ActUnitListMutexWrapper.cpp
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



#include "ActUnitListMutexWrapper.h" 
#include "Assert.h"
#include <algorithm>

ActUnitListMutexWrapper::ActUnitListMutexWrapper()
{
}

ActUnitListMutexWrapper::~ActUnitListMutexWrapper()
{
}

ActUnitListMutexWrapper::
ActUnitListMutexWrapper(const ActUnitListMutexWrapper& rhs)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   actUnitList_ = rhs.getListCopy();
}

ActUnitListMutexWrapper & 
ActUnitListMutexWrapper::operator=(const ActUnitListMutexWrapper& rhs)
{
   if (this != &rhs)
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

      actUnitList_ = rhs.getListCopy();
   }
   
   return *this;
}

ActUnitListMutexWrapper & 
ActUnitListMutexWrapper::operator=(const ActUnitList& rhs)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   actUnitList_ = rhs;
   
   return *this;
}


ActUnitList ActUnitListMutexWrapper::getListCopy() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   return actUnitList_;
}

void ActUnitListMutexWrapper::addToList(ActivityUnit *actUnit)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   actUnitList_.push_back(actUnit);
}

void ActUnitListMutexWrapper::removeFromList(ActivityUnit *actUnit)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);
   
      // make sure actUnit is found in the list -- error if not there!
   Assert (find (actUnitList_.begin(), actUnitList_.end(), actUnit)
	   != actUnitList_.end());
   
   actUnitList_.remove(actUnit);
}

int ActUnitListMutexWrapper::listSize() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(objectMutex_);

   return actUnitList_.size();
}