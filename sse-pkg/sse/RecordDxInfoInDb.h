/*******************************************************************************

 File:    RecordDxInfoInDb.h
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


#ifndef RecordDxInfoInDb_H
#define RecordDxInfoInDb_H

#include "RecordInDatabase.h"
#include <string>
#include <mysql.h>

class DxScienceDataRequest;

class RecordDxScienceDataRequest : public RecordInDatabase 
{
public:
   RecordDxScienceDataRequest(const string & tableName,
			       const DxScienceDataRequest& dxScienceDataRequest);
protected:
   virtual string stmt(MYSQL* db, const string & conjunction);
   const DxScienceDataRequest& scienceDataRequest_;
};

class DxActivityParameters;

class RecordDxActivityParameters : public RecordInDatabase 
{
public:
   RecordDxActivityParameters(const string & tableName,
			       const DxActivityParameters& dxActParam);
protected:
   virtual string stmt(MYSQL* db, const string & conjunction);
   const DxActivityParameters& dxActParam_;
};

class DxIntrinsics;

class RecordDxIntrinsics : public RecordInDatabase
{
public:
   RecordDxIntrinsics(const string & tableName,
		       const struct DxIntrinsics& dxIntrinsics);
protected:
   virtual string stmt(MYSQL* db, const string & conjunction);
   const struct DxIntrinsics& dxIntrinsics_;
};


class BaselineLimits;

class RecordBaselineLimits : public RecordInDatabase 
{
public:
   RecordBaselineLimits(const string & tableName,
			const BaselineLimits& baselineLimits);
protected:
   virtual string stmt(MYSQL* db, const string & conjunction);
   //fconst BaselineLimits & baselineLimits_;
   const BaselineLimits & baselineLimits_;
};




#endif // RecordDxInfoInDb_H