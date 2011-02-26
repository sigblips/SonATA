/*******************************************************************************

 File:    IdDb.cpp
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

/*++ IdDb.cpp - description of file contents
 * PURPOSE:  
 --*/
#include "IdDb.h"
#include "mysql.h"

IdNumberDb::IdNumberDb(const DbParameters& dbparam):
   dbparam_(dbparam)
{
   try {
      dbparam_.getDb();
   }
   catch (SseException & exception){
      throw IdCreationFailed("connection failed");
   }
}

IdNumberDb::~IdNumberDb()
{
}

ActivityId_t IdNumberDb::nextId()
{
   string query("INSERT INTO Activities () values();");

   if (mysql_query(dbparam_.getDb(), query.c_str()) != 0)
   {
      throw IdCreationFailed("db access failed");
   }

   ActivityId_t nextIdValue = mysql_insert_id(dbparam_.getDb());

   return nextIdValue;
}
