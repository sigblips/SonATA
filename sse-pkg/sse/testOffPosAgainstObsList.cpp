/*******************************************************************************

 File:    testOffPosAgainstObsList.cpp
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
 Test the OffPositions class against a series of observations
 listed in an ascii file, ie, generate off positions for each activity
 in the file.

 Each input file line has these space and/or tab separated columns:

 
 activityId	targetId  dxLowFreqMhz	ra2000Hours    dec2000Deg	isPrimary

eg,

18	20011	1400	17.74775	-28.248764	1
18	20867	1400	17.769614	-28.316055	0
18	21019	1400	17.746889	-28.722621	0
18	22897	1400	17.709839	-28.514956	0

The primary pointing (marked by the isPrimary col) should always come
first, followed by the synth beam pointings.

Output is the off positions, going to stdout.

*/

#include "SseUtil.h"
#include "SseAstro.h"
#include "OffPositions.h"
#include "AtaInformation.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

using namespace std;

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

   // read each line from the input
   string line;
   int lineNumber = 1;

   // skip first header line
   getline(infile, line);
   lineNumber++;
  
   string delimeter(" \t");
   const int actIdFieldNum = 0;
   //const int targetIdFieldNum = 1;
   const int dxFreqMhzFieldNum = 2;
   const int ra2000HoursFieldNum = 3;
   const int dec2000DegFieldNum = 4;
   const int isPrimaryFieldNum = 5;
   const unsigned int nExpectedFields = 6;

   int lastActId(-1);

   // primary beam size:
   const double primaryBeamsizeAtOneGhzDeg(3.5);
   const double arcSecPerDeg(3600);
      const double primaryBeamsizeAtOneGhzArcSec(primaryBeamsizeAtOneGhzDeg * 
						       arcSecPerDeg);

   const double synthBeamsizeAtOneGhzArcSec(1167);
   double minBeamSepFactor(2.0);
   int timeSinceLastObsSecs(200);

   OffPositions *offPositions(0);

   vector<OffPositions::Position> targets;


   while (getline(infile, line))
   {

      try {
	 
	 // parse the line into fields
	 cout << "-- " << lineNumber << ") " << line << endl;
	 
	 vector<string> fields = SseUtil::tokenize(line, delimeter);
	 if (fields.size() != nExpectedFields)
	 {
	    cerr << "not enough fields on line: " << lineNumber << endl;
	    cerr << "found: " << fields.size() << " expected: " << nExpectedFields << endl;
	    cerr << "line is: " << line << endl;
	    break;
	 }
	 
	 // convert the fields
	 int actId = SseUtil::strToInt(fields[actIdFieldNum]);
	 //int targetId = SseUtil::strToInt(fields[targetIdFieldNum]);
	 double dxFreqMhz = SseUtil::strToDouble(fields[dxFreqMhzFieldNum]);
	 double ra2000Hours = SseUtil::strToDouble(fields[ra2000HoursFieldNum]);
	 double dec2000Deg = SseUtil::strToDouble(fields[dec2000DegFieldNum]);
	 int isPrimary = SseUtil::strToInt(fields[isPrimaryFieldNum]);


	 if (isPrimary)
	 {
	    // should be start of new activity sequence
	    if (lastActId == actId)
	    {
	       cerr << "Error or line: " << lineNumber 
		    << " isPrimary indicated a new activity but the "
		    << " activity number did not change " << endl;
	       
	       break;

	    }

	    // find last activity's offs 
	    if (offPositions)
	    {
	       vector<OffPositions::Position> offs;

	       offPositions->getOffPositions(targets, timeSinceLastObsSecs,
					     offs);
	       
	       cout << "actId: " << lastActId << endl
		      << *offPositions << endl;
	       
	       //offPositions->printPrimaryFovGrid(cout);

	       targets.clear();
	       
	       delete offPositions;
	    }

	    // prepare for next activity
	    lastActId = actId;
	    
	    // set primary info
	    // center ra, dec in rads
	    // 
	    double synthBeamsizeRads = AtaInformation::ataBeamsizeRadians(
	       dxFreqMhz, synthBeamsizeAtOneGhzArcSec);
	    
	    //cout << "synth beamsize rads = " << synthBeamsizeRads << endl;

	    // primary beam center position:
	    double primaryCenterRaRads = SseAstro::hoursToRadians(ra2000Hours);
	    double primaryCenterDecRads = SseAstro::degreesToRadians(dec2000Deg);
	    
	    double primaryBeamsizeRads = AtaInformation::ataBeamsizeRadians(
	       dxFreqMhz, primaryBeamsizeAtOneGhzArcSec);
	    
	    //cout << "primaryBeamsizeRads = " << primaryBeamsizeRads << endl;
	    
	    offPositions = new OffPositions(synthBeamsizeRads,
					    minBeamSepFactor,
					    primaryCenterRaRads,
					    primaryCenterDecRads,
					    primaryBeamsizeRads);

	 }
	 else 
	 {
	    // not a primary pointing, so load the beam's target position
	    targets.push_back(OffPositions::Position(
	       SseAstro::hoursToRadians(ra2000Hours),
	       SseAstro::degreesToRadians(dec2000Deg)));

	 }

      }
      catch (SseException &except)
      {
	 cout << "Error on line: " << lineNumber << " " <<  except << endl;
      }

      // parse the line into fields
      lineNumber++;

#if 0
      if (lineNumber > 10)
      {
	 break;
      }
#endif

   }

   cout << "read " << lineNumber << " lines " <<endl;

}