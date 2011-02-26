/*******************************************************************************

 File:    StarFinder.cpp
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



// StarFinder.cpp       Star Info Parameters and Queries for seeker.schema databases

#include "StarFinder.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <vector>
#include <iosfwd>

#include <stdarg.h>
#include <ctime>
#include <algorithm>
#include <iostream>
#include "astro.h"
#include "Angle.h"
#include "doppler.h"
#include "precession.h"
#include "SseUtil.h"
#include "MysqlResultSet.h"

using std::string;
using std::vector;
using namespace std;

char *program_name;

const double MaxDistLightYears(225);
#define PCTOLY		3.2615633

string Parser::tmtotimestr(struct tm tmDate)
  {
  stringstream datestring;

  datestring.setf(ios::right);
  datestring << setw(2) << setfill('0')   << tmDate.tm_hour << ':' 
	     << setw(2) << setfill('0')   << tmDate.tm_min << ':'
	     << setw(2) << setfill('0')   << tmDate.tm_sec;
  return( datestring.str());
  }

string Parser::tmtostr(struct tm tmDate)
  {
  stringstream datestring;

  datestring.setf(ios::right);
  datestring << tmDate.tm_year + 1900 << "/"
	     << setw(2) << setfill('0')   << tmDate.tm_mon + 1 << "/"
	     << setw(2) << setfill('0')   << tmDate.tm_mday << " ";
  return( datestring.str());
  }

struct tm Parser::strtotm( string strDate )
  {
  struct tm tmDate;

  vector <string> tokens = Parser::Tokenize(strDate, " /\t\n-:" );

	    tmDate.tm_mon = tmDate.tm_mday = 0;
	    tmDate.tm_hour = tmDate.tm_min = tmDate.tm_sec = 0;
            tmDate.tm_year  =  atoi(tokens[0].c_str())-1900;
	    if ( tokens.size() > 1)
	    {
              tmDate.tm_mon =  atoi(tokens[1].c_str()) - 1;
	      if ( tokens.size() > 2)
	      {
               tmDate.tm_mday =  atoi(tokens[2].c_str());
	       if ( tokens.size() > 3)
	       {
                tmDate.tm_hour =  atoi(tokens[3].c_str());
	        if ( tokens.size() > 4)
		{
                 tmDate.tm_min =  atoi(tokens[4].c_str());
	         if ( tokens.size() > 5)
                    tmDate.tm_sec =  atoi(tokens[5].c_str());
		}
	       }
	      }
	    }
   return tmDate;
  }
string Parser::radtohrsstr(double radian_)
  {
  stringstream datestring;
  int raHours;
  int raMin;
  double raSec;
  rad_to_hms(radian_, &raHours, &raMin, &raSec);

  datestring.setf(ios::right);
  datestring << setw(2) << setfill('0')   << raHours << ':' 
	     << setw(2) << setfill('0')   << raMin << ':'
	     << setprecision(3) << setfill('0')   << raSec;
  return( datestring.str());
  }
string Parser::radtodegstr(double radian_)
  {
  stringstream datestring;
  int decDeg;
  int decMin;
  double decSec;
  rad_to_dms(radian_, &decDeg, &decMin, &decSec);
  datestring << setw(2) << setfill('0')   <<decDeg << ':' 
	     << setw(2) << setfill('0')   << decMin << ':'
	     << setprecision(3) << setfill('0')   << decSec;
  return( datestring.str());
  }

void Parser::printLine( int numFields, vector<int> fieldLengths)
{
       for ( int q = 0; q < numFields; q++) 
	  {
	  cout << "+";
	  for ( int p = 0; p <= fieldLengths[q]; p++) cout << "-";
	  }
       cout << "+" << endl;
}

vector<string> Parser::Tokenize(const string & source, const string &
			delimeters)
{
  vector<string> tokens;

  string::size_type start = 0;
  string::size_type end = 0;

  while ((start = source.find_first_not_of(delimeters, start)) !=
	 string::npos) {

    end = source.find_first_of(delimeters, start);
    if (end == string::npos)  // don't go off the end
    {
	end = source.size();  // get the last token
    }
    tokens.push_back(source.substr(start, end - start));

    start = end;

  }

  return tokens;
}

string StarFinder::getVersion(MYSQL *db) {
    stringstream mysqlversion;
    char versionDate[12];

    mysqlversion << "select ts from seeker_db_version;";
    if (0 != mysql_query(db, mysqlversion.str().c_str())) {
      cout << "mysql error " << mysql_error(db) << endl;
      return NULL;
    }
    MYSQL_RES* res_set = mysql_store_result(db);
    if (!res_set) {
       return NULL;
    }
    MYSQL_ROW row = mysql_fetch_row(res_set);
    strncpy(versionDate, row[0], 10);
    versionDate[4] = versionDate[5];
    versionDate[5] = versionDate[6];
    versionDate[6] = versionDate[8];
    versionDate[7] = versionDate[9];
    versionDate[8] = '\0';

    cout << "version date " << versionDate << endl;
    return(string(versionDate));
}


int StarFinder::getParmIndex( const char * pName)
  {
     int parameterIndex;
     if (strncasecmp( pName, "host", 3) == 0)parameterIndex = DB_HOST;
     else if (strncasecmp( pName, "name", 3) == 0)parameterIndex = DB_NAME;
     else if (strncasecmp( pName, "star", 3) == 0)parameterIndex = STAR_NUMBER;
     else if (strncasecmp( pName, "dut", 3) == 0)parameterIndex = DUT;
     else if (strncasecmp( pName, "ra", 2) == 0)parameterIndex = RA;
     else if (strncasecmp( pName, "dec", 3) == 0)parameterIndex = DEC;
     else if (strncasecmp( pName, "date", 3) == 0)parameterIndex = OBS_DATE;
     else if (strncasecmp( pName, "obslength", 4) == 0)parameterIndex = OBS_LENGTH;
     else if (strncasecmp( pName, "obspadding", 4) == 0)parameterIndex = OBS_PADDING;
     else if (strncasecmp( pName, "site", 3) == 0)parameterIndex = SITE;
     else {
       istringstream iss(pName);
       iss >> parameterIndex;
       if (!iss) {
	  cout << "Invalid Command: " << pName << endl;
	  parameterIndex = -1;
       }
     }
     return(parameterIndex);
  }

StarFinder::StarFinder()
  {
    time_t clock;
    stringstream filename;

   time(&clock);
   antennaLimits_ = 0;
   date_ = *gmtime(&clock);
   setiStarnum_ = 1000;
    obsLength_ = 195.00;
    obsPadding_ = 12.0;
    minimumBandwidth_ = 6.0;
   starInfo_ = NULL;
   dut1_ = 0.0;
   strcpy( dbHost_, "sol");
   strcpy( dbName_, "nss_star_ao2004S");
   strcpy( site_, "Arecibo");


   filename << "sf-" << date_.tm_year + 1900 << "-" << 
		    setw(2) << setfill('0') << 
		    date_.tm_mon + 1 << "-" <<
		    setw(2) << setfill('0') << 
		    date_.tm_mday ;
   strcpy(filename_, filename.str().c_str());
   cout << "Output File is " << filename_ << endl;

   ofstream oFile(filename_, ios::app);

    if(!oFile) {cerr<<"can't open output file: "<<filename_<<endl;
	exit(EXIT_FAILURE);
	}
  }



void StarFinder::print()
  {

  cout << "-------------------------------------------------------------" << endl;
  cout << "Parameter\tName\tValue\t\t\tDescription" << endl;
  cout << "-------------------------------------------------------------" << endl;
      {
      cout << DB_HOST << "\t\t" << QueryNames[DB_HOST] << dbHost_ 
		<< "\t\t\t" << QueryDescription[DB_HOST] << endl;
      cout << DB_NAME << "\t\t" << QueryNames[DB_NAME] << dbName_ 
		<< "\t" << QueryDescription[DB_NAME] << endl;
      cout << SITE << "\t\t" << QueryNames[SITE] << site_ << "\t\t" << QueryDescription[SITE] << endl;
      cout << "\t\t" << QueryNames[LONGITUDE] << Parser::radtodegstr(longitude_) << "\t\t" << QueryDescription[LONGITUDE] << endl;
      cout << "\t\t" << QueryNames[LATITUDE] << Parser::radtodegstr(latitude_) << "\t\t" << QueryDescription[LATITUDE] << endl;
      cout << OBS_DATE << "\t\t" << QueryNames[OBS_DATE] << Parser::tmtostr(date_) << 
		Parser::tmtotimestr(date_) << "\t" 
		<< QueryDescription[OBS_DATE] << endl;
      cout << "\t\t" << QueryNames[LMST] << Parser::radtohrsstr(lmst_) << "\t\t" << QueryDescription[LMST] << endl;
      cout << OBS_LENGTH << "\t\t" << QueryNames[OBS_LENGTH] << obsLength_ << "\t\t\t" << QueryDescription[OBS_LENGTH] << endl;
      cout << OBS_PADDING << "\t\t" << QueryNames[OBS_PADDING] << obsPadding_ << "\t\t\t" << QueryDescription[OBS_PADDING] << endl;
      cout << DUT << "\t\t" << QueryNames[DUT] << dut1_ << "\t\t\t" << QueryDescription[DUT] << endl;
      cout << STAR_NUMBER << "\t\t" << QueryNames[STAR_NUMBER] << setiStarnum_ << "\t\t\t" << QueryDescription[STAR_NUMBER] << endl;
      cout << RA << "\t\t" << QueryNames[RA] << Parser::radtohrsstr(ra_) << "\t\t" << QueryDescription[RA] << endl;
      cout << DEC << "\t\t" << QueryNames[DEC] << Parser::radtodegstr(dec_) << "\t\t" << QueryDescription[DEC] << endl;
      cout << "\t\t" << QueryNames[LY] << setprecision(3) << ly_ << "\t\t\t" << QueryDescription[LY] << endl;
      cout << "\t\t" << QueryNames[STAR_TYPE] << which_list_ << "\t\t\t" << QueryDescription[STAR_TYPE] << endl;
      cout << "\t\t" << QueryNames[STAR_NAME] << starname_ << "\t\t\t" << QueryDescription[STAR_NAME] << endl;
      cout << "\t\t" << QueryNames[RISE_TIME] << Parser::tmtotimestr(riseTime_) << "\t\t" << QueryDescription[RISE_TIME] << endl;
      cout << "\t\t" << QueryNames[TRANSIT_TIME] << Parser::tmtotimestr(transitTime_) << "\t\t" << QueryDescription[TRANSIT_TIME] << endl;
      cout << "\t\t" << QueryNames[SET_TIME] << Parser::tmtotimestr(setTime_) << "\t\t" << QueryDescription[SET_TIME] << endl;
      cout << "\t\t" << QueryNames[STAR_TYPE_MERIT] << starTypeMerit_ << "\t\t\t" << QueryDescription[STAR_TYPE_MERIT] << endl;
      cout << "\t\t" << QueryNames[STAR_OBS_MERIT] << starObsMerit_ << "\t\t\t" << QueryDescription[STAR_OBS_MERIT] << endl;
      cout << "\t\t" << QueryNames[STAR_DIST_MERIT] << starDistMerit_ << "\t\t\t" << QueryDescription[STAR_DIST_MERIT] << endl;
      cout << "\t\t" << QueryNames[STAR_OVERALL_MERIT] << setprecision(4)
	   << starOverallMerit_ << "\t\t\t" 
	   << QueryDescription[STAR_OVERALL_MERIT] << endl;
      cout << "\t\t" << QueryNames[TIME_LEFT_MERIT] << setprecision(3)
	   << timeLeftMerit_ << "\t\t\t" << QueryDescription[TIME_LEFT_MERIT] 
	   << endl;
      }
  }

void StarFinder::setDbHost(const char * databaseHost)
   {
    strncpy(dbHost_, databaseHost, STRING_SIZE);
     dbHost_[STRING_SIZE-1] = '\0';
   }

void StarFinder::setDbName(const char * databaseName)
   {
    strncpy(dbName_, databaseName, STRING_SIZE);
     dbName_[STRING_SIZE-1] = '\0';
   }


void StarFinder::findStar()
 {
         time_t obsTime = mktime(&date_);

      if (runQuery(obsTime) == 1)
      {
         lmst_ = antennaLimits_->lmst(obsTime);
	 lmst_.setPositive();
	 starTypeMerit_ = starInfo_->targetTypeMerit();
	 starObsMerit_ = starInfo_->targetObsMerit();
	 starDistMerit_ = starInfo_->targetDistMerit();
	 beforeSet_ = obsLength_/3600.*HTOR*obsPadding_;
	 starOverallMerit_ = starInfo_->overallMerit();
	 RaDec raDec = starInfo_->getPosition();
	 double rise, set;

	 AntennaLimits::Visibility visibility = antennaLimits_->riseSet(raDec, rise, set);
	 if (visibility == AntennaLimits::NEVER_UP) cout << "Star " << setiStarnum_ << " is never visible at " << site_ << endl;
	 double hourAngle = lmst_ - raDec.ra;
	 hourAngle = fmod(hourAngle + 2 * M_PI, 2 * M_PI);
			     // conversion from solar time to sidereal time
         const double STOUT = 0.9972;        

	 double timeLeft = starInfo_->getSet() - beforeSet_ - hourAngle;
	 timeLeftMerit_ = (timeLeft - 2 * obsLength_/60.0/60.0 * HTOR)/timeLeft;

	 if (timeLeftMerit_ < 0) timeLeftMerit_ = 0;

	 time_t rise_time = obsTime;
	 time_t set_time = rise_time;
	 

         rise_time +=
  	(int)((starInfo_->getRise() - hourAngle) * RTOH * 60 * 60 * STOUT);
         set_time +=
  	(int)((starInfo_->getSet() - hourAngle) * RTOH * 60 * 60 * STOUT);

	 time_t transit_time = obsTime;
	 transit_time -= (int)((hourAngle)*RTOH * 60 * 60 * STOUT);

	riseTime_ = *gmtime(&rise_time);
	setTime_ = *gmtime(&set_time);
	transitTime_ = *gmtime(&transit_time);

	//cout << "Coordinates Precessed to Epoch of Date " << endl;
	//cout << "RA   " << Parser::radtohrsstr(raDec.ra) << endl;
	//cout << "Dec  " << Parser::radtodegstr(raDec.dec)<< endl;


      }

 }

void StarFinder::printFields( ofstream &oFile)
   {
   }

long StarFinder::runQuery(time_t obsDate)
   {
   stringstream filename;
   db = mysql_init(0);
   cout << "Opening database " << dbName_ << " on host " << dbHost_ << endl;


   if (!mysql_real_connect(db, dbHost_, "", "", dbName_, 0, 0, 0)) {  
	 cerr << "connect:  MySQL error: " << mysql_error(db) << endl; 
	 return(0);
	 }
		  
         if (getAntennaLimits(db, site_)) {	// Read Antenna Limits

	if (setiStarnum_ == 0)   // Use ra and dec as set by user
	   {
             if (starInfo_ != NULL )delete starInfo_;
             Range reqRange(1745.0,3005.0);
	     double minRemainingTimeOnTargetRads(0);

	     double lmstRads = antennaLimits_->lmst(obsDate);
	     
             starInfo_ = new Target(RaDec(Radian(ra_), Radian(dec_)), 
			   RaDec(Radian(0.0), Radian(0.0)), 
			   0.0, setiStarnum_, "U ", reqRange, 
			   MaxDistLightYears,
			   lmstRads,
			   obsDate, *antennaLimits_,
			   obsLength_,
			   minimumBandwidth_,
			   minRemainingTimeOnTargetRads); 
	     ly_ = 0.0;
	     starname_[0] = '\0';
	     which_list_[0] = '\0';
             mysql_close(db);
             db = NULL;
             return(1);
	   }
        else if (queryStar( db, setiStarnum_, obsDate) == 1)
	   {
             mysql_close(db);
             db = NULL;
             return(1);
	   }
	 }

   mysql_close(db);
   db = NULL;
   return(0);
   }

void StarFinder::readParms()
 {
    std::string param;
    tm myDate;

    cout << "Enter Parameter Number|Name and Value or Command>> ";
    std::getline(std::cin, param);
    vector <string> tokens = Parser::Tokenize(param, " :/\t\n" );
    
    if (tokens.size() == 0 )return;

    if ((strcmp(tokens[0].c_str(), "exit") == 0) || 
	(strcmp(tokens[0].c_str(), "quit") == 0 )) exit(0);

    int parameterIndex = getParmIndex( tokens[0].c_str());

    if (tokens.size() >= 2)
    {

      switch( parameterIndex )
       {
	 case DB_HOST:
	    setDbHost( tokens[1].c_str());
	    break;

	 case DB_NAME:
	    setDbName( tokens[1].c_str());
	    break;

	 case DUT:
	    dut1_ = strtod(tokens[1].c_str(), NULL);
	    break;

	 case RA:
	    ra_ = HTOR*( strtod(tokens[1].c_str(), NULL) 
			 + strtod(tokens[2].c_str(), NULL)/60.0
			 + strtod(tokens[3].c_str(), NULL)/3600.0);
	    setiStarnum_ = 0;
	    break;

	 case DEC:
	    dec_ = DTOR*( strtod(tokens[1].c_str(), NULL) 
			 + strtod(tokens[2].c_str(), NULL)/60.0
			 + strtod(tokens[3].c_str(), NULL)/3600.0);
	    setiStarnum_ = 0;
	    break;

	 case STAR_NUMBER:
	    setiStarnum_ = atoi(tokens[1].c_str());
	    break;

	 case OBS_PADDING:
	    obsPadding_ = atoi(tokens[1].c_str());
	    break;

	 case OBS_LENGTH:
	    obsLength_ = atoi(tokens[1].c_str());
	    break;

	 case OBS_DATE:
	    myDate.tm_mon = myDate.tm_mday = 0;
	    myDate.tm_hour = myDate.tm_min = myDate.tm_sec = 0;
            myDate.tm_year   =  atoi(tokens[1].c_str())-1900;
	    if ( tokens.size() > 2)
	    {
              myDate.tm_mon =  atoi(tokens[2].c_str()) - 1;
	      if ( tokens.size() > 3)
	      {
               myDate.tm_mday =  atoi(tokens[3].c_str());
	       if ( tokens.size() > 4)
	       {
                myDate.tm_hour =  atoi(tokens[4].c_str());
	        if ( tokens.size() > 5)
		{
                 myDate.tm_min =  atoi(tokens[5].c_str());
	         if ( tokens.size() > 6)
                    myDate.tm_sec =  atoi(tokens[6].c_str());
		}
	       }
	      }
	    }
	       date_ = myDate;
	    break;

	 case SITE:
	    strcpy( site_, tokens[1].c_str());
	    break;

	 default:
	      cout << "Query Number " << parameterIndex << " out of range" 
		     << endl;
	    break;

       }
    } else {
      cout << "Too few arguments." << endl;
    }

 }

// sets up the AntennaLimits object

int StarFinder::getAntennaLimits( MYSQL *db, const string mainAntenna)
{
  if (antennaLimits_ != 0) {
     delete antennaLimits_;
     antennaLimits_ = NULL;
     }
  try {
    antennaLimits_ =
    new AntennaLimits(db, mainAntenna);

    longitude_ = antennaLimits_->getLongRads();
    latitude_ = antennaLimits_->getLatRads();
  }
  catch (...) {
    cout << "Site " << site_ << " not found in Antenna Table" << endl;
    if (antennaLimits_ != 0) {
	 delete antennaLimits_;
	 antennaLimits_ = NULL;
    }

     return(0);
   }

  return(1);
}



long StarFinder::queryStar(MYSQL *db, long starnum, time_t obsDate)
{
  stringstream sqlstmt;
  char ts[24];

  strncpy( ts, getVersion(db).c_str(), 9);
  if ( strncmp( ts, "20041101", 8 ) < 0 ){
  sqlstmt << "select starid, ra2000, dec2000, mura, mudec, parallax, "
	  << " starname, whichlist, remarks from starcat WHERE"
          << " starid = " << starnum << ";";
   }
   else {
   sqlstmt << "select targetId, ra2000Hours, dec2000Deg, mura, mudec, parallax,"
         << " targetName, whichlist, ephemFilename from TargetCat WHERE"
           << " targetId = " << starnum << ";";
   }

  //cout << ts << sqlstmt.str() << endl;

  if (mysql_query(db, sqlstmt.str().c_str()) != 0) {

     stringstream errorMsg;
     errorMsg << "StarFinder::queryStar: mysql_query() failed: ";
     errorMsg << mysql_error(db) << endl;
     
     cout << errorMsg.str();
     
     cout << " Star " << starnum << " is not a valid SETI star." << endl;
     return(0);
  }
  MysqlResultSet resultSet(mysql_store_result(db));
  if (resultSet.get() == 0) {
     cout << "Error retrieving Star "  << starnum << endl;
     return(0);
  }
  int numberOfRows = mysql_num_rows(resultSet.get());
  if (numberOfRows != 0 )
  {
  MYSQL_ROW row = mysql_fetch_row(resultSet.get());
  double ra_2000;
  double dec_2000;

  if ( strncmp( ts, "20040404", 8 ) < 0 ){
  ra_2000 = SseUtil::strToDouble(row[1]);
  dec_2000 = SseUtil::strToDouble(row[2]);
  }
  else {
  ra_2000 = SseUtil::hoursToRadians(SseUtil::strToDouble(row[1]));
  dec_2000 = SseUtil::degreesToRadians(SseUtil::strToDouble(row[2]));
  }
  ra_ = ra_2000;
  dec_ = dec_2000;
  double mu_ra = SseUtil::strToDouble(row[3]);
  double mu_dec = SseUtil::strToDouble(row[4]);
  double parallax = SseUtil::strToDouble(row[5]);
  ly_ = PCTOLY/parallax;
  strncpy(starname_, row[6], 16);
  starname_[15] = 0;
  strncpy(which_list_, row[7], 2);
  which_list_[2] = 0;
  if (strcmp( which_list_, "SC" ) == 0)
    {
     // Get the target ephemeris file.
     string targetEphemFile(SseSystem::getSetupDir() + row[8] );

     // targetInfo is for Spacecraft computation
     targetInfo_.type     = SPACECRAFT;
     targetInfo_.origin   = GEOCENTRIC;
     SseUtil::strMaxCpy(targetInfo_.targetfile, targetEphemFile.c_str(),
		DOPPLER_MAX_EPHEM_FILENAME_LEN); 
     // Get the earth ephemeris file.
     string earthEphemFile(SseSystem::getSetupDir() +
		 "earth.xyz");
     // from the doppler library:
     time_t obs_date_time = mktime(&date_);

     if (calculateSpacecraftPosition(&targetInfo_,
				     obs_date_time, 
				     dut1_, 
				     earthEphemFile.c_str()) != 0)
       {
	 cout << "Failed to calculateSpacecraftPosition" << endl;
	 return(0);
       }
     ra_ = ra_2000 = targetInfo_.ra2000;
     dec_ =dec_2000 = targetInfo_.dec2000;
     // convert from km to light years
     ly_ = targetInfo_.range/9.46*1.0e-12;
     parallax = targetInfo_.parallax;
    }
    if (starInfo_ != NULL )delete starInfo_;
    Range reqRange(1745.0,3005.0);
    double minRemainingTimeOnTargetRads(0);
    double lmstRads = antennaLimits_->lmst(obsDate);

    starInfo_ = new Target(RaDec(Radian(ra_2000), Radian(dec_2000)), 
			   RaDec(Radian(mu_ra), Radian(mu_dec)), 
			   parallax, starnum, which_list_, reqRange, 
			   MaxDistLightYears,
			   lmstRads,
			   obsDate, *antennaLimits_,
			   obsLength_,
			   minimumBandwidth_,
			   minRemainingTimeOnTargetRads); 
  //starInfo_->getObservations(db);
  return(1);
  }
  else
   {
   cout << "Star " << setiStarnum_ << " not in database." << endl;
   return(0);
   }
}

int main( int argc, char **argv)
{
   program_name = argv[0];
   StarFinder myTargets;
   int alive = 1;

   putenv("TZ=UTC");
   while (alive)
   {
   myTargets.findStar();
   myTargets.print();
   myTargets.readParms();
   }
}
