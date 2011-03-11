/*******************************************************************************

 File:    SseUtil.cpp
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



#include "SseUtil.h" 
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include "Assert.h"

#ifdef linux
#include <sys/statfs.h>
#else
#include <sys/statvfs.h>
#endif

using namespace std;

// find the size of a file
int SseUtil::getFileSize(const string &filename)
{
    int filesize = 0;

    struct stat buf;
    if ( stat(filename.c_str(), &buf) == 0)
    {
        filesize = buf.st_size;
    }
    else 
    {
	string errorStr("SseUtil::getFileSize can't determine filesize for "
			+ filename);
	throw SseException(errorStr);
    }

    return filesize;
}


double SseUtil::getDiskPercentUsed(const string &pathToDisk)
{

#ifdef linux
    struct statfs buf;
    int status = statfs(pathToDisk.c_str(), &buf);
#else
    struct statvfs buf;
    int status = statvfs(pathToDisk.c_str(), &buf);
#endif

    if (status != 0)
    {
	stringstream strm;
	strm << "SseUtil::getDiskPercentUsed(): "
	     << "Error accessing disk via path '"
	     << pathToDisk << "', " 
	     << "stat[v]fs generated errno: " << errno 
	     << ": " << strerror(errno) << endl;

	throw SseException(strm.str());
    }
    
    long totalBlocks = buf.f_blocks;
    long totalAvail = buf.f_bavail;

    double percentUsed = static_cast<double>(totalBlocks - totalAvail) / 
	totalBlocks * 100;
    
    return percentUsed;
}



// TBD fix me
#include <unistd.h>
string SseUtil::getHostname()
{
    const int MAXHOSTNAMELEN = 1024;  // TBD get from header file.
    char hostName[MAXHOSTNAMELEN];
    
    gethostname(hostName, MAXHOSTNAMELEN);
    return hostName;
}



bool SseUtil::truncateFile(const string &filename)
{
   ofstream fout(filename.c_str(), (ios::out | ios::trunc));
   if (!fout.is_open()) {
       cerr << "File truncate failed on " << filename << endl;
      return false;
   }
   fout.close();

   return true;
}

bool SseUtil::fileIsReadable(const string &filename)
{
    ifstream file(filename.c_str());
    if (!file) {
	return false;
    }
     
    file.close();
    return true;

}

void SseUtil::openOutputFileStream(ofstream &strm,
				   const string &filename)
{
    // open an output text stream attached to a file

    strm.open(filename.c_str(), (ios::out | ios::trunc));
    if (!strm.is_open())
    {
	stringstream strm;
	strm << " SseUtil::openOutputFileStream: failure opening:  " << filename;
	throw SseException(strm.str());
    }
}

vector<string> SseUtil::loadFileIntoStringVector(const string &filename)
{
    vector<string> loadedText;

    ifstream file(filename.c_str());
    if (!file)
    {
       cerr << "File Open failed on " << filename << endl;
       // TBD more error handling
    } 
    else
    {
	// load in the file
	string line;
	while (getline(file, line)) 
	{
	    loadedText.push_back(line);
	}
    }  
    file.close();

    return loadedText;
}

string  SseUtil::readFileIntoString(const string &filename)
{
    ifstream ifile(filename.c_str());
    Assert(ifile); // make sure it opened (TBD better error handling)

    ostringstream buf;
    char ch;
    while (buf && ifile.get(ch))
    {
	buf.put(ch);
    }
    
    return buf.str();

}


string SseUtil::intToStr(int value)
{
    stringstream strm;

    strm << value;

    return strm.str();
}

// convert a string to an int.
// Throws exception if an error occurs in the conversion.
int SseUtil::strToInt(const string & strValue)
{
    istringstream iss(strValue);
    int intValue;
    iss >> intValue; 

    // check for error in the conversion
    if (!iss) 
    {
       stringstream strm;
       strm << "SseUtil::strToInt error converting string: "
	    << "'" << strValue << "'\n";

       throw SseException(strm.str());
    }

    return intValue;
}

// convert a string to a double.
// Throws exception if an error occurs in the conversion.
double SseUtil::strToDouble(const string & strValue)
{
    istringstream iss(strValue);
    double doubleValue;
    iss >> doubleValue; 

    // check for error in the conversion
    if (!iss)
    {
       stringstream strm;
       strm << "SseUtil::strToDouble error converting string: "
	    << "'" << strValue << "'\n";

       throw SseException(strm.str());
    }

    return doubleValue;
}


// strMaxCpy copies at most destSize-1 characters
// (destSize being the size of the char buffer dest) from src
// to dest, truncating src if necessary.  The result is always
// null-terminated.

void SseUtil::strMaxCpy(char *dest, const char *src, size_t destSize)
{
    Assert(destSize > 0);
    Assert(dest != 0);
    Assert(src != 0);

    strncpy(dest, src, destSize);
    dest[destSize-1] = '\0';

}

// Send out a mail message using the ssemailmsg script

void SseUtil::mailMsg(const string &subject, const string &toList, 
		      const string &body)
{
    stringstream cmd;

    // replace any use of apostrophe

    char oldChar('\'');
    char newChar('`');

    string cleanSubject(subject);
    replace(cleanSubject.begin(), cleanSubject.end(), oldChar, newChar);

    string cleanBody(body);
    replace(cleanBody.begin(), cleanBody.end(), oldChar, newChar);

    cmd << "ssemailmsg.sh" 
	<< " '" << cleanSubject << "'"
	<< " '" << toList << "'"
	<< " '" << cleanBody  << "'" << endl;

    //cout << cmd.str() << endl;

    // int status = 
    system(cmd.str().c_str());

    // TBD check status;
}

// Search input string for all occurrences of the subString,
// and prepend a slash before each one.  The resulting string
// is returned as the function value.

string SseUtil::insertSlashBeforeSubString(const string & input, 
				  const string & subString)
{
    string output = input;
    string::size_type startIndex = 0;
    string::size_type keyIndex = 0;
    while ((keyIndex = output.find(subString, startIndex))
	   != string::npos)
    {
	output.insert(keyIndex, "\\");

	// skip to the next position after the substring
	startIndex = keyIndex + subString.length() + 2;
    }
    
    return output;

}



// return the current time in iso8601 format
string SseUtil::currentIsoDateTimeSuitableForFilename()
{
    time_t current_time_t;

    // get current time
    time(&current_time_t);

    return SseUtil::isoDateTimeSuitableForFilename(current_time_t);
}


string SseUtil::isoDateTimeSuitableForFilename(const time_t &clock)
{
    string isoDateTime = SseUtil::isoDateTime(clock);

    // change spaces to underscores and colons to hyphens
    // eg "2002-10-05 23:44:17 UTC" becomes
    // eg "2002-10-05_23-44-17_UTC"

    // tbd change these to use string find functions
    // to avoid hard coding the indexes

    // replace spaces
    isoDateTime.replace(10,1,"_");
    isoDateTime.replace(19,1,"_");

    // replace colons
    isoDateTime.replace(13,1,"-");
    isoDateTime.replace(16,1,"-");

    return isoDateTime;

}




// return just the current date in iso format (no time field)
string SseUtil::currentIsoDate()
{
    string date = currentIsoDateTime();

    // remove all chars after the date portion
    unsigned int nDateChars = string("YYYY-MM-DD").size();
    Assert(date.size() >= nDateChars);

    date.erase(nDateChars);  // erase the time portion

    return date;
}


// return the current time in iso8601 format
string SseUtil::currentIsoDateTime()
{
    time_t current_time_t;

    // get current time
    time(&current_time_t);

    return SseUtil::isoDateTime(current_time_t);
}

/**
   Return the given time as a string in iso8601 format in UTC.
   YYYY-MM-DD HH:MM:SS UTC
   Month range:1-12, Day range:1-31
   Fields have leading zero fill (eg, 2001-01-07 02:04:06)
*/

