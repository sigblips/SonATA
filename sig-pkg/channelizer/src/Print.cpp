/*******************************************************************************

 File:    Print.cpp
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

/*
 * Print operators
 */
#include "Beam.h"

using std::endl;

namespace chan {

ostream& operator << (ostream& s, const BeamSpec& beamSpec)
{
	s << "BeamSpec: src: " << beamSpec.src << ", pol: " << beamSpec.pol;
	s << ", freq: " << beamSpec.freq << ", bandwidth: " << beamSpec.bandwidth;
	s << ". oversampling: " << beamSpec.oversampling << endl;
	return (s);
}

ostream& operator << (ostream& s, const PacketInfo& packet)
{
	s << "PacketInfo: sample: " << packet.sample << ", totalChannels: ";
	s << packet.totalChannels << "usable channels: " << packet.usableChannels;
	s << ", bandwidth: " << packet.bandwidth << endl;
//	s << packet.hdr;
	return (s);
}

ostream& operator << (ostream& s, const DfbTiming& dfb)
{
	s << "DfbTiming: dfbs: " << dfb.dfbs << ", load: " << dfb.load;
	s << ", dfb: " << dfb.dfb << ", list: " << dfb.list;
	s << ", total: " << dfb.total << endl;
	return (s);
}

ostream& operator << (ostream& s, const BeamTiming& timing)
{
	s << "BeamTiming: packets: " << timing.packets << ", maxFlush: ";
	s << timing.maxFlush << ", flush: " << timing.store << ", set: ";
	s << timing.set << ", total: " << timing.total << endl;
	s << timing.dfb;
	return (s);
}

}