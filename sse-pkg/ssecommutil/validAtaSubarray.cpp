/*******************************************************************************

 File:    validAtaSubarray.cpp
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


#include "SseCommUtil.h"
#include "SseUtil.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctype.h>

using namespace std;

/*
  Validate an ATA subarray list.

  Must be of the format
  "ant<1-2 digit number><1 letter>[,ant<number><letter>]"
  where 'ant' is optional
  or, the single keyword 'antgroup'.

  Examples:
  ant3d
  ant3d,ant3e,ant3f
  3d,3e,4e
  antgroup

  Returns true if valid.  If not valid, returns false, and
  errorText contains an explanation of why it's invalid.
  Valid list:
    Not empty.
    No spaces between ant names.
    No repeated antenna names.
    No separator other than comma.
    No prefix other than 'ant'.
    TBD: case sensitive?
 */

bool SseCommUtil::validAtaSubarray(const string &subarray, string &errorText)
{
//   cout << ":validAtaSubarray input: " << subarray << endl;

   if (subarray == "antgroup")
   {
      return true;
   }

   stringstream errorStrm;
   errorStrm << "Invalid subarray: " << subarray << endl;

   vector<string> antNames = SseUtil::tokenize(subarray, ",");

#if 0
// DEBUG
   for(vector<string>::iterator it = antNames.begin();
       it != antNames.end(); ++it)
   {
      const string &name(*it);
      cout << name << endl;
   }
#endif

   if (antNames.empty())
   {
      errorText = "Ant list is empty";
      return false;
   }

   for(vector<string>::iterator it = antNames.begin();
       it != antNames.end(); ++it)
   {
      // only numbers, letters allowed in each name
      const string & name(*it);
      int digitCount(0);
      for (unsigned int i=0; i<name.size(); ++i)
      {
         if (!isalnum(name[i]))
         {
            errorStrm << "Invalid character '"
                      << name[i] << "' in ant name: " << name;

            errorText = errorStrm.str();
            return false;
         }
         
         if (isdigit(name[i]))
         {
            digitCount++;
         }
      }

      // must be at least one digit
      if (digitCount == 0)
      {
           errorStrm << "Invalid ant name, must contain a digit: "
                     << name;
           errorText = errorStrm.str();
           return false;
      }

      // if first char in name is not a number, then first 
      // part of name must be 'ant'
      if (!isdigit(name[0]))
      {
         if (name.find("ant") != 0)
         {
            errorStrm << "Invalid ant name, must begin with"
                      << " 'ant' or a number: " << name;
            errorText = errorStrm.str();
            return false;
         }
      }

      // check for duplicates:
      if (count(antNames.begin(), antNames.end(), name) > 1)
      {
         errorStrm << "Duplicate ant name: " << name;
         errorText = errorStrm.str();
         return false;
      }
   }

   return true;
}