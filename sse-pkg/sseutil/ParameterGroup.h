/*******************************************************************************

 File:    ParameterGroup.h
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


#ifndef ParameterGroup_H
#define ParameterGroup_H

#include "Parameter.h"
#include <list>
#include <iomanip>

using std::string;
using std::ostream;
using std::list;

class ParameterGroup
{

 public:

  ParameterGroup(const string &command);
  // default copy constructor and assignment operator are safe
  // except for paramlist
  virtual ~ParameterGroup();

  virtual const char * set(const string& paramName, const string &value, 
			   const string& valueType = "");
  virtual const char * show(const string& paramName, const string& valueType = "");

  virtual bool isValid() const;
  virtual const char * setDefault();
  virtual const char * save(const string& filename);
  virtual void help(ostream& os = cout) const;
  virtual string getCommand() const;

  friend ostream& operator << (ostream &strm, 
			       const class ParameterGroup& paramGroup);

 protected:

  virtual void indent(ostream &os, int level) const;
  virtual void addParam(Parameter &param);
  virtual void setCommand(string command);
  virtual void eraseParamList();
  virtual void sort();
  virtual const list<Parameter*>& getList() const;
  bool convertValueTypeToCategory(const string& valueType,
				  Parameter::Category & category,
				  string & errorText);

 private:

  string cmdName_;
  string returnValue_;
  list<Parameter*> paramList_;

};

#endif // ParameterGroup_H