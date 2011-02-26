/*******************************************************************************

 File:    doppler.h
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


/* doppler code (ported from TSS) */

#ifndef DOPPLER_H
#define DOPPLER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

/* target type */
#define STAR            1
#define SPACECRAFT      2

/* origin */
#define BARYCENTRIC     1
#define GEOCENTRIC      2

/* observing site x,y,z coords indexes */
#define DOPPLER_SITE_COORD_X 0
#define DOPPLER_SITE_COORD_Y 1
#define DOPPLER_SITE_COORD_Z 2

#define DOPPLER_MAX_EPHEM_FILENAME_LEN 256
  
    typedef struct target_data 
    {     
	/* 
	 *  data about star or spacecraft 
	 */
	int  type;           /* 1 = STAR, 2 = SPACECRAFT */
	double  ra2000;      /* J2000 Right Ascension radians */
	double  dec2000;     /* J2000 Declination radians */
	int  origin;         /* 1 = BARYCENTRIC, 2 = GEOCENTRIC */

	 /*
	  *  the following three only apply to stars 
	  */

	double  parallax;    /* arcsec */
	double  pmra;        /* proper motion, RA, asec per yr */
	double  pmdec;       /* proper motion, Dec, asec per yr */

	/* 
	  *  these parameters only apply to spacecraft (except for range,
	  *  which is also used for stars)
	  */
	double  epoch;       /* julian UTC date for which range, vel, 
				accel apply */
	double  range;       /* range to spacecraft in km */
	double  vel[3];      /* craft velocity vector, km/s, equatorial
				 J2000 */
	double  acc[3];      /* craft acceleration vector, km/s/s, equatorial
				J2000 */
        double lightTimeSec; /* light travel time sec */
        double rangeKm;      
        double rangeRateKmSec;  /* deldot */ 

	double  f0;          /* transmitter freq in MHz, at spacecraft */
	char targetfile[DOPPLER_MAX_EPHEM_FILENAME_LEN]; /* Name of target empheris file	*/
    } target_data_type;


    /* data pertinent to a telescope site */
    typedef struct site_data  
    {
	double  coor[3];        /* X,Y,Z terrestrial telescope coordinates,
				   m, ITRF system */
	double  altitude;       /* height above geoid (mean sea level) in
				   meters, UNUSED */
	double  foffset;        /* (station freq standard - true)/true, 
				   of order 1e-12 */
    } site_data_type;        

    /* computed doppler results */
    typedef struct doppler_parameters_data {
	double f_ratio;                 /* f_ratio = f_remote/f_main    */
	double drift_offset_factor;     /* (drift_remote-drift_main)/f_main */
	double curv_main_factor;        /* curvature_main/f_main        */
	double curv_remote_factor;      /* curvature_remote/f_remote    */
    } doppler_parameters_data_type;


   /* Calculate coordinates for the given spacecraft.
      Sets ra, dec in j2000 radians in the target_info struct.
      Assumes target ephemeris file is set in the  target_info->targetfile
      struct field.
      Returns 0 on success, nonzero on failure.
   */

    int calculateSpacecraftPosition(
	target_data_type *target_info,
	time_t obs_date_time,
	double diff_utc_ut1,     /* difference between UTC and UT1 (dut) */
	const char * earth_ephem_file);

    /* returns 0 for success, 1 for failure */
    int compute_doppler(
	target_data_type target_info,      
	site_data_type main_site_info,   
	site_data_type remote_site_info,
	time_t obs_date_time,
	double diff_utc_ut1,        /* difference between UTC and UT1 (dut) */
	const char * earth_ephem_file,
	doppler_parameters_data_type *doppler_parameters,  /* returned results */
	int verbosity);

    void printSiteInfo(const char *which_site, site_data_type site_info);
    void printStarInfo(target_data_type star_info);

/* TBD these are also defined in doppler_work.h.  Merge them together */
  void rad_to_dms( double theta, int *deg, int *arcmin, double *arcsec);
  void rad_to_hms( double theta, int *hrs, int *min, double *sec);


  
#ifdef __cplusplus
	   }
#endif

#endif /* DOPPLER_H */