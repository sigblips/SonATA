/*******************************************************************************

 File:    astro.h
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
 * astro.h - declaration of functions defined in astro.h
 * PURPOSE:  
 *****************************************************************/

#ifndef ASTRO_H
#define ASTRO_H

#ifdef __cplusplus
extern "C" {
#endif

#define RTOD            57.295779513082321
#define DTOR               0.0174532925199432
/* radians to hours */
#define RTOH 		(RTOD *24.0 / 360.0)
/* hours to radians */
#define HTOR		(M_PI / 12.0)

const double HzPerMHz = 1e6;

extern
void dmstorad(int torad, char *sign, int *deg, int *min, double *sec, double *rad);
extern
void julday(int year, int month, int day,
	    int hour, int min, double sec,
	    double *t2000, double *jd);
extern
double timetoJ2000(time_t ttime);
extern
time_t J2000totime(double t0);
extern
void sun_position(
		  double tJ2000,
		  double *solar_ra,
		  double *solar_dec,
		  double *solar_dist);
extern
int rise_set(time_t ttime, double ra, double dec,
	     int site, time_t *rise, time_t *transit,
	     time_t *setting, double *sid_rise, double *sid_transit,
	     double *sid_setting,
	     int *flags, double lon, double lat, double elev);

extern
void coord_trans (double *alt, double *az, double h, double dec, double lt);
extern
double slewtime (time_t timer, double ra1, double dec1, double ra2, double dec2);
#ifdef __cplusplus
}
#endif

#endif /* ASTRO_H */