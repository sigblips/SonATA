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
// Packet relay primary header file
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetrelay/include/System.h,v 1.2 2009/07/17 03:29:45 kes Exp $
//
#ifndef _SystemH
#define _SystemH

#include <assert.h>
#include <complex>
#include <netinet/in.h>
#include <string>
#include "ATAPacket.h"
#include "PrTypes.h"
#include "Sonata.h"

namespace sonata_packetrelay {

// counts
const int32_t PRINT_COUNT = 10000;
const int32_t SND_BUFSIZE = 16777216;
const size_t RCV_BUFSIZE = 16777216;

// system defaults
const IpAddress DEFAULT_INPUT_ADDR = "226.1.50.1";
const int32_t DEFAULT_INPUT_PORT = 50000;
const IpAddress DEFAULT_OUTPUT_ADDR = "226.1.51.1";
const int32_t DEFAULT_OUTPUT_PORT = 50000;

#ifndef NTOHL
#define NTOHS(x)						((x) = ntohs(x))
#define NTOHL(x)						((x) = ntohl(x))
#endif

// task priorities
const int32_t RELAY_PRIO = 22;

}

#endif
