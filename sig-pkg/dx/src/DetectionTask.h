/*******************************************************************************

 File:    DetectionTask.h
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
// Signal detection task
//
// This task controls signal detection for an observation
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/DetectionTask.h,v 1.3 2009/02/22 04:48:37 kes Exp $
//
#ifndef _DetectionTaskH
#define _DetectionTaskH

#include "Activity.h"
#include "CwTask.h"
#include "DxStruct.h"
#include "Msg.h"
#include "PulseTask.h"
#include "State.h"
#include "SuperClusterer.h"
#include "Task.h"

using namespace sonata_lib;

namespace dx {

//
// startup arguments
//
struct DetectionArgs {
	Queue *controlQ;					// control task queue
#ifdef notdef
	SuperClusterer *superClusterer;		// superclusterer

	DetectionArgs(): controlQ(0), superClusterer(0) {}
	DetectionArgs(Queue *controlQ_, SuperClusterer *superClusterer_):
			controlQ(controlQ_), superClusterer(superClusterer_) {}
#else
	DetectionArgs(): controlQ(0) {}
	DetectionArgs(Queue *controlQ_): controlQ(controlQ_) {}
#endif
};

//
// This is a generic task which handles data collection
// for a given data stream.
//
class DetectionTask: public QTask {
public:
	static DetectionTask *getInstance();
	~DetectionTask();

private:
	static DetectionTask *instance;

	Activity *activity;					// current activity
	DxActivityParameters params;		// activity parameters
	Queue *controlQ;					// control task queue
	Queue *cwQ;							// CWD command queue
	Queue *pulseQ;						// PD command queue
#ifdef notdef
	SuperClusterer *superClusterer;		// super clusterer
#endif

	MsgList *msgList;
	State *state;

	// dependent task info
	CwTask *cwTask;
	PulseTask *pulseTask;

	CwArgs cwArgs;
	PulseArgs pulseArgs;

	// methods
	void extractArgs();
	void createTasks();
	void startTasks();
	void handleMsg(Msg *msg);

	void startActivity(Msg *msg);
	void stopActivity(Msg *msg, bool stopAll = false);
	void sendDetectionComplete();
	void startCwDetection();
	void startPulseDetection();
	void stopDetection();

	// hidden
	DetectionTask(string name_);

	// forbidden
	DetectionTask(const DetectionTask&);
	DetectionTask& operator=(const DetectionTask&);
};

}

#endif