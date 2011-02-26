/*******************************************************************************

 File:    Serial.cpp
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
// DX serial connection class
//
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "Err.h"
#include "Serial.h"

namespace sonata_lib {

Serial::Serial(string name_, Unit unit_, int mode_,
		struct termios& termio_): Connection(name_, unit_, SerialConnection),
		mode(mode_), termio(termio_)
{
}

Serial::~Serial()
{
	if (isConnected())
		terminate();
}

Error
Serial::establish()
{
	Error err;
	struct termios oldtermio;

	if (isConnected())
		return (0);

	if ((connection = open(cname.c_str(), mode)) < 0)
		return (errno);

	// flush all output
	if (tcflush(connection, TCIOFLUSH)) {
		err = (Error) errno;
		close(connection);
		connection = -1;
		return (err);
	}

	// set speed and flags
	if (tcgetattr(connection, &oldtermio)) {
		err = (Error) errno;
		close(connection);
		connection = -1;
		return (err);
	}

	tcflush(connection, TCIFLUSH);

	// set the port attributes
	if (tcsetattr(connection, TCSANOW, &termio)) {
		err = (Error) errno;
		close(connection);
		connection = -1;
		return (err);
	}
	connected(true);
	return (0);
}

Error
Serial::terminate()
{
	if (!isConnected())
		return (0);

	close(connection);
	connection = 0;
	return (0);
}

Error
Serial::send(void *msg_, size_t len_)
{
	if (!isConnected())
		return (ERR_NC);
	if (write(connection, static_cast<void *> (msg_), len_) != (ssize_t) len_)
		return ((Error) errno);
	return (0);
}

Error
Serial::recv(void *msg_, size_t len_)
{
	char *msg = static_cast<char *> (msg_);
	int cc;
	Error err;

	if (!isConnected())
		return (ERR_NC);

	lockSend();
	while (len_ > 0) {
		cc = read(connection, msg, len_);
		if (cc <= 0) {
			if (errno != EWOULDBLOCK) {
				unlockSend();
				err = (Error) errno;
				return (err);
			}
			else
				cc = 0;
		}
		msg += cc;
		len_ -= cc;
	}
	unlockSend();
	return (0);
}

}