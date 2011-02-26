/*******************************************************************************

 File:    DxStatus.cpp
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


#include "sseDxInterfaceLib.h"



ostream& operator << (ostream &strm, const DxStatus &status)
{
//    strm << "Dx Status:  ";

    strm << status.timestamp << "  ";

    if (status.numberOfActivities == 0)
    {
	strm << "No Activities ";
    }
    else
    {
	for (int i=0; i<status.numberOfActivities &&
		 i<MAX_DX_ACTIVITIES; i++)
	{
	    strm << status.act[i] << "  ";
	}

    }
    return strm;

}

DxStatus::DxStatus()
    :
    numberOfActivities(0),
    alignPad(0)
{
    // use default constructor for DxActivityStatus
}

void DxStatus::marshall()
{
    timestamp.marshall();   // NssDate

    for (int i=0; i< MAX_DX_ACTIVITIES; ++i)
    {
	act[i].marshall();
    }
    HTONL(numberOfActivities);
}

void DxStatus::demarshall()
{
    marshall();
}
