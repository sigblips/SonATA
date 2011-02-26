/*******************************************************************************

 File:    testVal.h
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

// values
#ifndef _testValH
#define _testValH

#define ADJACENT_BINS			(8)
#define ITERATIONS_PER_BIN		(128)
#define TIMING_ITERATIONS		(1000)

const int FFTLEN_4 = 4;
const int FFTLEN_8 = 8;
const int FFTLEN_128 = 128;
const int FFTLEN_256 = 256;
const int FFTLEN_512 = 512;
const int FFTLEN_1024 = 1024;

const int OVERLAP_4 = 0;
const int OVERLAP_8 = 0;
const int OVERLAP_128 = 24;
const int OVERLAP_256 = 64;
const int OVERLAP_512 = 128;
const int OVERLAP_1024 = 256;

const int DFB_SAMPLES_4 = 1024;
const int DFB_SAMPLES_8 = 1024;
const int DFB_SAMPLES_256 = 1024;

#endif