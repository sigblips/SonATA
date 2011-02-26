/*******************************************************************************

 File:    Verbose.h
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


#ifndef verbose_H
#define verbose_H

// Define some macros that check a verbose level variable, and send the
// associated code to an output stream if the verbose level is >= to 
// the level defined in the macro name. 
// Higher value verbose levels also cause the
// lower value macros to execute.

// example use:
// int verboseLevel = 1;
// VERBOSE1(verboseLevel, 
//    "verbose level 1 msg is activated" << endl);

#include <ace/Synch.h>
#include "SseUtil.h"

// protect the verbose stream with a mutex
class Verbose {

 public:
    static ACE_Recursive_Thread_Mutex & getMutex();
    
 private:

    // don't allow objects to be created
    Verbose();
    ~Verbose();
    
};

// allow overrides of these macros

#ifndef VERBOSE_INFO

#define VERBOSE_INFO(verboseVar, strminfo, level) \
if ((verboseVar) >= (level)) \
{ ACE_Guard<ACE_Recursive_Thread_Mutex> guard(Verbose::getMutex()); \
std::clog << "____ <" << SseUtil::currentIsoDateTime() << "> " \
__FILE__ << "(" << __LINE__ << "): ____ \n" << strminfo; }

#define VERBOSE0(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 0)
#define VERBOSE1(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 1)
#define VERBOSE2(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 2)
#define VERBOSE3(verboseVar, strminfo) VERBOSE_INFO(verboseVar, strminfo, 3)

#endif // VERBOSE_INFO


#endif // verbose_H