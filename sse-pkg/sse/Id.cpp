/*******************************************************************************

 File:    Id.cpp
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

/*++ Id.cpp - supplies id numbers for classes
 * PURPOSE:  
 --*/

#include "Id.h"
#include "SseArchive.h"
#include "SseMessage.h"
#include "SseUtil.h"
#include "MsgSender.h"
#include <iostream>
#include <sstream>

using namespace std;

// ---- IdNumber ------------

IdNumber::IdNumber()
{
}

IdNumber::~IdNumber()
{
}

// ---- IdNumberCount ------------

IdNumberCount::IdNumberCount(ActivityId_t& nextId):
   nextId_(nextId)
{
}

IdNumberCount::~IdNumberCount()
{
}

// returns next id number
ActivityId_t IdNumberCount::nextId()
{
   return nextId_++;
}

// ---- IdNumberAsciiFile ------------

// use ascii file in archive directory to save id number
// persistently

IdNumberAsciiFile::IdNumberAsciiFile(const string & filename)
{
   idFilename_ = SseArchive::getArchiveSystemDir() + "/" + filename;

   // Create the file if it doesn't exist, and put 
   // in the initial value, in ascii.  This will be incremented
   // before first use.

   if (! SseUtil::fileIsReadable(idFilename_))
   {
      // try to create the file and put the initial value in it

      const ActivityId_t initialValue(0);
      write(initialValue);
   }

}

IdNumberAsciiFile::~IdNumberAsciiFile()
{
}

// returns next id number
ActivityId_t IdNumberAsciiFile::nextId()
{
   // tbd exception handling
   ActivityId_t id = read();

   id++;

   write(id);

   return id;
}

// store the newId in the file
void IdNumberAsciiFile::write(ActivityId_t newId)
{
   // overwrite file with new activity Id

   ofstream strm;
   strm.open(idFilename_.c_str(), (ios::out | ios::trunc));
   if (strm.is_open())
   {
      // store id value
      strm << newId << endl;
      strm.close();
   } 
   else 
   {
      stringstream strm;
      strm << "IdNumberAsciiFile.write(): Failed to write " 
	   << idFilename_ << endl;
      SseMessage::log(MsgSender,
                      newId, SSE_MSG_FILE_ERROR,
                      SEVERITY_ERROR, strm.str(),
                      __FILE__, __LINE__);
	
      throw IdCreationFailed("Failed to write activity id to file\n");
   }

}

// get (but don't modify) the id number in the file
ActivityId_t IdNumberAsciiFile::read() const
{
   ActivityId_t id(0);

   ifstream strm;
   strm.open(idFilename_.c_str(), (ios::in));
   if (strm.is_open())
   {
      // read Id
      strm >> id;
      if (!strm)
      {
	 stringstream strm;
	 strm <<  "IdNumberAsciiFile.read() Failed to read id from "
	      << idFilename_ << endl;
	 SseMessage::log(MsgSender,
                         id, SSE_MSG_FILE_ERROR,
                         SEVERITY_ERROR, strm.str(),
                         __FILE__, __LINE__);
         
	 throw IdCreationFailed("Failed to read activity id from file\n");
      }

   }
   else 
   {
      stringstream strm;
      strm << "IdNumberAsciiFile(): Failed to open " 
	   << idFilename_ << endl;
      SseMessage::log(MsgSender,
                      id, SSE_MSG_FILE_ERROR,
                      SEVERITY_ERROR, strm.str(),
                      __FILE__, __LINE__);
	
      throw IdCreationFailed("File open failed\n");
   }

   return id;

}
