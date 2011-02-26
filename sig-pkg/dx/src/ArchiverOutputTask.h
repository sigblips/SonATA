/*******************************************************************************

 File:    ArchiverOutputTask.h
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
// Archiver output handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiverOutputTask.h,v 1.4 2009/02/22 04:48:36 kes Exp $
//
#ifndef _ArchiverOutputTaskH
#define _ArchiverOutputTaskH

#include <unistd.h>
#include <sseDxInterface.h>
#include <sseInterface.h>
#include "Msg.h"
#include "QTask.h"
#include "Tcp.h"

using namespace sonata_lib;

namespace dx {

// DX/SSE output task arguments
struct ArchiverOutputArgs {
	Connection *archiver;			// ptr to connection
	
	ArchiverOutputArgs(): archiver(0) {}
	ArchiverOutputArgs(Connection *archiver_): archiver(archiver_) {}
};

//
// This task sends output to the archiver
//
// Notes:
//		Output usually consists of CD data.
//
class ArchiverOutputTask: public QTask {
public:
	static ArchiverOutputTask *getInstance();
	~ArchiverOutputTask();

protected:
	void extractArgs();
	void handleMsg(Msg *msg);

private:
	static ArchiverOutputTask *instance;

	int32_t msgNumber;
	Connection *archiver;

	void marshallComplexAmplitudes(Msg *msg);

	// hidden
	ArchiverOutputTask(string tname_);
	
	// forbidden
	ArchiverOutputTask(const ArchiverOutputTask&);
	ArchiverOutputTask& operator=(const ArchiverOutputTask&);
};

}
	
#endif