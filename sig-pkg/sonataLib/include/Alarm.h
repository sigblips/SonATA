/*******************************************************************************

 File:    Alarm.h
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
// Alarm
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Alarm.h,v 1.2 2008/02/25 22:35:22 kes Exp $
//
#ifndef _AlarmH
#define _AlarmH

#include <pthread.h>
#include <string>
#include "Err.h"

using std::string;

namespace sonata_lib {

class Alarm {
public:
	Alarm(string name_);
	~Alarm();

	void alarm(NssDate& alarmTime_);
	void name(string& name_) { name_ = aname; }

private:
	string aname;
	pthread_cond_t condition;
	pthread_mutex_t mutex;

	// forbidden
	Alarm(const Alarm&);
	Alarm& operator=(const Alarm&);
};

}

#endif