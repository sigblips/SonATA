/*******************************************************************************

 File:    LogTask.h
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
// Logging task
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/LogTask.h,v 1.2 2008/02/25 22:35:23 kes Exp $
//
#ifndef _LogTaskH
#define _LogTaskH

#include "Connection.h"
#include "Msg.h"
#include "QTask.h"

namespace sonata_lib {

//
// This task is instantiated as part of the process of creating
// a log.  It runs at a low priority, receiving messages from a
// queue and sending them to the specified connection.  This
// approach allows high priority tasks to perform logging without
// slowing the system down.
//
class LogTask: public QTask {
public:
	LogTask(string tname_, Connection *connection_);
	~LogTask();

protected:
	void extractArgs() { }
	void handleMsg(Msg *msg);
	void sendMsg(Msg *msg);

private:
	Connection *connection;

	// forbidden
	LogTask(const LogTask&);
	LogTask& operator=(const LogTask&);
};

}

#endif