/*******************************************************************************

 File:    ChErrMsg.cpp
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
// Channelizer-specific error messages
//
// $Header: /home/cvs/nss/sonata-pkg/channelizer/src/ChErrMsg.cpp,v 1.2 2008/08/31 01:51:23 kes Exp $
//
#include "ErrMsg.h"
#include "ChErr.h"
#include "ChErrMsg.h"

namespace sonata_lib {

ErrMsgList chList[] = {
	{ (ErrCode) ERR_CCC, "can't create connection" },
	{ (ErrCode) ERR_ECL, "empty channel list" },
	{ (ErrCode) ERR_ICN, "invalid channel number" },
	{ (ErrCode) ERR_NVA, "no channel packet vectors available" },
	{ (ErrCode) ERR_STAP, "start time already past" },
	{ (ErrCode) ERR_IPV, "invalid packet version" },

	{ ERR_END, "" }
};

void buildErrList()
{
	ErrMsg *errList = ErrMsg::getInstance();
	errList->addErrs(chList);
}

}