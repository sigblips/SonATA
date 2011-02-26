/*******************************************************************************

 File:    Reader.h
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

// Reader task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/Reader.h,v 1.4 2008/09/16 21:17:43 kes Exp $
//
#ifndef READER_H_
#define READER_H_

#include <fstream>

#include "System.h"
#include "Args.h"
#include "BeamPacketList.h"
#include "ChannelPacketList.h"
#include "Formatter.h"
#include "PsStruct.h"
#include "PsTypes.h"
#include "Queue.h"
#include "Sender.h"
#include "Task.h"

using namespace sonata_lib;
using std::ifstream;

namespace sonata_packetsend {

// forward declaration
struct ReadBuffer;

class ReaderTask: public Task {
public:
	static ReaderTask *getInstance(string name_, int prio_);
	static ReaderTask *getInstance();
	~ReaderTask();

	Queue *getQueue() { return (&inQ); }
	void setup();

private:
	static ReaderTask *instance;

	int32_t pktSize;					// size of a data packet
	int32_t loopcnt;					// # of additional buffers to send
//	std::streamsize size;				// buffer size
	ifstream fin;						// input file
	ReadBuffer *buf[READ_BUFFERS];		// input buffers

	Args *args;							// command-line args
	BeamPacketList *pktList;			// list of packets
	Queue inQ;							// input queue
	Queue *formatQ;						// input queue for formatter task
	FormatterTask *formatter;			// sender task

	// methods
	void extractArgs();
	void *routine();

	// hidden
	ReaderTask(string name_, int prio_);

	// forbidden
	ReaderTask(const ReaderTask&);
	ReaderTask &operator=(const ReaderTask&);
};

}
#endif /*READER_H_*/