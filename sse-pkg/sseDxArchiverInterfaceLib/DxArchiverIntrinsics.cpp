/*******************************************************************************

 File:    DxArchiverIntrinsics.cpp
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



#include "sseDxArchiverInterfaceLib.h"

using std::endl;

DxArchiverIntrinsics::DxArchiverIntrinsics()
    : port(-1)
{
    interfaceVersionNumber[0] = '\0';
    hostname[0] = '\0';
    name[0] = '\0';
}

void DxArchiverIntrinsics::marshall()
{
    HTONL(port);

    // no marshalling needed for char arrays
}

void DxArchiverIntrinsics::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, 
		      const DxArchiverIntrinsics &intrinsics)
{
    strm << "DxArchiver Intrinsics: " << endl
	 << "-----------------------" << endl
	 << "interfaceVersionNumber: "
	 << intrinsics.interfaceVersionNumber << endl
	 << "hostname: " << intrinsics.hostname << endl
	 << "dxPort: " << intrinsics.port << endl
	 << "name: " << intrinsics.name << endl;
    
    return strm;
}