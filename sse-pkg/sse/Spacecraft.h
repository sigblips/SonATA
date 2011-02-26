/*******************************************************************************

 File:    Spacecraft.h
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


#ifndef Spacecraft_H
#define Spacecraft_H

#include <string>
using std::string;

class Spacecraft
{
 public:

    static void calculatePosition(
       const string &ephemDir,
       const string &targetEphemFilename,
       const string &earthEphemFilename,
       time_t obsDate,
       double *ra2000Rads,
       double *dec2000Rads,
       double *rangeKm,
       double *rangeRateKmSec);

 private:

    Spacecraft();
    ~Spacecraft();

    // Disable copy construction & assignment.
    // Don't define these.
    Spacecraft(const Spacecraft& rhs);
    Spacecraft& operator=(const Spacecraft& rhs);

};

#endif // Spacecraft_H