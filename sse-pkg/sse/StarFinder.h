/*******************************************************************************

 File:    StarFinder.h
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

// StarFinder.h     Star info alias spacecraft
//			Text User Interface to retrieve
//			star right ascension and declination
//			plus rise, set, and observed frequencies 

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#include <mysql.h>
#include <stdarg.h>
#include "Target.h"
#include "AntennaLimits.h"
#include "doppler.h"
#include "SseUtil.h"
#include "SseSystem.h"
using std::string;
using std::vector;
using std::ofstream;

#define STRING_SIZE 80
#define SHORT_STRING 20

enum QueryNumber {    
		      DB_HOST,
		      DB_NAME,
		      SITE,
		      OBS_DATE,
		      OBS_LENGTH,
		      OBS_PADDING,
		      DUT,
		      STAR_NUMBER,
		      RA,
		      DEC,
		      LONGITUDE,
		      LATITUDE,
		      LMST,
		      LY,
		      STAR_TYPE,
		      RISE_TIME,
		      TRANSIT_TIME,
		      SET_TIME,
		      STAR_TYPE_MERIT,
		      STAR_OBS_MERIT,
		      STAR_DIST_MERIT,
		      STAR_OVERALL_MERIT,
		      TIME_LEFT_MERIT,
		      BEFORE_SET,
		      STAR_NAME,
		      LAST_QUERY
		      };

char * QueryDescription[] = { 
		      "Database Host",
		      "Database Name",
		      "Site",
		      "Obs Date (YYYY MM DD HH MM SS)",
		      "Observation Length",
		      "Observation Padding",
		      "UT1 - UTC",
		      "SETI Star Number",
		      "Right Ascension, J2000",
		      "Declination, J2000",
		      "Longitude (Degrees, West)",
		      "Latitude (Degrees)",
		      "LMST (Local Mean Sidereal Time)",
		      "Distance in Light Years",
		      "Star Type",
		      "Rise Time (UTC)",
		      "Transit Time (UTC)",
		      "Set Time (UTC)",
		      "Star Type Merit",
		      "Star Obs Merit",
		      "Star Dist Merit",
		      "Star Overall Merit",
		      "Time Left Merit",
		      "Before Set",
		      "Star Name",
			"Last Query", "         " };
char * QueryNames[] = { 
		      "Host       ",
		      "Name       ",
		      "Site       ",
		      "Date       ",
		      "ObsLength  ",
		      "ObsPadding ",
		      "DUT        ",
		      "Star       ",
		      "   Ra      ",
		      "   Dec     ",
		      "   Long    ",
		      "   Lat     ",
		      "   Lmst    ",
		      "   LY      ",
		      "   Type    ",
		      "   Rise    ",
		      "   Transit ",
		      "   Set     ",
		      "   Merit-1 ",
		      "   Merit-2 ",
		      "   Merit-3 ",
		      "   Merit-4 ",
		      "   Merit-5 ",
		      "   Before Set",
		      "   Name    ",
			"Last Query", "         " };
class Parser
 {
 public:
    static string tmtostr(struct tm tmDate);
    static string tmtotimestr(struct tm tmDate);
    static struct tm strtotm(string strDate);
    static string radtohrsstr(double radian);
    static string radtodegstr(double radian);
    static void printLine(int numFields, vector<int> fieldLengths);
    static vector<string> Tokenize(const string & source, 
   				 const string & delimeters);
 private:
    Parser();
    ~Parser();
 };

class StarFinder
{
private:
  MYSQL *db;
  enum QueryNumber queryNumber_;
  long setiStarnum_;
  struct tm riseTime_;
  struct tm setTime_;
  struct tm transitTime_;
  char site_[STRING_SIZE];
  struct tm date_;
  char dbHost_[STRING_SIZE];
  char dbName_[STRING_SIZE];
  ofstream oFile;
  char filename_[STRING_SIZE];
  Target *starInfo_;
  target_data_type targetInfo_;
  AntennaLimits *antennaLimits_;
  double dut1_;
  double ra_;
  double dec_;
  double ly_;
  double longitude_;
  double latitude_;
  Radian lmst_;
  RaDec rd_;
  char which_list_[3];
  double starTypeMerit_;
  double starObsMerit_;
  double starDistMerit_;
  double starOverallMerit_;
  double timeLeftMerit_;
  double obsLength_;
  double beforeSet_;
  double obsPadding_;
  char starname_[16];
  double minimumBandwidth_;

  void setDbHost(const char * databaseHost);
  void setDbName(const char * databaseName);
  void printFields(ofstream &oFile);
  void execQuery(const char* query, ofstream &oFile);
  long runQuery(time_t obsTime);
  int getParmIndex(const char *);
  int getAntennaLimits( MYSQL *db, const string mainAntenna);
  long queryStar( MYSQL *db, long starnum, time_t obsTime);
  string getVersion(MYSQL *db);

public:
  StarFinder();
  void findStar();
  void print();
  void readParms();
};