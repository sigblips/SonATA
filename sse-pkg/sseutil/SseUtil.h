/*******************************************************************************

 File:    SseUtil.h
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


#ifndef SseUtil_H
#define SseUtil_H

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "SseException.h"

#include <string>
#include <vector>
#include <iosfwd>

using std::string;
using std::vector;

class SseUtil
{
 public:
    static int getFileSize(const string &filename);
    static double getDiskPercentUsed(const string &pathToDisk);
    static string getHostname();
    static string getHostIpAddr(const string &hostname);
    static string intToStr(int value);
    static int strToInt(const string & strValue);
    static double strToDouble(const string & strValue);
    static bool strCaseEqual(const string &str1, const string &str2);
    static string strToLower(const string &strValue);
    static string strToUpper(const string &strValue);
    static bool fileIsReadable(const string &filename);
    static bool truncateFile(const string &filename);
    static void openOutputFileStream(std::ofstream &strm,
				     const string &filename);
    static string currentIsoDateTime();
    static string currentIsoDate();
    static string isoDateTime(const time_t &clock);
    static string isoDateTimeSuitableForFilename(const time_t &clock);
    static string isoDateTimeWithoutTimezone(const time_t &clock);
    static string isoTimeWithoutTimezone(const time_t &clock);
    static string currentIsoDateTimeSuitableForFilename();
    static string getEnvString(const string &environVar);
    static vector<string> tokenize(const string & source, const string &
				   delimiters);
    static vector<string> splitByDelimiter(const string & source,
                                           char delimiter);
    static vector<string> loadFileIntoStringVector(const string &filename);
    static string readFileIntoString(const string &filename);
    static void strMaxCpy(char *dest, const char *src, size_t destSize);
    static void mailMsg(const string &subject, const string &toList, 
			const string &body);
    static string insertSlashBeforeSubString(const string & input, 
					  const string & subString);
    static double round(double value, int numPlaces);
    static double dbToLinearRatio(double db);
    static double linearRatioToDb(double linear);

 private:
    SseUtil();
    ~SseUtil();

};

#endif // SseUtil_H