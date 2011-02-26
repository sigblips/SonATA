/*******************************************************************************

 File:    ReadFilter.cpp
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

#include <iostream>
#include <complex>
#include "Err.h"
#include "ReadFilter.h"

using std::cout;
using std::endl;

namespace sonata_lib {

/**
 * Read the filter specification
 * 
 * Description:\n
 * 	Reads a text file containing a header which specifies the
 * 	filter length, number of foldings, and overlap.  The result
 * 	is returned in the FilterSpec reference.\n
 * Notes:\n
 * 	Space for the coefficients is allocated here.
 */
Error
ReadFilter::readFilter(string& filename, FilterSpec& filter)
{
	ifstream fin;
	fin.open(filename.c_str());
	if (!fin.is_open())
		return (ERR_COF);
	Error err = readHeader(fin, filter);
	if (!err)
		err = readData(fin, filter);
	fin.close();
	return (err);
}

/**
 * Read the filter header
 * 
 * Description:\n
 * 	Reads the filter header, which should contain the value for length,
 * 	foldings and overlap.  Each is specified in the form "variable=value"
 * 	with one value per line.   * 	Case is unimportant, but all must
 * 	be specified and there may be no spaces:
 * 	Length=256
 * 	Foldings=7
 * 	Overlap=.25\n
 * Notes:\n
 * 	Any line which begins with # is a comment and is ignored.  
 */
Error
ReadFilter::readHeader(ifstream& fin, FilterSpec& filter)
{
	int cmds = 3;
	char input[256];
	string line;
	Error err = 0;

	while (cmds && !fin.eof()) {
		fin.getline(input, sizeof(input));
		line = input;
		if (fin.eof())
			continue;
		// test for comment
		if (!line.length() || line[0] == '#')
			continue;
		int pos = line.find("=");
		string cmd = line.substr(0, pos);
		string val = line.substr(pos + 1, line.length() - pos);
		if (cmd == "Length") {
			filter.fftLen = atoi(val.c_str());
			--cmds;
		}
		else if (cmd == "Foldings") {
			filter.foldings = atoi(val.c_str());
			--cmds;
		}
		else if (cmd == "Overlap") {
			filter.overlap = atoi(val.c_str());
			--cmds;
		}
		else {
			std::cout << "bad header object" << std::endl;
			exit(1);
		}
	}
	if (filter.fftLen < 1 || filter.foldings < 1
			|| filter.overlap > filter.fftLen) {
		err = ERR_IFH;
	}
	return (err);
}
	
/**
 * Read the filter coefficients.
 * 
 * Description:\n
 * 	Using the values specified in the header, the filter coefficients
 * 	are read and stored in an array of floats.\n
 * Notes:\n
 * 	There must be at least fftLen * foldings coefficients, but this
 * 	version does not check for excess values.
 */
Error
ReadFilter::readData(ifstream& fin, FilterSpec& filter)
{
	int n = filter.fftLen * filter.foldings;
	Error err = 0;

	filter.coeff = new float32_t[n];
	int i;
	for (i = 0; !fin.eof() && i < n; ) {
		char input[256];
		string line;
		while (!fin.eof()) {
			fin.getline(input, sizeof(input));
			line = input;
			if (!line.length() || line[0] == '#')
				continue;
			float c = atof(line.c_str());
			filter.coeff[i] = c;
			++i;
		}
	}
	if (i < n)
		err = ERR_IFC;
	return (err);
} 
			
}		