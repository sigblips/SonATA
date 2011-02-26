/*******************************************************************************

 File:    Semaphore.cpp
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
// Semaphore class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Semaphore.cpp,v 1.3 2008/11/19 00:00:25 kes Exp $
//
#include <sys/time.h>
#include "Sonata.h"
#include "Err.h"
#include "Semaphore.h"

namespace sonata_lib {

Semaphore::Semaphore(string name_, int count_): sname(name_)
{
	sem_init(&sem, 0, count_);
}

Semaphore::~Semaphore()
{
	sem_destroy(&sem);
}

Error
Semaphore::wait(int milliseconds_)
{
	if (milliseconds_ < 0)
		return (sem_wait(&sem));
	else if (milliseconds_ > 0) {
		int seconds = milliseconds_ / MSEC_PER_SEC;
		milliseconds_ -= seconds * MSEC_PER_SEC;
		int microseconds = milliseconds_ * USEC_PER_MSEC;
		timeval time;
		gettimeofday(&time, NULL);
		if ((time.tv_usec += microseconds) >= USEC_PER_SEC) {
			time.tv_sec++;
			time.tv_usec -= USEC_PER_SEC;
		}
		timespec timeout;
		timeout.tv_sec = time.tv_sec + seconds;
		timeout.tv_nsec = time.tv_usec * NSEC_PER_USEC;
		return (sem_timedwait(&sem, &timeout));
	}
	else
		return (sem_trywait(&sem));
}	

int32_t
Semaphore::getCount()
{
	int val = 0;
	sem_getvalue(&sem, &val);
	return (val);
}
		
void
Semaphore::signal()
{
	sem_post(&sem);
}
	
void
Semaphore::name(string& name_)
{
	name_ = sname;
}

}
