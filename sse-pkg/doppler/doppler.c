/*******************************************************************************

 File:    doppler.c
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


/* $Id: doppler.c,v 1.11 2003/02/06 03:02:01 kevin Exp $ */

#include "doppler.h"
#include "doppler_work.h"
#include <stdio.h>

void compute_utc_time(time_t obs_date_time,  double dut1, 
		      civil_time_type *utc_time);


void printParameters(target_data_type target_info,
		     site_data_type main_site_info,
		     site_data_type remote_site_info);

/* compute doppler for the given target 
   returns 0 on success, nonzero on failure
 */

int compute_doppler(
     target_data_type target_info,
     site_data_type main_site_info,
     site_data_type remote_site_info,
     time_t obs_date_time,
     double diff_utc_ut1,     /* difference between UTC and UT1 (dut) */
     const char * earth_ephem_file,
     doppler_parameters_data_type *doppler_parameters, /* returned results */
     int verbosity)
    {
     app_place_type geocent;
     app_place_type main_app;
     app_place_type remote_app;
     civil_time_type utc_time;
     int status;
     
     status = 0;

     compute_utc_time(obs_date_time, diff_utc_ut1, &utc_time);
     if (verbosity) list_civil_time(utc_time);

     // compute spacecraft RA/DEC from ephemeris file
     if (target_info.type == SPACECRAFT)
     {
	 status = get_target_data(&target_info, utc_time, target_info.targetfile);
	 if (status != 0)
	 {
	     fprintf(stderr, "Failed in doppler library call to get_target_data (error = %d)\n",
		     status);
	     return status;
	 }
     }

     if (verbosity >= 3)
	 printParameters(target_info, main_site_info, remote_site_info);

     /* UNABERATED RA and DEC */
     status = unaberated_ra_dec( target_info, utc_time, 
			(char *)earth_ephem_file, // cast off const
			&geocent, verbosity );
     if (status != 0)
     {
	 fprintf(stderr, "Failed in doppler library call to unaberated_ra_dec (error = %d)\n", status);
	 return status;
     }

     if ( verbosity == 3 )printf( "GEOCENTRIC CALCULATIONS\n" );
     status = geocentric( target_info, utc_time,
		 (char *) earth_ephem_file,  // cast off const
		 &geocent, verbosity );

     if (status != 0)
     {
	 fprintf(stderr, "Failed in doppler library call to geocentric (error = %d)\n", status);
	 return status;
     }

     if ( verbosity == 3 )printf( "\n ====== MAIN SITE -  ======\n\n" );
     topocentric( geocent, main_site_info, &main_app, verbosity); 

     if ( verbosity == 3 )printf( "\n ====== REMOTE SITE  ======\n\n" );
     topocentric( geocent, remote_site_info, &remote_app, verbosity); 

     if(verbosity)printf("main doppler %.12f remote doppler %.12f\n",
          main_app.doppler, remote_app.doppler);
     if(verbosity)printf("main x %.4f y %.4f z %.4f\n", 
			  main_site_info.coor[DOPPLER_SITE_COORD_X],
			  main_site_info.coor[DOPPLER_SITE_COORD_Y],
			  main_site_info.coor[DOPPLER_SITE_COORD_Z]);

     if(verbosity)printf("remote x %.4f y %.4f z %.4f\n", 
			  remote_site_info.coor[DOPPLER_SITE_COORD_X],
			  remote_site_info.coor[DOPPLER_SITE_COORD_Y],
			  remote_site_info.coor[DOPPLER_SITE_COORD_Z]);

     doppler_parameters->f_ratio = remote_app.doppler / main_app.doppler;
     doppler_parameters->drift_offset_factor = (remote_app.drift - main_app.drift) /
					    main_app.doppler;
     doppler_parameters->curv_main_factor = main_app.curve / main_app.doppler;
     doppler_parameters->curv_remote_factor = remote_app.curve / remote_app.doppler; 

#if 1
     if (verbosity) {
	 printf("\nFinal results:\n");
	 printf("f_ratio %.12f\nDrift Offset factor %.6e\n",
		doppler_parameters->f_ratio, 
		doppler_parameters->drift_offset_factor );
         printf("main curvature %.6e\nremote curvature %.6e\n",
		doppler_parameters->curv_main_factor,
		doppler_parameters->curv_remote_factor );
	 printf("\n");
     }
#endif

     return(status);
  }



