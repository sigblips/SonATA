/*******************************************************************************

 File:    Parameter.h
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


#ifndef Parameter_H
#define Parameter_H

#include <string>
#include <iostream>

using std::string;
using std::ostream;
using std::cout;

class Parameter
{
 public:
  enum Category {DEFAULT, MIN, MAX, CURRENT};
  typedef Category ParamCategory;

  // default copy constructor and assignment operator are safe

  virtual ~Parameter() {}; 

  virtual string getName() const = 0;
  virtual string getUnits() const = 0;
  virtual string getHelp() const = 0;

  virtual bool setDefault() = 0;
  virtual bool isValid() const   = 0;

  // helper functions for swig interface
  virtual bool convertFromString(const string &value, ParamCategory category,
				 string & errorText) = 0;
  virtual string convertToString(ParamCategory category) const = 0;

  virtual void writeValues(const string &command, ostream &os) const = 0;
  virtual void print(ostream& os = cout) const = 0;  // to assist the list

};

#endif // Parameter_H