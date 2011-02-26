/*******************************************************************************

 File:    Log.h
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
// System log class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Log.h,v 1.3 2009/05/24 23:15:04 kes Exp $
//
#ifndef _LogH
#define _LogH

#include <sstream>
#include <stdarg.h>
#include <sseInterface.h>
#include <sseDxInterface.h>
#include "Sonata.h"
//#include "Args.h"
#include "ErrMsg.h"
#include "Connection.h"
#include "Lock.h"
#include "LogTask.h"
#include "Partition.h"
#include "Timer.h"

using std::ostringstream;

namespace sonata_lib {

//
// This is the generic DX log class
//
// All logging is done via queues and low-priority tasks:
//	(1) When the log object is created, a corresponding log handler
//		task is also created.  This task has a low priority and talks
//		directly to the logging device (which may be a socket, a disk
//		file, the console, a serial port, or even another queue).
//	(2) To log a data item, the detailed log message string is
//		constructed by the caller, then passed to the log function
//		along with a severity and message code.  A DxMsg is
//		constructed with data type NssError and sent to the log
//		handler task input queue.  Control is then returned to the
//		caller.
//	(3) The log handler task will read the message from the queue
//		and dispose of it as required.
//
// Notes:
//		The use of a separate task to perform the actual logging
//		allows the relatively slow work of logging to be removed
//		from the actual processing flow.
//
class Log {
public:
	static Log *getInstance(NssMessageSeverity severity_ = SEVERITY_ERROR,
			DxMessageCode msgCode_ = SEND_DX_MESSAGE,
			Connection *connection_ = 0);
	static void timeout(int32_t msec);

	~Log();

	void setSeverity(NssMessageSeverity severity_);
	bool isEnabled(NssMessageSeverity severity_) { return (severity_ >= severity); }
//	void log(NssMessageSeverity severity_, DxMessageCode code_,
//			ostringstream& str);
	void log(NssMessageSeverity severity_, DxMessageCode code_,
			int32_t activityId_, const char *file_, const char *func_,
			int32_t line_, const char *fmt_, ...);

private:
	static Log *instance;

	NssMessageSeverity severity;		// lowest level enabled
	Lock llock;							// serialization lock
	DxMessageCode msgCode;				// message code to send
	LogTask task;						// logging task
	Connection *connection;				// log connection
	ErrMsg *errList;					// error messages
	MsgList *msgList;					// message list
	PartitionSet *partitionSet;			// partitions
	Queue *logQ;						// logging queue
	string lname;						// log name

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

	Log(string name_, NssMessageSeverity severity_ = SEVERITY_ERROR,
			DxMessageCode msgCode_ = SEND_DX_MESSAGE,
			Connection *connection_ = 0);

	// forbidden
	Log(const Log&);
	Log& operator=(const Log&);
};

// logging macros
#ifdef notdef
#define LogData(severity, code, msg)	{ if (dataLog->isEnabled(severity)) { \
									ostringstream ostr; \
									ostr << __FILE__ << ": " << msg; \
									dataLog->log(severity, \
									(DxMessageCode) code, ostr); \
									} \
								}

#define LogSse(severity, code, msg)	{ if (sseLog->isEnabled(severity)) { \
									ostringstream ostr; \
									ostr << __FILE__ << ": " << msg; \
									sseLog->log(severity, \
									(DxMessageCode) code, ostr); \
									} \
								}
#endif
#define LogInfo(code, activityId, fmt, args...)	Log::getInstance()->log(SEVERITY_INFO, \
									(DxMessageCode) code, activityId, \
									__FILE__, __FUNCTION__, __LINE__, \
									fmt, ##args)
#define LogWarning(code, activityId, fmt, args...) Log::getInstance()->log(SEVERITY_WARNING, \
									(DxMessageCode) code, activityId, \
									__FILE__, __FUNCTION__, __LINE__, \
									fmt, ##args)
#define LogError(code, activityId, fmt, args...) Log::getInstance()->log(SEVERITY_ERROR, \
									(DxMessageCode) code, activityId, \
									__FILE__, __FUNCTION__, __LINE__, \
									fmt, ##args)
#define LogFatal(code, activityId, fmt, args...) { \
									Log::getInstance()->log(SEVERITY_ERROR, \
										(DxMessageCode) code, activityId, \
										__FILE__, __FUNCTION__, __LINE__, \
										fmt, ##args); \
										Log::timeout(3000); \
										Fatal(code); \
									}

}

#endif