/*******************************************************************************

 File:    SseArchive.cpp
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



#include <ace/Synch.h>
#include "SseArchive.h" 
#include "SseUtil.h"
#include "Assert.h"

using namespace std;

static void makeDir(string dir);
static bool dirIsAccessible(string dirPath);

// Determine the SSE archive root directory.
// Use the environment variable override if it's set,
// else use the default.

string SseArchive::getArchiveDir()
{
    string archiveDir = SseUtil::getEnvString("SSE_ARCHIVE"); 
    if (archiveDir == "")
    {
	// get the default
	string home = SseUtil::getEnvString("HOME");
	Assert (home != "");  // assume HOME is defined
 
        string defaultSubdir("/sonata_archive");
        archiveDir = home + defaultSubdir;
    }
    Assert (archiveDir != ""); // should never be undefined
    archiveDir += "/";     // always add a trailing slash
  
    return archiveDir;

}

// return the subdirectory for temporary logs
string SseArchive::getArchiveTemplogsDir()
{
    return getArchiveDir() + "templogs/";
}

// return the subdirectory for permanent logs
string SseArchive::getArchivePermlogsDir()
{
    return getArchiveDir() + "permlogs/";
}


// return the subdirectory for permanent logs
string SseArchive::getArchiveSystemlogsDir()
{
    return getArchivePermlogsDir() + "systemlogs/";
}

// return the subdirectory for permanent logs
string SseArchive::getArchiveErrorlogsDir()
{
    return getArchivePermlogsDir() + "errorlogs/";
}


// return the subdirectory for system files
string SseArchive::getArchiveSystemDir()
{
    return getArchiveDir() + "system/";
}

// return the subdirectory for system files
string SseArchive::getConfirmationDataDir()
{
    return getArchiveDir() + "confirmdata/";
}


// Prepare the data products archive subdirectory
// & return it.  Because this subdir is date
// dependent there is no separate "get" method
// for it.
string SseArchive::prepareDataProductsDir(int actId)
{
    string isoDate = SseUtil::currentIsoDate();
    string dataDir = getArchiveDir() + isoDate + "/"; 

    // TBD error handling
    makeDir(dataDir);

    // Add subdir based on activity Id
    dataDir += "/act" + SseUtil::intToStr(actId) + "/"; 
    makeDir(dataDir);

    return dataDir;
}

// prepare the archive 
void SseArchive::setup()
{
   // Make sure the archive dir exists.  If it doesn't
   // try to create it.  If the create fails, then bail out.

   string archiveDir = getArchiveDir();
   if (!dirIsAccessible(archiveDir))
   {
       cerr << "Archive directory does not exist: "
	    << archiveDir << endl;

       // try to create it
       makeDir(archiveDir);
       if (dirIsAccessible(archiveDir))
       {
	   cerr << "Created archive directory: "  << archiveDir << endl;
       }
       else
       {
	   cerr << "Error: could not create archive directory: "
		<< archiveDir << endl;
	   cerr << "Please check the SSE_ARCHIVE environment variable." << endl;
	   throw 1;  // TBD use better exception types
       }
   }

   // TBD more error handling

   // create the non-date-dependent archive subdirs if 
   // they don't already exist.

   // templogs
   makeDir(getArchiveTemplogsDir()); 

   // permlogs
   makeDir(getArchivePermlogsDir());

   // systemlogs & errorlogs
   makeDir(getArchiveSystemlogsDir());
   makeDir(getArchiveErrorlogsDir());

   // system dir
   makeDir(getArchiveSystemDir()); 

   // confirmation data
   makeDir(getConfirmationDataDir());


}

// Create error logs in the permlogs directory
 
static ACE_Recursive_Thread_Mutex errorLogMutex;

SseArchive::ErrorLog::ErrorLog()
    : Log(getArchiveErrorlogsDir() + 
	  "errorlog-" + SseUtil::currentIsoDate() + ".txt", errorLogMutex)
{}

static ACE_Recursive_Thread_Mutex systemLogMutex;

SseArchive::SystemLog::SystemLog()
    : Log(getArchiveSystemlogsDir() + 
	  "systemlog-" + SseUtil::currentIsoDate() + ".txt", systemLogMutex)
{}


#include <sys/types.h>
#include <sys/stat.h>

// Create a directory.  It's not an error if the directory
// already exists.
static void makeDir(string dir)
{
   mode_t mode = S_IRWXU |  // Owner R,W,X
       S_IRGRP | S_IXGRP |  // Group R,X
       S_IROTH | S_IXOTH;   // Others R,X
 
   mkdir(dir.c_str(), mode);

}

static bool dirIsAccessible(string dirPath)
{
    struct stat buf;

    if (stat(dirPath.c_str(), &buf) == 0)
    {
	return true;
    }
    return false;

}