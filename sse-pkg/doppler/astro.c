/*******************************************************************************

 File:    astro.c
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

/*++  astro.c
 *
 *	$Header: /home/cvs/nss/sse-pkg/doppler/astro.c,v 1.2 2003/09/25 20:06:46 jane Exp $
 *
 *	$Log: astro.c,v $
 *	Revision 1.2  2003/09/25 20:06:46  jane
 *	Comments added.
 *
 *	Revision 1.1  2003/02/06 04:37:07  lrm
 *	mods to build shared object libraries
 *	
 *	Revision 1.3  2002/09/27 02:39:24  kevin
 *	* lib/astro.c, lib/astro.h: removed functions which are already
 *	defined in precession.c
 *	include precession.h, since some of these functions are used.
 *	
 *	Revision 1.2  2002/09/26 21:59:58  kevin
 *	* lib/astro.c, lib/astro.h: removed functions which are already
 *	defined in precession.c
 *	
 *	Revision 1.1  2002/07/29 22:25:37  kevin
 *	* lib/astro.c:  astronomical routines to TSS.
 *	
 *	Revision 5.5  1998/08/27 16:40:49  jane
 *	Add a day for rise/set check.
 *
 * Revision 5.4  98/08/20  13:52:47  13:52:47  jane (Jane Jordan)
 * New routines debugged.
 * 
 * Revision 5.3  98/08/19  16:19:06  16:19:06  jane (Jane Jordan)
 * New routines: get_rise_set(), get_rise_set_ha(), read_limits(), sort_dec()
 * to compute rise/set times using antenna limits.
 * 
 * Revision 5.2  97/05/20  00:48:00  00:48:00  kevin (Kevin Dalley)
 * * lib/astro.c, include/astro.h : remove LAT and LONG defines, add
 * const and void where appropriate, delete unused variables, delete
 * riseset which is obsolete, moved slewtime to arecibo.c
 * 
 *	Revision 5.1  1997/01/17 03:49:00  kevin
 *	somewhat ANSI-fy C code
 *	remove need for include/TSSascii.h and replace static variables in
 *	include/TSSascii.h with functions using static variables in
 *	lib/TSSasccii.c, a new file
 *
 *	Revision 5.0  1996/08/28 11:49:20  scs
 *	Greenbank Baseline.
 *
 * Revision 4.3  95/03/06  16:36:59  16:36:59  gary (Gary Heiligman)
 * Added Obliquity, EvalPoly, PrecessFK5, and sun_position,
 * all taken from the Meeus libraries.
 * 
 * Revision 4.2  95/03/02  17:35:29  17:35:29  gary (Gary Heiligman)
 * NO changes.
 * 
 * Revision 4.1  95/02/03  16:49:20  16:49:20  jane (Jane Jordan)
 * Sidereal rise/set added to rise_set.
 * 
 * Revision 4.0  95/01/03  16:43:02  16:43:02  jane (Jane Jordan)
 * OZ Baseline.
 * 
 * Revision 2.1  94/05/26  10:45:09  10:45:09  jane (Jane Jordan)
 * Observatory info read from database.
 * 
 * Revision 2.0  93/08/24  15:43:02  15:43:02  scs (Version Control)
 * "Build1"
 * 
 * Revision 1.1  92/10/21  10:43:38  10:43:38  jane (Jane Jordan)
 * Initial revision
 * 
 * Revision 1.1  92/10/04  18:22:13  18:22:13  jane (Jane Jordan)
 * Initial revision
 * 
 * Revision 2.0  92/09/25  15:47:50  15:47:50  scs (scs software)
 * "Arecibo Baseline"
 * 
 *
   Astronomy routines extracted from star.reach4.c

   Keith Richardson
   Sterling Federal Systems
   Coding Date:  10/22/92
   
   
   AUTHORS:	Keith D. Richardson, Sterling Federal Systems
   		Overall design and coding.
		Gary Heiligman, Sterling Federal Systems
		Design and coding of parts of astronomical routines.
   
   FUNCTIONS:
   
   julday		gh, kdr - converts UT1 time to TDT
   SiderealTime		gh, kdr - calcs Julian centuries since J2000
   rise_set		gh, kdr	- calcs star rise and set
   modpi		gh, kdr - reduces angles to between -pi and +pi
   modpi2		gh, kdr - reduces angles to between 0 and 2 pi
   coord_trans		gh, kdr - converts hour angle to az, el
   
*/   
   
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "astro.h"
#include "precession.h"

