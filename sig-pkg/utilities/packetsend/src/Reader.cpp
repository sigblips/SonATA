/*******************************************************************************

 File:    Reader.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/src/Reader.cpp,v 1.6 2009/05/25 00:21:19 kes Exp $

#include <fftw3.h>
#include "PsErr.h"
#include "Reader.h"

namespace sonata_packetsend {

ReaderTask *ReaderTask::instance = 0;

ReaderTask *
ReaderTask::getInstance(string name_, int prio_)
{
	static Lock l;
	l.lock();
	Assert(!instance);
	if (!instance)
		instance = new ReaderTask("Reader", READER_PRIO);
	l.unlock();
	return (instance);
}

ReaderTask *
ReaderTask::getInstance()
{
	static Lock l;
	l.lock();
//	Assert(instance);
	if (!instance)
		instance = new ReaderTask("Reader", READER_PRIO);
	l.unlock();
	return (instance);
}

/**
 * Create the receiver task.
*/
ReaderTask::ReaderTask(string name_, int prio_): Task(name_, prio_),
		inQ("reader")
{
}

/**
 * Destroy the receiver task.
 *
 * Description:\n
 * 	Frees all resources before destroying the task.
*/
ReaderTask::~ReaderTask()
{
	for (int32_t i = 0; i < READ_BUFFERS; ++i)
		delete buf[i];
}

void
ReaderTask::extractArgs()
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
ReaderTask::routine()
{
	Error err;

	args = Args::getInstance();
	Assert(args);
	pktList = static_cast<BeamPacketList *> (Task::args);
	Assert(pktList);
	formatter = FormatterTask::getInstance();
	Assert(formatter);
	formatQ = formatter->getQueue();
	Assert(formatQ);

	loopcnt = args->getRepeat();

	// send the formatter task the address of the return queue
	formatQ->send(&inQ);

	// set up the free buffers
	if (args->sendChannels())
		pktSize = sizeof(ChannelDataPacket);
	else
		pktSize = sizeof(BeamDataPacket);
	for (int32_t i = 0; i < READ_BUFFERS; ++i) {
		buf[i] = new ReadBuffer(pktSize);
		Assert(buf[i]);
		if (err = inQ.send(buf[i]))
			Fatal(err);
	}
	fin.open(args->getInputFile().c_str());
	Assert(fin.is_open());

	// read the file a large block at a time, then extract the packets
	// and queue them to the packet sender
	bool done = false;
	while (!done) {
		ReadBuffer *b;
		if (err = inQ.recv(reinterpret_cast<void **> (&b)))
			Fatal(err);
		Assert(b);
		std::cout << "b = " << b << ", packets = " << b->packets << std::endl;

		// first pass through the file, do everything normally
		if (!fin.eof()) {
			fin.read((char *) b->buf, b->size);
			int32_t count = fin.gcount();
			b->packets = count / pktSize;
			formatQ->send(b);
		}
		else if (loopcnt-- > 0) {
			// we've done the entire file, so now we need to resend the
			// buffer.
			formatQ->send(b);
		}
		else
			done = true;
	}
	// send a null packet to terminate the transfer
	formatQ->send(0);
	return (0);
}

}