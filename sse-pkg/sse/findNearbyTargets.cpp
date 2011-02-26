/*******************************************************************************

 File:    findNearbyTargets.cpp
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
  Find targets in the database that are closer to the 
  specified RA/Dec position than the given angular separation.
 */

#include "SseAstro.h"
#include "SseUtil.h"
#include "MysqlQuery.h"
#include "CmdLineParser.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

MYSQL* connectToDatabase(const string &dbHost, 
			 const string &dbName);

int main(int argc, char * argv[])
{
   double defaultCenterRaHours = 0.0;
   double defaultCenterDecDeg = 0.0;
   double defaultAngSepDeg = 1;
   string defaultDbName = "tom_iftest";
   string defaultDbHost = "sol";

   string dbNameArg("-dbname");
   string dbHostArg("-dbhost");
   string raHoursArg("-rahours");
   string decDegArg("-decdeg");
   string angSepDegArg("-angsepdeg");

   CmdLineParser parser;
   parser.addStringOption(
      dbHostArg, defaultDbHost, "database host");
   parser.addStringOption(
      dbNameArg, defaultDbName, "database name");
   parser.addDoubleOption(
      raHoursArg, defaultCenterRaHours, "RA center (hours)");
   parser.addDoubleOption(
      decDegArg, defaultCenterDecDeg, "Dec center (deg)");
   parser.addDoubleOption(
      angSepDegArg, defaultAngSepDeg, "max angular separation from center (deg)");

   if (! parser.parse(argc, argv))
   {
      cerr << parser.getErrorText() << endl;
      cerr << parser.getUsage();
      exit(1);
   }

   double centerRaHours = parser.getDoubleOption(raHoursArg);
   double centerDecDeg = parser.getDoubleOption(decDegArg);
   double maxAngSepDeg = parser.getDoubleOption(angSepDegArg);

   double decUpperLimitDeg = centerDecDeg + (maxAngSepDeg);
   double decLowerLimitDeg = centerDecDeg - (maxAngSepDeg);

   double maxAngSepRads(SseAstro::degreesToRadians(maxAngSepDeg));
   
   double centerRaRads(SseAstro::hoursToRadians(centerRaHours));
   double centerDecRads(SseAstro::degreesToRadians(centerDecDeg));

   try {

      string dbHost(parser.getStringOption(dbHostArg));
      string dbName(parser.getStringOption(dbNameArg));

      MYSQL* db = connectToDatabase(dbHost, dbName);

      // Query database
      stringstream targetCatQuery;

      targetCatQuery << "select targetId, "
                     << "ra2000Hours, dec2000Deg, "
                     << "catalog, aliases "
                     << "from TargetCat WHERE ";
   
      // only consider targets tagged for automatic selection
      targetCatQuery << " autoSchedule = 'Yes'";


      // set declination limits
      targetCatQuery << " and dec2000Deg >= " << decLowerLimitDeg
                     << " and dec2000Deg <= " << decUpperLimitDeg;

      enum resultCols { targetIdCol, 
                        ra2000HoursCol, dec2000DegCol,
                        catalogCol, aliasesCol, numCols };

      MysqlQuery query(db);
      query.execute(targetCatQuery.str(), numCols, __FILE__, __LINE__);

      cout << "TargId"
           << "\tRaHours"
           << "\t\tDecDeg "
           << "\t\tAngSepDeg " 
           << "\tCatalog "
           << "\tAliases "
           << endl;

      cout.precision(6);
      cout.setf(std::ios::fixed);

      while (MYSQL_ROW row = mysql_fetch_row(query.getResultSet()))
      {
         int targetId(query.getInt(row, targetIdCol, 
                                        __FILE__, __LINE__));

         double ra2000Hours(query.getDouble(row, ra2000HoursCol,
                                            __FILE__, __LINE__));

         double ra2000Rads(SseAstro::hoursToRadians(ra2000Hours));
      
         double dec2000Deg(query.getDouble(row, dec2000DegCol,
                                             __FILE__, __LINE__));

         double dec2000Rads = SseAstro::degreesToRadians(dec2000Deg);
      
         string catalog(query.getString(row, catalogCol,
                                          __FILE__, __LINE__));

         string aliases(query.getString(row, aliasesCol,
                                          __FILE__, __LINE__));

         double angSepRads(SseAstro::angSepRads(ra2000Rads, dec2000Rads,
                                                centerRaRads, centerDecRads));

         if (angSepRads <= maxAngSepRads)
         {
            cout << targetId 
                 << "\t" << ra2000Hours
                 << "\t" << dec2000Deg 
                 << "\t" << SseAstro::radiansToDegrees(angSepRads)
                 << "\t" << catalog
                 << "\t\t" << aliases
                 << endl;
            
         }
      }

   }
   catch (SseException &except)
   {
      cerr << except << endl;
   }

}



MYSQL* connectToDatabase(const string &databaseHost, 
			 const string &databaseName)
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
      "",  // user
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
