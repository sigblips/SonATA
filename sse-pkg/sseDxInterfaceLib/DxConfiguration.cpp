/*******************************************************************************

 File:    DxConfiguration.cpp
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

ostream& operator << (ostream &strm, const DxConfiguration &config)
{
    strm << "Dx Configuration:" << endl
	 << "------------------" << endl
	 << "site: " << SseMsg::getSiteIdName(config.site) << endl
	 << "dxId: " << config.dxId << endl
	 << "a2dClockrate: " << config.a2dClockrate << endl
	 << "archiverHostname: " << config.archiverHostname << endl
	 << "archiverPort: " << config.archiverPort
	 << endl;

    return strm;

}

DxConfiguration::DxConfiguration()
    :site(SITE_ID_UNINIT),
     dxId(-1),
     a2dClockrate(0),
     archiverPort(0),
     alignPad(0)
{
    archiverHostname[0] = '\0';
}

void DxConfiguration::marshall()
{
    SseMsg::marshall(site);
    NTOHL(dxId);
    NTOHD(a2dClockrate);
    NTOHL(archiverPort);

    // no need to marshall archiveHostname char array
}

void DxConfiguration::demarshall()
{
    marshall();
}