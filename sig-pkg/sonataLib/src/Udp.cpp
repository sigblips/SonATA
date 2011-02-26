/*******************************************************************************

 File:    Udp.cpp
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
// UDP connection class: derived from Connection
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Udp.cpp,v 1.6 2009/05/24 23:33:54 kes Exp $
//

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Err.h"
#include "Udp.h"

namespace sonata_lib {

Udp::Udp(string cname_, Unit unit_): Connection(cname_, unit_, UdpConnection),
		port(DEFAULT_PORT)
{
	strcpy(hostIp, DEFAULT_IP);
}

Udp::~Udp()
{
	groupSet.clear();
}

void
Udp::setAddress(const IpAddress hostIp_, int32_t port_, bool nonblock_,
	bool bind_)
{
	strcpy(hostIp, hostIp_);
	port = port_;
	nonblock = nonblock_;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	if (strlen(hostIp)) {
		struct hostent *host;
		if (!(host = gethostbyname(hostIp)))
			Fatal(127);
		memcpy(&saddr.sin_addr, host->h_addr, host->h_length);
		hostaddr = saddr.sin_addr.s_addr;
	}
	else
		saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (connection >= 0)
		close();
	Error err = create(nonblock);
	if (err)
		Fatal(err);
	if (bind_ && (err = bind()))
		close();
}

//
// addGroup: add a group to the multicast list
//
// Synopsis:
//		void addGroup(groupIp_);
//		const IpAddress groupIp_;			// group address
// Description:
//		Adds a new group to the multicast list.  If it is the first
//		port added to the list, the IP_MULTICAST socket option is
//		enabled.
// Notes:
//		setAddress must be called first with the port number to be
//		bound to the socket and an address INADDR_ANY.
//

Error
Udp::addGroup(const IpAddress groupIp_)
{
	string grp(groupIp_);

	// if this is the first multicast group added to the list, set
	// the socket option for multicast
	if (groupSet.find(grp) == groupSet.end()) {
		// add the group to the set
		groupSet.insert(grp);
		// add the group to the list
		ip_mreq mreq;
		bzero(&mreq, sizeof(mreq));
		hostent *h = gethostbyname(grp.c_str());
		memcpy(&mreq.imr_multiaddr.s_addr, h->h_addr, h->h_length);
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if (::setsockopt(connection, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
				sizeof(mreq)) < 0) {
			return (errno);
		}
	}
	return (0);
}

Error
Udp::establish()
{
	return (0);
}

Error
Udp::terminate()
{
	if (connection >= 0)
		close();
	return (0);
}

Error
Udp::connect()
{
	return (0);
}

//
// create: create a socket and set its options
//
Error
Udp::create(bool nonblock_)
{
	int s, optval = 1, ttl = 16;
	Error err;

	// open the socket
	if ((s = ::socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return (errno);

	// set nonblocking socket if requested
	if (nonblock_ && (::fcntl(s, F_SETFL, O_NONBLOCK) < 0))
		return (errno);

	// set socket options
	if (::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &optval,
			sizeof(optval)) < 0) {
		err = errno;
		::close(s);
		return (err);
	}
	if (::setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *) &optval,
			sizeof(optval)) < 0) {
		err = errno;
		::close(s);
		return (err);
	}
	if (::setsockopt(s, SOL_SOCKET, IP_TTL, (char *) &ttl, sizeof(ttl)) < 0) {
		err = errno;
		::close(s);
		return (err);
	}
	// socket was successfully created: record it
	connection = s;
	return (0);
}

//
// bind: bind the socket to an address
//
Error
Udp::bind()
{
	Error err;

	if (connection < 0)
		return (ERR_NS);

	// set up the socket structure
	if (::bind(connection, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		err = errno;
		terminate();
		return (err);
	}
	return (0);
}

//
// set receive buffer size.
//
Error
Udp::setRcvBufsize(size_t& size_)
{
	if (connection < 0)
		return (ERR_NS);

	Error err;
	int32_t sz = size_;
	socklen_t bufsize = sizeof(sz);
	if (::setsockopt(connection, SOL_SOCKET, SO_RCVBUF, &sz, bufsize) < 0) {
		err = errno;
		close();
		return (err);
	}
	if (::getsockopt(connection, SOL_SOCKET, SO_RCVBUF, &sz, &bufsize) < 0) {
		err = errno;
		close();
		return (err);
	}
	size_ = sz;
	return (0);
}

//
// set send buffer size
//
Error
Udp::setSndBufsize(size_t& size_)
{
	if (connection < 0)
		return (ERR_NS);

	Error err;
	size_t sz = size_;
	socklen_t bufsize = sizeof(sz);
	if (::setsockopt(connection, SOL_SOCKET, SO_SNDBUF, &sz, bufsize) < 0) {
		err = errno;
		close();
		return (err);
	}
	if (::getsockopt(connection, SOL_SOCKET, SO_SNDBUF, &sz, &bufsize) < 0) {
		err = errno;
		close();
		return (err);
	}
	size_ = sz;
	return (0);
}

//
// recv: receive a packet
//
Error
Udp::recv(void *msg_, size_t len)
{
	if (connection < 0)
		return (ERR_NS);

	char *msg = static_cast<char *> (msg_);
	struct sockaddr_in from;
	socklen_t fromLen = sizeof(from);

	if (recvfrom(connection, (void *) msg, len, 0, (sockaddr *) &from,
			&fromLen) != (ssize_t) len) {
		return (errno);
	}
	return (0);
}

//
// send: send a packet
//
Error
Udp::send(void *msg_, size_t len)
{
	if (connection < 0)
		return (ERR_NS);

	char *msg = static_cast<char *> (msg_);

	if (sendto(connection, (void *) msg, len, 0, (struct sockaddr *) &saddr,
			sizeof(saddr)) < 0) {
		return (errno);
	}
	return (0);
}

//
// send: send a packet to a specific address
//
Error
Udp::send(void *msg_, size_t len, const IpAddress& dest, int32_t port)
{
	if (connection < 0)
		return (ERR_NS);

	char *msg = static_cast<char *> (msg_);
	IpAddress addr;
	struct hostent *host = 0;
	struct sockaddr_in s_addr;

	strcpy(addr, dest);
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port);
	if (!strlen(addr) || !(host = gethostbyname(addr)))
		Fatal(127);
	memcpy(&s_addr.sin_addr, host->h_addr, host->h_length);

	if (sendto(connection, (void *) msg, len, 0, (struct sockaddr *) &s_addr,
			sizeof(s_addr)) < 0) {
		return ((Error) errno);
	}
	return (0);
}

/**
 * Close the connection.
 *
 * Description:\n
 * 	Shuts down both reads and writes on the socket, then closes it
 * 	and resets the connection variable
 */
void
Udp::close()
{
	if (connection >= 0) {
		::shutdown(connection, SHUT_RDWR);
		::close(connection);
		connection = -1;
	}
}

}
