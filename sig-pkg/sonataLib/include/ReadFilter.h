/*******************************************************************************

 File:    ReadFilter.h
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

// Class to read a filter specification

#ifndef READFILTER_H_
#define READFILTER_H_

#include <fstream>
#include <string>
#include "Sonata.h"

using std::ifstream;
using std::string;

namespace sonata_lib {

/**
 * DFB filter specification
 * 
 * Description:\n
 * 	Specifies the filter to be used by the DFB library.
 * 	This allows dynamic modification of the filter characteristics
 */
struct FilterSpec {
	int32_t fftLen;						// length of the DFT
	int32_t foldings;					// # of foldings
	float64_t overlap;					// % of overlap
	float32_t *coeff;					// filter coefficients

	FilterSpec(): fftLen(0), foldings(0), overlap(0), coeff(0) {}
};

class ReadFilter {
public:
	ReadFilter() {}
	~ReadFilter() {}
	
	Error readFilter(string& filename, FilterSpec& filter);
	
private:
	Error readHeader(ifstream& fin, FilterSpec& filter);
	Error readData(ifstream& fin, FilterSpec& filter);
};

}

#endif /*READFILTER_H_*/