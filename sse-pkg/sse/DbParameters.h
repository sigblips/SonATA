/*******************************************************************************

 File:    DbParameters.h
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

#ifndef DBPARAMETERS_H
#define DBPARAMETERS_H

#include "SeekerParameterGroup.h"
#include <mysql.h>

using std::string;
using std::endl;

class DbParametersInternal;

class DbParameters : public SeekerParameterGroup
{
 public:

  DbParameters(string command);
  DbParameters(const DbParameters& rhs);
  DbParameters& operator=(const DbParameters& rhs);
 ~DbParameters();

  // database access methods
  bool useDb() const;  
  MYSQL *getDb();

  const string getDbHost() const;
  const string getDbName() const;

 protected:

  void addParameters();

 private:

  DbParametersInternal* internal_;

};



#endif /* DBPARAMETERS_H */