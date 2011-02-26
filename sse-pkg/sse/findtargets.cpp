/*******************************************************************************

 File:    findtargets.cpp
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
  Find position and other information for spacecraft 
  or calibration targets.
 */

#include "SseAstro.h"
#include "SseUtil.h"
#include "MysqlQuery.h"
#include "Angle.h"
#include "CmdLineParser.h"
#include "SseSystem.h"
#include "Spacecraft.h"
#include "CalTargets.h"
#include "AtaInformation.h"
#include "Assert.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <time.h>

const double KmPerAu(149597870.691);
const double CeeKmSec(299792.458);

const int MaxIdLen(5);
const int MaxNameLen(15);
const double MinFreqMhz(500);
const double MaxFreqMhz(11200);

using namespace std;

MYSQL* connectToDatabase(const string &dbHost, 
			 const string &dbName,
                         const string &dbUser);

struct SpacecraftInfo 
{
   int targetId;
   string name;
   double xmit1FreqMhz;
   double xmit2FreqMhz;
   string ephemFilename;

};

#if 0
struct TargetInfo 
{
   int targetId;
   string name;
   double ra2000Hours;
   double dec2000Deg;
   double fluxJy;
};
#endif

void getSpacecraftInfo(MYSQL *db, time_t obsTime, 
                       vector<TargetInfo> & targetInfoVect);
void readSpacecraftInfo(MYSQL *db, vector<SpacecraftInfo> & craft);
void printHoursInHm(double decimalHours);
void printHoursField(double decimalHours);
double addTzOffset(double timeHoursUtc, double tzOffsetHours);
double dopplerShift(double freqMhz, double delDotKmSec);
void printCalTargetFluxes(const vector<TargetInfo> & targetInfoVect);

