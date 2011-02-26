/*******************************************************************************

 File:    Keyboard.cpp
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
// Keyboard: keyboard connection class, derived from Connection
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Keyboard.cpp,v 1.3 2009/05/24 23:27:58 kes Exp $
//

#include <errno.h>
#include <unistd.h>
#include "Err.h"
#include "Keyboard.h"

namespace sonata_lib {

Keyboard::Keyboard(string name_, Unit unit_):
		Connection(name_, unit_, KeyboardConnection)
{
}

Keyboard::~Keyboard()
{
	if (isConnected())
		terminate();
}

Error
Keyboard::establish()
{
	if (isConnected())
		return (0);

	// open standard input
	// duplicate the standard input
	if ((connection = dup(0)) < 0)
		return (errno);
	connected(true);
	return (0);
}

Error
Keyboard::terminate()
{
	if (isConnected()) {
		close(connection);
		connection = -1;
	}
	connected(false);
	return (0);
}

Error
Keyboard::recv(void *msg_, size_t len_)
{
	char *msg = static_cast<char *> (msg_);
	int cc;
	Error err;

	if (!isConnected())
		return (ERR_NC);

	while (len_ > 0) {
		cc = read(connection, msg, len_);
		if (cc <= 0) {
			if (errno != EWOULDBLOCK) {
				err = errno;
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

}