/*******************************************************************************

 File:    readMask.cpp
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

// Reads a frequency mask from a file.

#include <tcl.h>
#include <stdlib.h>
#include <utime.h>
#include <vector>
#include "sseDxInterface.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <string>

using namespace std;

bool lessFreq( const struct FrequencyBand & f1, const struct FrequencyBand & f2)
{
  return ( f1.centerFreq < f2.centerFreq );
}

int32_t readMask( vector<struct FrequencyBand>& freqBandList,
		  string & maskFileName,
                  int32_t * numberOfFreqBands,
                  NssDate * maskVersionDate,
		  struct FrequencyBand * bandCovered)
{
    Tcl_Interp *interp;
    int code;
    int maskc;
#if (TCL_MAJOR_VERSION <= 8) and (TCL_MINOR_VERSION <= 3)
    char **maskv;
#else
    const char **maskv;
#endif
    int i;
    struct stat statBuffer;
    int32_t statCode = 0;


    interp = Tcl_CreateInterp();
    if (interp == NULL) {
	cout << "Error creating Interpreter" << endl;
	return -1;
    }
  
    code = Tcl_Init(interp);
    if (code != TCL_OK) {
	cout << "Error initializing Interpreter" << endl;
	Tcl_DeleteInterp(interp);
	return -1;
    }

    // TODO: cast should not be necessary, but Tcl_EvalFile takes non-const
    // on Linux with tcl8.3
    code = Tcl_EvalFile(interp, const_cast<char *>(maskFileName.c_str()) );
    if (code != TCL_OK) {
	cout << "Error reading file " << maskFileName << " code " << code << endl;
	Tcl_DeleteInterp(interp);
	return code;
    }

    statCode = stat( maskFileName.c_str(), &statBuffer );
    if ( statCode < 0 )
    {
	perror("stat");
	cout << "Error on stat " << statCode << endl;
	return 0;
    }

    maskVersionDate->tv_sec = statBuffer.st_mtime;
    maskVersionDate->tv_usec = 0;

    // frequency and bandwith pair for bandcovered
    const char *maskList = Tcl_GetVar(interp, "bandcovered", 0);
    if (maskList != NULL) {
	if (Tcl_SplitList(interp, maskList, &maskc, &maskv) == TCL_OK) {
      
	    if ( (maskc/2)*2 != maskc ) {
		cout << "missing data in file  " << maskFileName << endl;
		free((char *)maskv);
		return 0;
	    }

	    for (i = 0; i < maskc; i+=2) {
		float64_t freq;
		float64_t bandwidth;

		if (TCL_OK != Tcl_GetDouble(interp, maskv[i], &freq)) {
		    cout << "Frequency Tcl_GetDouble Error" << endl;
		    Tcl_DeleteInterp(interp);
		    free((char *)maskv);
		    return -1;
		}
		bandCovered->centerFreq = freq;

		if (TCL_OK != Tcl_GetDouble(interp, maskv[i+1], &bandwidth)) {
		    cout << "Bandwidth Tcl_GetDouble Error" << endl;
		    Tcl_DeleteInterp(interp);
		    free((char *)maskv);
		    return -1;
		}
		bandCovered->bandwidth = bandwidth;
	    }
	}
	free((char *)maskv);

	// frequency and bandwith pairs for masks

	const char *maskList = Tcl_GetVar(interp, "masks", 0);
	if (maskList != NULL) {
	    if (Tcl_SplitList(interp, maskList, &maskc, &maskv) == TCL_OK) {
      
		if ( (maskc/2)*2 != maskc ) {
		    cout << "missing data in file  " << maskFileName << endl;
		    free((char *)maskv);
		    return 0;
		}

		*numberOfFreqBands = maskc/2;
		for (i = 0; i < maskc; i+=2) {
		    float64_t freq;
		    float64_t bandwidth;
		    struct FrequencyBand freqBand;

		    if (TCL_OK != Tcl_GetDouble(interp, maskv[i], &freq)) {
			cout << "Frequency Tcl_GetDouble Error" << endl;
			Tcl_DeleteInterp(interp);
			free((char *)maskv);
			return -1;
		    }
		    freqBand.centerFreq = freq;

		    if (TCL_OK != Tcl_GetDouble(interp, maskv[i+1], &bandwidth)) {
			cout << "Bandwidth Tcl_GetDouble Error" << endl;
			Tcl_DeleteInterp(interp);
			free((char *)maskv);
			return -1;
		    }
		    freqBand.bandwidth = bandwidth;
		    freqBandList.push_back(freqBand);
		}
	    }
	    free((char *)maskv);
	}
	sort (freqBandList.begin(), freqBandList.end(), lessFreq);

	Tcl_DeleteInterp(interp);
	
	return 1;
    }

    Tcl_DeleteInterp(interp);

    return 0; 
}