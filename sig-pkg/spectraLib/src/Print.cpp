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
#include "Spectra.h"

using std::endl;

namespace spectra {

ostream& operator << (ostream& s, const ResData& resData)
{
	s << "ResData: res: " << resData.res  << ", fftLen: " << resData.fftLen;
	s << ", overlap: " << resData.overlap << endl;
	return (s);
}

ostream& operator << (ostream& s, const ResInfo& resInfo)
{
	s << "ResInfo: res: " << resInfo.res << ", specLen: " << resInfo.specLen;
	s << ", spectra: " << resInfo.nSpectra << endl;
	return (s);
}

ostream& operator << (ostream& s, const SpectraTiming::r& res)
{
	s << "resolution timing: resComputes: " << res.resComputes;
	s << ", fft: " << res.fft << ", swap: " << res.swap;
	s << ", rescale: " << res.rescale << ", total: " << res.total << endl;
	return (s);
}

ostream& operator << (ostream& s, const SpectraTiming& timing)
{
	s << "SpectraTiming: computes: " << timing.computes;
	s << ", total: " << timing.total << endl << timing.res;
	return (s);
}

}