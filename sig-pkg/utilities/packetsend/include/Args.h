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
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/Args.h,v 1.5 2008/09/16 21:17:29 kes Exp $
//
#ifndef _ArgsH
#define _ArgsH

#include <string>
#include <sseDxInterface.h>
#include "BeamPacket.h"
#include "Err.h"
#include "PsStruct.h"
#include "PsVersion.h"
#include "System.h"

using namespace sonata_lib;
using std::string;

namespace sonata_packetsend {

class Args {
public:
	static Args *getInstance();

	~Args();

	Error parse(int argc, char **argv);

	bool sendChannels() { return (channels); }
	bool resequence() { return (reseq); }
   bool resetPolarization() { return (resetPol);}
	uint32_t getSrc() { return (src); }
	ATADataPacketHeader::PolarizationCode getPol() { return (pol); }
	const HostSpec& getOutput() { return (output); }
	string getInputFile() { return (inputFile); }
	IpAddress& getOutputAddr() { return (output.addr); }
	int32_t getOutputPort() { return (output.port); }
	int32_t getBurst() { return (burst); }
	int32_t getDelay() { return (delay); }
	int32_t getNchan() { return (nchan); }
	int32_t getRepeat() { return (loop); }

private:
	static Args *instance;

	bool channels;						// channel packets, not beam
	bool reseq;							// reset the header sequence numbers
   bool resetPol;					// reset the polarization
	int32_t loop;						// resend last two buffers forever
	uint32_t nchan;						// Number of channels to send
	uint32_t burst;						// burst length of packets
	uint32_t delay;						// delay in usec between bursts
	ATADataPacketHeader::PolarizationCode pol;
	uint32_t src;						// source
	string inputFile;					// input file
	HostSpec output;					//  output address specification

	// hidden
	Args();

	// forbidden
	Args(const Args&);
	Args& operator=(const Args&);
};

}

#endif