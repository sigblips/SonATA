/*******************************************************************************

 File:    pktTest.cpp
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

/*
** pktTest.cpp
**
** Created: <DATE>
** Author:
**
*/

#include <unistd.h>
#include "basics.h"
#include "BeamPacket.h"
#include "ChannelPacket.h"


const char *Pgm = "pktTest";

void
usage()
{
	OUTL("usage: " << Pgm);
	exit(-1);
}

void
parseArgs(int argc, char *argv[])
{
	const char *optstring = "";		// getopt options arguments
	extern char *optarg;			// getopt argument value return string
	extern int	optind;				// current argv index
	bool done = false;				// getopt option parsing state
	extern int optopt;
	extern int opterr;
	opterr = 0;

	while ( !done )
    {
        switch ( getopt(argc,argv,optstring) )
        {
            case -1:
                done = true;
                break;

            default:
            case '?':
                USAGE("Invalid option: " << optopt );
        }
    }

	if ( optind < argc )
		USAGE("Too many arguments");
}

//----------------------------------------------------------------------
// main
//----------------------------------------------------------------------
int
main(int argc, char *argv[])
{
	parseArgs(argc, argv);

	ATADataPacketHeader hdr;
	CONFIRM(hdr.group == (uint8_t) ATADataPacketHeader::UNDEFINED);
	CONFIRM(hdr.version == (uint8_t) ATADataPacketHeader::CURRENT_VERSION);
	CONFIRM(hdr.bitsPerSample == (uint8_t) ATADataPacketHeader::DEFAULT_BITS);
	CONFIRM(hdr.binaryPoint == (uint8_t)
			ATADataPacketHeader::DEFAULT_BINARY_POINT);
	CONFIRM(hdr.order == ATADataPacketHeader::CORRECT_ENDIAN);
	CONFIRM(hdr.type == (uint8_t) ATADataPacketHeader::DEFAULT_TYPE);
	CONFIRM(hdr.streams == (uint8_t) ATADataPacketHeader::DEFAULT_STREAMS);
	CONFIRM(hdr.polCode == (uint8_t) ATADataPacketHeader::UNDEFINED);
	CONFIRM(hdr.src == ATADataPacketHeader::UNDEFINED);
	CONFIRM(hdr.seq == ATADataPacketHeader::UNDEFINED);
	CONFIRM(hdr.chan == ATADataPacketHeader::UNDEFINED);
	CONFIRM(hdr.freq == ATADataPacketHeader::DEFAULT_FREQ);
	CONFIRM(hdr.absTime == ATADataPacketHeader::TIME_NOT_SET);
	CONFIRM(hdr.flags == 0);
	CONFIRM(hdr.len == ATADataPacketHeader::DEFAULT_LEN);

	// Beam (104MHz) packet
	ATAPacket *ataPkt = new BeamPacket();
	hdr = ataPkt->getHeader();
	CONFIRM(hdr.group == ATADataPacketHeader::ATA);
	CONFIRM(hdr.src == ATADataPacketHeader::BEAM_104MHZ);
	CONFIRM(hdr.chan == ATADataPacketHeader::DEFAULT_BEAM);
	CONFIRM(hdr.bitsPerSample == BEAM_BITS);
	CONFIRM(hdr.len == BEAM_SAMPLES);
	CONFIRM(ataPkt->getPacketSize() == sizeof(BeamDataPacket));
	for (int32_t i = 0; i < hdr.len; i++)
		ataPkt->putSample(i, complex<float>(i, -i));

	BeamPacket ataPkt2;
	memcpy(&ataPkt2, ataPkt, ataPkt2.getSize());

	BeamDataPacket data, data2;
	ataPkt->getPacket(&data);
	ataPkt2.getPacket(&data2);
	memcmp(&data, &data2, ataPkt2.getPacketSize());

#if __BYTE_ORDER == __BIG_ENDIAN
	CONFIRM(!ataPkt->marshall(ATADataPacketHeader::FORCE_BIG_ENDIAN));
	CONFIRM(ataPkt->hdr.len == ataPkt2.hdr.len);
	CONFIRM(ataPkt->samples[1] == ataPkt2.samples[1]);
	CONFIRM(ataPkt->marshall(ATADataPacketHeader::FORCE_LITTLE_ENDIAN));
#else
	CONFIRM(!ataPkt->marshall(ATADataPacketHeader::FORCE_LITTLE_ENDIAN));
	CONFIRM(ataPkt->getHeader().len == (ataPkt2.getHeader()).len);
	CONFIRM(ataPkt->getSample(1) == ataPkt2.getSample(1));
	CONFIRM(ataPkt->marshall(ATADataPacketHeader::FORCE_BIG_ENDIAN));
#endif
	CONFIRM(ataPkt->getHeader().len != ataPkt2.getHeader().len);
	CONFIRM(ataPkt->getSample(1) != ataPkt2.getSample(1));

	CONFIRM(ataPkt->demarshall());
	CONFIRM(memcmp(&ataPkt2,ataPkt,sizeof(ataPkt2)) == 0);
	CONFIRM(!ataPkt->demarshall());
	CONFIRM(memcmp(&ataPkt2,ataPkt,sizeof(ataPkt2)) == 0);

	// Channel (400KHz) packet
	ATAPacket *ataPkt3 = new ChannelPacket(ATADataPacketHeader::CHAN_400KHZ,
			2);
	hdr = ataPkt3->getHeader();
	CONFIRM(hdr.group == ATADataPacketHeader::ATA);
	CONFIRM(hdr.src == ATADataPacketHeader::CHAN_400KHZ);
	CONFIRM(hdr.chan == 2);
	CONFIRM(hdr.bitsPerSample = ATADataPacketHeader::CHANNEL_BITS);
	CONFIRM(hdr.len == ATADataPacketHeader::CHANNEL_SAMPLES);
	uint32_t *c = static_cast<uint32_t *> (ataPkt->getSamples());
	for (int32_t i = 0; i < hdr.len; i++)
		ataPkt3->putSample(i, complex<float>(-i, i));

	ChannelPacket ataPkt4;
	memcpy(&ataPkt4,ataPkt3, ataPkt4.getSize());
	ChannelDataPacket data3, data4;
	ataPkt3->getPacket(&data3);
	ataPkt4.getPacket(&data4);
	memcmp(&data3, &data4, ataPkt4.getPacketSize());

#if __BYTE_ORDER == __BIG_ENDIAN
	CONFIRM(!ataPkt3->marshall(ATADataPacketHeader::FORCE_BIG_ENDIAN));
	CONFIRM(ataPkt3->hdr.len == ataPkt4.hdr.len);
	CONFIRM(ataPkt3->samples[1] == ataPkt4.samples[1]);
	CONFIRM(ataPkt3->marshall(ATADataPacketHeader::FORCE_LITTLE_ENDIAN));
#else
	CONFIRM(!ataPkt3->marshall(ATADataPacketHeader::FORCE_LITTLE_ENDIAN));
	CONFIRM(ataPkt3->getHeader().len == ataPkt4.getHeader().len);
	CONFIRM(ataPkt3->getSample(1) == ataPkt4.getSample(1));
	CONFIRM(ataPkt3->marshall(ATADataPacketHeader::FORCE_BIG_ENDIAN));
#endif
	CONFIRM(ataPkt3->getHeader().len != ataPkt4.getHeader().len);
	CONFIRM(ataPkt3->getSample(1) != ataPkt4.getSample(1));

	CONFIRM(ataPkt3->demarshall());
	CONFIRM(memcmp(&ataPkt4,ataPkt3,sizeof(ataPkt4)) == 0);
	CONFIRM(!ataPkt3->demarshall());
	CONFIRM(memcmp(&ataPkt4,ataPkt3,sizeof(ataPkt4)) == 0);

	return 0;
}