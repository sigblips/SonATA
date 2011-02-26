/*******************************************************************************

 File:    printSseMsgDoc.cpp
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

#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iosfwd>
#include <iostream>
#include <iomanip>

#include "SseMsgDoc.h"
#include "SseMsg.h"

using namespace std;

int countReturns( const char * text)
{
    int count = 0;


    while (*text != '\0' ){
	if ( *text == '\n' ) count++;
	text++;
    }
    return(count);
}

int main() {
   stringstream strm;
   int ncode = sizeof(  msgInfo )/ sizeof (struct SseMsgDocumentation);

   strm << "\n\n      Error   Severity\tName/" << endl;
   strm << "      Number\t\tDescription/" << endl;
   strm << "\t\t\tUser Action\n" << endl;

   cout << strm.str();
   int lineCount = 6;
   for ( int n = 0; n < ncode; n++ ) {
	cout << "      " << msgInfo[n].code << "    " 
	     << SseMsg::nssMessageSeverityToString(msgInfo[n].severity) 
	     << "\t"
	     << msgInfo[n].name 
	     << "\n\t\t\t"
	     << msgInfo[n].description << "\n\t\t\t" 
	     << msgInfo[n].userAction << endl;
	 lineCount += (3 + countReturns(msgInfo[n].description.c_str())
		      + countReturns(msgInfo[n].userAction.c_str()));
	 if (lineCount >= 54 ) {
	     lineCount = 7;
	     cout << "" << strm.str();
	 }
   }
}
