/*******************************************************************************

 File:    convertGalGridToTargetCat.cpp
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

/* Convert input file into a target catalog for ATA/Prelude galactic survey

 Input ASCII file format is CSV, as follows:
    # = comment lines

    GalLong(deg),GalLat(deg),RA(J2000-hours),Dec(J2000-deg)
 or
    GalLong(deg),GalLat(deg)
 or
    RA(J2000-hours),Dec(J2000-deg)
*/


/*
 Output (to stdout) is suitable for loading into the seeker 
 TargetCat database table.

 In addition to its own pointing coordinates,
 each target is also assigned a primary pointing position.
 These primary positions are created such that they cover the
 galactic synthesized beam grid with a much coarser grid based 
 on the primary beam size, so that a limited number of primary
 pointings will be used, to aid commensal observers.

 For testing purposes, this program can also create multiple copies
 of the input grid, spaced at regular RA intervals in hours,
 so that targets are available at any time.   Note that the 
 gal coords for these copies are kept the same as the original data.
*/

#define GalRaDecCoords
#define CYGNUSX3


#include "SseUtil.h"
#include "SseAstro.h"
#include "Assert.h"
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class Pointing
{
public:
   double glongDeg;
   double glatDeg;
   double raHours;
   double decDeg;
   double raRads;
   double decRads;
   int targetId;

   Pointing();
   friend ostream &operator<<(ostream& os,
                             const Pointing& pointing);
};

Pointing::Pointing()
   :
   glongDeg(0.0),
   glatDeg(0.0),
   raHours(0.0),
   decDeg(0.0),
   raRads(0.0),
   decRads(0.0),
   targetId(-1)
{
}

ostream &operator<<(ostream& strm,
		    const Pointing& pointing)
{
   strm << " pointing:"
	<< " targetId: " << pointing.targetId
	<< " glongDeg: " << pointing.glongDeg
	<< " glatDeg: " << pointing.glatDeg
	<< " raHours: " << pointing.raHours
	<< " decDeg: " << pointing.decDeg
	<< " raRads: " << pointing.raRads
	<< " decRads: " << pointing.decRads;
   
   return strm;
}


vector<Pointing> PrimaryPointingLookupTable; 

// Return the primary target id whose angular separation is
// closest to the given target pointing

int lookupPrimaryTargetId(const Pointing & targetPointing)
{
   Assert(PrimaryPointingLookupTable.size() > 0);
   Pointing nearestPrimaryPointing(PrimaryPointingLookupTable[0]);
   
   double minAngSepRads = 2 * M_PI;

   for (vector<Pointing>::const_iterator it = 
	   PrimaryPointingLookupTable.begin();
	it != PrimaryPointingLookupTable.end(); ++it)
   {
      const Pointing & primaryPointing = *it;

      double diffRads = SseAstro::angSepRads(
         primaryPointing.raRads, primaryPointing.decRads,
         targetPointing.raRads, targetPointing.decRads);

      if (diffRads < minAngSepRads)
      {
	 minAngSepRads = diffRads;
	 nearestPrimaryPointing = primaryPointing;
      }
	  
   }

   return nearestPrimaryPointing.targetId;
}

void createPrimaryPointingLookupTable(int & targetId, int raOffsetHours);
void createCygnusx3PrimaryPointingLookupTable(int & targetId);

void printPrimaryPointingDbTableLines();
void printDbTableLine(int targetId, double galLongDeg, double galLatDeg,
		      double raHours, double decDeg,
		      int primaryTargetId, bool autoSchedule);

