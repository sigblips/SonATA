/*******************************************************************************

 File:    RecordInDatabase.h
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
 * PURPOSE:  
 *****************************************************************/

#ifndef RecordInDatabase_H
#define RecordInDatabase_H
#include <sstream>
#include "mysql.h"

using std::string;

// Record data in in the database.
// If the data is already
// found in the database, then the id number
// is returned (no duplication). If it isn't found
// then the data is recorded and the new id
// is returned.
// The id column name is assumed to be "id".

class RecordInDatabase
{
public:
  RecordInDatabase(const string & tableName);
  virtual ~RecordInDatabase();

  virtual unsigned int record(MYSQL* db);

protected:

  virtual string stmt(MYSQL* db, const string & conjunction) = 0;

private:

  virtual unsigned int find(MYSQL* db);
  virtual unsigned int store(MYSQL* db);
  virtual string findStmt(MYSQL* db);
  virtual string storeStmt(MYSQL* db);

  string tableName_;

};


#endif