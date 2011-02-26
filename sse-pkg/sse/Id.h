/*******************************************************************************

 File:    Id.h
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

/*****************************************************************
 * Id.h - supplies id numbers for classes
 * PURPOSE:
 * supplies id numbers for classes
 *****************************************************************/

#ifndef ID_H
#define ID_H

#include "SseException.h"
#include "ActivityId.h"
#include <string>

using std::string;

class IdCreationFailed : public SseException
{
public:
  IdCreationFailed(const string& what_arg):
    SseException(what_arg)
  {
  }
  IdCreationFailed(const string& what_arg, const string& fileName, int line):
    SseException(what_arg, fileName, line)
  {
  }
};

// Base class for creating id numbers
class IdNumber
{
public:
  virtual ActivityId_t nextId() = 0;
  virtual ~IdNumber();

protected:
  // prevent instantiation of this class
  IdNumber();
};
  

// Count from initial value, not persistent
class IdNumberCount : public IdNumber
{
public:
  IdNumberCount(ActivityId_t& nextId);
  virtual ~IdNumberCount();

  virtual ActivityId_t nextId();
protected:
  ActivityId_t& nextId_;
};


// Id implementation using persistent file storage
// from one invocation of program to another
class IdNumberAsciiFile : public IdNumber
{
public:
  IdNumberAsciiFile(const string & filename);
  virtual ~IdNumberAsciiFile();
  virtual ActivityId_t nextId();

protected:
  virtual ActivityId_t read() const;
  virtual void write(ActivityId_t newId);

  string idFilename_;

};


#endif /* ID_H */