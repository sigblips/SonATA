/*******************************************************************************

 File:    ErrMsg.cpp
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
// Error msg class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/ErrMsg.cpp,v 1.3 2008/03/10 05:00:52 kes Exp $
//
#include "ErrMsg.h"
#include "Lock.h"

namespace sonata_lib {

ErrMsgList eList[] = {
	{ ERR_NE, "" },

	{ ERR_DLK,	"thread deadlock" },
	{ ERR_ILT, "invalid mutex type" },
	{ ERR_LNO, "thread doesn't own lock" },
	{ ERR_CHS, "can't handle signal" },
	{ ERR_TNR, "task is not runnig" },
	{ ERR_CDT, "can't detach thread" },
	{ ERR_TAR, "thread is already running" },
	{ ERR_NC, "no connection" },
	{ ERR_ICT, "invalid connection type" },
	{ ERR_IHN, "invalid host name" },
	{ ERR_HNF, "host not found" },
	{ ERR_NS, "no socket" },
	{ ERR_MAF, "memory allocation failed" },
	{ ERR_NMA, "no message available" },
	{ ERR_NDA, "no data available" },
	{ ERR_IMT, "invalid message type" },
	{ ERR_IDL, "invalid data length" },
	{ ERR_BAA, "buffer already allocated" },
	{ ERR_NBA, "no buffer available" },
	{ ERR_QSE, "queue send error" },
	{ ERR_BWE, "buffer write error" },
	{ ERR_CAMP, "can't allocate message pool" },
	{ ERR_IU, "invalid unit" },
	{ ERR_IMBT, "invalid memory block type" },
	{ ERR_SE, "send error" },
	{ ERR_IRT, "invalid request type" },
	{ ERR_VNM, "version number mismatch" },
	{ ERR_ICL, "invalid command line" },
	{ ERR_IPT, "invalid polarization type" },
	{ ERR_CCS, "can't create socket" },
	{ ERR_CBS, "can't bind socket" },
	{ ERR_NBP, "no buffer pair" },
	{ ERR_INT, "internal error" },
	{ ERR_LHTL, "lock held too long" },
	{ ERR_DC, "duplicate channel" },
	{ ERR_LMP, "late or missing packets" },
	{ ERR_LNI, "log not initialized" },
	{ ERR_IFH, "invalid filter file header" },
	{ ERR_IFC, "invalid filter file coefficients" },
	{ ERR_COF, "can't open filter file" },
	{ ERR_IAT, "invalid alarm time" },

	{ ERR_END, "" }
};

ErrMsg *ErrMsg::instance = 0;

ErrMsg *
ErrMsg::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ErrMsg();
	l.unlock();
	return (instance);
	
}

ErrMsg::ErrMsg(): noMsg("No corresponding msg")
{
	addErrs(eList);
}

void
ErrMsg::addErrs(const ErrMsgList *list)
{
	for (int i = 0; list[i].code != ERR_END; i++) {
		errList.insert(pair<ErrCode, string>(list[i].code,
				string(list[i].cstr)));
	}
}

ErrMsg::~ErrMsg()
{
}

string&
ErrMsg::getErrMsg(ErrCode code)
{
	ErrMap::iterator pos;

	pos = errList.find(code);
	if (pos == errList.end())
		return (noMsg);
	return (pos->second);
}

}