/*******************************************************************************

 File:    Alarm.cpp
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
// Alarm class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Alarm.cpp,v 1.3 2009/05/24 23:23:15 kes Exp $
//
#include <sys/time.h>
#include "Sonata.h"
#include "Alarm.h"

namespace sonata_lib {

Alarm::Alarm(string name_): aname(name_)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
}

Alarm::~Alarm()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condition);
}

void
Alarm::alarm(NssDate& alarmTime_)
{
	Error err = pthread_mutex_lock(&mutex);
	if (err)
		Fatal(err);

	timespec time;
	time.tv_sec = alarmTime_.tv_sec;
	time.tv_nsec = alarmTime_.tv_usec * 1000;
	while ((err = pthread_cond_timedwait(&condition, &mutex, &time))) {
		if (err == ETIMEDOUT)
			break;
		if (err == EINTR)
			continue;
		Fatal(err);
	}
	pthread_mutex_unlock(&mutex);
}

}
