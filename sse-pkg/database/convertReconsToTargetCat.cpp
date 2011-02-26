/*******************************************************************************

 File:    convertReconsToTargetCat.cpp
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


// Convert data from Recons Nearest Stars list to TargetCat table format.
// Output is suitable for loading into the seeker TargetCat database table.

/* 
Input format:
#catId,ra2000hms,dec2000dms,pmMagArcsecYr,pmAngleDeg,parallaxArcsec,spectype,vmag
GJ  551      ,14 29 43.0,-62 40 46,3.853,281.5,0.76887,M5.5 V  *, 11.09
GJ  559    A ,14 39 36.5,-60 50 02,3.710,277.5,0.74723,G2   V  N,  0.01
GJ  559    B ,14 39 35.1,-60 50 14,3.724,284.8,0.74723,K0   V  N,  1.34
...

- Vmag may be empty.
- Input lines that start with # are ignored.
*/

#include "SseUtil.h"
#include "SseAstro.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

void printDbTableLine(int targetId, const string & catalog,
                      double raHours, double decDeg,
                      double pmRaMasYr, double pmDecMasYr,
                      double parallaxMas,
                      const string & spectralType,
                      double bMag, double vMag,
                      const string &aliases);

double getFilterMag(const string & magString)
{
   double filterMag=-99;  // no value
   if (magString.find_first_not_of(" \t") != string::npos)
   {
      filterMag = SseUtil::strToDouble(magString);
   }

   return filterMag;
}

string removeSpaces(const string & input)
{
   string cleaned(input);
   string::iterator pos = remove(cleaned.begin(), cleaned.end(), ' ');

   // erase the "removed" elements
   cleaned.erase(pos, cleaned.end());

   return cleaned;
}


