/*******************************************************************************

 File:    RWLock.cpp
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

/**
* Read/write lock class definition
*/
#include "Err.h"
#include "RWLock.h"

namespace sonata_lib {

RWLock::RWLock(string name_): rwname(name_)
{
	pthread_rwlockattr_t attr;

	pthread_rwlockattr_init(&attr);
	pthread_rwlock_init(&rwlock, &attr);
	pthread_rwlockattr_destroy(&attr);
}

RWLock::~RWLock()
{
	pthread_rwlock_destroy(&rwlock);
}

/**
* acquire the lock for reading
*
* Notes:\n
*	The data protected by the lock should not be modified.
*/
void
RWLock::rdlock()
{
	Error err = pthread_rwlock_rdlock(&rwlock);
	if (err)
		Fatal(err);
}

/**
* acquire the lock for writing
*
* Notes:\n
*	This call will block if the lock is already held for reading or writing.
*/
void
RWLock::wrlock()
{
	Error err = pthread_rwlock_wrlock(&rwlock);
	if (err)
		Fatal(err);
}

/**
* Release the lock
*
* Notes:\n
*	Used when either read or write lock is held.
*/
void
RWLock::unlock()
{
	Error err = pthread_rwlock_unlock(&rwlock);
	if (err)
		Fatal(err);
}

/**
* name the lock
*/
void
RWLock::name(const string& name_)
{
	rwname = name_;
}

}