int main(int argc, char *argv[])
{
   cout.precision(6);           // show N places after the decimal
   cout.setf(std::ios::fixed);  // show all decimal places up to precision


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

   // print table header
   cout << "---- " << endl
        << "-- ATA Survey Grid targets" << endl
	<< "-- input file: " << filename << endl
	<< " " << endl;

   //const int firstTargetId = 20000;  // skip over other target lists.
   //const int firstTargetId = 26000;  // extended grid
   const int firstTargetId = 28000;  // cygnusx3
   int targetId = firstTargetId;

   // Create multiple copies of the gal plane grid, 
   // at regularly spaced RA intervals, to provide
   // targets available at any time for testing.
   // Assumes input data is at ~ra=18 hours

   int startRaOffsetHours = 0;  // -18
   int endRaOffsetHours = 0;   // 4
   int stepRaOffsetHours = 2;  
   for (int raOffsetHours = startRaOffsetHours;
	raOffsetHours <= endRaOffsetHours;
	raOffsetHours += stepRaOffsetHours)
   {
      // use separate group of target IDs for each copy of the grid.
      // round up to the next nearest "even" number group
      const int idSep = 1000;
      int remainder = targetId % idSep;
      targetId += (idSep - remainder) + idSep;

#if 0
      cout << "-- RA offset hours: " << raOffsetHours << endl;
      cout << "-- targetIds start at: " << targetId << endl;
      cout << "--" << endl;
#endif

#ifdef CYGNUSX3
      createCygnusx3PrimaryPointingLookupTable(targetId);
#else
      createPrimaryPointingLookupTable(targetId, raOffsetHours);
#endif

      // add a gap between end of primary list & regular target list
      targetId += 100;  
   
#ifdef GalRaDecCoords

      // for each line in file, parse into fields and create a database line
      const int GalLongFieldNum=0;
      const int GalLatFieldNum=1;

      const int RaFieldNum=2;
      const int DecFieldNum=3;

#else

      const int RaFieldNum=0;
      const int DecFieldNum=1;

#endif

      //const unsigned int NExpectedFields = 4;
      const unsigned int NExpectedFields = 2;  // glong,glat or ra,dec only
   
      const char delimiter(',');

      bool autoSchedule(true);

      // seek to beginning of input file
      infile.seekg(0, std::ios::beg);

      string line;
      int lineNumber = 0;

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

	 try {

	    Pointing pointing;

#ifdef GalRaDecCoords
            
	    pointing.glongDeg = SseUtil::strToDouble(fields[GalLongFieldNum]);
	    pointing.glatDeg = SseUtil::strToDouble(fields[GalLatFieldNum]);

            if (fields.size() > 2)
            {
               // Read ra,dec
               pointing.raHours = SseUtil::strToDouble(fields[RaFieldNum]);
               pointing.decDeg = SseUtil::strToDouble(fields[DecFieldNum]);
            }
            else
            {
               // Generate ra,dec from gal coords
               double glongRads(SseAstro::degreesToRadians(pointing.glongDeg));
               double glatRads(SseAstro::degreesToRadians(pointing.glatDeg));

               double raRads;
               double decRads;
               SseAstro::galToEqu2000(glongRads, glatRads, &raRads, &decRads);
               
               pointing.raHours = SseAstro::radiansToHours(raRads);
               pointing.decDeg = SseAstro::radiansToDegrees(decRads);
            }
#else 
// RA dec coords only
               // Read ra,dec
               pointing.raHours = SseUtil::strToDouble(fields[RaFieldNum]);
               pointing.decDeg = SseUtil::strToDouble(fields[DecFieldNum]);

               pointing.raRads = SseAstro::hoursToRadians(pointing.raHours);
               pointing.decRads = SseAstro::degreesToRadians(pointing.decDeg);

               double galLongRads;
               double galLatRads;
               SseAstro::equ2000ToGal(pointing.raRads, pointing.decRads,
                                      &galLongRads, &galLatRads);
               
               pointing.glongDeg = SseAstro::radiansToDegrees(galLongRads);
               pointing.glatDeg = SseAstro::radiansToDegrees(galLatRads);
#endif

	    pointing.raHours += raOffsetHours;
	    if (pointing.raHours < 0)
	    {
	       const double HoursPerDay(24.0);
	       pointing.raHours += HoursPerDay;
	    }

	    pointing.raRads = SseAstro::hoursToRadians(pointing.raHours);
	    pointing.decRads = SseAstro::degreesToRadians(pointing.decDeg);

	    int primaryTargetId = 
	       lookupPrimaryTargetId(pointing);

	    printDbTableLine(targetId, pointing.glongDeg, pointing.glatDeg,
			     pointing.raHours, pointing.decDeg,
			     primaryTargetId, autoSchedule);
	    targetId++;

	 }
	 catch (SseException &except)
	 {
	    cout << "error on line: " << lineNumber << " " 
		 << except << endl;
	    break;
	 }
      
      }

      // clear eofbit & failbit due to end-of-file
      infile.clear();

   }

   infile.close();

   printPrimaryPointingDbTableLines();
    
}


void addPrimaryPointingRaDec(int targetId, double raHours, double decDeg)
{
      Pointing pointing;

      pointing.targetId = targetId;

      pointing.raHours = raHours;
      pointing.decDeg = decDeg;
      
      pointing.raRads = SseAstro::hoursToRadians(raHours);
      pointing.decRads = SseAstro::degreesToRadians(decDeg);
      
      double galLongRads;
      double galLatRads;
      SseAstro::equ2000ToGal(pointing.raRads, pointing.decRads,
                             &galLongRads, &galLatRads);

      pointing.glongDeg = SseAstro::radiansToDegrees(galLongRads);
      pointing.glatDeg = SseAstro::radiansToDegrees(galLatRads);

      PrimaryPointingLookupTable.push_back(pointing);
}


// Store primary pointing positions in the lookup table.
// The incoming targetId is used as the initial id value,
// and is incremented as needed.