int main(int argc, char *argv[])
{
   // get filename from argv
   if (argc != 4)
   {
      cerr << "usage: " << argv[0] << " <TargetCat catalog name> <first targetId> <input filename>" << endl;
      exit(1);
   }

   string catalog(argv[1]);

   string firstIdStr(argv[2]);
   int firstTargetId;
   try {
      firstTargetId = SseUtil::strToInt(firstIdStr);
   }
   catch(SseException &except)
   {
      cerr << "Error: invalid first target id: " << firstIdStr << endl;
      cerr << except;
      exit(1);
   }

   string filename(argv[3]);
   ifstream infile(filename.c_str());
   if (!infile)
   {
      cerr << "Error: can't open input file: " << filename << endl;
      exit(1);
   }

   cout.precision(9);           // show N places after the decimal
   cout.setf(std::ios::fixed);  // show all decimal places up to precision
   
   cout << "-- $"   // split over 2 lines so does not get expanded here
        << "Id$ --" << endl;

   cout << "-- targetId,catalog,"
	 << "ra2000Hours,dec2000Deg,pmRaMasYr,pmDecMasYr,parallaxMas,"
	 << "spectralType,bMag,vMag,aliases,primaryTargetId,"
        << "autoschedule" << endl;

   int targetId = firstTargetId;

   // for each line in file, parse the line into
   // fields and create a database line

   char delimiter(',');
   
   enum fields { CatIdFieldNum, RaFieldNum, DecFieldNum,
                 PmMagFieldNum, PmAngleFieldNum, 
                 ParallaxFieldNum, SpecTypeFieldNum,
                 VMagFieldNum, NumFields };

   const unsigned int NExpectedFields = NumFields;

   // Assuming horizon is 15 deg el or higher
   double ataMinDecDeg = -35.0;

   int lineNumber = 0;
   string line;

   try {
      
      while (getline(infile, line))
      {
         lineNumber++;

         // parse the line into fields
         //cout << "-- " << lineNumber << ") " << line << endl;

         vector<string> fields = SseUtil::splitByDelimiter(line, delimiter);
         if (fields.size() > 0)
         {
            // skip comments starting with #
            if (fields[0].find("#") != string::npos)
            {
               continue;
            }
         }

         if (fields.size() != NExpectedFields)
         {
            cerr << "not enough fields on line: " << lineNumber << endl;
            cerr << "line is: " << line << endl;
            break;
         }

         // break out ra & dec
         const string CoordDelim = " ";

         //cout << "ra: " << fields[RaFieldNum] << endl;
         enum RaFieldsIndices { RaHoursFieldNum, RaMinFieldNum, RaSecFieldNum, 
                                NExpectedRaCoordFields };
         vector<string> raFields = SseUtil::tokenize(fields[RaFieldNum], CoordDelim);
         if (raFields.size() != NExpectedRaCoordFields)
         {
            cerr << "not enough ra fields on line: " << lineNumber << endl;
            cerr << "line is: " << line << endl;
            break; 
         }


         double raHours = SseAstro::hoursToDecimal(
            SseUtil::strToInt(raFields[RaHoursFieldNum]),
            SseUtil::strToInt(raFields[RaMinFieldNum]), 
            SseUtil::strToDouble(raFields[RaSecFieldNum])
	    );


         //cout << "dec: " << fields[DecFieldNum] << endl;

         enum DecFieldsIndices { DecDegFieldNum, DecMinFieldNum, DecSecFieldNum, 
                                 NExpectedDecCoordFields };
         vector<string> decFields = SseUtil::tokenize(fields[DecFieldNum], CoordDelim);
         if (decFields.size() != NExpectedDecCoordFields)
         {
            cerr << "not enough dec fields on line: " << lineNumber << endl;
            cerr << "line is: " << line << endl;
            break; 
         }

         char sign(decFields[DecDegFieldNum][0]);
         double decDeg = SseAstro::degreesToDecimal(
            sign,
            SseUtil::strToInt(decFields[DecDegFieldNum]),
            SseUtil::strToInt(decFields[DecMinFieldNum]), 
            SseUtil::strToDouble(decFields[DecSecFieldNum])
	    );

         const double MasPerSec(1000);

         // convert parallax magnitude and angle to offsets in ra & dec
         double pmMagArcSecYr = SseUtil::strToDouble(fields[PmMagFieldNum]);
         double pmMagMasYr = pmMagArcSecYr * MasPerSec;

         double pmAngleDeg = SseUtil::strToDouble(fields[PmAngleFieldNum]);
         double pmAngleRads = SseAstro::degreesToRadians(pmAngleDeg);

         double pmRaMasYr = pmMagMasYr * sin(pmAngleRads);
         double pmDecMasYr = pmMagMasYr * cos(pmAngleRads);


         double parallaxMas = SseUtil::strToDouble(fields[ParallaxFieldNum])
            * MasPerSec;

         string specType = removeSpaces(fields[SpecTypeFieldNum]);

         const double bMag = -99;

         double vMag = getFilterMag(fields[VMagFieldNum]);

         string aliases = removeSpaces(fields[CatIdFieldNum]);

         if (decDeg > ataMinDecDeg)
         {
            printDbTableLine(targetId, catalog,
                             raHours, decDeg,
                             pmRaMasYr, pmDecMasYr,
                             parallaxMas,
                             specType,
                             bMag, vMag, 
                             aliases);
         }
           
         /*
           Increment target id whether it's used or
           not, so that there's always a one-to-one mapping
           with the input catalog no matter how 
           it's filtered.
         */
         targetId++;
      }
   }
   catch (SseException &except)
   {
      cerr << "error on line: " << lineNumber << endl
           << line << endl
           << except << endl;
   }
   
}

void printDbTableLine(int targetId, const string & catalog, 
                      double raHours, double decDeg,
                      double pmRaMasYr, double pmDecMasYr,
                      double parallaxMas,
                      const string & spectralType,
                      double bMag, double vMag,
                      const string &aliases)
{
    int primaryTargetId(-1);
    string autoschedule("Yes");  // enable auto scheduling

    cout
	 << targetId << ","
	 << catalog << ","
	 << raHours << ","
	 << decDeg << ","
	 << pmRaMasYr << "," 
	 << pmDecMasYr << ","
	 << parallaxMas << ","
	 << spectralType << ","
	 << bMag << ","
	 << vMag << ","
	 << aliases << ","
	 << primaryTargetId << ","
	 << autoschedule
         << endl;
}

