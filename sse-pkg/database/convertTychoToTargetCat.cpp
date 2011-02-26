/*******************************************************************************

 File:    convertTychoToTargetCat.cpp
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


// Convert the Tycho file format to TargetCat table format.
// Output is suitable for loading into the seeker TargetCat database table
// with the 'load data local infile' command.

/* 

Tycho format in ascii is:

32 fields, separated by '|'.

Field # (starting at 0) and definition:
0: tycho number
1: mean position flag (' ') means normal position
2: mRAdeg    []? Mean Right Asc, ICRS, epoch J2000, deg
3: mDEdeg    []? Mean Decl, ICRS, at epoch J2000, deg 
4: pmRa  mas/yr
5: pmDec mas/yr
17 : BTmag
19 : VTmag

No parallax or spectral type information available.

Note on BT & VT mags:
    Blank when no magnitude is available. Either BTmag or VTmag is
    always given. Approximate Johnson photometry may be obtained as:
    V   = VT -0.090*(BT-VT)
    B-V = 0.850*(BT-VT)


*/

#include "SseUtil.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>

using namespace std;

void printDbTableLine(int targetId, const string & catalog,
                      double raHours, double decDeg,
                      double pmRaMasYr, double pmDecMasYr,
                      double parallaxMas,
                      const string & spectralType,
                      double bMag, double vMag,
                      const string &aliases);

string removeSpaces(const string & input)
{
   string cleaned(input);
   string::iterator pos = remove(cleaned.begin(), cleaned.end(), ' ');

   // erase the "removed" elements
   cleaned.erase(pos, cleaned.end());

   return cleaned;
}

string replaceChar(const string &input, char oldChar, char newChar)
{
   string updated(input);
   replace(updated.begin(), updated.end(), oldChar, newChar);

   return updated;
}

int main(int argc, char *argv[])
{
   // get filename from argv
   if (argc != 2)
   {
      cerr << "usage: " << argv[0] << " <filename>" << endl;
      exit(1);
   }

   string filename(argv[1]);
   ifstream infile(filename.c_str());
   if (!infile)
   {
      cerr << "error, can't open input file: " << filename << endl;
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

   int firstTargetId = 1000000;  

   int targetId = firstTargetId;

   string line;
   int lineNumber = 1;

   // skip first header line
   getline(infile, line);
   lineNumber++;

   // for each line in file, parse the line into
   // fields and create a database line

   char delimiter('|');
   const int TychoIdFieldNum = 0;
   const int MeanPositionFlagFieldNum = 1;
   const int RaFieldNum = 2;
   const int DecFieldNum = 3;
   const int PmRaFieldNum = 4;
   const int PmDecFieldNum = 5;
   const int BtMagFieldNum = 17;
   const int VtMagFieldNum = 19;

   const unsigned int NExpectedFields = 32;

   // Assuming horizon is 15 deg el or higher
   double ataMinDecDeg(-35.0);
   const double DegPerHour(15);
   string catalog("tycho2remainder");
   double parallaxMas(0.01);  // 0.01 = 100kpc
   string specType("");

   try 
   {
      while (getline(infile, line))
      {
         // parse the line into fields
         //cout << "-- " << lineNumber << ") " << line << endl;

         vector<string> fields = SseUtil::splitByDelimiter(line, delimiter);
         if (fields.size() != NExpectedFields)
         {
            cerr << "not enough fields on line: " << lineNumber << endl;
            cerr << "line is: " << line << endl;
            break;
         }

         string & tychoId=fields[TychoIdFieldNum];

         const string goodMeanPos(" ");
         string & meanPosFlag=fields[MeanPositionFlagFieldNum];

         if (meanPosFlag == goodMeanPos)
         {
            double raHours = SseUtil::strToDouble(fields[RaFieldNum]) /
               DegPerHour;
            
            double decDeg = SseUtil::strToDouble(fields[DecFieldNum]);

            double pmRaMasYr = SseUtil::strToDouble(fields[PmRaFieldNum]);
            double pmDecMasYr = SseUtil::strToDouble(fields[PmDecFieldNum]);

            double bMag(-99);
            double vMag(-99);

            const string &btMagStr = fields[BtMagFieldNum];
            const string &vtMagStr = fields[VtMagFieldNum];

            if (! removeSpaces(btMagStr).empty() && ! removeSpaces(vtMagStr).empty())
            {
               double bt = SseUtil::strToDouble(btMagStr);
               double vt = SseUtil::strToDouble(vtMagStr);

               // conversion from tycho2 catalog description
               vMag = vt -0.090*(bt-vt);
               double bminusv = 0.850*(bt-vt);
                  
               bMag = bminusv + vMag;
            }
            string aliases("TYC" + replaceChar(tychoId,' ','-'));
            
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

         lineNumber++;

// debug
#if 0
         if (lineNumber > 2000)
         {
            break;
         }
#endif         

      }   

   }
   catch (SseException & except)
   {
      cerr << "lineNumber: " << lineNumber << ": "
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