int main(int argc, char * argv[])
{
   string defaultObsTimeString = "current time";
   string defaultDbName = "nss_iftest";
   string defaultDbHost = "localhost";
   string defaultDbUser = "";
   int defaultTzOffset = 0;  // UTC
   double defaultCalFreqMhz = 1420;
   double defaultCalMinHours = 0.5;

   const string spacecraftTargetType("spacecraft");
   const string calPhaseTargetType("calphase");
   const string calDelayTargetType("caldelay");

   string defaultTargetType = spacecraftTargetType;

   string dbNameArg("-dbname");
   string dbHostArg("-dbhost");
   string dbUserArg("-dbuser");
   string obsTimeArg("-timeutc");
   string timeZoneArg("-tzoffset");
   string targetTypeArg("-type");
   string calFreqMhzArg("-calfreqmhz");
   string calMinHoursArg("-calhours");

   CmdLineParser parser;
   parser.addStringOption(
      dbHostArg, defaultDbHost, "database host");
   parser.addStringOption(
      dbNameArg, defaultDbName, "database name");
   parser.addStringOption(
      dbUserArg, defaultDbUser, "database user");
   parser.addStringOption(
      obsTimeArg, defaultObsTimeString, "obs time as 'yyyy-mm-dd hh:mm:ss'");
   parser.addDoubleOption(
      timeZoneArg, defaultTzOffset, "rise/set timezone offset in hours from UTC");
   parser.addStringOption(
      targetTypeArg, defaultTargetType, "target type: "
      + spacecraftTargetType + " | " + calPhaseTargetType + " | "
      + calDelayTargetType);
   parser.addDoubleOption(
      calFreqMhzArg, defaultCalFreqMhz, "cal freq for finding strongest source");
   parser.addDoubleOption(
      calMinHoursArg, defaultCalMinHours, "minimum cal uptime in hours");

   if (! parser.parse(argc, argv))
   {
      cerr << parser.getErrorText() << endl;
      cerr << parser.getUsage();
      exit(1);
   }

   // default to current time
   time_t obsTime;
   time(&obsTime);

   //Convert time string to time_t in UTC
   string obsTimeString = parser.getStringOption(obsTimeArg);
   if (obsTimeString != defaultObsTimeString)
   {
      // Iso 9001 format (mostly)
      string format("%Y-%m-%d %H:%M:%S");
      const char *timeZone = "TZ=UTC";
      putenv(const_cast<char *>(timeZone));
      struct tm timestruct;
  
      char *timePtr = strptime(obsTimeString.c_str(), 
                               format.c_str(), &timestruct);
      if (! timePtr)
      {
         cerr << "Error: invalid utc time format: '"
              << obsTimeString << "'" << endl;
         cerr << parser.getUsage();
         exit(1);
      }
      obsTime = mktime(&timestruct);
   }

   cout << "Obs time: " << SseUtil::isoDateTime(obsTime) << endl;

   try {

      string dbHost(parser.getStringOption(dbHostArg));
      string dbName(parser.getStringOption(dbNameArg));
      string dbUser(parser.getStringOption(dbUserArg));

      MYSQL* db = connectToDatabase(dbHost, dbName, dbUser);

      string targetType(parser.getStringOption(targetTypeArg));
      if (targetType != spacecraftTargetType &&
          targetType != calPhaseTargetType &&
          targetType != calDelayTargetType)
      {
         cerr << "Invalid target type: " << targetType << endl;
         cerr << parser.getUsage();
         exit(1);
      }

      cout.precision(6);
      cout.setf(std::ios::fixed);

      int tzOffsetHours(static_cast<int>(parser.getDoubleOption(timeZoneArg)));
      cout << "Rise/Set Timezone: UT";
      if (tzOffsetHours != 0)
      {
         if (tzOffsetHours > 0)
         {
            cout << "+";
         }
         cout << tzOffsetHours;
      }
      cout << endl;

      vector<TargetInfo> targetInfoVect;
      double calFreqMhz(parser.getDoubleOption(calFreqMhzArg));
      if (calFreqMhz < MinFreqMhz || calFreqMhz > MaxFreqMhz)
      {
         cerr << calFreqMhzArg << " is out of range, must be between "
              << MinFreqMhz << " and " << MaxFreqMhz << endl;
         exit(1);
      }

      const double minUpTimeForCalHours(parser.getDoubleOption(
                                           calMinHoursArg));
      if (minUpTimeForCalHours <= 0.0)
      {
         cerr << "Option " << calMinHoursArg << " must be > zero." << endl;
         exit(1);
      }

      CalTargets calTargets;
      if (targetType == spacecraftTargetType)
      {
         getSpacecraftInfo(db, obsTime, targetInfoVect);
         cout << endl;
      }
      else
      {
         calTargets.loadTargetsFromDb(db, targetType);
         calTargets.computeFluxAtFreq(calFreqMhz);
         targetInfoVect = calTargets.getTargetInfo();

         printCalTargetFluxes(targetInfoVect);

         cout << endl;
      }

      /*
        Find the cal target with the max flux at the
        cal freq that's up at least the min amount of time.
      */
      double maxFluxAtCalFreq(-1);
      int targetIndexWithMaxFlux(-1);

      cout << setw(MaxIdLen) << "Id" 
           << setw(MaxNameLen) << "Name"
           << "\tRaHours"
           << "\t\tDecDeg" 
           << "\t\tRise" 
           << "\tSet"
           << "\tVisib"
           << "\tToRise"
           << "\tHmLeft"
           << "\tTotUp"
           << endl;

      // adjust ata horizon by the atmospheric refraction
      double atmosRefractDeg(SseAstro::atmosRefractDeg(AtaInformation::AtaHorizonDeg));
      double adjHorizonDeg(AtaInformation::AtaHorizonDeg - atmosRefractDeg);

      for (unsigned int i=0; i<targetInfoVect.size(); ++i)
      {
         try {

            double riseHoursUtc, transitHoursUtc, setHoursUtc;
            double untilRiseHours, untilSetHours;

            SseAstro::riseTransitSet(targetInfoVect[i].ra2000Hours,
                                     targetInfoVect[i].dec2000Deg,
                                     AtaInformation::AtaLongWestDeg,
                                     AtaInformation::AtaLatNorthDeg,
                                     adjHorizonDeg,
                                     obsTime,
                                     &riseHoursUtc,
                                     &transitHoursUtc,
                                     &setHoursUtc,
                                     &untilRiseHours,
                                     &untilSetHours);
            

            cout << setw(MaxIdLen) << targetInfoVect[i].targetId 
                 << setw(MaxNameLen) << targetInfoVect[i].name;
            
            cout << "\t" << targetInfoVect[i].ra2000Hours 
                 << "\t" << targetInfoVect[i].dec2000Deg;

            cout << "\t";
            
            double riseHoursTzOffset = addTzOffset(riseHoursUtc, tzOffsetHours);
            printHoursInHm(riseHoursTzOffset);

            cout << "\t";
            double setHoursTzOffset = addTzOffset(setHoursUtc, tzOffsetHours);
            printHoursInHm(setHoursTzOffset);

            // total hours up
            double totalHoursUp = 2 * 
               SseAstro::hourAngle(targetInfoVect[i].dec2000Deg,
                                   AtaInformation::AtaLatNorthDeg,
                                   adjHorizonDeg);
            cout << "\t";
            if (untilSetHours > 0)
            {
               cout << "Up";
            }
            else
            {
               cout << "--";
            }

            cout << "\t";
            printHoursField(untilRiseHours);

            cout << "\t";
            printHoursField(untilSetHours);

            cout << "\t";
            printHoursField(totalHoursUp);

            cout << endl;

            // Find best cal target that's up
            if (untilSetHours > minUpTimeForCalHours)
            {
               if (targetInfoVect[i].fluxJy > maxFluxAtCalFreq)
               {
                  maxFluxAtCalFreq = targetInfoVect[i].fluxJy;
                  targetIndexWithMaxFlux = i;
               }
            }
         }
         catch(SseException &except)
         {
            cerr << except << endl;
         }

      }
      
      Assert(targetIndexWithMaxFlux < static_cast<signed>(targetInfoVect.size()));
      if (targetType != spacecraftTargetType)
      {
         if (targetIndexWithMaxFlux >= 0)
         {
            cout.precision(0);
            
            cout << endl << "  Strongest cal target at " << calFreqMhz 
                 << " MHz: Id# "
                 << targetInfoVect[targetIndexWithMaxFlux].targetId
                 << " (" 
                 <<  targetInfoVect[targetIndexWithMaxFlux].name << ")"
                 << endl;
         }  
         else
         {
            cout << endl 
                 << "  No 'strongest cal target', none match specified freq & uptime." << endl;
         }
      }
   }
   catch(SseException &except)
   {
      cerr << except << endl;
   }

}

