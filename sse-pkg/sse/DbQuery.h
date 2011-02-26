/*******************************************************************************

 File:    DbQuery.h
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


#ifndef DbQuery_H
#define DbQuery_H

// Prepare and submit a database query,
// and process the results.  

#include <mysql.h>
#include <string>

using std::string;

class DbQuery
{
 public:
   DbQuery(MYSQL *db, int actId, int verboseLevel);
   virtual ~DbQuery();

   virtual void execute();

protected:

   virtual string prepareQuery() = 0;
   virtual void submitQuery(const string & queryStmt);
   virtual void processQueryResults() = 0;

   virtual void setNumberOfRequestedCols(int nRequestedCols);
   virtual void setSubclassName(const string & subclassName);
   virtual void setContext(const string & context);

   virtual string getSubclassName();
   virtual MYSQL_RES * getResultSet();
   virtual int getVerboseLevel();

private:

   string subclassName_;
   string context_;
   MYSQL *db_;
   int actId_;
   int verboseLevel_;
   MYSQL_RES *resultSet_;
   unsigned int nRequestedCols_;

   // Disable copy construction & assignment.
   // Don't define these.
   DbQuery(const DbQuery& rhs);
   DbQuery& operator=(const DbQuery& rhs);

};

#endif // DbQuery_H