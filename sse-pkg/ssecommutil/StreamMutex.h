/*******************************************************************************

 File:    StreamMutex.h
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


#ifndef StreamMutex_H
#define StreamMutex_H

/*
 Mutex protect output operator access to a stream.
 The constructor & destructor acquire and release the mutex
 so that all output operator use in-between is protected.

 Sample usage:
 
    ACE_Recursive_Thread_Mutex mutex;
    stringstream strm;

    StreamMutex(strm, mutex)
       << "This" << " output" << " is all mutex protected" << endl;

*/
#include <string>
#include <iostream>

class ACE_Recursive_Thread_Mutex;

class StreamMutex
{
 public:
   StreamMutex(std::ostream &strm, ACE_Recursive_Thread_Mutex &mutex);
   virtual ~StreamMutex();

   template <class Type>
   ostream & operator << (Type const & data)
   { 
      strm_ << data;
      
      return strm_;
   }

 private:

    // Disable copy construction & assignment.
    // Don't define these.
   StreamMutex(const StreamMutex& rhs);
   StreamMutex& operator=(const StreamMutex& rhs);

   mutable ostream &strm_;  // must be mutable for op << to work
   ACE_Recursive_Thread_Mutex &mutex_;    
};

#endif // StreamMutex_H