void getSpacecraftInfo(MYSQL *db, time_t obsTime, 
                       vector<TargetInfo> & targetInfoVect)
{
   string ephemDir(SseSystem::getSetupDir());
   //cout << "ephemDir: " << ephemDir << endl;

   string earthEphemFilename("earth.xyz");

   vector<SpacecraftInfo> craft;
   readSpacecraftInfo(db, craft);
   
   cout << setw(MaxIdLen) << "Id" 
        << setw(MaxNameLen) << "Name"
        << "\tDistAu"
        << "\tDeldot" 
        << "\tXmit1MHz" 
        << "\tXmit2MHz" 
        << "\tDopplerX1" 
        << "\tDopplerX2" 
        << endl;

   for (unsigned int i=0; i<craft.size(); ++i)
   {
      double ra2000Rads, dec2000Rads;
      double rangeKm, rangeRateKmSec;

      try {

         Spacecraft::calculatePosition(
            ephemDir, craft[i].ephemFilename,
            earthEphemFilename, obsTime,
            &ra2000Rads, &dec2000Rads,
            &rangeKm, &rangeRateKmSec
            );
            
         double ra2000Hours(SseAstro::radiansToHours(ra2000Rads));
         double dec2000Deg(SseAstro::radiansToDegrees(dec2000Rads));

         TargetInfo targetInfo;
         targetInfo.targetId = craft[i].targetId;
         targetInfo.name = craft[i].name;
         targetInfo.ra2000Hours = ra2000Hours;
         targetInfo.dec2000Deg = dec2000Deg;
         targetInfo.fluxJy = 0;
#if 1
         stringstream strm;
         
         // distAu & deldot are only good to about 1 decimal
         strm.precision(1);           // show N places after the decimal 
         strm.setf(std::ios::fixed);  // show all decimal places up to precision

         double distAu(rangeKm / KmPerAu);

         strm
            << setw(MaxIdLen) << targetInfo.targetId
            << setw(MaxNameLen) << targetInfo.name 
            << "\t" << setw(5) << distAu
            << "\t" << setw(6) << rangeRateKmSec;

         // xmit freqs should be good to khz
         strm.precision(3);
         int maxFreqWidth(8);

         strm
            << "\t" << setw(maxFreqWidth) << craft[i].xmit1FreqMhz
            << "\t" << setw(maxFreqWidth) << craft[i].xmit2FreqMhz
            << "\t" << setw(maxFreqWidth) 
            << dopplerShift(craft[i].xmit1FreqMhz, rangeRateKmSec)
            << "\t" << setw(maxFreqWidth) 
            << dopplerShift(craft[i].xmit2FreqMhz, rangeRateKmSec)
            << endl;
         
         cout << strm.str();

#endif

         targetInfoVect.push_back(targetInfo);
      }
      catch(SseException &except)
      {
         cerr << except << endl;
      }

   }
}

