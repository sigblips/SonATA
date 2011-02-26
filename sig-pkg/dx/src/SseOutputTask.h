/*******************************************************************************

 File:    SseOutputTask.h
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
// Detector->SSE output handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SseOutputTask.h,v 1.4 2009/02/22 04:48:38 kes Exp $
//
#ifndef _SseOutputTaskH
#define _SseOutputTaskH

#include <unistd.h>
#include <sseInterface.h>
#include <sseDxInterface.h>
#include "Msg.h"
#include "QTask.h"
#include "System.h"
#include "Tcp.h"

using namespace sonata_lib;

namespace dx {

// DX/SSE output task arguments
struct SseOutputArgs {
	Connection *sse;				// ptr to connection

	SseOutputArgs(): sse(0) {}
	SseOutputArgs(Connection *sse_): sse(sse_) {}
};

//
// This task receives input from the SSE and sends it on to the
// command processor via a queue.  If communication is lost (i.e.,
// there is no connection to the SSE), then a request is sent to
// the broadcast task to re-establish the connection.
//
// Notes:
//		This task does not have an input queue - all input comes from
//		the SSE
//
class SseOutputTask: public QTask {
public:
	static SseOutputTask *getInstance(string tname_ = "");
	~SseOutputTask();

protected:
	void extractArgs();
	void handleMsg(Msg *msg);

private:
	static SseOutputTask *instance;

	int32_t msgNumber;
	Connection *sse;

	void marshallBaseline(void *data);
	void marshallComplexAmplitudes(void *data);
	void marshallPulseSignal(void *data);

	// hidden
	SseOutputTask(string tname_);
	// forbidden
	SseOutputTask(const SseOutputTask&);
	SseOutputTask& operator=(const SseOutputTask&);
};

}

#endif