/*******************************************************************************

 File:    Args.h
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
// Command-line arguments class
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetrelay/include/Args.h,v 1.4 2009/07/17 03:29:14 kes Exp $
//
#ifndef _ArgsH
#define _ArgsH

#include <string>
#include "PrStruct.h"
//#include "Sonata.h"

using namespace sonata_lib;
using std::string;

namespace sonata_packetrelay {

class Args {
public:
	static Args *getInstance();

	~Args();

	Error parse(int argc, char **argv);

	bool cloneData() { return (clone); }
	bool toFile() { return (writeToFile); }
	bool printHeader() { return (prtHdr); }
	bool rawPackets() { return (rawPkts); }
	int32_t getPackets() { return (packets); }
	int32_t getPrintCount() { return (prtCnt); }
	int32_t getRawPacketSize() { return (pktSize); }
	const HostSpec& getInput() { return (input); }
	const HostSpec& getOutput() { return (output); }
	IpAddress& getInputAddr() { return (input.addr); }
	int32_t getInputPort() { return (input.port); }
	IpAddress& getOutputAddr() { return (output.addr); }
	int32_t getOutputPort() { return (output.port); }
	string& getOutputFile() { return (outFile); }

private:
	static Args *instance;

	bool clone;							// clone data into two streams
	bool writeToFile;					// the data to a file
	bool prtHdr;						// print header
	bool rawPkts;						// packets are just blocks of bytes
	int32_t prtCnt;						// print every prtcnt header
	int32_t pktSize;					// raw packet size in bytes
	uint32_t packets;					// # of packets to relay
	string outFile;						// output file
	HostSpec input;						// input host specification
	HostSpec output;					//  output address specification

	// hidden
	Args();

	// forbidden
	Args(const Args&);
	Args& operator=(const Args&);
};

}

#endif