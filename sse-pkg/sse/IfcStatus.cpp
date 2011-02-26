/*******************************************************************************

 File:    IfcStatus.cpp
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



#include "IfcStatus.h" 
#include <iostream>

using namespace std;

IfcStatus::IfcStatus() 
    :
    attnDbLeft(0), attnDbRight(0), 
    stxStatus(0), goodStxStatus(false),
    stxCountLeft(0), stxMeanLeft(-1), stxVarLeft(-1), 
    stxCountRight(0), stxMeanRight(-1), stxVarRight(-1)
{}

ostream& operator << (ostream& strm, const IfcStatus& status)
{
    strm << "IFC status:\n"
	 << "  Name: " << status.name << endl
	 << "  Time: " << status.timeStamp << endl
	 << "  Left  Attn:   " << (dec) << status.attnDbLeft << " dB" << endl
	 << "  Right Attn:   " << (dec) << status.attnDbRight << " dB" << endl
	 << "  IF Source: " << status.ifSource << endl;

    strm << "  STX pol: " << status.stxPol << endl;
    strm << "  STX Status: 0x";
    strm.fill('0');
    strm.width(8);
    strm << (hex) << status.stxStatus << (dec) << endl;
    
    strm << "  Good Stx Status: " << status.goodStxStatus << endl;

    strm << "\tcount\tmean\tvar" << endl;

    strm << "LCP:\t";
    strm.precision(3);
    strm << status.stxCountLeft << "\t";
    strm.precision(3);
    strm << status.stxMeanLeft << "\t";
    strm.precision(4);
    strm << status.stxVarLeft << endl;

    strm << "RCP:\t";
    strm.precision(3);
    strm << status.stxCountRight << "\t";
    strm.precision(3);
    strm << status.stxMeanRight << "\t";
    strm.precision(4);
    strm << status.stxVarRight << endl;

    return strm;
}


