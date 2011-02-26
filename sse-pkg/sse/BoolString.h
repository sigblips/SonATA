/*******************************************************************************

 File:    BoolString.h
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


#include <string>
using std::string;

#ifndef BoolString_H
#define BoolString_H

// Converts back and forth from a bool to a string representation
// (eg, on/off, or yes/no).
// stringToBool method will throw an exception on error.

class BoolString
{
 public:

    BoolString(const string &trueString, const string &falseString);
    virtual ~BoolString();

    bool stringToBool(const string &boolString);   // throws on error
    const string & boolToString(bool value);
    const string & getUsageString() { return usageString_; };

private:    
    string trueString_;
    string falseString_;
    string usageString_;

    // Disable copy construction & assignment.
    // Don't define these.
    //BoolString(const BoolString& rhs);
    //BoolString& operator=(const BoolString& rhs);


};

#endif // BoolString_H