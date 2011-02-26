/*******************************************************************************

 File:    Relay.h
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

// Relay task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetrelay/include/Relay.h,v 1.3 2009/07/17 03:29:34 kes Exp $
//
#ifndef READER_H_
#define READER_H_

#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "System.h"
#include "Args.h"
#include "BeamPacket.h"
#include "ChannelPacket.h"
#include "PrStruct.h"
#include "PrTypes.h"
#include "Task.h"
#include "Udp.h"

using namespace std;
using namespace sonata_lib;

namespace sonata_packetrelay {

class RelayTask: public Task {
public:
	static RelayTask *getInstance(string name_, int prio_);
	static RelayTask *getInstance();
	~RelayTask();

private:
	static RelayTask *instance;

	bool printHeader;					// print packet headers
	int32_t printCount;					// # of packets between header print
	uint64_t packetCount;				// total # of packets received
	ATAPacket *pkt;						// packet
	ofstream fout;						// output file
	HostSpec in;
	HostSpec out;
	Udp *input;							// input socket
	Udp *output;						// output socket

	Args *args;							// command-line args

	// methods
	void extractArgs();
	void *routine();

	// hidden
	RelayTask(string name_, int prio_);

	// forbidden
	RelayTask(const RelayTask&);
	RelayTask &operator=(const RelayTask&);
};

}
#endif /*READER_H_*/