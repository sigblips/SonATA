/*******************************************************************************

 File:    BoolString.cpp
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



#include "BoolString.h" 

BoolString::~BoolString()
{
}

BoolString::BoolString(const string &trueString, const string &falseString)
    : trueString_(trueString),
      falseString_(falseString)
{
    usageString_ = "value must be " + trueString_ + " or " + falseString_;
}

bool BoolString::stringToBool(const string &boolString)
{
    if (boolString == trueString_)
	return true;
    else if (boolString == falseString_)
	return false;
    else
	throw 1;   // TBD Fix me 
}

const string & BoolString::boolToString(bool value)
{
    if (value)
	return trueString_;
    else
	return falseString_;
}
