/*******************************************************************************

 File:    Dx.h
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
// DX main task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/Dx.h,v 1.2 2009/02/13 03:06:31 kes Exp $
//
#ifndef DX_H_
#define DX_H_

#include "Sonata.h"
#include "Args.h"
#include "CmdTask.h"
#include "ControlTask.h"
#include "SseConnectionTask.h"
#include "SseInputTask.h"
#include "SseOutputTask.h"
#include "Partition.h"
#include "Tcp.h"

using namespace sonata_lib;

namespace dx {

class Dx {
public:
	static Dx *getInstance();
	~Dx();

	void run(int argc, char **argv);

private:
	static Dx *instance;

	Connection *sse;

	CmdTask *cmdTask;
	ControlTask *controlTask;
	SseConnectionTask *sseConnectionTask;
	SseInputTask *sseInputTask;
	SseOutputTask *sseOutputTask;

	CmdArgs cmdArgs;
	ControlArgs controlArgs;
	SseConnectionArgs sseConnectionArgs;
	SseInputArgs sseInputArgs;
	SseOutputArgs sseOutputArgs;

	void init();						// perform system initialization
	void createPartitions();			// allocate partition space
	void createConnections();			// create external connections
	void createTasks();					// create primary tasks
	void startTasks();

	// hidden
	Dx();

	// forbidden
	Dx& operator=(const Dx&);
	Dx(const Dx&);
};

}

#endif /*DX_H_*/