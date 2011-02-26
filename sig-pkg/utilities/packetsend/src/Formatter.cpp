/*******************************************************************************

 File:    Formatter.cpp
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
// Packetsend reader task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/src/Formatter.cpp,v 1.3 2008/08/12 21:36:42 kes Exp $

#include <fftw3.h>
#include "PsErr.h"
#include "Formatter.h"

namespace sonata_packetsend {

FormatterTask *FormatterTask::instance = 0;

FormatterTask *
FormatterTask::getInstance(string name_, int prio_)
{
	static Lock l;
	l.lock();
	Assert(!instance);
	if (!instance)
		instance = new FormatterTask("Formatter", FORMATTER_PRIO);
	l.unlock();
	return (instance);
}

FormatterTask *
FormatterTask::getInstance()
{
	static Lock l;
	l.lock();
//	Assert(instance);
	if (!instance)
		instance = new FormatterTask("Formatter", FORMATTER_PRIO);
	l.unlock();
	return (instance);
}

/**
 * Create the receiver task.
*/
FormatterTask::FormatterTask(string name_, int prio_): Task(name_, prio_),
		inQ("reader")
{
}

/**
 * Destroy the receiver task.
 *
 * Description:\n
 * 	Frees all resources before destroying the task.
*/
FormatterTask::~FormatterTask()
{
}

void
FormatterTask::extractArgs()
{
}

/**
 * Read input packets from file.
 *
 * Description:\n
 * 	Reads packets into a large buffer,then places them in a queue for
 * 	the sender task.\n
 * Notes:\n
 * 	The sender queue should be large enough to hold an entire buffer
 * 	of packets, to avoid stalling the reader.
 */
void *
FormatterTask::routine()
{
	Error err;

	args = Args::getInstance();
	Assert(args);
	pktList = static_cast<BeamPacketList *> (Task::args);
	Assert(pktList);
//	reader = ReaderTask::getInstance();
//	Assert(reader);
//	readQ = reader->getQueue();
//	Assert(readQ);
	sender = SenderTask::getInstance();
	Assert(sender);
	sendQ = sender->getQueue();
	Assert(sendQ);

	// kludge: first message from the reader is a pointer to the
	// reader's queue
	if (err = inQ.recv(reinterpret_cast<void **> (&readQ)))
			Fatal(err);
	Assert(readQ);

	// wait for full buffers from the reader
	bool done = false;
	while (!done) {
		ReadBuffer *b;
		if (err = inQ.recv(reinterpret_cast<void **> (&b)))
			Fatal(err);
		if (!b) {
			done = true;
			continue;
		}

		for (int32_t i = 0; i < b->packets; ++i) {
			ATAPacket *pkt = pktList->alloc();
			Assert(pkt);
			ATADataPacket *p =
				(ATADataPacket *) ((char *) b->buf + i * pkt->getPacketSize());
			pkt->putPacket(p);
			pkt->demarshall();
			sendQ->send(pkt);
		}
		readQ->send(b);
	}
	// send a null packet to terminate the transfer
	sendQ->send(0);
}

}