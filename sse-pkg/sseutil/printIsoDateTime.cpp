/*******************************************************************************

 File:    printIsoDateTime.cpp
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

#include <iostream>
#include <cstdlib>
#include "SseUtil.h"

using namespace std;

// print a unix timestamp as an iso date-time

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
	cout << "usage: " << argv[0] << " <unix-timestamp>" << endl;
	exit(1);
    }

    try {

	time_t timestamp = SseUtil::strToInt(argv[1]);
	cout << SseUtil::isoDateTime(timestamp) << endl;
    
    }
    catch (...)
    {
	cerr << "invalid timestamp: " << argv[1] << endl;
	exit(1);
    }

}