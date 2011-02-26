/*******************************************************************************

 File:    test.cpp
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
// test of the Spectra library
//
#include "Spectra.h"
#include "SpectraTest.h"

using std::endl;

using namespace spectra;

/**
* Run a test of the Spectra library.
*
* Description:\n
*	Runs a simple test of the Spectra library.
* @see		SpectraTest
*/
int
main(int argc, char *argv[])
{
	Spectra spectra;

	uint32_t version = spectra.getVersion();
	int32_t major = version >> 16;
	int32_t minor = version & 0xff;
	std::string dig = "";
	if (minor < 10)
		dig = "0";
	std::stringstream s;
	s << "Version = " << major << "." << dig.c_str() << minor << endl;

	uint32_t ifVersion = spectra.getIfVersion();
	major = ifVersion >> 16;
	minor = ifVersion & 0xff;
	dig = "";
	if (minor < 10)
		dig = "0";
	s << "IFVersion = " << major << "." << dig.c_str() << minor << endl;
	OUTL(s.str());

	CONFIRM(ifVersion == SPECTRA_IFVERSION);

	SpectraTest test;
	test.test();
}
									