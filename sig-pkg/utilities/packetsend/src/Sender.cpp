/*******************************************************************************

 File:    Sender.cpp
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
// Sender task class
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/src/Sender.cpp,v 1.14 2009/05/25 00:21:42 kes Exp $
//
#include "PsErr.h"
#include "Sender.h"

namespace sonata_packetsend {

SenderTask *SenderTask::instance = 0;

/**
 * Packet transmitter; singleton class.
 *
 * Description:\n
 *	Controls output of the channelizer by serializing packets to
 * 	ensure that they go out in sequential order.
*/
SenderTask *
SenderTask::getInstance(string name_, int prio_)
{
	static Lock l;
	l.lock();
	Assert(!instance);
	if (!instance)
		instance = new SenderTask("Sender", SENDER_PRIO);
	l.unlock();
	return (instance);
}

SenderTask *
SenderTask::getInstance()
{
	static Lock l;
	l.lock();
//	Assert(instance);
	if (!instance)
		instance = new SenderTask("Sender", SENDER_PRIO);
	l.unlock();
	return (instance);
}

/**
 * Create the sender task.
*/
SenderTask::SenderTask(string name_, int prio_): Task(name_, prio_),
		nchan(1), burst(0), delay(0), seq(0), src(DEFAULT_SRC),
		code(ATADataPacketHeader::XLINEAR),  inQ("SenderQ", OUTPUT_COUNT),
		connection(0), args(0), pktList(0)
{
}

SenderTask::~SenderTask()
{
}

void
SenderTask::extractArgs()
{
}

/**
 * Write output packets to multicast.
 *
 * Description:\n
 * 	Gets a packet from the input queue, adds the correct time, polarization
 * 	and source, then sends the packet to the multicast address.\n
 * Notes:\n
 * 	The sender queue should be large enough to hold an entire buffer
 * 	of packets, to avoid stalling the reader.
 */
void *
SenderTask::routine()
{
	Error err;

	args = Args::getInstance();
	Assert(args);
	pktList = static_cast<BeamPacketList *> (Task::args);
	Assert(pktList);

	reseq = args->resequence();
	src = args->getSrc();
	code = args->getPol();
	base = args->getOutput();
	burst = args->getBurst();
	delay = args->getDelay();
	nchan = args->getNchan();
	resetPol = args->resetPolarization();

	// convert the base ip address to integer
	inet_aton(base.addr, &base.inaddr);

	connection = new Udp(std::string("sender"), (sonata_lib::Unit) 0);
	if (!connection)
		Fatal(ERR_CCC);
	connection->setAddress(base.addr, base.port);
	size_t size = SEND_BUFSIZE;
	connection->setSndBufsize(size);

	uint64_t n = 0;
	bool done = false;
	// loop waiting for packets
	timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	while (!done) {
		for (int32_t i = 0; i < burst; ++i) {
			ATAPacket *pkt;
			if (err = inQ.recv(reinterpret_cast<void **> (&pkt)))
				Fatal(ERR_ERP);
			// zero packet means end of data
			if (!pkt) {
				done = true;
				break;
			}
			ATADataPacketHeader& hdr = pkt->getHeader();
			if (reseq) {
				if (seq)
					hdr.flags |= ATADataPacketHeader::DATA_VALID;
				hdr.seq = ++seq;
			}
			hdr.src = src;
			if (resetPol) hdr.polCode = code;
			timespec t;
			clock_gettime(CLOCK_REALTIME, &t);
			hdr.absTime = (uint64_t) t.tv_sec << 32 | t.tv_nsec;
			in_addr tmp;
			IpAddress ipAddr;
			for (int j = 0; j < nchan; ++j) {
				tmp.s_addr = htonl(ntohl(base.inaddr.s_addr) + j);
				strcpy(ipAddr, inet_ntoa(tmp));
				hdr.chan = j;
//				pkt->putHeader(hdr);
				if (err = send(connection, pkt, ipAddr, base.port)) {
					Fatal(err);
				}
				++n;
			}
			pktList->free((BeamPacket *) pkt);
		}
		if (delay)
			usleep(delay);
	}
	clock_gettime(CLOCK_REALTIME, &end);
	printStats(n, start, end);
	::exit(1);
}

Error
SenderTask::send(Udp *udp, ATAPacket *pkt, const IpAddress& ipAddr,
		int32_t port)
{
	if (!udp)
		return (ERR_NC);
	return (udp->send(pkt->getPacket(), pkt->getPacketSize(), ipAddr, port));
}

void
SenderTask::printStats(uint64_t n, const timespec& start, const timespec& end)
{
	float64_t dt = end.tv_sec - start.tv_sec;
	int32_t nsec;
	if ((nsec = end.tv_nsec - start.tv_nsec) < 0)
		nsec += 1000000000;
	dt += (float64_t) nsec / 1e9;
	float bytes;
	if (args->sendChannels())
		bytes = n * sizeof(ChannelDataPacket);
	else
		bytes = n * sizeof(BeamDataPacket);
	float64_t rate = (8 * bytes / dt) / 1e6;
	std::cout << n << " packets sent in " << dt << " sec" << std::endl;
	std::cout << "Total = " << bytes << " bytes, rate = " << rate << " Mbps";
	std::cout << std::endl;
}

}
