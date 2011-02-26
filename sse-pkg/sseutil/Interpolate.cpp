/*******************************************************************************

 File:    Interpolate.cpp
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


#include <Interpolate.h>
#include <float.h>
#include <iostream>

using namespace std;

InterpolateLinear::InterpolateLinear():
  maxY_(DBL_MIN),
  minY_(DBL_MAX)
{
}

InterpolateLinear::~InterpolateLinear()
{
}

void InterpolateLinear::addValues(double x, double y)
{
  table_[x] = y;
  maxY_ = max(y, maxY_);
  minY_ = min(y, minY_);
};
bool InterpolateLinear::inter(double x, double &y) const
{
  InterpLinearMap::const_iterator lower =
    table_.lower_bound(x);
  if ( lower == table_.end()) {
    return false;
  } else if (lower == table_.begin() && x < (*lower).first ){
    return false;
  } else if ((*lower).first == x) {
    y = (*lower).second;
    return true;
  } else {
    map<double,double,less<double> >::const_iterator before_lower = lower;
    before_lower--;
    double inter_value = (*before_lower).second +
      ((x - (*before_lower).first)/((*lower).first - (*before_lower).first)) *
      ((*lower).second - (*before_lower).second);
    y = inter_value;
    return true;
  }
  
}

double InterpolateLinear::maxY() const
{
  return maxY_;
}

double InterpolateLinear::minY() const
{
  return minY_;
}

ostream& operator << (ostream &strm, 
                      const InterpolateLinear &interpLinear)
{
   InterpLinearMap::const_iterator it;

   for (it = interpLinear.table_.begin();
        it != interpLinear.table_.end(); ++it)
   {
      strm << (*it).first << " " << (*it).second << endl;
   }

   return strm;
}