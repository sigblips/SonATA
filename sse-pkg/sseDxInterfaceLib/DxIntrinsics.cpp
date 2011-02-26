/*******************************************************************************

 File:    DxIntrinsics.cpp
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

static const double HzPerMHz = 1e6;

DxIntrinsics::DxIntrinsics() 
    : 
    hzPerSubchannel(0.0),
    maxSubchannels(0),
    serialNumber(0)
{
    interfaceVersionNumber[0] = '\0';
    host[0] = '\0';
    codeVersion[0] = '\0';

    // use default constructor for NssDate fields
}

void DxIntrinsics::marshall()
{
    // no marshalling needed for these char arrays:
    // interfaceVersionNumber
    // dxName
    // dxHostName
    // dxCodeVersion
    channelBase.marshall();

    HTONL(foldings);
    HTONF(oversampling);
    // no marshalling needed for these char arrays:
    // filterName

    HTOND(hzPerSubchannel);
    HTONL(maxSubchannels);
    HTONL(serialNumber);

    birdieMaskDate.marshall();  // NssDate
    rcvrBirdieMaskDate.marshall();  // NssDate
    permMaskDate.marshall();    // NssDate

}

void DxIntrinsics::demarshall()
{
    // marshall & demarshall are the same since they
    // just swap the byte order
    marshall();
}


ostream& operator << (ostream &strm, const DxIntrinsics &intrinsics)
{
    strm << "Dx Intrinsics: " << endl;
    strm << "----------------" << endl;
    strm << "sseDxInterfaceVersionNumber: ";
    strm << intrinsics.interfaceVersionNumber << endl;
    strm << "dxName: " << intrinsics.name << endl;
    strm << "dxHostName: " << intrinsics.host << endl;
    strm << "dxCodeVersion: " <<  intrinsics.codeVersion << endl;
    strm << "channelBase: " << intrinsics.channelBase;
    strm << "foldings: " << intrinsics.foldings << endl;
    strm << "oversampling: " << intrinsics.oversampling << endl;
    strm << "filterName: " << intrinsics.filterName << endl;

    strm << "hzPerSubchannel: " << intrinsics.hzPerSubchannel << endl;
    strm << "maxSubchannels: " << intrinsics.maxSubchannels << endl;

    // compute the bandwidth

    strm << "bandwidthMHz: " << (intrinsics.maxSubchannels *
	intrinsics.hzPerSubchannel) / HzPerMHz << endl;

    strm << "serialNumber: " <<  intrinsics.serialNumber << endl;
    strm << "birdieMaskDate: " << intrinsics.birdieMaskDate << endl;
    strm << "rcvrBirdieMaskDate: " << intrinsics.birdieMaskDate << endl;
    strm << "permMaskDate: " << intrinsics.permMaskDate << endl;
    strm << endl;

    return strm;

}