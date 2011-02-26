/*******************************************************************************

 File:    RotatingFileLogger.cpp
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
Rotating file logger.

Writes text to a log file.  If the log file exceeds
the maximum specified size, then the logs are rotated,
a new log is created, and the oldest log is deleted.
Older logs get the suffix ".1, .2, etc".

Parameters:
- Log file directory
- Log file basename
- Max log file size in bytes
- Max number of log files in the rotation

Example:

Assume the log file dir is "/home/fred",
the basename is "log.txt", and the max number
of log files is 3.

Then at any one time the existing logs will be: 

/home/fred/log.txt   (current log)
/home/fred/log.txt.1 (next oldest)
/hoem/fred/log.txt.2 (oldest)

When log.txt exceeds the maximum size,
the logs are rotated like this:

/home/fred/log.txt.1 is copied to /home/fred/log.txt.2
/home/fred/log.txt is copied to /home/fred/log.txt.1
/home/fred/log.txt is recreated empty.


*/

#include "RotatingFileLogger.h" 
#include "SseUtil.h"
#include "SseException.h"
#include <iostream>
#include <sstream>

using namespace std;


static void openFileStreamAppend(ofstream &strm,
                                 const string &filename)
{
    strm.open(filename.c_str(), (ios::app));
    if (!strm.is_open())
    {
        stringstream strm;
        strm << "RotatingFileLogger::"
             << "openOutputFileStream: failure opening:  " 
             << filename << "\n";
        throw SseException(strm.str());
    }
}

static void renameFile(const string &oldPath, const string &newPath)
{
   if (rename(oldPath.c_str(), newPath.c_str()) == -1)
   {
      // TBD check errno

      stringstream strm;
      strm << "RotatingFileLogger::"
           << "renameFile: error renaming " 
           << oldPath << " to " << newPath << "\n";
      throw SseException(strm.str()); 
   }
}


RotatingFileLogger::RotatingFileLogger(
   const string &logDir,
   const string &baseLogname,
   int maxFileSizeBytes,
   int maxFiles)
   :
   logDir_(logDir),
   baseLogname_(baseLogname),
   maxFileSizeBytes_(maxFileSizeBytes),
   maxFiles_(maxFiles)
{
   // TBD exception/error handling
   baseLogFullPath_ = logDir_ + "/" +  baseLogname_;
   openStream();
}

RotatingFileLogger::~RotatingFileLogger()
{
}

void RotatingFileLogger::openStream()
{
   // TBD catch open error
   openFileStreamAppend(strm_, baseLogFullPath_); 
}

int RotatingFileLogger::fileSizeBytes()
{
   return strm_.tellp();
}

void RotatingFileLogger::checkForFileRotation()
{
   if (fileSizeBytes() > maxFileSizeBytes_)
   {
      rotateFiles();
   }
}

void RotatingFileLogger::closeStream()
{
   strm_.flush();
   strm_.close();
}

void RotatingFileLogger::rotateFiles()
{
   closeStream();

   shuffleFiles();

   openStream();
}

/*
  For all log files of the right name that
  exist, copy the newer ones to the older ones.
 */

void RotatingFileLogger::shuffleFiles()
{
   vector<string> logFilenames;

   // base filename
   logFilenames.push_back(baseLogFullPath_);
   
   // older logs
   for (int i=1; i<maxFiles_; ++i)
   {
      stringstream filename;
      filename << baseLogFullPath_
               <<  "." << SseUtil::intToStr(i);

      logFilenames.push_back(filename.str());
   }

// DEBUG - print filenames
#if 0
   for (vector<string>::iterator it=logFilenames.begin();
        it != logFilenames.end(); ++it)
   {
      cout << *it << endl;
   }
#endif   

   // shuffle them.  The full sequence of older files
   // may not be in place so verify the existence of
   // each.

   vector<string>::reverse_iterator it=logFilenames.rbegin();
   string olderFile(*it++);
   for (; it != logFilenames.rend(); ++it)
   {
      const string &newerFile(*it);
      if (SseUtil::fileIsReadable(newerFile))
      {
#if 0
         cout << "copy " << newerFile << " to " << olderFile << endl;
#endif
         renameFile(newerFile, olderFile);
      }
      olderFile = *it;
   }

}