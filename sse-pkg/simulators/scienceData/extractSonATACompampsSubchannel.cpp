/*******************************************************************************

 File:    extractSonATACompampsSubchannel.cpp
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



/* Extract one subchannel from an SonATA/SSE CompAmps file.

 Input file contains one or more binary writes of the
 ComplexAmplitudeHeader struct followed by the variable
 length SubchannelCoef1KHz struct.

 Output is a single subchannel, specified by an offset (0-15),
 with the header center frequency adjusted appropriately.
*/


#include "sseDxInterface.h"
#include "SseUtil.h"
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

static const bool Debug = false;

void extractCompAmps(ifstream &fin, int subchannelOffset)
{
    ComplexAmplitudeHeader compAmpHeader;

    // read in the next header
    while (fin.read((char*)&compAmpHeader, sizeof compAmpHeader))
    {
	compAmpHeader.demarshall();
        if (Debug)
        {
           cerr << "Incoming " << dec << compAmpHeader << endl;
	}
	// data sanity checks

	const int MAX_SUBCHANNELS = 10000;

	// note: it's ok for the start subchannel id to go negative
	// which can happen on the left edge of the dx band
	// in the multisubchannel archive data format
	if (compAmpHeader.startSubchannelId > MAX_SUBCHANNELS)
	{
	    cerr << "Invalid startSubchannelId in input file: "
		 << compAmpHeader.startSubchannelId << endl
                 << compAmpHeader << endl;
	    break;
	}

	int nSubchannels = compAmpHeader.numberOfSubchannels;
	if (nSubchannels < 1 || nSubchannels > MAX_SUBCHANNELS)
	{
	    cerr << "Invalid number of subchannels in input file: "
                 << nSubchannels << endl
                 << compAmpHeader << endl;
	    break;
	}

        if (subchannelOffset > nSubchannels-1)
        {
           cerr << "Error: requested subchannelOffset exceeds number "
                << "of available subchannels in file." << endl
                << compAmpHeader << endl;

           break;
        }

        int startSubchannelId = compAmpHeader.startSubchannelId;
        SubchannelCoef1KHz subchannelData;
 
        // pick out the requested subchannel (based on the subchannelOffset)
		
        // skip the subchannels before the desired subchannel
        for (int i=0; i<subchannelOffset && i < nSubchannels-1; ++i)
        {
           startSubchannelId++;

           if (Debug)
           {
              cerr << "skipping pre subchannel " << i << endl;
           }
           fin.read((char *)&subchannelData, sizeof(SubchannelCoef1KHz));
        }

	// Now at the desired subchannel.  Write out updated header and the
        // subchannel.
        {
           /*
             Update header fields, and write it.

             Adjust the center frequency to match the selected
             subchannel.  The one in the incoming header is defined to be the
             center freq of the first subchannel.
           */
           
           double hzPerMHz = 1e6;
           double offsetMHz = (subchannelOffset) * compAmpHeader.hzPerSubchannel / hzPerMHz;
           compAmpHeader.rfCenterFreq += offsetMHz; 
           compAmpHeader.numberOfSubchannels = 1;
           compAmpHeader.startSubchannelId = startSubchannelId;
           compAmpHeader.marshall();
           cout.write((char *) &compAmpHeader, sizeof(compAmpHeader));

           // put out the desired subchannel
           fin.read((char *)&subchannelData, sizeof(SubchannelCoef1KHz));
           cout.write((char *) &subchannelData, sizeof(SubchannelCoef1KHz));
        }


        // skip the data after the desired subchannel
        for (int i=subchannelOffset+1; i<=nSubchannels-1; ++i)
        {
           if (Debug)
           {
              cerr << "skipping post subchannel " << i << endl;
           }
           fin.read((char *)&subchannelData, sizeof(SubchannelCoef1KHz));
        }

    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Extract SonATA Complex Amplitudes subchannel (in binary)" << endl;
        cerr << "Usage: " << argv[0] 
             << " <infile> <subchannel offset (0-15)> > <outfile>" << endl;
        exit(1);
    };

    try {

       // try to open file for reading 
       char *inFilename(argv[1]);
       ifstream fin;
       fin.open(inFilename, ios::in | ios::binary);
       if (!fin.is_open())
       {
          cerr << "Can't open file: " << inFilename << endl;
          exit(1);
       }

       int subchannelOffset(SseUtil::strToInt(argv[2]));
       const int maxSubchannelOffset(15);

       if (subchannelOffset < 0 || subchannelOffset > maxSubchannelOffset)
       {
          cerr << "invalid subchannel offset: " << argv[2] << endl
               << "valid subchannel offset range: 0 - "
               <<  maxSubchannelOffset << endl;
          exit(1);
       }
       
       extractCompAmps(fin, subchannelOffset);

       fin.close();
    }
    catch (SseException &except)
    {
       cerr << except << endl;
       exit(1);
    }
}