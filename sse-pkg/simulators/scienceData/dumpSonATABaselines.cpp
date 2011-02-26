/*******************************************************************************

 File:    dumpSonATABaselines.cpp
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



// dumps an ascii version of an SonATA/SSE baselines file.
// File contains consecutive binary writes of: 
//   BaselineHeader struct
//   variable length array of type BaselineValue, which is 
//      numberOfSubchannels long

#include "sseDxInterface.h"
#include "SseUtil.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

void printBaselines(ifstream &fin)
{
    BaselineHeader header;

    cout.precision(6);           // show N places after the decimal
    cout.setf(std::ios::fixed);  // show all decimal places up to precision

    int nValuesPerRow = 1;

    // read in all the baselines
    // first read the header
    while (fin.read((char*)&header, sizeof header))
    {
	header.demarshall();
	cout << header;

	// now pull out the variable length array
	// array length is the numberOfSubchannels

        const int maxSubchannels = 10000;  // sanity check
	int numberOfSubchannels = header.numberOfSubchannels;

	if (numberOfSubchannels < 1 || numberOfSubchannels > maxSubchannels)
	{
	    cerr <<  "ERROR: subchannels value " << numberOfSubchannels
		 << " out of range.  Must be 1 - " << maxSubchannels << endl;
	    exit(1);
	}

	BaselineValue *baselineValue = new BaselineValue[numberOfSubchannels];
	fin.read((char *)baselineValue, sizeof(BaselineValue) * numberOfSubchannels);

	// print the values
	for (int i=0; i< header.numberOfSubchannels; ++i)
	{
            baselineValue[i].demarshall();
	    cout << baselineValue[i] << " ";
	    if (i % nValuesPerRow == (nValuesPerRow -1))
		cout << endl;
	}
	cout << endl;

	delete [] baselineValue;

    }
}



void dumpBaselines(char *inFilename)
{
    // try to open it for reading 
    ifstream fin;
    fin.open(inFilename, ios::in | ios::binary);
    if (!fin.is_open())
    {
	cerr << "Can't open file: " << inFilename << endl;
	exit(1);
    }

    printBaselines(fin);

    fin.close();

}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Dumps SonATA baseline file in ASCII" << endl;
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        exit(1);
    };

    char *inFilename(argv[1]);
    dumpBaselines(inFilename);

}