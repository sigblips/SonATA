/*******************************************************************************

 File:    DebugLog.h
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


#ifndef DebugLog_H
#define DebugLog_H

#include <ace/Synch.h>
#include "SseUtil.h"
#include "RotatingFileLogger.h"

/*
  See .cpp file for documentation.
 */

// DEBUG

#ifndef SSECOMMUTIL_VERBOSE_OVERRIDE
  #include "Verbose.h"
#endif

class DebugLog
{
 public:

   static DebugLog * instance();
   virtual ~DebugLog();

   RotatingFileLogger & getLogger();
   static ACE_Recursive_Thread_Mutex & getMutex();


 private:

   // This is a Singleton, so only allow one instance
   DebugLog();

   static DebugLog * instance_;
   RotatingFileLogger logger_;

   // Disable copy construction & assignment.
   // Don't define these.
   DebugLog(const DebugLog& rhs);
   DebugLog& operator=(const DebugLog& rhs);

};

#ifdef SSECOMMUTIL_VERBOSE_OVERRIDE

#define VERBOSE_INFO(verboseVar, strminfo, level) \
if ((verboseVar) >= (level)) \
{ ACE_Guard<ACE_Recursive_Thread_Mutex>\
      guard(DebugLog::getMutex());\
  try { \
     DebugLog::instance()->getLogger() \
     << "____ <" << SseUtil::currentIsoDateTime() << "> " \
     __FILE__ << "(" << __LINE__ << "): ____ \n" << strminfo;\
  } \
  catch (SseException & except)\
  {\
       cerr << except;\
  }\
}

#define VERBOSE0(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 0)
#define VERBOSE1(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 1)
#define VERBOSE2(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 2)
#define VERBOSE3(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 3)

#endif

#endif // DebugLog_H