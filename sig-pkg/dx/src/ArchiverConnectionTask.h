/*******************************************************************************

 File:    ArchiverConnectionTask.h
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
// Task to initiate a connection with the archiver
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverConnectionTask.h,v 1.4 2009/02/22 04:48:36 kes Exp $
//
#ifndef _ArchiverConnectionTaskH
#define _ArchiverConnectionTaskH

#include <sseDxInterface.h>
#include "Args.h"
#include "Msg.h"
#include "Struct.h"
#include "QTask.h"
#include "State.h"
#include "Tcp.h"

using namespace sonata_lib;

namespace dx {

// DX/archiver connection task startup arguments
struct ArchiverConnectionArgs {
	Connection *archiver;				// ptr to archiver connection

	ArchiverConnectionArgs(): archiver(0) {}
	ArchiverConnectionArgs(Connection *archiver_): archiver(archiver_) {}
};

class ArchiverConnectionTask: public QTask {
public:
	static ArchiverConnectionTask *getInstance();
	~ArchiverConnectionTask();

protected:
	void extractArgs();
	void handleMsg(Msg *msg);

private:
	static ArchiverConnectionTask *instance;

	string archiverHost;
	int32_t archiverPort;
	int32_t retrySleep;
	Tcp *archiver;

	State *state;

	Error contact();

	// hidden
	ArchiverConnectionTask(string tname_);

	// forbidden
	ArchiverConnectionTask(const ArchiverConnectionTask&);
	ArchiverConnectionTask& operator=(const ArchiverConnectionTask&);
};

}

#endif