/* astronomical defines */
#ifndef PI
#define PI		3.1415926535897932	/* pi, to HP double signif  */
#endif
#define TWOPI		6.2831853071795865	/* 2 pi, to HP double signif */
#define DTOR		0.0174532925199432
#define RTOD		57.295779513082321
#define SToR		DTOR / 3600.0		/* converts arcsec to radians */
#define AZRATE	 	143.2394		/* az seconds/radian 	*/
#define ZARATEUP	1718.8734		/* ZA seconds/radian uphill */
#define ZARATEDOWN	1432.3945		/* ZA seconds/radian downhill */
#define SIDRATE		1.0027379093508056	/* sidereal days per solar day*/
#define SIDPERCENTURY	(1 / TWOPI / SIDRATE / 36525)
					/* sidereal radians per Julian century*/

#define MAX_REACH_RECORDS 3000	/*  Default number reachability records  */
#define SEC_IN_22_HOURS 79200  	/*  Number seconds in 22 hrs 		*/



time_t system_time, rise_time, set_time, transit_time, tt2;
int visible_is_1;
long read_limits();

/*++	dmstorad --	converts degrees, minutes, and seconds to radians
   *
   * PURPOSE:	Utility routine for calculating angles
 *
 * AUTHOR:	Gary M. Heiligman, Sterling Federal Systems
 *
 * REVISION HISTORY:
 *	04/13/92	gh	initially coded
 *
 * FILES USED:		None
 * FUNCTION RETURN:	None
 *
 * INPUT PARAMETERS:
 *	torad		int value	if = 0, converting from radians to
 *					DMS; otherwise, from DMS to radians
 * PARAMETERS:
 *	sign	char	ref	sign of angle
 *	deg	int	ref	degrees of arc
 *	min	int	ref	minutes of arc
 *	sec	double	ref	seconds of arc
 *	rad	double	ref	radians of arc
 *
 * STANDARDS VIOLATIONS:	None
 * NOTES:			None
 *
 --*/
void dmstorad(int torad, char *sign, int *deg, int *min, double *sec, double *rad)
{
	double absdeg;

	if ( torad ) {
		*rad = ((fabs( *sec ) / 60.0 + (double)(abs( *min ))) / 60.0 +
			(double)(abs( *deg ))) * DTOR;
		if ( *deg < 0 || *min < 0 || *sec < 0.0 || *sign == '-' )
			*rad *= -1;
	}
	else {
		if ( *rad < 0 ) {
			absdeg = -(*rad) * RTOD;
			*sign = '-';
		}
		else {
			absdeg = (*rad) * RTOD;
			*sign = '+';
		}
		*deg = floor( absdeg );
		*min = floor( 60.0 * (absdeg - (double)*deg) );
		*sec = 60.0 * (60.0 * (absdeg - (double)*deg) - (double)*min);
	}
}
/*++	julday --	converts yyyy/mm/dd hh:mm:ss.s UT to c.cccc... TDT
 *
 * PURPOSE:	converts date and time in UT1 
 *		into the interval elapsed from J2000.0 in centuries TDT
 *
 * AUTHOR:	Gary M. Heiligman, Sterling Federal Systems
 *
 * REVISION HISTORY:
 *	04/03/92	gh	initially coded
 *
 * FILES USED:		None
 * FUNCTION RETURN:	None
 *
 * INPUT PARAMETERS:
 *	year	int 	value	calendar year (Gregorian if >1582 Oct 15) 
 *	month	int	value	calendar month, 1-12
 *	day	int	value	calendar day, 1-31
 *	hour	int	value	UT hours, 0-23
 *	min	int	value	UT minutes, 0-59
 *	sec	double	value	UT seconds, 0-59.9999999
 *
 * OUTPUT PARAMETERS:
 *	t2000	double	ref	Julian 2000.0 date in centuries TDT
 *	jd	double	ref	Julian day number TDT
 *
 * GLOBAL VARIABLES:		None
 * FUNCTIONS USED:		None
 * CALLED BY:
 *	main()
 * STANDARDS VIOLATIONS:	None
 *
 --*/
