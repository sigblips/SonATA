/*******************************************************************************

 File:    Relay.cpp
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
// Packet relay task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetrelay/src/Relay.cpp,v 1.5 2009/07/17 03:31:02 kes Exp $

#include "Relay.h"
#include "PrErr.h"

namespace sonata_packetrelay {

RelayTask *RelayTask::instance = 0;

RelayTask *
RelayTask::getInstance(string name_, int prio_)
{
	static Lock l;
	l.lock();
	Assert(!instance);
	if (!instance)
		instance = new RelayTask("Relay", RELAY_PRIO);
	l.unlock();
	return (instance);
}

RelayTask *
RelayTask::getInstance()
{
	static Lock l;
	l.lock();
//	Assert(instance);
	if (!instance)
		instance = new RelayTask("Relay", RELAY_PRIO);
	l.unlock();
	return (instance);
}

/**
 * Create the receiver task.
*/
RelayTask::RelayTask(string name_, int prio_): Task(name_, prio_)
{
}

/**
 * Destroy the receiver task.
 *
 * Description:\n
 * 	Frees all resources before destroying the task.
*/
RelayTask::~RelayTask()
{
}

void
RelayTask::extractArgs()
{
}

/**
 * Receive and process incoming packets.
 *
 * Description:\n
 * 	Processes packets from the Udp socket and passes them onto the
 *	beam object for processing.
 */
void *
RelayTask::routine()
{
	pkt = new BeamPacket;

	args = Args::getInstance();
	Assert(args);

	uint64_t packets = args->getPackets();
	printHeader = args->printHeader();
	printCount = args->getPrintCount();

	if (args->rawPackets())
		printHeader = false;

	// create the input socket
	input = new Udp("input", (sonata_lib::Unit) 0);
	Assert(input);
	in = args->getInput();
	IpAddress any = "0.0.0.0";
	IpAddress addr;
	memcpy(addr, args->getInputAddr(), sizeof(addr));
	int32_t port = args->getInputPort();
	input->setAddress(any, port, false, true);
	Assert(!input->addGroup(addr));

	// set the buffer size
	size_t size = RCV_BUFSIZE;
	input->setRcvBufsize(size);

	if (args->toFile()) {
		fout.open(args->getOutputFile().c_str(), ios::binary);
		Assert(fout.is_open());
	}
	else {
		// create the output socket
		out = args->getOutput();
		inet_aton(out.addr, &out.inaddr);

		output = new Udp(std::string("transmit"), (sonata_lib::Unit) 0);
		if (!output)
			Fatal(ERR_CCC);
		output->setAddress(out.addr, out.port);
		// set the buffer size
		size = SND_BUFSIZE;
		output->setSndBufsize(size);
	}

	// we may be working with raw packets
	void *data;
	if (args->rawPackets()) {
		size = args->getRawPacketSize();
		data = new uint8_t[size];
	}
	else {
		size = pkt->getPacketSize();
		data = pkt->getPacket();
	}
	// read incoming packets and retransmit them
	while (true) {
#if RELAY_TIMING
		uint64_t t0 = getticks();
#endif
		if (packets && packetCount >= packets)
			break;
		Error err = input->recv(data, size);
		if (err) {
			switch (err) {
			case EAGAIN:
			case EINTR:
				continue;
			default:
				Fatal(err);
				break;
			}
		}
		if (printHeader && (packetCount % printCount == 0)) {
			ATADataPacketHeader hdr = pkt->getHeader();
			hdr.demarshall();
			hdr.printHeader();
		}
		++packetCount;

		// got a packet; route it
#if RELAY_TIMING
		uint64_t t1 = getticks();
#endif
		if (fout.is_open())
			fout.write((char *) data, size);
		else {
			output->send(data, size, out.addr, out.port);
			if (args->cloneData()) {
				output->send(data, size, out.addr, out.port + 1);
			}
		}
#if RELAY_TIMING
		uint64_t t2 = getticks();
#endif
#if RELAY_TIMING
		volatile float64_t rdt = elapsed(t1, t0);
		volatile float64_t sdt = elapsed(t2, t1);
		volatile float64_t tdt = elapsed(t2, t0);
//		int cpu = sched_getcpu();
//		Assert(!cpu);
		uint64_t t3 = getticks();
#endif
	}
	fout.flush();
	fout.close();
	::exit(0);
	return (0);
}

}