/*******************************************************************************

 File:    DebugLog.cpp
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



/*
  Handle debug log output via the VERBOSE<n> macros,
  overriding the definitions in the Verbose.h header file.
  
  Output goes to the rotating debug logs in the SSE archive, 
  i.e., sonata_archive/templogs/sse-debug-log.txt[n].
  These files are shuffled in a 'conveyor belt' fashion:
  when the sse-debug-log.txt file exceeds the maximum
  allowed size, its contents are moved to the 
  sse-debug-log.txt.1 file.  The contents of that file are in
  turn moved to the sse-debug-log.txt.2 file, and so on.
  The contents of the last sse-debug-log.txt.N file
  are discarded.

  The maximum size of each file can be set via the 
  SSE_DEBUG_LOG_MAX_FILESIZE_MEGABYTES environment variable.

  Note that this class is a Singleton. 

 */


#include "DebugLog.h" 
#include "SseArchive.h"

static const string MaxFileSizeEnvVar(
   "SSE_DEBUG_LOG_MAX_FILESIZE_MEGABYTES");

static string LogDir(SseArchive::getArchiveTemplogsDir());
static string BaseLogname("sse-debug-log.txt");
static const double DefaultMaxFileSizeMegaBytes(25);
static const double SmallestAllowedMaxFileSizeMegabytes(0.010);
static const double LargestAllowedMaxFileSizeMegabytes(500);
static const int MaxLogfiles(4);
static const double BytesPerMegabyte(1e6);

// set up the Singleton
DebugLog * DebugLog::instance_ = 0;
static ACE_Recursive_Thread_Mutex singletonLock_;

static double getMaxFileSizeMegaBytes()
{
   // Check env var first
    string filesizeMbytesString = SseUtil::getEnvString(
       MaxFileSizeEnvVar); 
    if (filesizeMbytesString == "")
    {
       return DefaultMaxFileSizeMegaBytes;
    }
    else
    {
       try 
       {
          double requestedMaxFileSizeMegabytes = 
             SseUtil::strToDouble(filesizeMbytesString);

          if (requestedMaxFileSizeMegabytes >=
              SmallestAllowedMaxFileSizeMegabytes && 
             requestedMaxFileSizeMegabytes <=
              LargestAllowedMaxFileSizeMegabytes)
          {
             return requestedMaxFileSizeMegabytes;
          }
          else
          {
             SseArchive::ErrorLog()
                << "Error: Environment variable "
                << MaxFileSizeEnvVar
                << " value of '"
                << filesizeMbytesString
                << "' is invalid."
                << " Value must be between " 
                << SmallestAllowedMaxFileSizeMegabytes
                << " and "
                << LargestAllowedMaxFileSizeMegabytes
                << ".  Using default: " 
                << DefaultMaxFileSizeMegaBytes << endl;

             return DefaultMaxFileSizeMegaBytes;
          }

       }
       catch(SseException &except)
       {
             SseArchive::ErrorLog() 
                << "Error: Environment variable "
                << MaxFileSizeEnvVar
                 << " value of '"
                << filesizeMbytesString
                << "' is not a valid number. " 
                << "Using default: "
                << DefaultMaxFileSizeMegaBytes << endl;
             
             return DefaultMaxFileSizeMegaBytes;
       }
    }
}


DebugLog * DebugLog::instance()
{
   // Use "double-check locking optimization" design 
   // pattern to prevent initialization race condition.

   if (instance_ == 0)
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(singletonLock_);
      if (instance_ == 0)
      {
         instance_ = new DebugLog();
      }
   }

   return instance_;
}


DebugLog::DebugLog()
   :
   logger_(LogDir, BaseLogname, 
           static_cast<int>(getMaxFileSizeMegaBytes() * BytesPerMegabyte),
           MaxLogfiles)
{
}

DebugLog::~DebugLog()
{
}

RotatingFileLogger & DebugLog::getLogger()
{
   return logger_;
}
  
ACE_Recursive_Thread_Mutex & DebugLog::getMutex()
{
   return singletonLock_;
}