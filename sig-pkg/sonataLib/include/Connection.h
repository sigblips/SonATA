/*******************************************************************************

 File:    Connection.h
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
// Connection base class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Connection.h,v 1.4 2009/05/24 23:12:38 kes Exp $
//
#ifndef _ConnectionH
#define _ConnectionH

#include <string>
#include "Err.h"
#include "Lock.h"
#include "Types.h"

using std::string;

namespace sonata_lib {

class Connection {
public:
	Connection(string cname_, Unit unit_, ConnectionType type_):
			connectFlag(false), connection(-1), connType(type_), cname(cname_),
			unit(unit_), slock("s" + cname_)
	{
	}
	~Connection() {}

	void lockSend() { slock.lock(); }
	void unlockSend() { slock.unlock(); }

	virtual Error establish() = 0;
	virtual Error terminate() = 0;

	virtual Error setRcvBufsize(size_t& size_) { return (0); }
	virtual Error setSndBufsize(size_t& size_) { return (0); }

	virtual Error send(void *msg_, size_t size_) { return (0); }
	virtual Error recv(void *msg_, size_t size_) { return (ERR_NDA); }

	ConnectionType type() { return (connType); }
	bool isConnected() { return (connectFlag); }

protected:
	bool connectFlag;				// is there a connection?
	int connection;					// connection id
	ConnectionType connType;		// connection type
	string cname;					// connection name
	Unit unit;					// target unit
	Lock slock;					// lock (send only)

	void connected(bool flag) { connectFlag = flag; }

private:
	// forbidden
	Connection(const Connection&);
	Connection& operator=(const Connection&);
};

}

#endif