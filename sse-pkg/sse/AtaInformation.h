/*******************************************************************************

 File:    AtaInformation.h
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


#ifndef AtaInformation_H
#define AtaInformation_H

class AtaInformation
{
 public:

    static const double AtaMinSkyFreqMhz = 500.0;
    static const double AtaMaxSkyFreqMhz = 11200.0;
    static const double AtaPrimaryFovAt1GhzDeg = 3.5;   
    static const double Ata32BeamsizeAt1GhzArcSec = 408;  // approx.
    static const double Ata42BeamsizeAt1GhzArcSec = 348;  // ~ 348" x 168", at 40 deg dec, per ATA NSF Proposal June 2005
    static const double Ata350BeamsizeAt1GhzArcSec = 110;  
    static const double ArcSecPerDeg = 3600;
//  static const double Ata1BeamsizeAt1GhzArcSec = AtaPrimaryFovAt1GhzDeg * ArcSecPerDeg;
    static const double Ata1BeamsizeAt1GhzArcSec = 12600;
    static const double AtaDefaultSkyFreqMhz = 1420.0;
    static const double AtaMinDecLimitDeg = -34.17;  // 15 deg El at Hat Creek

    static const double AtaLongWestDeg = 121.471802777778;
    static const double AtaLatNorthDeg = 40.8173611111;
    static const double AtaHorizonDeg = 16.5;
    static const double AtaMinHorizonDeg = 15;

    static double ataBeamsizeRadians(double freqMhz, 
				    double beamsizeAtOneGhzArcSec);

 private:

    // don't allow this class to be instantiated
    AtaInformation();
    virtual ~AtaInformation();

    // Disable copy construction & assignment.
    // Don't define these.
    AtaInformation(const AtaInformation& rhs);
    AtaInformation& operator=(const AtaInformation& rhs);
};

#endif // AtaInformation_H