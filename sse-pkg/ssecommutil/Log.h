/*******************************************************************************

 File:    Log.h
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


#ifndef Log_H
#define Log_H

// Appends output stream data to a log file.
// Creating the class opens the file, and the destructor
// closes it.
// Typically you'll want to use a subclass of 
// Log that specifies the log filename.
// E.g,
//
//    class ErrorLog : public Log
//    {
//    public:
//      ErrorLog() : Log("errorlog.txt") {}
//    };
//
// Then you can easily send output to the log by creating
// temporary Errorlog objects, like so:
//
// ErrorLog() << "This" << " output" << " goes to the error log" << endl;

#include <fstream>
#include <string>
#include <iostream>

using std::string;
using std::ofstream;
using std::ostream;
using std::cerr;
using std::endl;

class ACE_Recursive_Thread_Mutex;

class Log
{
 public:
    Log(const string &filename, ACE_Recursive_Thread_Mutex &mutex);
    virtual ~Log();

    template <class Type>
    ostream & operator << (Type const & data)
    { 
	if (strm_.is_open())
	{
	  strm_ << data;
	}
	return strm_;
    }

    private:

    // Disable copy construction & assignment.
    // Don't define these.
    Log(const Log& rhs);
    Log& operator=(const Log& rhs);


    ACE_Recursive_Thread_Mutex &mutex_;    
    mutable ofstream strm_;  // must be mutable for op << to work

};

#endif // Log_H