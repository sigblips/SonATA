/*******************************************************************************

 File:    PsErr.h
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
// Packetsend-specific error codes
//
// $Header: /home/cvs/nss/sonata-pkg/utilities/packetsend/include/PsErr.h,v 1.1 2008/03/10 05:26:22 kes Exp $
//
#ifndef _PsErrH
#define _PsErrH

#include "Err.h"

namespace sonata_lib {

// internal error codes

enum PsErrCode {
	ERR_IBO = ERR_END + 1,				// input buffer overflow
	ERR_CCC,							// can't create connection
	ERR_ERP								// error receiving packet
};

}

#endif
