/*******************************************************************************

 File:    Tcp.cpp
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
// TCP connection class: derived from Connection
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Tcp.cpp,v 1.3 2009/05/24 23:33:07 kes Exp $
//

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Err.h"
#include "Tcp.h"

namespace sonata_lib {

Tcp::Tcp(string cname_, Unit unit_, ConnectionType type_,
		string host_, int port_): Connection(cname_, unit_, type_),
		port(port_), socket(0)
{
	strcpy(host, host_.c_str());
}

Tcp::~Tcp()
{
	if (isConnected())
		terminate();
}

void
Tcp::setAddress(const IpAddress host_, int32_t port_)
{
	strcpy(host, host_);
	port = port_;
}

Error
Tcp::establish()
{
	if (isConnected())
		return (0);

	switch (type()) {
	case ActiveTcpConnection:
		return (connect());
	case PassiveTcpConnection:
		return (accept());
	default:
		Fatal(ERR_ICT);
	}
	return (0);
}

Error
Tcp::terminate()
{
	if (isConnected()) {
		if (socket > 0) {
			close(socket);
			socket = 0;
			if (type() == ActiveTcpConnection)
				connection = 0;
		}
		if (connection > 0) {
			close(connection);
			connection = 0;
		}
	}
	connected(false);
	return (0);
}

Error
Tcp::connect()
{
	Error err;
	struct hostent *hp;
	struct sockaddr_in sin;

	if ((socket <= 0) && (err = create()))
		return (err);

	// look up the host
	if (!strlen(host))
		return (ERR_IHN);
	if (!(hp = gethostbyname(host)))
		return (ERR_HNF);

	// create the sockaddr_in structure
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = (short) AF_INET;
	sin.sin_port = (u_short) htons(port);
	sin.sin_addr.s_addr = *((int *) hp->h_addr);
	if (::connect(socket, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		err = errno;
		terminate();
		return (err);
	}
	connection = socket;
	connected(true);
	return (0);
}

Error
Tcp::accept()
{
	int s;
	Error err;
	socklen_t len = sizeof(struct sockaddr_in);
	struct sockaddr_in sin;

	// create the listening socket if it does not exist
	if (socket <= 0) {
		if ((err = create()) || (err = bind()) || (err = listen()))
			return (err);
	}

	// accept a connection
	if ((s = ::accept(socket, (struct sockaddr *) &sin, &len)) < 0) {
		err = errno;
		if (errno != EWOULDBLOCK)
			terminate();
		return (err);
	}

	// close the listening socket, the record the connection
	close(socket);
	socket = 0;
	connection = s;
	connected(true);
	return (0);
}

Error
Tcp::create()
{
	int s, optval = 1;
	Error err;
	struct linger linger;

	// open the socket
	if ((s = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return (errno);

	// set socket options
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &optval,
			sizeof(optval)) < 0) {
		err = errno;
		::close(s);
		return (err);
	}
	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval,
			sizeof(optval)) < 0) {
		err = errno;
		::close(s);
		return (err);
	}

	// enable SO_LINGER
	linger.l_onoff = 1;
	linger.l_linger = 0;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &linger,
			sizeof(linger)) < 0) {
		err = errno;
		::close(s);
		return (err);
	}

	// socket was successfully created: record it
	socket = s;
	return (0);
}

Error
Tcp::bind()
{
	Error err;
	struct sockaddr_in sin;

	if (socket <= 0)
		return (ERR_NS);

	// set up the socket structure
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = (short) AF_INET;
	sin.sin_port = (u_short) port;
	sin.sin_addr.s_addr = INADDR_ANY;

	if (::bind(socket, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		err = errno;
		terminate();
		return (err);
	}
	return (0);
}

Error
Tcp::listen()
{
	Error err;

	if (socket <= 0)
		return (ERR_NS);

	// listen on the socket
	if (::listen(socket, 1) < 0) {
		err = errno;
		terminate();
		return (err);
	}
	return (0);
}

Error
Tcp::recv(void *msg_, size_t len_)
{
	char *msg = static_cast<char *> (msg_);
	Error err;
	int cc;

	while (len_ > 0) {
		cc = ::recv(connection, msg, len_, 0);
		if (cc <= 0) {
			if (errno != EWOULDBLOCK) {
				err = cc < 0 ? errno : ENOTCONN;
				terminate();
				return (err);
			}
			else
				cc = 0;
		}
		msg += cc;
		len_ -= cc;
	}
	return (0);
}

Error
Tcp::send(void *msg_, size_t len_)
{
	char *msg = static_cast<char *> (msg_);
	int cc;

	lockSend();
	while (len_ > 0) {
		cc = ::send(connection, msg, len_, 0);
		if (cc <= 0) {
			unlockSend();
			return (errno);
		}
		msg += cc;
		len_ -= cc;
	}
	unlockSend();
	return (0);
}

}
