/*******************************************************************************

 File:    coordToDecimal.cpp
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


#include "SseUtil.h"
#include "SseAstro.h"
#include <sstream>
#include <iostream>
#include <cstdlib>

using namespace std;

// convert coordinate in (hours|deg) min sec to decimal

int main(int argc, char *argv[])
{
   int minArgs = 4;
   int maxArgs = 7;
   if (argc != minArgs && argc != maxArgs)
   {
      cerr << "convert coords to decimal" << endl;
      cerr << "usage: " << argv[0] << " <hour|deg> <min> <sec> [<hour|deg> <min> <sec>]" 
           << endl;
      exit(1);
   }

   int argIndex = 1;
   
   try {

      stringstream strm;
      strm.precision(9);
      strm.setf(std::ios::fixed);  // show all decimal places up to precision

      // get first coord triplet
      int firstArgIndex=argIndex;
      int deg = SseUtil::strToInt(argv[argIndex++]);
      int min = SseUtil::strToInt(argv[argIndex++]);
      double sec = SseUtil::strToDouble(argv[argIndex++]);

#if 0
      cout << "deg: " << deg
           << ", min: " << min
           << ", sec: " << sec 
           << endl;
#endif

      char sign(argv[firstArgIndex][0]); 
      double decimalValue = SseAstro::degreesToDecimal(sign, deg, min, sec);
      strm << decimalValue;

      // get additional coord triplet, if given
      if (argc == maxArgs)
      {
         firstArgIndex=argIndex;
         sign = argv[firstArgIndex][0];
         deg = SseUtil::strToInt(argv[argIndex++]);
         min = SseUtil::strToInt(argv[argIndex++]);
         sec = SseUtil::strToDouble(argv[argIndex++]);
         decimalValue = SseAstro::degreesToDecimal(sign, deg, min, sec);
         strm << " " << decimalValue;
      }

      cout << strm.str() << endl;
    
   }
   catch (SseException &except)
   {
      cerr << except << endl;
      exit(1);
   }

}