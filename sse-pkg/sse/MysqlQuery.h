/*******************************************************************************

 File:    MysqlQuery.h
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


#ifndef MysqlQuery_H
#define MysqlQuery_H

#include <mysql.h>
#include <string>

using std::string;

class MysqlQuery
{
 public:
    MysqlQuery(MYSQL *conn);
    virtual ~MysqlQuery();

    virtual void execute(const string &query, int numExpectedCols, 
                         const string & fileName = "",
			 int lineNum = -1);

    virtual MYSQL_RES * getResultSet();

    static int getInt(MYSQL_ROW row, int colIndex, 
                      const string & fileName = "", int lineNum = -1);

    static double getDouble(MYSQL_ROW row, int colIndex,
                            const string & fileName = "",
                            int lineNum = -1);

    static string getString(MYSQL_ROW row, int colIndex,
                            const string & fileName = "",
                            int lineNum = -1);

    static string getStringNoThrow(MYSQL_ROW row, int colIndex, 
                                   const string & fileName = "",
                                   int lineNum = -1);

 private:

    void freeResultSet();

    MYSQL *conn_;
    MYSQL_RES *resultSet_;

    // Disable copy construction & assignment.
    // Don't define these.
    MysqlQuery(const MysqlQuery& rhs);
    MysqlQuery& operator=(const MysqlQuery& rhs);

};

#endif // MysqlQuery_H