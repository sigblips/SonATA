/*******************************************************************************

 File:    Task.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Task.cpp,v 1.6 2009/05/24 23:32:51 kes Exp $
//

#include <sched.h>
#include <signal.h>
#include "Err.h"
#include "Semaphore.h"
#include "Task.h"
#include "Types.h"

namespace sonata_lib {

#ifdef notdef
class SignalHandlerTask: public Task {
private:
	static SignalHandlerTask *SignalHandlerTask::instance_;

	SignalHandlerTask() : Task("SignalHandlerTask", true)
	{
		blockSignals();
	}

public:
	static SignalHandlerTask *instance()
	{
		if (!instance_)
			instance_ = new SignalHandlerTask();
		return (instance_);
	}

	virtual void
	setupSignals()
	{
		Error err;
		struct sigaction act;
		sigset_t sigSet;

		sigemptyset(&sigSet);
		sigaddset(&sigSet, SIGQUIT);
		sigaddset(&sigSet, SIGINT);
		sigaddset(&sigSet, SIGPIPE);
		sigaddset(&sigSet, SIGTSTP);
		if (err = pthread_sigmask(SIG_UNBLOCK, &sigSet, NULL))
			Fatal(err);

		act.sa_handler = handleSignal;
		sigaction(SIGQUIT, &act, NULL);
		sigaction(SIGINT, &act, NULL);
		sigaction(SIGPIPE, &act, NULL);
		sigaction(SIGTSTP, &act, NULL);
	}

	virtual void
	handleSignal(int sig)
	{
		Fatal(ERR_CHS);
	}

	virtual void *
	routine(void *)
	{
		Semaphore forever("SigHandler", 0);

		while (1)
			forever.wait();

		return (0);
	}
};

//
// There is exactly one instance of the signal handler task
//
SignalHandlerTask *SignalHandlerTask::instance_ = 0;
SignalHandlerTask *Task::signalHandlerTask = 0;
#endif

//
// base class for all DX tasks
//
Task::Task(string tname_, int prio_, bool realtime_, bool detach_): args(0),
		log(0), cancel(false), detached(detach_), realtime(realtime_),
		running(false), cancelable(false), prio(prio_), tid(0), tname(tname_)
{
}

Task::~Task()
{
	if (running)
		kill();
}

pthread_t
Task::id()
{
	return (tid);
}

int
Task::priority()
{
	if (!running)
		Fatal(ERR_TNR);
	return (prio);
}

void
Task::priority(int prio_)
{
	if (!running)
		Fatal(ERR_TNR);

	sched_param param;
	Error err = pthread_attr_getschedparam(&attr, &param);
	if (err)
		Fatal(err);
	param.sched_priority = prio_;
	err = pthread_attr_setschedparam(&attr, &param);
	if (err)
		Fatal(err);
	prio = prio_;
}

void
Task::name(string& name_)
{
	name_ = tname;
}

void
Task::detach()
{
	if (!running)
		Fatal(ERR_TNR);

	if (!detached) {
		detached = true;
		Error err = pthread_detach(tid);
		if (err)
			Fatal(ERR_CDT);
	}
}

void
Task::kill()
{
	pthread_t cid = pthread_self();

	if (pthread_equal(cid, tid))
		exit((void *) NormalExit);
	else
		cancel = true;
}

void *
Task::join()
{
	void *rval;
	Error err = pthread_join(id(), &rval);
	if (err)
		Fatal(err);
	return (rval);
}

void
Task::start(void *args_)
{
	if (running)
		Fatal(ERR_TAR);
	running = true;

#ifdef notdef
	// start the signal handler if necessary
	if (!signalHandlerTask) {
		signalHandlerTask = SignalHandlerTask::instance();
		signalHandlerTask->start(0);
	}
#endif

	args = args_;

	// set up the attributes for the thread
	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	Error err;
	if (detached) {
		err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (err)
			Fatal(err);
	}
	struct sched_param param;
	if (realtime) {
		err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		if (err)
			Fatal(err);
		param.sched_priority = prio;
		err = pthread_attr_setschedparam(&attr, &param);
		if (err)
			Fatal(err);
	}
	else {
		err = pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
		if (err)
			Fatal(err);
	}
	err = pthread_create(&tid, &attr, startup, this);
	if (err)
		Fatal(err);
	int policy;
	pthread_getschedparam(tid, &policy, &param);
	param.sched_priority = prio;
	err = pthread_setschedparam(tid, SCHED_FIFO, &param);
	if (err)
		Fatal(err);
	pthread_getschedparam(tid, &policy, &param);
	param.sched_priority = prio;

	Debug(DEBUG_TASK, (int32_t) tid, tname.c_str());
}

void
Task::yield()
{
	if (running)
		sched_yield();
}

void
Task::exit(void *rval)
{
	running = false;
	pthread_exit(rval);
}

void
Task::blockSignals()
{
	sigset_t sigSet;
	sigemptyset(&sigSet);
	sigaddset(&sigSet, SIGQUIT);
	sigaddset(&sigSet, SIGINT);
	sigaddset(&sigSet, SIGPIPE);
	sigaddset(&sigSet, SIGTSTP);
	sigaddset(&sigSet, SIGUSR1);
	Error err = pthread_sigmask(SIG_BLOCK, &sigSet, NULL);
	if (err)
		Fatal(err);
}

//
// startup: provides a static function
//
void *
Task::startup(void *args)
{
	Task *task = (Task *) args;

	void *rval;
	rval = task->routine();
	task->exit(rval);
	return (0);
}

}