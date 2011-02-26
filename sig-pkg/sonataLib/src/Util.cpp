/*******************************************************************************

 File:    Util.cpp
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
// Utility functions
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Util.cpp,v 1.2 2008/02/25 22:35:56 kes Exp $
//
#include <unistd.h>
#include "Util.h"
#include "Err.h"

namespace sonata_lib {

uint32_t
GetNextMsg()
{
	static uint32_t nextMsg = 0;

	return (nextMsg++);
}

void
GetNssDate(NssDate& nssDate, timeval *time)
{
	timeval tv;

	if (!time)
		gettimeofday(&tv, NULL);
	else
		tv = *time;

	nssDate.tv_sec = tv.tv_sec;
	nssDate.tv_usec = tv.tv_usec;
}
	
}