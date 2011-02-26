/*******************************************************************************

 File:    ArchiverConnectionTask.cpp
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
// Task to a initiate a connection to the archiver
//
// This task attempts to connect to the archiver whenever the
// configuration is changed or there is no archiver connection.
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverConnectionTask.cpp,v 1.3 2009/02/13 03:06:29 kes Exp $
//
#include <errno.h>
#include <unistd.h>
#include "System.h"
#include "ArchiverConnectionTask.h"
#include "Err.h"
#include "State.h"
#include "Timer.h"

namespace dx {

ArchiverConnectionTask *ArchiverConnectionTask::instance = 0;

ArchiverConnectionTask *
ArchiverConnectionTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ArchiverConnectionTask("ArchiverConnection");
	l.unlock();
	return (instance);
}

ArchiverConnectionTask::ArchiverConnectionTask(string tname_):
		QTask(tname_, ARCHIVER_CONNECTION_PRIO),
		retrySleep(ARCHIVER_RETRY_SLEEP_TIME)
{
}

ArchiverConnectionTask::~ArchiverConnectionTask()
{
}

void
ArchiverConnectionTask::extractArgs()
{
#if ASSIGN_CPUS
	// assign the task processor affinity in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	cpu_set_t affinity;
	CPU_ZERO(&affinity);
	int32_t n = 0;
	if (nCpus > 2) {
		// remove affinity for cpu 1
		++n;
	}
	if (nCpus > 3) {
		// remove affinity for cpu 2
		++n;
	}
	// assign affinity
	for (int32_t i = n; i < nCpus; ++i)
		CPU_SET(i, &affinity);
	pid_t tid = gettid();
	int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
	Assert(rval >= 0);
#endif
	// extract startup parameters
	ArchiverConnectionArgs *connectionArgs
			= static_cast<ArchiverConnectionArgs *> (args);
	Assert(connectionArgs);
	archiver = static_cast<Tcp *> (connectionArgs->archiver);
	Assert(archiver);

	state = State::getInstance();
	Assert(state);
}

void
ArchiverConnectionTask::handleMsg(Msg *msg)
{
	Error err;
	switch (msg->getCode()) {
	case InitiateConnection:
		err = contact();
		if (err)
			Fatal(err);
		break;
	default:
		break;
	}
}

Error
ArchiverConnectionTask::contact()
{
#ifdef notdef
	const string hostname(state->getArchiverHost());
	Debug(DEBUG_ARCHIVE, 0, hostname.c_str());
	int32_t port = state->getArchiverPort();
	archiver->setAddress(hostname.c_str(), port);

	Error err;
	while ((err = archiver->establish())) {
		Timer timer;
		timer.sleep(retrySleep);
	}
#else
	Error err = 0;
	do {
		Timer timer;
		timer.sleep(retrySleep);

		const string hostname(state->getArchiverHost());
		Debug(DEBUG_ARCHIVE, 0, hostname.c_str());
		int32_t port = state->getArchiverPort();
		archiver->setAddress(hostname.c_str(), port);
		err = archiver->establish();
	} while (err);
#endif

	Debug(DEBUG_ARCHIVE, 0, "archiver connected");
	return (0);
}

}