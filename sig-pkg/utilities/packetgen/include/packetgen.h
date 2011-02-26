/*******************************************************************************

 File:    packetgen.h
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

/**
* Packet generator program.
*/
#ifndef _PacketGenH
#define _PacketGenH

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdint.h>
#include <vector>
#include <alarm.h>
#include <sseInterface.h>

#include "BeamPacket.h"
#include "ChannelPacket.h"
#include "Gaussian.h"

using namespace gauss;
using namespace sonata_lib;
using namespace std;

/**
 * Signal inserted into the sample stream.
 */
struct PacketSig {
	SigType type;						// signal type
	float64_t freq;						// signal frequency
	float64_t drift;					// signal drift (Hz/sec)
	float64_t snr;						// signal snr
	float64_t tStart;					// signal Pulse Signal Start Time
	float64_t tOn;						// signal Pulse Signal On Time
	float64_t tOff;						// signal Pulse Signal Off Time
	uint8_t pol;						// signal polarization

	PacketSig(): type(CwSignal), freq(0.0), drift(0.0), snr(0.0), tStart(0.0), 
			tOn(1.0), tOff(0.0), pol(ATADataPacketHeader::BOTH) {}
	PacketSig(SigType type_, float64_t freq_, float64_t drift_, float64_t snr_,
			float64_t tStart_, float64_t tOn_, float64_t tOff_, uint8_t pol_):
			type(type_), freq(freq_), drift(drift_), snr(snr_),
			tStart(tStart_), tOn(tOn_), tOff(tOff_), pol(pol_) {}
};

typedef vector<PacketSig> PacketSigList;

// function declarations
void usage();
void parseArgs(int argc, char **argv);
void alarmHandler(int signal);
void fullThrottle();
void openFile(const string& file);
void createPackets(Gaussian& xGen, Gaussian& yGen, 
		ATAPacket& xPkt, ATAPacket& yPkt);
void sendPackets(ATAPacket& xPkt, ATAPacket& yPkt);
void printSummary(ComplexInt8 *data, int n);
void fPrintSummary(ComplexFloat32 *data, int n);
void printStatistics();

//---------------------------------------------------------------------
// in-line class under test
//---------------------------------------------------------------------
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

class MCSend
{
public:
	MCSend(int, uint32_t, string);
	void send(const void *, size_t);
	int getSendHWM() { return sendHWM; }
private:
	int sockFd;
	sockaddr_in mcastAddr;
	int sendHWM;
};

#endif