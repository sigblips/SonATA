/*******************************************************************************

 File:    Udp.h
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
// UDP connection class; derived from DxConnection class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Udp.h,v 1.4 2008/08/21 16:36:58 kes Exp $
//
#ifndef _UdpH
#define _UdpH

#include <set>
#include <string>
#include <fcntl.h>
#include <sseDxInterface.h>
#include "Sonata.h"
#include "Connection.h"
#include "Types.h"

typedef std::set<string>	GroupSet;

namespace sonata_lib {

#define DEFAULT_IP			"127.0.0.1"

class Udp: public Connection {
public:
	Udp(string name_, Unit unit_);
	~Udp();

	void setAddress(const IpAddress hostIp_, int32_t port_,
			bool nonblock_ = true, bool bind_ = false);
	Error addGroup(const IpAddress groupIp_);
//	void resetGroups();

	Error establish();
	Error terminate();

	Error setRcvBufsize(size_t& size_);
	Error setSndBufsize(size_t& size_);
	Error recv(void *msg, size_t len);
	Error send(void *msg, size_t len);
	Error send(void *msg, size_t len, const IpAddress& addr, int32_t port);

private:
	bool nonblock;					// blocking or non-blocking
	int32_t port;					// port
	uint32_t hostaddr;				// host ip address (dot notation)
	GroupSet groupSet;				// groups assigned to socket
	IpAddress hostIp;				// target host
	struct sockaddr_in saddr;		// socket address

	Error connect();				// active connection
	Error create(bool nonblock_); // create a socket
	Error bind();					// bind the socket
	void close();					// close the socket

	// forbidden
	Udp(const Udp&);
	Udp& operator=(const Udp&);
};

}

#endif