void createCygnusx3PrimaryPointingLookupTable(int & targetId)
{
/*
Cyg X-3
20.540494, 40.95775

Plus 1degree in galactic longitude
20.593500, 41.76007

Minus 1 degree in galactic latitude
20.610930, 40.359670

Plus 1degree in galactic longitude and minus 1 degree in galactic latitude
20.664310, 41.154730
*/

   addPrimaryPointingRaDec(targetId++, 20.540494, 40.95775);
   addPrimaryPointingRaDec(targetId++, 20.593500, 41.76007);
   addPrimaryPointingRaDec(targetId++, 20.610930, 40.359670);
   addPrimaryPointingRaDec(targetId++, 20.664310, 41.154730);
}

// create regularly spaced primary pointing positions
// and store them in the lookup table.
// The incoming targetId is used as the initial id value,
// and is incremented as needed.

void createPrimaryPointingLookupTable(int & targetId, int raOffsetHours)
{

#if 0
   const double glongMinDeg = -4.5;
   const double glongMaxDeg = 4.5;
#else
   const double glongMinDeg = 5.5;
   const double glongMaxDeg = 6.5;
#endif
   const double glongStepDeg = 1.0;

   for (double glongDeg = glongMinDeg; 	
	glongDeg <= glongMaxDeg;
	glongDeg += glongStepDeg)
   {
      Pointing pointing;

      pointing.glongDeg = glongDeg;

      const double glatMinDeg = -0.5;
      const double glatMaxDeg = 0.5;
      const double glatStepDeg = 1.0;

      for (double glatDeg = glatMinDeg; 
	   glatDeg <= glatMaxDeg;
	   glatDeg += glatStepDeg)
      {
	 pointing.glatDeg = glatDeg;
	 pointing.targetId = targetId++;

	 // convert to j2000 ra,dec
	 double glongRads = SseAstro::degreesToRadians(glongDeg);
	 double glatRads = SseAstro::degreesToRadians(glatDeg);
	    
	 double raRads;
	 double decRads;
         SseAstro::galToEqu2000(glongRads, glatRads, &raRads, &decRads);
	 
	 pointing.raRads = raRads;
	 pointing.decRads = decRads;

	 pointing.raHours = SseAstro::radiansToHours(raRads);
	 pointing.decDeg = SseAstro::radiansToDegrees(decRads);

	 //cout << pointing << endl;

	 // adjust ra by offset
	 pointing.raHours += raOffsetHours;

	 if (pointing.raHours < 0)
	 {
	    const double HoursPerDay(24.0);
	    pointing.raHours += HoursPerDay;
	 }

	 pointing.raRads = SseAstro::hoursToRadians(pointing.raHours);

	 PrimaryPointingLookupTable.push_back(pointing);
      }
   }
}


void printPrimaryPointingDbTableLines()
{
   cout << "-- primary target pointings --\n";

   const int primaryTargetId(-1); // dummy value for primary targets themselves
   bool autoSchedule(false);

   for (vector<Pointing>::const_iterator it = 
	   PrimaryPointingLookupTable.begin();
	it != PrimaryPointingLookupTable.end(); ++it)
   {
      const Pointing & pointing = *it;

      printDbTableLine(pointing.targetId,
		       pointing.glongDeg, pointing.glatDeg,
		       pointing.raHours, pointing.decDeg,
		       primaryTargetId, autoSchedule);
   }

   cout << "-- end primary target pointings --\n";
   cout << "-- --\n";
}


void printDbTableLine(int targetId, double galLongDeg, double galLatDeg,
		      double raHours, double decDeg,
		      int primaryTargetId, bool autoSchedule)
{
#ifdef CYGNUSX3
   string catalog("cygnusx3"); 
#else
   string catalog("galsurvey"); 
#endif    

    string autoScheduleText("No");
    if (autoSchedule)
    {
       autoScheduleText = "Yes";
    }
    stringstream aliases;
    aliases.precision(6);           // show N places after the decimal
    aliases.setf(std::ios::fixed);  // show all decimal places up to precision

    aliases << "galLongDeg: " << galLongDeg
            << " galLatDeg: " << galLatDeg ;

    cout << "INSERT INTO TargetCat (targetId, catalog, "
	 << "ra2000Hours, dec2000Deg, "
	 << "aliases, primaryTargetId, "
	 << "autoschedule"
	 << ") ";

    cout << "VALUES (" 
	 << targetId << ", "
	 << "'" << catalog << "', "
	 << raHours << ", "
	 << decDeg << ", "
	 << "'" << aliases.str() << "', "
	 << primaryTargetId << ", "
	 << "'" << autoScheduleText << "'"
	 << ");" << endl;
}
