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
 * Print routines to print structures.
 */
#include "Dfb.h"

using std::endl;

namespace dfb {

ostream& operator << (ostream& s, const DfbInfo& dfbInfo)
{
	s << "DfbInfo: rawFftLen: " << dfbInfo.rawFftLen << ", fftLen: ";
	s << dfbInfo.fftLen << ", overlap: " << dfbInfo.overlap << endl;
	s << " foldings: " << dfbInfo.foldings << ", samplesPerChan: ";
	s << dfbInfo.samplesPerChan << ", dataLen: " << dfbInfo.dataLen << endl;
	s << " nRawCoeff: " << dfbInfo.nRawCoeff << ", nCoeff: ";
	s << dfbInfo.nCoeff << endl;
	return (s);
}

ostream& operator << (ostream& s, const DfbTiming& timing)
{
	s << "DfbTiming: iterations: " << timing.iterations << ", total dfbs: ";
	s << timing.dfbs << ", iterate: " << timing.iterate << ", wola: " << endl;
	s << " fft: " << timing.fft << ", store: " << timing.store;
	s << " dfb: " << timing.dfb << endl;
	return (s);
}

}