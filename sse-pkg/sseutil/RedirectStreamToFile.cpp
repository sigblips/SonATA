/*******************************************************************************

 File:    RedirectStreamToFile.cpp
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



#include "RedirectStreamToFile.h" 

using namespace std;

// Redirect a stream to an output file.
// Note: the redirect stays in effect as long as the object exists.
// Ref: "The C++ Standard Library" by Nicolai Josuttis.
//
RedirectStreamToFile::RedirectStreamToFile(ostream& strm, 
					   const string &outfilename)
    :strm_(strm)
{
    // create the output file
    outfile_.open(outfilename.c_str());
    if (!outfile_)
    {
	cerr << "Warning: RedirectStreamToFile can't open file " << outfilename <<endl;
	//throw 1;  // tbd fix this

	// tbd error handling here

	return;
    }
    

    // save the output buffer of the original stream
    strmBuffer_ = strm_.rdbuf();

    //redirect output to the file
    strm_.rdbuf (outfile_.rdbuf());

}

RedirectStreamToFile::~RedirectStreamToFile()
{
   //restore old output buffer (restore original stream)
    strm_.rdbuf (strmBuffer_);
}