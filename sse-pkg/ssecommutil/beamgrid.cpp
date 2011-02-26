/*******************************************************************************

 File:    beamgrid.cpp
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
  create a grid of offset positions to
  test beam shape
*/

#include "SseAstro.h"
#include "SseUtil.h"
#include "Angle.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

void printOffsets(int count, double azOffsetDeg,
                  double elOffsetDeg)
{
   cout << count << " " 
        << "--offsetaz" << " " << azOffsetDeg
        << " --offsetel" << " " << elOffsetDeg << endl; 
}

int main(int argc, char *argv[])
{
   int nArgsExpected = 1;
   if (argc != nArgsExpected)
   {
      cerr << "usage tbd" << endl;
      exit(1);
   }

   try {

      cout.precision(6);
      cout.setf(std::ios::fixed);  // show all decimal places up to precision

      //double offsetSpacingDeg(1);
      double minPerDeg(60.0);
      double beamsizeArcMin(1);
      double offsetSpacingDeg(beamsizeArcMin/minPerDeg);  // TBD arg
      //double stepsizeDeg(offsetSpacingDeg);

      int count(0);

      // nominal center
      double azOffsetDeg(0);
      double elOffsetDeg(0);

      // generate a square offset grid starting in the lower
      // lefthand corner

      int quarterGridNsteps(3);
      double fullLeftAz(quarterGridNsteps * -1 * offsetSpacingDeg);
      double lowerLeftEl(fullLeftAz);
      
      azOffsetDeg = fullLeftAz;
      elOffsetDeg = lowerLeftEl;

      int squareSideSteps(quarterGridNsteps * 2 + 1);
      for (int i = 0; i < squareSideSteps; ++i)
      {
         azOffsetDeg = fullLeftAz;
         for (int j = 0; j < squareSideSteps; ++j)
         {
            printOffsets(count++, azOffsetDeg, elOffsetDeg);
            azOffsetDeg += offsetSpacingDeg;
         }
         elOffsetDeg += offsetSpacingDeg;
      }
      

#if 0
      // 3x3 grid around center
      // tbd handle wrap around zero az
      // center cross
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg);
      printOffsets(count++, azOffsetDeg, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg, elOffsetDeg-stepsizeDeg);

      // corners
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg-stepsizeDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg-stepsizeDeg);

      // 5x5 grid
      // center cross
      stepsizeDeg = 2 * offsetSpacingDeg;
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg);
      printOffsets(count++, azOffsetDeg, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg, elOffsetDeg-stepsizeDeg);

      // corners
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg-stepsizeDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg-stepsizeDeg);

      // fill in next to corners (knight jumps)
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg+stepsizeDeg/2);
      printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg-stepsizeDeg/2);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg+stepsizeDeg/2);
      printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg-stepsizeDeg/2);

      printOffsets(count++, azOffsetDeg+stepsizeDeg/2, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg+stepsizeDeg/2, elOffsetDeg-stepsizeDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg/2, elOffsetDeg+stepsizeDeg);
      printOffsets(count++, azOffsetDeg-stepsizeDeg/2, elOffsetDeg-stepsizeDeg);


      // Extend off of each axis
      for (int step =3; step <= 6; step++)
      {
         stepsizeDeg = step * offsetSpacingDeg;         
         printOffsets(count++, azOffsetDeg+stepsizeDeg, elOffsetDeg);
         printOffsets(count++, azOffsetDeg-stepsizeDeg, elOffsetDeg);
         printOffsets(count++, azOffsetDeg, elOffsetDeg+stepsizeDeg);
         printOffsets(count++, azOffsetDeg, elOffsetDeg-stepsizeDeg);
      }      
#endif

    }
    catch (SseException &except)
    {
	cerr << except << endl;
	exit(1);
    }

}