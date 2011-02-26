/*******************************************************************************

 File:    Tcp.h
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
// TCP connection class; derived from Connection class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Tcp.h,v 1.3 2009/02/12 18:08:05 kes Exp $
//
#ifndef _TcpH
#define _TcpH

#include <string>
#include "Sonata.h"
#include "Connection.h"
#include "Types.h"

namespace sonata_lib {

class Tcp: public Connection {
public:
	Tcp(string name_, Unit unit_, ConnectionType type_,
			string host_ = "", int port_ = -1);
	~Tcp();

	void setAddress(const IpAddress sseIp_, int32_t port_);

	Error establish();
	Error terminate();

	Error send(void *msg_, size_t len_);
	Error recv(void *msg_, size_t len_);

private:
	int32_t port;					// target port
	int socket;						// socket
	IpAddress host;					// target host

	Error connect();				// active connection
	Error accept();				// accept connections
	Error create();				// create a socket
	Error bind();				// bind the socket
	Error listen();				// listen on the socket

	// forbidden
	Tcp(const Tcp&);
	Tcp& operator=(const Tcp&);
};

}

#endif