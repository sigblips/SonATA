/*******************************************************************************

 File:    Lock.cpp
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

//
// Lock class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Lock.cpp,v 1.3 2008/11/18 23:56:44 kes Exp $
//

#include <stdio.h>
#include <sys/time.h>
#include "Sonata.h"
#include "Err.h"
#include "Lock.h"

namespace sonata_lib {

Lock::Lock(string name_, LockType type_): mname(name_)
{
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	// set the type of mutex
	switch (type_) {
	case NormalLock:
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
		break;
	case RecursiveLock:
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		break;
	default:
		Fatal(ERR_ILT);
	}
	// set priority inheritance
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
		

//	attr.__mutexkind = PTHREAD_MUTEX_ERRORCHECK_NP;

	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

Lock::~Lock()
{
	pthread_mutex_destroy(&mutex);
}

//
// lock: claim the specified lock
//
// Notes:
//		If the lock is recursive (can be locked multiple times by
//		the same thread) and is already locked, the use count is
//		simply incremented.
//		A warning: count cannot be used to tell whether the lock
//		is owned, because its value can change during execution
//		due to preemption.  Instead, we use the value of owner to
//		determine whether or not the current thread already owns
//		the lock.
void
Lock::lock()
{
	// either the lock is not owned, or it is owned by another
	// thread, so wait for it
#if LOCK_TIMING
	uint64_t t0 = getticks();
#endif
	Error err = pthread_mutex_lock(&mutex);
	if (err)
		Fatal(err);
#if LOCK_TIMING
	uint64_t t1 = getticks();
	++timing.locks;			
	float t = elapsed(t1, t0);
	if (t > timing.maxWait)
		timing.maxWait = t;
	timing.wait += t;
#endif
}

//
// trylock: try to claim the specified lock
//
// Returns: 1 if lock is owned, 0 if lock is not owned.
//
bool
Lock::trylock()
{
	return (!pthread_mutex_trylock(&mutex));
}

void
Lock::unlock()
{
	int err = pthread_mutex_unlock(&mutex);
	if (err)
		Fatal(err);
}

void
Lock::name(string& name_)
{
	name_ = mname;
}

}