void julday(int year, int month, int day,
	    int hour, int min, double sec,
	    double *t2000, double *jd)
{
	int a, b, y, m;
	double jd1, tod;

	if (month > 2) {
		y = year;
		m = month;
	}
	else {
		y = year - 1;
		m = month + 12;
	}
	a = y / 100;
	b = (y >= 1582 || (y == 1582 && (m >= 10 || (m == 10 && day >= 4)))) ?
		2 - a + a / 4 : 0;
	jd1 = floor(365.25 * (double)(y + 4716)) +
		floor(30.6001 * (double)(m + 1)) + (double)(day + b) - 1524.5;
	tod = (double)hour / 24.0 + (double)min / 1440.0 + sec / 86400.0;
	*t2000 = (jd1 - 2451545.0 + tod) / 36525.0;
	*jd = jd1 + tod;
}
/*++ timetoJ2000	--	Converts time formats
 *
 * PURPOSE:	Converts times to J2000 format (double, Julian centuries
 *		since 2000.0) from time() format (time_t, seconds since 1970.0)
 *
 * AUTHOR:	Gary Heiligman, Sterling Federal Systems
 *
 * REVISION HISTORY:
 *	8/31/92	 gh	initially written
 *
 * STANDARDS VIOLATIONS:	None
 * GLOBAL VARIABLES:	None
 *
 * INPUT PARAMETERS:
 *	ttime	value	time_t	time in time() format
 *
 * FUNCTION RETURN:
 *	double	= time in J2000 format
 *
 * OUTPUT PARAMETERS:	None
 * FUNCTIONS CALLED:	None
 *
 --*/

double timetoJ2000(time_t ttime)
{
                            /* Seconds per Julian Century  = 3155760000 */
			    /* Centuries since 1/1/1970 = .3     */
	return ((double)ttime / 3155760000.0 - 0.3);
}

time_t J2000totime(double t0)
{
	return (time_t)((t0 + 0.3) * 3155760000.0 + 0.5);
}

/* sun_position() -- calculates the sun's position (low precision)
 *
 * PURPOSE: calculate's sun's position in coordinates of epoch.
 *
 * INPUT VARIABLE:
 *    double	tJ2000		Julian centuries since 2000.0
 * OUTPUT VARIABLES:
 *    double*	solar_ra	sun's RA in radians
 *    double*	solar_dec	sun's Dec in radians
 *    double*	solar_dist	sun's distance in AU    
 * FUNCTIONS CALLED:
 *    Obliquity() in lib/astro.c
 * NOTES:  taken from SunPosLo() in astro/sunposlo.c
 *
 --*/
void sun_position(
		  double tJ2000,
		  double *solar_ra,
		  double *solar_dec,
		  double *solar_dist)
{
   double L, M, C, e, e2, Lon, Obl;

   Obl = Obliquity(tJ2000);
   L = (280.46646 + tJ2000 * (36000.76983 + tJ2000 * 0.0003032)) * DTOR;
   M = (357.52910 + tJ2000 * (35999.05028 - tJ2000 * 0.0001561)) * DTOR;
   e = 0.016708617 - tJ2000 * (0.000042040 + tJ2000 * 0.0000001236);
   e2 = e * e;
   /* Equation of the center, in terms of e and M */
   C = e * (2 - 0.25 * e2) * sin (M)
      + 1.25 * e2 * sin (2 * M)
      + 1.0833333333 * e * e2 * sin (3 * M);
   Lon = modpi2(L + C);
   *solar_ra = modpi2(atan2(cos(Obl) * sin(Lon), cos(Lon)));
   *solar_dec = asin(sin(Obl) * sin(Lon));
   *solar_dist = 1.0000002 * (1 - e2) / (1 + e * cos(M + C));
}

/*++ rise_set.c	-- Determines the rise and set time for stars
 *
 * PURPOSE:	Determines whether or not a star is up or not.  If the star is
 *		up, riseset() returns the rise, transit, and set times of this
 *		appearance.  If not, it returns the rise, transit, and set
 *		times of the next appearance.
 *
 * AUTHOR:	Gary Heiligman, Sterling Federal Systems
 *
 * REVISION HISTORY:
 *	8/31/92		gh	initially written
 *
 * STANDARDS VIOLATIONS:	None
 * GLOBAL VARIABLES:	None
 *
 * INPUT PARAMETERS:
 *	ttime	value	time_t	Date and time in seconds since 1970.0
 *	ra	value	double	Right Ascension in radians at epoch ttime
 *	dec	value	double	Declination in radians at epoch ttime
 *	site	value	int	Number of site--	0 :  Arecibo
 *							1 :  Goldstone
 *							2 :  Tidbinbilla
 *				site is no longer used, it has been replaced
 *				lon, lat, and elev, see below
 *	lon	value	double	longitude of site
 *	lat	value	double	latitude of site
 *	elev	value	double	elevation of site
 * FUNCTION RETURN:
 *	int	= 0	star is not visible at time ttime
 *		= 1	star is visible at time ttime
 *
 * OUTPUT PARAMETERS:
 *	rise	ref	time_t	Date and time of rising
 *	transit	ref	time_t	Date and time of transit
 *	setting	ref	time_t	Date and time of setting
 *	flags	ref	int	Flags special situations--
 *					+1 : object is always above horizon
 *					-1 : object is always below horizon
 *
 * FUNCTIONS CALLED:
 *	modpi2()	see slewtest.c
 *	SiderealTime()	"
 *	timetoJ2000()
 *	J2000totime()
 *
 --*/

