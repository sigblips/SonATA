/*******************************************************************************

 File:    Task.h
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
// Task class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Task.h,v 1.6 2009/05/24 23:17:14 kes Exp $
//
#ifndef _TaskH
#define _TaskH

#include <pthread.h>
#include <string>
#include "Sonata.h"
#include "Queue.h"

namespace sonata_lib {

#define SCHED_POLICY				(SCHED_FIFO)

// base class for system tasks

// forward declaration
class Log;

class SignalHandlerTask;

class Task {
public:
	Task(string name_, int prio_, bool realtime_ = true,
			bool detach_ = true);
	virtual ~Task();

	pthread_t id();
	int priority();
	void priority(int prio_);
	void name(string& name_);
	Queue *getInputQueue() { return (0); }
	const char *getName() { return (tname.c_str()); }

	void detach();
	void kill();
	void *join();

	void start(void *args_ = 0);

protected:
	void *args;
	Log *log;

	void exit(void *);
	void blockSignals();
	virtual void *routine() = 0;
	virtual void extractArgs() = 0;
	static void *startup(void *args);

	void yield();
	bool testCancel() { return (cancel); }

private:
	bool cancel;
	bool detached;
	bool realtime;
	bool running;
	bool cancelable;
	int prio;
	pthread_attr_t attr;
	pthread_t tid;
	string tname;

	// forbidden
	Task(const Task&);
	Task& operator=(const Task&);
};

}

#endif