/*******************************************************************************

 File:    Interpolate.h
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
 * Interpolate.h - declaration of functions defined in Interpolate.h
 *
 * PURPOSE:  
 *****************************************************************/

#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <map>
#include <functional>
#include <iosfwd>

typedef std::map<double,double,std::less<double> > InterpLinearMap;

class InterpolateLinear
{
public:
  InterpolateLinear();
  virtual ~InterpolateLinear();
  virtual void addValues(double x, double y);
  virtual bool inter(double x, double &y) const;
  virtual double maxY() const;	// maximum Y value
  virtual double minY() const;	// minimum Y value

  friend std::ostream& operator << (std::ostream &strm, 
     const InterpolateLinear &interpLinear);

private:
  InterpLinearMap table_;
  double maxY_;
  double minY_;
};


#endif /* INTERPOLATE_H */