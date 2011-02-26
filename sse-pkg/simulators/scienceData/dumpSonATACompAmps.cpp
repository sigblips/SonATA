/*******************************************************************************

 File:    dumpSonATACompAmps.cpp
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

// dumps an ascii version of an SonATA/SSE CompAmps file.
// File contains one or more binary writes of the
// ComplexAmplitudeHeader struct followed by the variable
// length SubchannelCoef1KHz struct.

#include "sseDxInterface.h"
#include "SseUtil.h"

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;


void printCompAmps(ifstream &fin)
{
    cout.precision(6);           // show N places after the decimal
    cout.setf(std::ios::fixed);  // show all decimal places up to precision

    ComplexAmplitudeHeader compAmpHeader;

    // read in the next header
    while (fin.read((char*)&compAmpHeader, sizeof compAmpHeader))
    {
	compAmpHeader.demarshall();
	cout << "# " << dec << compAmpHeader << endl;
	
	// data sanity checks

	const int MAX_SUBCHANNELS = 10000;

	// note: it's ok for the start subchannel id to go negative
	// which can happen on the left edge of the dx band
	// in the multisubchannel archive data format
	if (compAmpHeader.startSubchannelId > MAX_SUBCHANNELS)
	{
	    cout << "invalid startSubchannelId: "
		 << compAmpHeader.startSubchannelId << endl;
	    break;
	}

	int nSubchannels = compAmpHeader.numberOfSubchannels;
	if (nSubchannels < 1 || nSubchannels > MAX_SUBCHANNELS)
	{
	    cout << "Invalid number of subchannels: " << nSubchannels << endl;
	    break;
	}

	// now grab the subchannel data and print the comp amp values.

	for (int subchannel = compAmpHeader.startSubchannelId;
	     subchannel < compAmpHeader.startSubchannelId + nSubchannels; subchannel++)
	{
	    SubchannelCoef1KHz subchannelData;
	    fin.read((char *)&subchannelData, sizeof(SubchannelCoef1KHz));

	    subchannelData.demarshall();  

	    cout << "Subchannel: " << subchannel << endl;
	    cout << "#Index  Real  Imag" << endl << endl;

	    for (int i=0; i<MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; ++i)
	    {
		// cast to int so it's not printed as a char

	        // This assumes 8 bit bytes to make the masks easy.
	        // get sign extended, 2's complement value

		signed char realValue = 
		    (subchannelData.coef[i].pair & 0xF0) >> 4;
		if (realValue & 0x08 ) realValue |= 0xF0;
	    
		signed char imagValue = 
		    (subchannelData.coef[i].pair & 0x0F);
		if (imagValue & 0x08 ) imagValue |= 0xF0;

	        // print values.  cast to int so they don't print
	        // as chars

		cout << setw(4) << i << "     ";

		cout << setw(2) << (int) realValue << "   " 
		     << setw(2) << (int) imagValue << endl;

	    }
	    cout << endl;
	}

	cout << endl;
    }
}

void dumpCompAmps(char *inFilename)
{
    // try to open it for reading 
    ifstream fin;
    fin.open(inFilename, ios::in | ios::binary);
    if (!fin.is_open())
    {
	cerr << "Can't open file: " << inFilename << endl;
	exit(1);
    }

    printCompAmps(fin);

    fin.close();

}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Dumps SonATA Complex Amplitudes file in ASCII" << endl;
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        exit(1);
    };

    char *inFilename(argv[1]);
    dumpCompAmps(inFilename);

}