/*******************************************************************************

 File:    test_doppler_nodb.c
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

/*++ doppler_test.c -- test of doppler code
 *
 --*/
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "local-libdop-subset.h"

/*extern long debug; */

void fill_database_parms(
     target_data_type *star_info,
     site_data_type *main_site_info,
     site_data_type *remote_site_info);

int main(void)
{
   long starnum;
   time_t obs_date;
   struct Doppler_Parameters doppler_parameters;
   struct NSS_BC_TIME obs_time;

   target_data_type star_info;
   site_data_type main_site_info;
   site_data_type remote_site_info;
   civil_time_type utc_time;
   char earth_ephem_file[80];
   double dut1;
   int debug;

   obs_time.tm_nsec = 0;      /* nanoseconds */
   obs_time.tm_sec = 0;       /* seconds */
   obs_time.tm_min = 0;       /* minutes */
   obs_time.tm_hour = 0;      /* hours */
   obs_time.tm_yday = 0;      /* year day */
   /* obs_time.tm_year = 0; */  /* year */

   /* global debug flag for get_doppler_parms */
   debug = 3;

   /*time(&obs_date);*/
   obs_date=1021504629;  /* May 2002 */

   printf("obs_date is %d\n", obs_date);

#if 0
   printf( "Enter SETI Star Number >>" );
   scanf( "%d", &starnum );
#endif
   starnum = 4023;

   printf( "Enter SETI Star Number >> %d\n", starnum);

#if 0
   get_doppler_parms( starnum, obs_date, &obs_time, &doppler_parameters, 0.0 );
#endif

     strcpy( earth_ephem_file, "./earth.xyz" );

     dut1 = 0.0;
     compute_utc_time(obs_date, &obs_time, dut1, &utc_time);
     if( debug )list_civil_time(utc_time);

     fill_database_parms(&star_info, &main_site_info, &remote_site_info);

     printParameters(starnum, star_info, main_site_info, remote_site_info);

     compute_doppler(star_info, main_site_info, remote_site_info, 
			    utc_time, earth_ephem_file, &doppler_parameters, debug);






#if 0
   printf( "f_ratio %.12f\nDrift Offset factor %.6e\n",
	       doppler_parameters.f_ratio, doppler_parameters.drift_offset_factor );
   printf( "main curvature %.6e\nremote curvature %.6e\n",
	       doppler_parameters.curv_main_factor, doppler_parameters.curv_remote_factor );
#endif
   return 0;
}

void fill_database_parms(
     target_data_type *star_info,
     site_data_type *main_site_info,
     site_data_type *remote_site_info)
{
/*  star 4023 */
star_info->type=1; /* 1 == STAR, 2 == SPACECRAFT */
star_info->ra2000=1.952187129382;
star_info->dec2000= 0.091205573759;
star_info->origin = 1;  /* 1 == BARYCENTRIC, 2 == GEOCENTRIC */
star_info->parallax =  0.264400000000;
star_info->pmra =  0.573000000000;
star_info->pmdec = -3.716000000000;
star_info->range = 1.1670490093e+14;

/* site info: main site -- Arecibo */
main_site_info->coor[0] = 2390453.850000000100;  /* X */
main_site_info->coor[1] =  -5564816.460000000000; /* y */
main_site_info->coor[2] =  1994663.399999999900; /* z */
main_site_info->altitude = 0.000000000000;
main_site_info->foffset = 0.000000000001;

/* site info: remote site - Jodrell Bank */
remote_site_info->coor[0] =  3822633.000000000000;
remote_site_info->coor[1] =  -154108.799999999990;
remote_site_info->coor[2] =  5086486.269999999600;
remote_site_info->altitude =  0.000000000000;
remote_site_info->foffset =  0.000000000000;

}