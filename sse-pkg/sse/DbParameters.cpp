/*******************************************************************************

 File:    DbParameters.cpp
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
 * DBParameters.cpp - declaration of functions defined in DBParameters.cpp
 * PURPOSE:  
 *****************************************************************/

#ifndef DBPARAMETERS_CPP
#define DBPARAMETERS_CPP

#include "DbParameters.h"
#include "ChoiceParameter.h" 
#include "AnyValueParameter.h" 
#include "RangeParameter.h"
#include "Assert.h"
#include "SseException.h"
 
using namespace std;

static const char *ChoiceOn = "on";
static const char *ChoiceOff = "off";

struct DbParametersInternal
{
public:
    
   // methods:
   DbParametersInternal();  
   // default copy constructor and assignment operator are safe
    
   AnyValueParameter<string> host;
   AnyValueParameter<string> name;
   AnyValueParameter<string> user;
   AnyValueParameter<string> passwd;
   RangeParameter<int32_t> port;
   ChoiceParameter<string> useDb;
    
   bool isConnected();
   void connect();
   void disconnect();
   void reset();
   void setUtcTime();
    
   MYSQL *db;   
   bool connected;    
};

DbParametersInternal::DbParametersInternal():
   host("host", "", "database server host", "localhost"),
   name("name", "", "database name", "sonatadb"),
   user("user", "", "database user name", ""),
   passwd("passwd", "", "database password", ""),
   port("port", "number", "database server port", 0, 0, 65535),
   useDb("usedb", "", "use database", ChoiceOff),
   db(NULL),
   connected(false)
{}

DbParameters::DbParameters(string command) : 
   SeekerParameterGroup(command),
   internal_(new DbParametersInternal())
{
   internal_->useDb.addChoice(ChoiceOn);
   internal_->useDb.addChoice(ChoiceOff);

   addParameters();
}

DbParameters::DbParameters(const DbParameters& rhs):
   SeekerParameterGroup(rhs.getCommand()),
   internal_(new DbParametersInternal(*rhs.internal_))
{
   // copy is not connected
   internal_->reset();

   setSite(rhs.getSite());
   addParameters();
}

DbParameters& DbParameters::operator=(const DbParameters& rhs)
{
   if (this == &rhs)
   {
      return *this;
   }
  
   setCommand(rhs.getCommand());
   eraseParamList();
   setSite(rhs.getSite());
   delete internal_;
   internal_ = new DbParametersInternal(*rhs.internal_);

   addParameters();

   // copy is not connected
   internal_->reset();

   return *this;
}

  
DbParameters::~DbParameters()
{
   if (internal_->isConnected())
   {
      internal_->disconnect();
   }

   delete internal_;
}


void DbParameters::addParameters()
{
   addParam(internal_->host);
   addParam(internal_->name);
   addParam(internal_->user);
   addParam(internal_->passwd);
   addParam(internal_->port);
   addParam(internal_->useDb);

   sort();
}



bool DbParameters::useDb() const
{
   if (internal_->useDb.getCurrent() == ChoiceOn)
   {
      return true;
   }
   else if (internal_->useDb.getCurrent() == ChoiceOff)
   {
      return false;
   }

   AssertMsg(0, "Incorrect value for useDb");
}


MYSQL* DbParameters::getDb()
{
   if (!internal_->isConnected())
   {
      internal_->connect();
      internal_->setUtcTime();
   }
   return(internal_->db);
}

/*
  Set the per-session time zone to UTC.
 */
void DbParametersInternal::setUtcTime()
{
   const string setUtcCmd("set time_zone = '+00:00'");
   if (mysql_query(db, setUtcCmd.c_str()) != 0)
   {
      stringstream errStrm;
      
      errStrm << "failed to set mysql session to UTC: "
              << mysql_error(db) << endl;

      throw SseException(errStrm.str(),
                         __FILE__, __LINE__, 
                         SSE_MSG_DBERR, SEVERITY_ERROR);
   }
}

bool DbParametersInternal::isConnected() 
{
   return connected;
}

void DbParametersInternal::connect()
{
   stringstream strm;

   db = mysql_init(NULL);

   if (!db)
   {
      throw SseException(
         "::connect() mysql_init() failed. Cannot allocate new handler.",
         __FILE__,  __LINE__, SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   if (!mysql_real_connect(
      db, 
      host.getCurrent().c_str(), 
      user.getCurrent().c_str(), 
      passwd.getCurrent().c_str(),
      name.getCurrent().c_str(),
      port.getCurrent(), 
      NULL,  // socket name
      0))    // flags
   {
      strm << "::connect() MySQL error: " << mysql_error(db) << endl;
   
      throw SseException(   strm.str(),
                            __FILE__,  __LINE__, SSE_MSG_DBERR, SEVERITY_ERROR);
   }

   connected = true;

}


void DbParametersInternal::disconnect()
{
   mysql_close(db);

   reset();
}

void DbParametersInternal::reset()
{
   connected = false;
   db = NULL;
}

const string DbParameters::getDbHost() const
{
   return internal_->host.getCurrent();
}

const string DbParameters::getDbName() const
{
   return internal_->name.getCurrent();
}



#endif /* DBPARAMETERS_CPP */