string SseUtil::isoDateTime(const time_t &clock)
{
    // convert to iso format
    stringstream timeString;
    timeString << setfill('0');  // set leading zeros
    // note forced 2 digit field widths set below

    // get time in UTC 
    tm t;
    gmtime_r(&clock, &t);  // use thread-safe version

    // YYYY-MM-DD
    timeString << t.tm_year + 1900 << "-";  // year 
    timeString << setw(2) << t.tm_mon + 1 << "-";  // month range: 1-12
    timeString << setw(2) << t.tm_mday;    // day range: 1-31

    timeString << " ";   // date - time separator

    // hh:mm:ss
    timeString << setw(2) << t.tm_hour << ":";   // hours
    timeString << setw(2) << t.tm_min << ":";    // minutes
    timeString << setw(2) << t.tm_sec;        // seconds

    timeString << " UTC";

    return timeString.str();
}


string SseUtil::isoTimeWithoutTimezone(const time_t &clock)
{
    // convert to iso format
    stringstream timeString;
    timeString << setfill('0');  // set leading zeros
    // note forced 2 digit field widths set below

    // get time in UTC 
    tm t;
    gmtime_r(&clock, &t);  // use thread-safe version

    // hh:mm:ss
    timeString << setw(2) << t.tm_hour << ":";   // hours
    timeString << setw(2) << t.tm_min << ":";    // minutes
    timeString << setw(2) << t.tm_sec;        // seconds

    return timeString.str();
}



string SseUtil::isoDateTimeWithoutTimezone(const time_t &clock)
{
    string dateTime = isoDateTime(clock);

    // remove all chars after the datetime portion
    unsigned int nDesiredChars = 
	string("YYYY-MM-DD HH:MM:SS").size();
    Assert(dateTime.size() >= nDesiredChars);

    dateTime.erase(nDesiredChars);  // erase the time zone

    return dateTime;
}


/*
  Round the value to numPlaces decimal places
 */

double SseUtil::round(double value, int numPlaces)
{
   Assert(numPlaces >= 0);

   const double base(10.0);
   double roundingConst(0.5);
   if (value < 0.0)
   {
      roundingConst = -roundingConst;
   }

   double factor = pow(base, numPlaces);
   int temp = static_cast<int>(value * factor + roundingConst);

   return temp / factor;
}

double SseUtil::dbToLinearRatio(double db)
{
   const double base(10);
   return pow(base, db/10);
}

double SseUtil::linearRatioToDb(double linearRatio)
{
   const double base(10);

   // Note: just let zero case go to inf

   return (base * log10(linearRatio));
}
