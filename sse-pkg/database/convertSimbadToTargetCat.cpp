/*******************************************************************************

 File:    convertSimbadToTargetCat.cpp
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


// Convert data from Simbad to TargetCat table format.
// Output is suitable for loading into the seeker TargetCat database table.

/* 
Simbad query format:
----------------------

format object "%COO(d;A;ICRS)| %COO(d;D;ICRS) | %PM(A)| %PM(D)| %PLX(V)| %SP(S) | %FLUXLIST(B;F) 
| %FLUXLIST(V;F) | %IDLIST(S;HIP) | %IDLIST(S;HD) | %IDLIST(S;TYC)| %IDLIST(1)"

echo "RA J2000 <deg> | Dec J2000 <deg> | pm RA mas/yr | pm Dec mas/yr | parallax mas | spectralTy
pe | bMag | vMag | HIP ID | HD ID | Tycho ID | main ID"

<star ids here>

--------------
Example data file input lines:

242.6013096 | +43.8176447 | 132.52 | -298.38 | 55.11 | K0V | 7.57 | 6.67 | HIP 79248 | HD 145675 | TYC 3067-576-1| G 180-35
295.4665500 | +50.5175231 | -135.15 | -163.53 | 46.70 | G3V | 6.86 | 6.20 | HIP 96901 | HD 186427 | TYC 3565-1525-1| NLTT 48138
181.88946 | -39.54833 | ~ | ~ | ~ | M8 |  | 18.9 |  |  | | DENIS-P J120733.4-393254

Notes: 
- Bmag & Vmag may be empty.
- Parallax may equal '~' indicating no value.
- Input lines that start with # are ignored.
- Be sure to comment out or trim away all simbad header lines.
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

   char delimiter('|');
   enum fields { RaFieldNum, DecFieldNum, 
                 PmRaFieldNum, PmDecFieldNum,
                 ParallaxFieldNum, SpecTypeFieldNum,
                 BMagFieldNum, VMagFieldNum, HipFieldNum,
                 HdFieldNum, TychoFieldNum, MainIdFieldNum,
                 NumFields };

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
         
         const double DegPerHour(15);
         double raHours = SseUtil::strToDouble(fields[RaFieldNum])
            / DegPerHour;

         double decDeg = SseUtil::strToDouble(fields[DecFieldNum]);

         const string invalidPm("~");
         double pmRaMasYr = 0;
         if (fields[PmRaFieldNum].find_first_of(invalidPm)
             == string::npos)
         {
            pmRaMasYr = SseUtil::strToDouble(fields[PmRaFieldNum]);
         }

         double pmDecMasYr = 0;
         if (fields[PmDecFieldNum].find_first_of(invalidPm)
             == string::npos)
         {
            pmDecMasYr = SseUtil::strToDouble(fields[PmDecFieldNum]);
         }

         double parallaxMas = 0.01;  // 0.01 mas = 100 kpc
         const string invalidParallax("~");
         if (fields[ParallaxFieldNum].find_first_of(invalidParallax)
             == string::npos)
         {
            parallaxMas = SseUtil::strToDouble(fields[ParallaxFieldNum]);
         }
 
         string specType = removeSpaces(fields[SpecTypeFieldNum]);

         double bMag = getFilterMag(fields[BMagFieldNum]);

         double vMag = getFilterMag(fields[VMagFieldNum]);

         string aliases = removeSpaces(fields[HipFieldNum]) + " "
            + removeSpaces(fields[HdFieldNum]) + " "
            + removeSpaces(fields[TychoFieldNum]) + " "
            + fields[MainIdFieldNum];
         
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

