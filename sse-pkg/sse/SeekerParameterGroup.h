/*******************************************************************************

 File:    SeekerParameterGroup.h
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


#ifndef SeekerParameterGroup_H
#define SeekerParameterGroup_H

#include "ParameterGroup.h"
#include "sseInterface.h"
#include "ChoiceParameter.h"
#include <machine-dependent.h>
#include <mysql.h>
#include <vector>
#include <string>

class Site;


class SeekerParameterGroup : public ParameterGroup
{
 public:
   SeekerParameterGroup(const string &command);
   SeekerParameterGroup(const string &command,
                        const string &dbTableName,
                        const string &idColNameInActsTable);

   // default copy constructor and assignment operator are safe
   // though site_ will be shared between objects

   virtual ~SeekerParameterGroup();

   virtual void setSite(Site *site);
   virtual Site *getSite() const;

   virtual string getDbTableName() const;
   virtual string getIdColNameInActsTable() const;

   virtual void help(ostream& os = cout) const;

   virtual unsigned int record(MYSQL* db);

   virtual void restore(MYSQL* db,
                        uint32_t activityId);

 protected:

   virtual void addImmedCmdHelp(const string &cmdHelp);
   virtual void printAllImmedCmdHelp(ostream& os, int indentLevel) const;
    
   virtual void restoreGroup(MYSQL* db,
                             const string &tableName,
                             const string &tableIdName,
                             uint32_t activityId) const;
    
   static Polarization convertPolarization(ChoiceParameter<string> &param);
    
   static bool convertOnOffToBool(ChoiceParameter<string> &param);
       
 private:

   string dbTableName_;
   string idColNameInActsTable_;
   Site *site_;
   mutable std::vector<string> immedCmdHelp_;
};

#endif // SeekerParameterGroup_H