/*******************************************************************************

 File:    Args.cpp
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
// Command-line arguments class (singleton)
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetrelay/src/Args.cpp,v 1.5 2009/07/17 03:30:39 kes Exp $
//
#include <iostream>
#include <fcntl.h>
#include "System.h"
#include "Args.h"
#include "BeamPacket.h"
#include "Lock.h"
#include "PrVersion.h"

using std::cout;
using std::endl;
using namespace sonata_lib;

namespace sonata_packetrelay {

static string usage = "packetrelay [-?] [-c] [-f file] [-I inputHost] [-i inputPort] [-O outputHost] [-o outputPort] [-p[nnn]\n\
	-?: print usage and exit\n\
	-c: clone input into two data streams\n\
	-f file: write to output file instead of socket\n\
	-I addr: input address\n\
	-i port: input port\n\
	-n packets: # of packets to relay\n\
	-O addr: output address\n\
	-o port: output port\n\
	-pn: print every nth header (default 100)\n\
	-s pktsize: raw packets of size pktsize\n\
	-V: print version and exit\n\\n\\n";

Args *Args::instance = 0;

Args *
Args::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new Args();
	l.unlock();
	return (instance);
}

Args::Args(): clone(false), writeToFile(false), prtHdr(false), rawPkts(false),
		prtCnt(PRINT_COUNT), pktSize(0), packets(0), outFile(""),
		input(DEFAULT_INPUT_ADDR, DEFAULT_INPUT_PORT),
		output(DEFAULT_OUTPUT_ADDR, DEFAULT_OUTPUT_PORT)
{
}

Args::~Args()
{
}

// grant access to external variables
namespace std {
	extern char *optarg;
	extern int optind, optopt, opterr;
}

Error
Args::parse(int argc, char **argv)
{
	bool done = false;
	const char *optstring = "?cVf:I:i:n:O:o:p:s:";

	opterr= 0;
	while (!done) {
		switch (getopt(argc, argv, optstring)) {
		case -1:
			done = true;
			break;
		case '?':
			cout << usage;
			exit(1);
		case 'c':
			clone = true;
			break;
		case 'f':
			writeToFile = true;
			outFile = optarg;
			break;
		case 'I':
			memcpy(input.addr, optarg, sizeof(input.addr));
			break;
		case 'i':
			input.port = atoi(optarg);
			break;
		case 'n':
			packets = atoi(optarg);
			break;
		case 'O':
			memcpy(output.addr, optarg, sizeof(output.addr));
			break;
		case 'o':
			output.port = atoi(optarg);
			break;
		case 'p':
			prtHdr = true;
			if (optarg)
				prtCnt = atoi(optarg);
			Assert(prtCnt > 0);
			break;
		case 's':
			rawPkts = true;
			pktSize = atoi(optarg);
			break;
		case 'V':
			cout << PRVERSION << endl;
			exit(1);
		default:
			cout << "Invalid option: " << optopt << endl;
			cout << usage;;
			exit(-1);
		}
	}
	return (0);
}

}