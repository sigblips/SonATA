/*******************************************************************************

 File:    InputTask.h
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
// Input task
//
// Task to handle processing of input channel packets
//
#ifndef _InputTaskH
#define _InputTaskH

#include <sseDxInterface.h>
#include "System.h"
#include "Activity.h"
//#include "ChannelPacketList.h"
#include "Msg.h"
#include "QTask.h"
#include "State.h"

using namespace sonata_lib;

namespace dx {

struct InputArgs {
	dx::Unit unit;					// unit

	InputArgs(): unit(dx::UnitNone) {}
	InputArgs(dx::Unit unit_): unit(unit_) {}
};

class InputTask: public QTask {
public:
	InputTask(string name_);
	~InputTask();

private:
	dx::Unit unit;
	Activity *activity;				// ptr to current activity
	Channel *channel;

	MsgList *msgList;
	State *state;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	void startActivity(Msg *msg);
	void processPacket(Msg *msg);

	// forbidden
	InputTask(const InputTask&);
	InputTask& operator=(const InputTask&);
};

}

#endif