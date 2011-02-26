/*******************************************************************************

 File:    Log.cpp
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



#include "ace/Synch.h"
#include "Log.h" 
#include "SseUtil.h"

using namespace std;


Log::Log(const string &filename, ACE_Recursive_Thread_Mutex &mutex)
    :mutex_(mutex)
{
    mutex_.acquire();

    // open an output text stream attached to a file
    strm_.open(filename.c_str(), (ios::app));
    if (! strm_.is_open())
    {
        cerr << "Log(): File Open failed on " << filename << endl;

	// TBD better error handling
    }
    else 
    {
	// timestamp 
	strm_ << SseUtil::currentIsoDateTime() << " ";
    }
}

Log::~Log()
{ 
    if (strm_.is_open())
    {
	strm_.flush(); 
	strm_.close();
    }

    mutex_.release();
}


