/*******************************************************************************

 File:    File.cpp
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
// File connection class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/File.cpp,v 1.3 2009/05/24 23:27:24 kes Exp $
//
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "File.h"

namespace sonata_lib {

File::File(string name_, string filename_):
		Connection(name_, UnitNone, FileConnection), filename(filename_)
{
}

File::~File()
{
	if (isConnected())
		terminate();
}

Error
File::establish()
{
	if (isConnected())
		return (0);

	// open the file
	if ((connection = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND,
			S_IREAD | S_IWRITE | S_IRGRP | S_IROTH)) < 0) {
		return (errno);
	}
	connected(true);
	return (0);
}

Error
File::terminate()
{
	if (isConnected()) {
		close(connection);
		connection = -1;
	}
	connected(false);
	return (0);
}

Error
File::send(void *msg_, size_t len_)
{
	ssize_t cc;

	if (!isConnected())
		return (ERR_NC);
	if ((cc = write(connection, msg_, len_)) != (ssize_t) len_)
		return (errno);
	fdatasync(connection);
	return (0);
}

}