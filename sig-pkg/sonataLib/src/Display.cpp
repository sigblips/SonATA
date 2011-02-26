/*******************************************************************************

 File:    Display.cpp
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
// Display: keyboard connection class, derived from Connection
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Display.cpp,v 1.3 2009/05/24 23:26:11 kes Exp $
//

#include <errno.h>
#include <unistd.h>
#include "Display.h"

namespace sonata_lib {

Display::Display(string name_, Unit unit_):
		Connection(name_, unit_, DisplayConnection)
{
}

Display::~Display()
{
	if (isConnected())
		terminate();
}

Error
Display::establish()
{
	if (isConnected())
		return (0);

	// open standard output
	// duplicate the standard output
	if ((connection = dup(1)) < 0)
		return (errno);
	connected(true);
	return (0);
}

Error
Display::terminate()
{
	if (isConnected()) {
		close(connection);
		connection = -1;
	}
	connected(false);
	return (0);
}

Error
Display::send(void *msg_, size_t len_)
{
	ssize_t cc;

	if ((cc = write(connection, msg_, len_)) != (ssize_t) len_)
		return (errno);
	return (0);
}

}
