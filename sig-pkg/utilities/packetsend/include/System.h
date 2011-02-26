/*******************************************************************************

 File:    System.h
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
// Channelizer primary header file
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/System.h,v 1.8 2008/09/16 21:41:37 kes Exp $
//
#ifndef _SystemH
#define _SystemH

#include <assert.h>
#include <complex>
#include <netinet/in.h>
#include <string>
#include <fftw3.h>
#include "ATAPacket.h"
#include "PsTypes.h"
#include "Sonata.h"

namespace sonata_packetsend {

// counts
const int32_t INPUT_COUNT = 262145;
const int32_t OUTPUT_COUNT = 10000;
const int32_t READ_BUFFERS = 2;
const int32_t BEAM_PACKETS = 100000;
const int32_t CHANNEL_PACKETS = 100000;
const int32_t SEND_BUFSIZE = 16777216;

// system defaults
const string DEFAULT_CFG_FILE = "packetsend.cfg";
const string DEFAULT_INPUT_FILE = "BeamData";
const IpAddress DEFAULT_OUTPUT_ADDR = "226.1.50.1";
const int32_t DEFAULT_OUTPUT_PORT = 50000;
const uint32_t DEFAULT_SRC = ATADataPacketHeader::BEAM_104MHZ;
const int32_t DEFAULT_BURST = INPUT_COUNT;
const int32_t DEFAULT_DELAY = 0;
const int32_t DEFAULT_NCHAN = 1;
const IpAddress CHANNEL_OUTPUT_ADDR = "227.1.1.1";
const int32_t CHANNEL_OUTPUT_PORT = 51000;

#ifndef NTOHL
#define NTOHS(x)						((x) = ntohs(x))
#define NTOHL(x)						((x) = ntohl(x))
#endif

// task priorities
const int32_t READER_PRIO = 22;
const int32_t FORMATTER_PRIO = 20;
const int32_t SENDER_PRIO = 21;
const int32_t LOG_PRIO = 1;

}

#endif
