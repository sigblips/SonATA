/*******************************************************************************

 File:    RecordInDatabase.cpp
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


#include "RecordInDatabase.h"
#include "MysqlResultSet.h"
#include "SseException.h"
#include "SseUtil.h"
#include "Assert.h"

using namespace std;
static const string IdColName("id");

RecordInDatabase::RecordInDatabase(const string & tableName):
  tableName_(tableName)
{
}


RecordInDatabase::~RecordInDatabase()
{
}

// Returns id of the row that contains the
// recorded information.

unsigned int RecordInDatabase::record(MYSQL* db)
{
   // To avoid duplication, only store the
   // data if there's not already an identical copy
   // in the database

   unsigned int id = find(db);
   if (id == 0)
   {
      id = store(db);
   }

   return(id);
}

unsigned int RecordInDatabase::find(MYSQL* db)
{
   if (!db)
   {
      stringstream errorStrm;
      errorStrm << "::find() in table: " << tableName_
		<< " MySQL error: NULL db\n";

      throw SseException(errorStrm.str(),
			 __FILE__, __LINE__, 
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   string sqlstmt(findStmt(db));
   if (mysql_query(db, sqlstmt.c_str()) != 0)
   {
      stringstream errorStrm;
      errorStrm << "::find(): MySQL error: in table: "
		<< tableName_ << " "
		<< mysql_error(db) << endl;

      throw SseException(errorStrm.str(),
			 __FILE__, __LINE__, 
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   MysqlResultSet resultSet(mysql_store_result(db));
   if (resultSet.get() == 0)
   {
      return(0);
   }

   if (mysql_num_rows(resultSet.get()) > 1)
   {
      stringstream errorStrm;
      errorStrm << "::find() error in table: "
		<< tableName_
		<< " found multiple rows."
		<< " with SQL statement "
		<< sqlstmt << endl
		<< "found " << mysql_num_rows(resultSet.get()) 
		<< " rows" << endl;

      throw SseException(errorStrm.str(),
			 __FILE__, __LINE__, 
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   MYSQL_ROW row = mysql_fetch_row(resultSet.get());
   int id(0);
   if (row)
   {
      // trap null pointer for id
      const int idColIndex(0);
      if (row[idColIndex] == NULL)
      {
	 stringstream errorStrm;
	 errorStrm << "::find() error in table: "
		   << tableName_
		   << " found null pointer looking for"
		   << " column: " << IdColName
		   << " with SQL statement "
		   << sqlstmt << endl;
	 
	 throw SseException(errorStrm.str(),
			    __FILE__, __LINE__, 
			    SSE_MSG_DBERR, SEVERITY_ERROR);
	 
      }

      id = SseUtil::strToInt(row[idColIndex]); 
   }

   return(id);
  
}

unsigned int RecordInDatabase::store(MYSQL* db)
{
   if (!db)
   {
      stringstream errorStrm;
      errorStrm << "::store() in table: " << tableName_
		<< " MySQL error: NULL db\n";

      throw SseException(errorStrm.str(),
			 __FILE__, __LINE__, 
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   string sqlstmt(storeStmt(db));

   if (mysql_query(db, sqlstmt.c_str()) != 0)
   {
      stringstream errorStrm;
      errorStrm << "::store() MySQL error in table: "
		<< tableName_ << " " << mysql_error(db) 
		<< " with SQL statement " << sqlstmt
		<< endl;
      throw SseException(errorStrm.str(),
			 __FILE__, __LINE__, 
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   return(mysql_insert_id(db));
}

string RecordInDatabase::findStmt(MYSQL* db)
{
   stringstream sqlstmt;

   sqlstmt << "SELECT " << IdColName << " "
	   << "FROM " << tableName_ << " WHERE ";

   string conjunction("AND");
   sqlstmt << stmt(db, conjunction);
   return sqlstmt.str();
}

string RecordInDatabase::storeStmt(MYSQL* db)
{
   stringstream sqlstmt;

   sqlstmt << "INSERT INTO " << tableName_  << " SET ";

   string conjunction(",");
   sqlstmt << stmt(db, conjunction);
   return sqlstmt.str();
}