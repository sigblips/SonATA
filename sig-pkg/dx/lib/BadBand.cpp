/*******************************************************************************

 File:    BadBand.cpp
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

//
// Bad band structures
//
#include <iostream>
#include "BadBand.h"

namespace dx {

//
// print operators
//
#ifdef notdef
ostream& operator << (ostream& s, const DaddPath& daddPath)
{
	s << "DaddPath: " << endl;
	s << "----" << endl;
	s << "bin: " << daddPath.bin << endl;
	s << "drift: " << daddPath.drift << endl;
	s << "power: " << daddPath.power << endl;
	return (s);
}
#endif
ostream& operator << (ostream& s, const BadBand& band)
{
	s << "BadBand: " << endl;
	s << "----" << endl;
	s << "bin: " << band.bin << endl;
	s << "width: " << band.width << endl;
	s << "pol" << band.pol << endl;
	return (s);
}

ostream& operator << (ostream& s, const CwBadBand& cwBand)
{
	s << "CwBadBand: " << endl;
	s << "----" << endl;
	s << cwBand.band << endl;
	s << "paths: " << cwBand.paths << endl;
	s << "maxPath: " << cwBand.maxPath << endl;
	return (s);
}

ostream& operator << (ostream& s, const PulseBadBand& pulseBand)
{
	s << "PulseBadBand:" << endl;
	s << "----" << endl;
	s << pulseBand.band << endl;
	s << pulseBand.res << endl;
	s << "pulses: " << pulseBand.pulses << endl;
	s << "tooManyTriplets: " << pulseBand.tooManyTriplets << endl;
	return (s);
}

}