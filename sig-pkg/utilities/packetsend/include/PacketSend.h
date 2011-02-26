/*******************************************************************************

 File:    PacketSend.h
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
// Packet sender main task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/PacketSend.h,v 1.2 2008/08/12 21:28:46 kes Exp $
//
#ifndef PACKETSEND_H_
#define PACKETSEND_H_

#include "Args.h"
#include "BeamPacketList.h"
#include "ChannelPacketList.h"
#include "Formatter.h"
#include "Reader.h"
#include "Sender.h"
#include "Timer.h"

namespace sonata_packetsend {

class PacketSend {
public:
	static PacketSend *getInstance();
	~PacketSend();

	void run(int argc, char **argv);

private:
	static PacketSend *instance;

	Args *args;							// command-line arguments
	BeamPacketList *pktList;
	FormatterTask *formatter;
	ReaderTask *reader;
	SenderTask *sender;

	void init();						// perform system initialization

	// hidden
	PacketSend();

	// forbidden
	PacketSend& operator=(const PacketSend&);
	PacketSend(const PacketSend&);
};

}

#endif /*PACKETSEND_H_*/