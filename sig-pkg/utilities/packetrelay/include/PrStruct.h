/*******************************************************************************

 File:    PrStruct.h
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

// Structures

#ifndef PRSTRUCT_H_
#define PRSTRUCT_H_

#include "System.h"

namespace sonata_packetrelay {

// network host specification data
struct HostSpec {
	IpAddress addr;						// name/address
	int32_t port;						// port
	in_addr inaddr;						// host IP address

	HostSpec(): port(DEFAULT_OUTPUT_PORT) {
		memcpy(addr, DEFAULT_OUTPUT_ADDR, sizeof(addr));
	}
	HostSpec(const IpAddress addr_, int32_t port_): port(port_) {
		memcpy(addr, addr_, sizeof(addr));
	}
};

}

#endif /*PRSTRUCT_H_*/