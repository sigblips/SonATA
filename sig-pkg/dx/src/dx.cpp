/*******************************************************************************

 File:    dx.cpp
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
// DX entry point
//
// This file performs the initialization sequence for the DX
// 
// $Header: /home/cvs/nss/sonata-pkg/dx/src/dx.cpp,v 1.2 2009/02/24 18:54:06 kes Exp $
//
#define DEFINE_GLOBALS

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
//#include "linux-defs.h"
//#include "BWSIntTask.h"

#include "System.h"
#include "Err.h"
#include "Globals.h"
#include "Init.h"
#include "Inittab.h"
#include "Timer.h"
//#include "DxDiskFile.h"

//#include "Dx400KHzPacket.h"

//
// Initial entry point, performing initialization and system
// startup
//
int
main(int argc, char **argv)
{
	void *data;
	int i;
	timeval start, end;
	Error err;
	MemBlk *blk1, *blk2, *blk3;
	Timer timer;

#ifdef notdef
	int fd = open("/tmp/dx", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd >= 0) {
		loff_t len = 128 * 1024;
		char c[len];
		memset(c, 0, len);
		lseek64(fd, 256*1024, SEEK_SET);
		int n = write(fd, c, len);
		if (n != len)
			exit(1);
	}

	Lock l("test");
	DiskFile h;
	{
		DiskFile f("test", "/tmp/dx", &l, (loff_t) 0, 67174400);
		h = f;
	}
	DiskFile g("test2", "/tmp/dx", &l, (loff_t) 67174400, 67174400);
	size_t len = 128 * 1024;
	char c[len];
	memset(c, 0, len);
	h.writeBlk((void *) c, len, (loff_t) 128*1024);
	g.writeBlk((void *) c, len, (loff_t) 0);
#endif
		
#ifdef notdef	
	ChannelMap channels;
	Channel *first = new Channel(0, 0, 1, 1, 1.00, .5);
	Channel *second = new Channel(0, 0, 2, 2, 2.00, .6);
	Channel *third = new Channel(0, 0, 3, 3, 3.00, .7);

	cout << "size = " << channels.size() << endl;
	channels.add(second);
	channels.add(third);
	channels.add(first);
	cout << "size = " << channels.size() << endl;
	Channel *se = channels.find(2);
	Channel *fi = channels.find(1.00);
	for (Channel *f = channels.getFirst(); f; f = channels.getNext()) {
		cout << "ch = " << f->getChannelNum() << ", freq = ";
		cout << f->getChannelCenterFreq() << endl;
		delete f;
	}
	
	UdpConnection udp("test", UnitSse);
	IpAddress any = "0.0.0.0";
	udp.setAddress(any, MULTICAST_PORT, false, true);
	err = udp.addGroup(MULTICAST_ADDR);
	err = udp.addGroup(MULTICAST_ADDR);
	
	400KHzPacket pkt;
	memset(&pkt, 0, sizeof(pkt));
	err = udp.recv(&pkt, sizeof(pkt));
	pkt.marshall();
#endif

	if (err = Args.parse(argc, argv))
		Fatal(err);

	// set the maximum # of subchannels
	State.setMaxSubchannels(Args.getUsableSubchannels());

//	SetDebugMask(DEBUG_PD_INPUT);
//	SetDebugMask(DEBUG_DSP_INPUT);
//	SetDebugMask(DEBUG_INPUT);
//	SetDebugMask(DEBUG_CONTROL);
//	SetDebugMask(DEBUG_ARCHIVE);
//	SetDebugMask(DEBUG_CONFIRM);
//	SetDebugMask(DEBUG_SIGNAL);
//	SetDebugMask(DEBUG_CWD);
//	SetDebugMask(DEBUG_PD);
//	SetDebugMask(DEBUG_DADD);
//	SetDebugMask(DEBUG_LOCK);
//	SetDebugMask(DEBUG_DETECT);
//	SetDebugMask(DEBUG_CWD_CONFIRM);
//	SetDebugMask(DEBUG_COLLECT);
//	SetDebugMask(DEBUG_BASELINE);
//	SetDebugMask(DEBUG_BIRDIE_MASK);
//	SetDebugMask(DEBUG_FREQ_MASK);
//	SetDebugMask(DEBUG_SIGNAL_CLASS);
//	SetDebugMask(DEBUG_CMD);
//	SetDebugMask(DEBUG_QTASK);
//	SetDebugMask(DEBUG_DSP_CMD);
//	SetDebugMask(DEBUG_MSGLIST);
//	SetDebugMask(DEBUG_STATE);
//	SetDebugMask(DEBUG_SUBCHANNEL);
//	SetDebugMask(DEBUG_PASS_THRU);
//	SetDebugMask(DEBUG_SPECTROMETRY);

//	Debug(DEBUG_ALWAYS, 0, "do init");
	if (err = Init())
#ifdef notdef
		LogFatal(err, -1, "initialization failed");
#else
		Fatal(err);
#endif
//	Debug(DEBUG_ALWAYS, 1, "init done");

#ifdef notdef
	uint8_t *hostbuf;
	int iterations = 10;
	uint32_t len = 64 * 1024;
	uint32_t dma = BWS_DMA;
	uint32_t lock = BWS_NOLOCK_SRAMA;

	hostbuf = new uint8_t[len];
	gettimeofday(&start, 0);
	for (i = 0; i < iterations; i++) {
		if (err = dxLeftDsp->readSRAMBlk(hostbuf, len, 0, dma | lock))
			LogError(err, -1, "");
	}
	gettimeofday(&end, 0);
	delete [] hostbuf;
#endif

#ifdef notdef
	gettimeofday(&start, 0);
	for (i = 0; i < 10000000; i++) {
		blk1 = PartitionSet.alloc(8224);
		data = blk1->getData();
		blk1->free();
	}
	gettimeofday(&end, 0);

	blk2 = PartitionSet.alloc(512);
	blk3 = PartitionSet.alloc(8224);

	blk1->free();
	blk2->free();
	blk3->free();
#endif

	for (int32_t i = 0; ; ++i)
		timer.sleep(100);
}
	