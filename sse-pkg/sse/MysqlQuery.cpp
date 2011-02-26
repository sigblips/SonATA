/*******************************************************************************

 File:    MysqlQuery.cpp
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


#include "MysqlQuery.h" 
#include "SseException.h"
#include "SseUtil.h"
#include <iostream>
#include <sstream>

using namespace std;

// Verify that the db column char pointer in the row is valid (non null).
// If it's ok return it, else throw an exception.
static char * validDbCharPtr(MYSQL_ROW row, int colIndex,
			     const string & fileName, int lineNum)
{
   if (! row[colIndex])
   {
      stringstream strm;
      strm << "Found null database column pointer in column index: " 
	   << colIndex << endl;

      throw SseException(strm.str(),
			 fileName, lineNum,
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   return row[colIndex];
}


MysqlQuery::MysqlQuery(MYSQL *conn)
   :
   conn_(conn),
   resultSet_(NULL)
{
}

MysqlQuery::~MysqlQuery()
{
   freeResultSet();
}

void MysqlQuery::execute(const string &query, int numExpectedCols,
                         const string &fileName, int lineNum)
{
   freeResultSet();    // in case this method is called multiple times

   const string methodName("MysqlQuery::execute: ");

   if (mysql_query(conn_, query.c_str()) != 0)
   {
      stringstream errorMsg;

      errorMsg << methodName << "mysql_query() failed: " 
	       << "'" << query << "'" << endl
	       << "Mysql error: " << mysql_error(conn_) << endl;
      throw SseException(errorMsg.str(),
			 fileName,  lineNum, 
                         SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   resultSet_ = mysql_store_result(conn_);
   if (resultSet_ == NULL)
   {
      // there was no result set, see if one was expected
      if (mysql_field_count(conn_) > 0)
      {
	 throw SseException(
            methodName + "mysql_store_result: Empty result set.\n",
            fileName,  lineNum, 
            SSE_MSG_DBERR, SEVERITY_ERROR);
      }
   }
   else
   {
      unsigned int numReceivedCols = mysql_num_fields(resultSet_);
      if (numExpectedCols != static_cast<int>(numReceivedCols))
      {
         stringstream strm;
         strm << methodName << " expected " 
              << numExpectedCols << " cols "
              << "from db query but only received " 
              << numReceivedCols << endl;
         
         throw SseException(strm.str(), fileName, lineNum,
                            SSE_MSG_DBERR, SEVERITY_ERROR);
      }
 
   }

}

int MysqlQuery::getInt(MYSQL_ROW row, int colIndex, 
                       const string & fileName,
		       int lineNum)
{
   // Let this throw if it needs to 
   const char * charPtr = validDbCharPtr(row, colIndex,
                                         fileName, lineNum);

   try {
      return SseUtil::strToInt(charPtr);
   }
   catch(SseException &except)
   {
      // return line number information for invalid conversion
      throw SseException(except.descrip(), fileName, lineNum);
   }

}

double MysqlQuery::getDouble(MYSQL_ROW row, int colIndex, 
                             const string & fileName,
                             int lineNum)
{
   // Let this throw if it needs to 
   const char * charPtr = validDbCharPtr(row, colIndex, fileName, lineNum);

   try {
      return SseUtil::strToDouble(charPtr);
   }
   catch(SseException &except)
   {
      // return line number information for invalid conversion
      throw SseException(except.descrip(), fileName, lineNum);
   }
}

string MysqlQuery::getString(MYSQL_ROW row, int colIndex, 
                             const string & fileName,
                             int lineNum)
{
   return validDbCharPtr(row, colIndex, fileName, lineNum);
}

// Return a valid string, even if the pointer is null
string MysqlQuery::getStringNoThrow(MYSQL_ROW row, int colIndex, 
                                    const string & fileName,
                                    int lineNum)
{
   if (! row[colIndex])
   {
      return "null";
   }

   return row[colIndex];
}

MYSQL_RES * MysqlQuery::getResultSet()
{
   return resultSet_;
}

void MysqlQuery::freeResultSet()
{
   if (resultSet_ != NULL)
   {
      mysql_free_result(resultSet_);
   }

   resultSet_ = NULL;
}