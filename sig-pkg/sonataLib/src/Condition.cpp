/*******************************************************************************

 File:    Condition.cpp
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
// Condition class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Condition.cpp,v 1.3 2008/11/12 02:17:39 kes Exp $
//
#include "Condition.h"

namespace sonata_lib {

Condition::Condition(string name_): cname(name_)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&condition);
	pthread_mutex_destroy(&mutex);
}

Error
Condition::wait(Lock *lock_)
{
	if (lock_)
		return (pthread_cond_wait(&condition, &lock_->mutex));
	else
		return (pthread_cond_wait(&condition, &mutex));
}

}
