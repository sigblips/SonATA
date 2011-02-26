/*******************************************************************************

 File:    doppler_work.h
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

/*****************************************************************
 * libdop.h - declaration of functions defined in the DOPPLER library
 * PURPOSE:  
 *****************************************************************/

#ifndef LIB_DOPPLER_WORK_H
#define LIB_DOPPLER_WORK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "doppler.h"

  typedef struct app_place  /* stores geocentric or topocentric apparent stuff */
  {
    double  epoch;         /* Julian Date for which calculations made */
    int  view;             /* 2 = geocenter, 3 = topocenter */
    double  gast;          /* Greenwich Apparent Sidereal Time, radians */
    double  ra;            /* apparent RA in apparent coords of date, radians */
    double  dec;           /* apparent Dec in apparent coords of date, radians */
    double  range;         /* range from observation point in km */
    double  doppler;       /* doppler factor */
    double  drift;         /* 1st derivative of doppler, 1/s */
    double  curve;         /* for topocenter, 2nd derivative of doppler, 1/s/s */
    double  fpred;         /* for spacecraft: predicted observed frequency, MHz */
    /* for stars, -1.0 */
    double  dpred;         /* for spacecraft: predicted drift rate, Hz/s */
    double  ha;            /* Hour Angle, radians.  If geocenter, Greenwich HA */
    double  unab_ra;	/* Unaberated RA */
    double  unab_dec;	/* Unaberated DEC */
    double  vrel[3];        /* velocity vector wrt geocenter, m/s, J2000 frame */
    double  arel[3];        /* accel vector wrt geocenter, m/s/s, J2000 frame */
  } app_place_type;


  typedef struct civil_time
  {      
    /* 
     * stores an epoch as civil time 
     */
    int  year;         /* eg 1994 */
    int  month;        /* 1 = jan, 12 = dec */
    int  day;
    int  hour;         /* UTC, 0 to 24 normally */
    int  min;          /* 0 to 60 normally */
    double  sec;       /* 0.0 to 60.0 normally */
    double  dut;       /* UT1 - UTC, sec, always < 1 */
  } civil_time_type;


  int get_target_data(target_data_type *tar, civil_time_type time, 
		      char *targetfile);

  int geocentric(target_data_type star, civil_time_type time,
		 char *targetfile, app_place_type *geo, int verbosity );

  int topocentric( app_place_type geo, site_data_type stat, 
		   app_place_type *topo, int verbosity );

  long juldayfromdate(int year, int month, int day);

  double vector_mag( double v[] );

  int rect_to_radec( double vector[], double *ra, double *dec, double *r );

  void radec_to_rect( double ra, double dec, double r, double vector[] );

  void rad_to_dms( double theta, int *deg, int *arcmin, double *arcsec);

  void rad_to_hms( double theta, int *hrs, int *min, double *sec);
  void list_target_data( target_data_type cat );

  void list_app_place( app_place_type app );
  void list_site_data( site_data_type site);

  void list_civil_time( civil_time_type time );

  
#ifdef __cplusplus
	   }
#endif

#endif /* LIB_DOPPLER_WORK_H */