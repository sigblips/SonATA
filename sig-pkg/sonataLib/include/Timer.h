/*******************************************************************************

 File:    Timer.h
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
// Interval timer
//
// Puts the current task to sleep for the specified number of
// milliseconds
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Timer.h,v 1.2 2008/02/25 22:35:23 kes Exp $
//
#ifndef _TimerH
#define _TimerH

#include "Semaphore.h"

namespace sonata_lib {

class Timer {
public:
	Timer(): sem("", 0) {}
	void sleep(int milliseconds_) { sem.wait(milliseconds_); }

private:
	Semaphore sem;

	// forbidden
	Timer(const Timer&);
	Timer& operator=(const Timer&);
};

}

#endif