int
rise_set(time_t ttime,
	 double ra,
	 double dec,
	 int site,
	 time_t *rise,
	 time_t *transit,
	 time_t *setting,
	 double *sid_rise,
	 double *sid_transit,
	 double *sid_setting,
	 int *flags,
	 double lon,
	 double lat,
	 double elev)
{
	double ha, cosha, lmst, lstrise, lstset, t0, riset0, transt0, sett0;
	int visible, thru0;
	long ha_rise,ha_set;
	time_t sid;
	/*
	 * Get the hour angle for rise and set
	 */
	cosha = (sin(elev) - sin(dec) * sin(lat)) / (cos(dec) * cos(lat));
	if (cosha < -1.0) {
		*flags = 1;
		ha = 0;
	}
	else if (cosha > 1.0) {
		*flags = -1;
		ha = 0;
	}
	else {
		*flags = 0;
		ha = acos(cosha);
	}
	/*
	 * Get the sidereal rise and set times
	 */
	lstrise = modpi2(ra - ha + TWOPI);
	lstset = modpi2(ra + ha);
	thru0 = lstrise > lstset;
	/*
	 * Determine the local sidereal time
	 */
	t0 = timetoJ2000(ttime);
	lmst = modpi2(SiderealTime(t0) - lon + TWOPI);
	/*
	 * Is the target currently visible?  Here is a logic table
	 * lstrise > lstset	lmst > lstrise	lmst < lstset	visible
	 *	T		T		T		---
	 *	T		T		F		T
	 *	T		F		T		T
	 *	T		F		F		F
	 *	F		T		T		T
	 *	F		T		F		F
	 *	F		F		T		F
	 *	F		F		F		---
	 */
	if (thru0) {
		visible = (lmst > lstrise) || (lmst < lstset);
	}
	else {
		visible = (lmst > lstrise) && (lmst < lstset);
	}
	/*
	 * The next set time is always the correct one to use
	 */
	sett0 = SIDPERCENTURY * modpi2(lstset - lmst + TWOPI) + t0;
	/*
	 * Get the rise and transit times
	 */
	transt0 = sett0 - SIDPERCENTURY * ha;
	riset0 = sett0 - 2 * SIDPERCENTURY * ha;

	*sid_rise = lstrise;

	*sid_transit = lmst;

	*sid_setting = lstset;

	*rise = J2000totime(riset0);

	*transit = J2000totime(transt0);

	*setting = J2000totime(sett0);

	ha_rise = *rise - *transit;
	ha_set = *setting - *transit;

	return(visible);
}


/*  Library function that returns a slew time between two stars
*   given: ra and dec in radians, and the Julian Date in centuries.
*   This code is written for HP 9000s in Standard C.
*   
*   Keith Richardson, Gary Heiligman
*   Created July 30, Aug 6, 1992
*   
*   This was abstracted from code by the following and may require their
*   permission prior to use:  Jeffrey Sax; Willmann-Bell, Inc.		
*   P.O. Box 35025; Richmond, VA 23235	(804) 320-7016
*/

   
   /*  Extracted from program slewtest.c for callib.h  kdr 9/14/92 
      Convert coordinates from hour angle to az and al
      Input:  dec (declination); h, (hour angle); lt (LATitide)
      Output: az (azimuth); alt (altitude)
      */
void coord_trans (double *alt, double *az, double h, double dec, double lt)
{
      double y, x, cos_h, sin_lt, cos_dec, cos_lt;
      cos_h = cos (h);
      sin_lt = sin (lt);
      cos_dec = cos (dec);
      cos_lt = cos (lt);
      y = -cos_dec * sin (h);
      x = sin (dec) * cos_lt - cos_dec * cos_h * sin_lt;
      *az = atan2 (y, x);
      if (*az < 0)
	 *az += 2 * PI;	
      *alt = asin(sin(dec) * sin_lt + cos_dec * cos_h * cos_lt);	    
   }
	 