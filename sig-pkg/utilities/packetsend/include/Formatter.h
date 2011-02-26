/*******************************************************************************

 File:    Formatter.h
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

// Formatter task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/Formatter.h,v 1.2 2008/08/12 21:28:12 kes Exp $
//
#ifndef FORMATTER_H_
#define FORMATTER_H_

#include <fstream>

#include "System.h"
#include "Args.h"
#include "BeamPacketList.h"
#include "PsStruct.h"
#include "PsTypes.h"
#include "Queue.h"
//#include "Reader.h"
#include "Sender.h"
#include "Task.h"

using namespace sonata_lib;

namespace sonata_packetsend {

class FormatterTask: public Task {
public:
	static FormatterTask *getInstance(string name_, int prio_);
	static FormatterTask *getInstance();
	~FormatterTask();

	Queue *getQueue() { return (&inQ); }
	void setup();

private:
	static FormatterTask *instance;

	Args *args;							// command-line args
	BeamPacketList *pktList;			// list of beam packets
	Queue inQ;							// input queue
	Queue *readQ;						// input queue for reader task
	Queue *sendQ;						// input queue for sender task
	SenderTask *sender;					// sender task

	// methods
	void extractArgs();
	void *routine();

	// hidden
	FormatterTask(string name_, int prio_);

	// forbidden
	FormatterTask(const FormatterTask&);
	FormatterTask &operator=(const FormatterTask&);
};

}
#endif /*FORMATTER_H_*/