/*******************************************************************************

 File:    WriteScienceData.cpp
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


#include "WriteScienceData.h"
#include "sseDxInterface.h"
#include "SseException.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

static void openFileBinaryAppend(ofstream &strm, const string &filename)
{
   strm.open(filename.c_str(), (ios::app | ios::binary ));
   if (!strm.is_open())
   {
       string msg;
       msg = "File Open failed on " + filename;
       throw SseException(msg, __FILE__, __LINE__,
   		SSE_MSG_FILE_ERROR, SEVERITY_ERROR);
   }

}


// append variable length baseline to the end of the given file (nss binary format).
// assumes data is already  marshalled
// TBD: clean this up when the deprecated, fixed length Baseline struct is removed

void writeVariableLengthBaselineToNssFile(
    const BaselineHeader &hdr,
    float baselineValues[],
    int numberOfBaselineValues,
    const string& filename)
{
    ofstream fout;
    openFileBinaryAppend(fout, filename);

    fout.write((const char*)&hdr, sizeof(hdr));
    fout.write((const char*)baselineValues, sizeof(float) * numberOfBaselineValues);
    fout.close();

}


// append complex amps to the end of the given file (nss binary format)
void writeCompAmpsToNssFile(const ComplexAmplitudes &compamps, 
			    const string& filename)
{
    ofstream fout;
    openFileBinaryAppend(fout, filename);

    fout.write((const char *)&compamps, sizeof(compamps));
    fout.close();
}
