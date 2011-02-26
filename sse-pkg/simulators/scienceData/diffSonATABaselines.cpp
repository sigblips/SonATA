/*******************************************************************************

 File:    diffSonATABaselines.cpp
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


// takes two binary SonATA baseline files on input.
// input File contains consecutive binary writes of Baseline struct.
// subtracts one from the other, then writes the difference
// out to a new file.

#include "sseDxInterface.h"
#include "SseUtil.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

void dumpDiffBaselines(ifstream &fin1, ifstream &fin2)
{
    Baseline baseline1;
    Baseline baseline2;
    Baseline diffBaseline;

    // read in all the baselines
    while (fin1.read((char*)&baseline1, sizeof baseline1))
    {
	baseline1.demarshall();

	fin2.read((char*)&baseline2, sizeof baseline2);
	baseline2.demarshall();

	diffBaseline.header = baseline1.header;

	// diff the values
	for (int i=0; i< baseline1.header.numberOfSubchannels; ++i)
	{
	    diffBaseline.baselineValues[i] = 
		baseline1.baselineValues[i] - baseline2.baselineValues[i];
	}

	diffBaseline.marshall();
	cout.write((char*)&diffBaseline, sizeof diffBaseline);

    }
}


void fileSizeCheck(char *inFilename)
{
    // A sanity check to make sure file size is an even multiple
    // of the baseline size.
    int filesize = SseUtil::getFileSize(inFilename);
    if ( (filesize % sizeof(Baseline)) != 0)
    {
	cerr << "Error: File size is not an even multiple of the";
	cerr << " baseline size. " << endl;
        cerr << "filesize: " << filesize << endl;
        cerr << "sizeof(Baseline) is: " << sizeof(Baseline) << endl;
	exit(1);
    }
}


void diffBaselines(char *inFilename1, char *inFilename2)
{
    // try to open files for reading 
    ifstream fin1;
    fin1.open(inFilename1, ios::in | ios::binary);
    if (!fin1.is_open())
    {
	cerr << "Can't open first file: " << inFilename1 << endl;
	exit(1);
    }

    fileSizeCheck(inFilename1);


    // try to open it for reading 
    ifstream fin2;
    fin2.open(inFilename2, ios::in | ios::binary);
    if (!fin2.is_open())
    {
	cerr << "Can't open second file: " << inFilename2 << endl;
	exit(1);
    }

    fileSizeCheck(inFilename2);

    dumpDiffBaselines(fin1, fin2);

    fin1.close();
    fin2.close();

}



int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "create diff of SonATA baseline files" << endl;
        cerr << "Usage: " << argv[0] << " <filename1> <filename2> > outfile" << endl;
        exit(1);
    };

    char *inFilename1(argv[1]);
    char *inFilename2(argv[2]);
    diffBaselines(inFilename1, inFilename2);

}