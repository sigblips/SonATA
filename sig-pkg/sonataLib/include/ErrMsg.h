/*******************************************************************************

 File:    ErrMsg.h
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
// Error message class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/ErrMsg.h,v 1.3 2008/03/10 04:57:19 kes Exp $
//
#ifndef _ErrMsgH
#define _ErrMsgH

#include <map>
#include <string>
#include "Sonata.h"
#include "Err.h"

using std::map;
using std::pair;
using std::string;

namespace sonata_lib {

struct ErrMsgList {
	ErrCode code;
	const char *cstr;
};

typedef map<ErrCode, string> ErrMap;

class ErrMsg {
public:
	static ErrMsg *getInstance();
	
	~ErrMsg();

	void addErrs(const ErrMsgList *list);
	string& getErrMsg(ErrCode code);

private:
	static ErrMsg *instance;

	ErrMap errList;
	string noMsg;

	ErrMsg();
};

}

#endif