void readSpacecraftInfo(MYSQL *db, vector<SpacecraftInfo> & craft)
{
   stringstream targetCatQuery;
   
   targetCatQuery << "select targetId, name, "
                  << "xmit1FreqMhz, xmit2FreqMhz, "
                  << "ephemFilename "
                  << "from Spacecraft WHERE ";
   targetCatQuery << " ephemFilename != ''";

   enum resultCols { targetIdCol, nameCol, xmit1FreqCol, xmit2FreqCol,
                     ephemCol, numCols };

   MysqlQuery query(db);
   query.execute(targetCatQuery.str(), numCols, __FILE__, __LINE__);

   while (MYSQL_ROW row = mysql_fetch_row(query.getResultSet()))
   {
      int targetId(query.getInt(row, targetIdCol, 
                                __FILE__, __LINE__));
         
      string name(query.getString(row, nameCol,
                                  __FILE__, __LINE__));

      double xmit1FreqMhz(query.getDouble(row, xmit1FreqCol, 
                                          __FILE__, __LINE__));
      
      double xmit2FreqMhz(query.getDouble(row, xmit2FreqCol, 
                                          __FILE__, __LINE__));
      
      string ephemFilename(query.getString(row, ephemCol,
                                           __FILE__, __LINE__));

      SpacecraftInfo craftInfo;
      craftInfo.targetId = targetId;
      craftInfo.name = name;
      craftInfo.xmit1FreqMhz = xmit1FreqMhz;
      craftInfo.xmit2FreqMhz = xmit2FreqMhz;
      craftInfo.ephemFilename = ephemFilename;

      craft.push_back(craftInfo);
   }
   
}




MYSQL* connectToDatabase(const string &databaseHost, 
			 const string &databaseName,
			 const string &databaseUser)
{
   MYSQL * db = mysql_init(NULL);
   if (!db)
   {
      throw SseException(
	 "::connect() mysql_init() failed. Cannot allocate new handler.",
	 __FILE__,  __LINE__, SSE_MSG_DBERR, SEVERITY_ERROR);
   }
   
   if (!mysql_real_connect(
      db, 
      databaseHost.c_str(), 
      databaseUser.c_str(), 
      "",  // password 
      databaseName.c_str(),
      0,     // port number
      NULL,  // socket name
      0))    // flags
   {
      stringstream strm;
      strm << "::connect() MySQL error: " << mysql_error(db) << endl;
      throw SseException(strm.str(), __FILE__,  __LINE__, 
                         SSE_MSG_DBERR, SEVERITY_ERROR);
   }
   
   return db;
   
}

void printHoursInHm(double decimalHours)
{
   int hours, mins;
   double secs;
   SseAstro::decimalHoursToHms(decimalHours,
                               &hours, &mins, &secs);

   stringstream strm;
   int width(2);
   strm.fill('0');
   strm << setw(width) << hours << ":" << setw(width) <<  mins;

   cout << strm.str();
}

void printHoursField(double hours)
{
   if (hours > 0)
   {
      printHoursInHm(hours);
   }
   else
   {
      cout << "  -  ";
   }
}

double addTzOffset(double timeHoursUtc, double tzOffsetHours)
{
   const double HoursPerDay(24);
   double offsetHours = timeHoursUtc + tzOffsetHours;
   if (offsetHours >= HoursPerDay)
   {
      offsetHours -= HoursPerDay;
   }
   else if (offsetHours < 0)
   {
      offsetHours += HoursPerDay;
   }

   return offsetHours;
}

/*
  Apply deldot to given freq to determine doppler shifted
  value.
 */
double dopplerShift(double freqMhz, double delDotKmSec)
{
   // use negative of deltadot to make freq shift go in the
   // correct direction
   double dopplerShiftMhz((-delDotKmSec / CeeKmSec) * freqMhz);
   
   double shiftedFreqMhz(freqMhz + dopplerShiftMhz);

   return shiftedFreqMhz;
}

void printCalTargetFluxes(const vector<TargetInfo> & targetInfoVect)
{
   stringstream outStrm;
   outStrm.precision(0);
   outStrm.setf(std::ios::fixed);
        
   outStrm << setw(MaxIdLen) << "Id" 
           << setw(MaxNameLen) << "Name"
           << "\t" << "FreqMhz: FluxJy"
           << endl;

   for (unsigned int target=0; target < targetInfoVect.size(); ++target)
   {
      const TargetInfo & targetInfo(targetInfoVect[target]);
      const vector<Flux> & fluxVect(targetInfo.fluxVect);

      outStrm << setw(MaxIdLen) << targetInfo.targetId 
              << setw(MaxNameLen) << targetInfo.name;
      
      for (unsigned int fluxIndex=0; fluxIndex < fluxVect.size(); ++fluxIndex)
      {
         const int fluxWidth(4);
         outStrm << "\t" << fluxVect[fluxIndex].freqMhz_ << ": " 
                 << setw(fluxWidth) << fluxVect[fluxIndex].fluxJy_;
      }
      outStrm << endl;
   }

   cout << outStrm.str();
}