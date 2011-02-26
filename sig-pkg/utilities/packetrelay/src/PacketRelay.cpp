/*******************************************************************************

 File:    PacketRelay.cpp
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
// Packet relay main task
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetrelay/src/PacketRelay.cpp,v 1.1 2008/09/17 19:04:17 kes Exp $
//

#include "PacketRelay.h"
#include "PrErrMsg.h"
#include "Lock.h"

namespace sonata_packetrelay {

PacketRelay *PacketRelay::instance = 0;

PacketRelay *
PacketRelay::getInstance()
{
	Lock l;
	l.lock();
	if (!instance)
		instance = new PacketRelay();
	l.unlock();
	return (instance);
}

PacketRelay::PacketRelay()
{
}

void
PacketRelay::run(int argc, char **argv)
{
	args = Args::getInstance();
	Assert(args);

	Assert(!args->parse(argc, argv));

	// initialize the system, creating tasks
	init();

	Timer timer;
	for (uint64_t i = 0; ; ++i)
		timer.sleep(100);
	return;
}

void
PacketRelay::init()
{
	// initialize the error list
	buildErrList();

#ifdef notdef
	ErrMsg *errMsg = ErrMsg::getInstance();
	string s = errMsg->getErrMsg((ErrCode) ERR_CCC);
	std::cout << s << std::endl;
	s = errMsg->getErrMsg(ERR_DLK);
	std::cout << s << std::endl;
	s = errMsg->getErrMsg(ERR_LNI);
	std::cout << s << std::endl;
#endif

	relay = RelayTask::getInstance();
	Assert(relay);

	// start the reader and sender tasks
	relay->start();
}

}