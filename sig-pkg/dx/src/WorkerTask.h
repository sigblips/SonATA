/*******************************************************************************

 File:    WorkerTask.h
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
// Ethernet worker task
//
// Worker task which performs polyphase filter and cornerturn processing
// to produce output subchannels.  A number of these tasks may be created
// to handle multiple input streams.
//
// $Heaer: $
//
#ifndef _WorkerTaskH
#define _WorkerTaskH

#include "Dfb.h"
#include "System.h"
#include "Channel.h"
#include "Lock.h"
#include "Msg.h"
#include "Spectrometer.h"
#include "State.h"

using namespace sonata_lib;

namespace dx {

struct WorkerArgs {
	dx::Unit unit;						// unit
	Queue *collectionQ;					// collection task queue

	WorkerArgs(): unit(dx::UnitNone), collectionQ(0) {}
	WorkerArgs(dx::Unit unit_, Queue *collectionQ_): unit(unit_),
			collectionQ(collectionQ_) {}
};

struct WorkerTiming {
	uint64_t iterations;				// total # of times run
	float buildArrays;
	float dfb;
	float spectrometer;
	float total;

	WorkerTiming(): iterations(0), buildArrays(0), dfb(0),
			spectrometer(0), total(0) {}
};

class WorkerTask: public QTask {
public:
	WorkerTask(string name_);
	~WorkerTask();

	void startActivity(Activity *act);
	void stopActivity(Activity *act);

private:
	dx::Unit unit;
	int32_t activityId;
	uint32_t sampleBufSize;
	ComplexFloat32 *sampleBuf;
	ComplexFloat32 *rOut[MAX_SUBCHANNELS];
	ComplexFloat32 *lOut[MAX_SUBCHANNELS];
	Activity *activity;
	Channel *channel;
	WorkerTiming timing;

	MsgList *msgList;
	Queue *collectionQ;
	Spectrometer *spectrometer;
	State *state;

	Lock wlock;

	// methods
	void lock() { wlock.lock(); }
	void unlock() { wlock.unlock(); }

	void extractArgs();
	void handleMsg(Msg *msg);
	void startBaseline();
	void startDataCollection();
	void completeDataCollection();
	void processData(Msg *msg);
	void stopActivity(Msg *msg);
	void buildOutputArray(ComplexFloat32 **array, ComplexFloat32 *buf,
			int32_t samples, int32_t subchannels, int32_t usable);
};

}

#endif