void compute_utc_time(time_t obs_date_time,
		      double dut1, civil_time_type *utc_time)
{
     struct tm *gmt;

     gmt = gmtime(&obs_date_time);
     utc_time->year  = gmt->tm_year + 1900;
     utc_time->month = gmt->tm_mon + 1;
     utc_time->day   = gmt->tm_mday;
     utc_time->hour  = gmt->tm_hour;
     utc_time->min   = gmt->tm_min;
     utc_time->sec   = (double)gmt->tm_sec;
     utc_time->dut   = dut1;

}

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
    const char * earth_ephem_file)
{
     civil_time_type utc_time;
     int status = 0;
     int verbosity = 0;

     compute_utc_time(obs_date_time, diff_utc_ut1, &utc_time);
     if (verbosity) list_civil_time(utc_time);

     // compute spacecraft RA/DEC from ephemeris file
     if (target_info->type == SPACECRAFT)
     {
	 status = get_target_data(target_info, utc_time, target_info->targetfile);
	 if (status != 0)
	 {
	     fprintf(stderr, "Failed in doppler library call to get_target_data (error = %d)\n",
		     status);
	     return status;
	 }
     }
     else {
	 fprintf(stderr, "Doppler library error: calculateSpacecraftPosition() spacecraft not specified");
	 status = 1;
     }

     if (verbosity >= 3) {
	 printStarInfo(*target_info);
	 printf("\n");
     }

     return status;
}


void printParameters(
     target_data_type target_info,
     site_data_type main_site_info,
     site_data_type remote_site_info)
{
     printStarInfo(target_info);
     printf("\n");

     printSiteInfo("main site" , main_site_info);
     printf("\n");

     printSiteInfo("remote site" , remote_site_info);
     printf("\n");
}


void  printStarInfo(target_data_type target_info)
{
     printf("target info:\n");
     printf("type: %d (1=star 2=spacecraft)\n", target_info.type);           /* 1 = star, 2 = spacecraft */
     printf("ra2000: %.12f rad\n", target_info.ra2000);      /* J2000 Right Ascension radians */
     printf("dec2000: %.12f rad\n", target_info.dec2000);     /* J2000 Declination radians */
     printf("origin: %d (1=barycentric 2=geocentric)\n", target_info.origin);         /* 1 for barycentric, 2 for geocentric */
    /*
     *  the following three only apply to stars 
     */
    printf("parallax: %.12f arcsec\n", target_info.parallax);    /* arcsec */
    printf("pmra: %.12f asec/yr\n", target_info.pmra);        /* proper motion, RA, asec per yr */
    printf("pmdec: %.12f asec/yr\n", target_info.pmdec);       /* proper motion, Dec, asec per yr */

    printf("range: %20.10e km\n", target_info.range); 
}

void printSiteInfo(const char *which_site, site_data_type site_info)
{
    printf("site info: %s\n", which_site);

   /* X,Y,Z terrestrial telescope coordinates, m, 
                               ITRF system */
    printf("x: %.12f m\n", site_info.coor[DOPPLER_SITE_COORD_X]);
    printf("y: %.12f m\n", site_info.coor[DOPPLER_SITE_COORD_Y]);
    printf("z: %.12f m\n", site_info.coor[DOPPLER_SITE_COORD_Z]);

    /* height above geoid (mean sea level) in meters, 
                               UNUSED */

    printf("alt: %.12f m\n", site_info.altitude);


    /* (station freq standard - true)/true, 
                               of order 1e-12 */

    printf("foffset: %.12f\n", site_info.foffset);
}