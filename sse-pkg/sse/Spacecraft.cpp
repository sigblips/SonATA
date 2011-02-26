/*******************************************************************************

 File:    Spacecraft.cpp
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



#include "Spacecraft.h" 
#include "doppler.h"
#include "SseUtil.h"
#include <sstream>

using namespace std;

/*
Calculate the J2000 coordinates of the moving target
  for the given time.
Coordinates are returned  as J2000 RA/Dec in radians.

Inputs:
   ephemdDir: location of ephemeris files (.xyz format)
   target ephemeris file
   earth ephemeris file
   obsDate: observing date-time

Throws: SseException on failure
*/   

void Spacecraft::calculatePosition(
   const string &ephemDir,
   const string &targetEphemFilename,
   const string &earthEphemFilename,
   time_t obsDate,
   double *ra2000Rads,
   double *dec2000Rads,
   double *rangeKm,
   double *rangeRateKmSec
)
{
   if (targetEphemFilename == "")
   {
      throw SseException("Spacecraft ephem filename is blank\n",
                         __FILE__, __LINE__,
			 SSE_MSG_RD_TARG_EPHEM, SEVERITY_ERROR);
   }

   target_data_type target_info;   
   target_info.type = SPACECRAFT;
   target_info.origin = GEOCENTRIC;
   
   // Get the target ephemeris file.  
   string targetEphemFile(ephemDir + "/" + targetEphemFilename);
   
   if (!SseUtil::fileIsReadable(targetEphemFile))
   {
      stringstream strm;
      strm << "Can't read target ephemeris file " 
	   << targetEphemFile << endl;
      
      throw SseException(strm.str(), __FILE__, __LINE__,
			 SSE_MSG_RD_TARG_EPHEM, SEVERITY_ERROR);
   }

   SseUtil::strMaxCpy(target_info.targetfile, targetEphemFile.c_str(),
                      DOPPLER_MAX_EPHEM_FILENAME_LEN);
   
   // Get the earth ephemeris file.
   string earthEphemFile(ephemDir + "/" + earthEphemFilename);
   
   if (!SseUtil::fileIsReadable(earthEphemFile))
   {
      stringstream strm;
      strm  << "Can't read earth ephemeris file " 
	    << earthEphemFile << endl;
      
      throw SseException(strm.str(), __FILE__, __LINE__,
			 SSE_MSG_RD_EARTH_EPHEM, SEVERITY_ERROR);
   }
   
   double diff_utc_ut1 = 0; // TBD is this still needed?
   
   // from the doppler library:
   if (calculateSpacecraftPosition(&target_info,
				   obsDate,
				   diff_utc_ut1,
				   earthEphemFile.c_str()) != 0)
   {
      stringstream strm;
      strm  << "Failed to calculateSpacecraftPosition using "
	    << "target ephem file: " << targetEphemFilename << endl;
       
      throw SseException(strm.str(), __FILE__, __LINE__,
			 SSE_MSG_SPACECRAFT_POS, SEVERITY_ERROR);
   }

   *ra2000Rads = target_info.ra2000;
   *dec2000Rads = target_info.dec2000;

#if 0
   cout << "Target lightSec: " << target_info.lightTimeSec << endl
        << "Target rangeKm: " << target_info.rangeKm << endl
        << "Target deldot km/s: " << target_info.rangeRateKmSec << endl;
#endif

   *rangeKm = target_info.rangeKm;
   *rangeRateKmSec = target_info.rangeRateKmSec;

}
