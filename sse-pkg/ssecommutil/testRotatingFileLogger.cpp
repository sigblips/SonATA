/*******************************************************************************

 File:    testRotatingFileLogger.cpp
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

#include <iostream>

using namespace std;

#include "SseUtil.h"
#include "SseArchive.h"
#include "RotatingFileLogger.h"
#include "SseException.h"

void writeText(RotatingFileLogger * logger, const string &text)
{
   *logger << SseUtil::currentIsoDateTime() <<  ": " << text << "\n";
}

static const int MaxFileSizeBytes(50);
static const int MaxLogfiles(4);

int main()
{
   try
   {
   cout << "hello, test rotating file logger" << endl;
   //SseUtil::getFileSize("/dev/null");

   string logDir(SseArchive::getArchiveTemplogsDir());
   string baseLogname("test-sse-debug-log.txt");
   
#if 0
   cout << "logDir: " << logDir
        << " baseLogname: " << baseLogname << endl;
#endif

   RotatingFileLogger * logger = new RotatingFileLogger(
      logDir, baseLogname, MaxFileSizeBytes,
      MaxLogfiles);

   writeText(logger, "line1 1234567890");
   writeText(logger, "line2 1234567890");
   writeText(logger, "line3 1234567890");

   delete logger;

   }
   catch (SseException &except)
   {
      cerr << except << endl;
   }
   
}