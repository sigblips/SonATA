/*******************************************************************************

 File:    ast_const.h
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

/* 	ast_const.h
 *	
 *	$Header: /home/cvs/nss/sse-pkg/ssecommutil/ast_const.h,v 1.1 2002/11/23 18:36:31 nss Exp $
 *
 *	$Log: ast_const.h,v $
 *	Revision 1.1  2002/11/23 18:36:31  nss
 *	initial port of beamstuff.c code from SCS
 *
 *	Revision 1.2  1997/04/22 00:10:52  kevin
 *	protect definition of PI
 *	
 *	Revision 1.1  1996/09/26 10:07:48  jane
 *	Initial revision
 *
 * Revision 1.1  95/02/06  11:59:08  11:59:08  jane (Jane Jordan)
 * Initial revision
 * 

	various arcane constants used in astronomy
	supports precess package

01/22/95 JWD	made by pulling out the constants from precess.h, precession.h, precess.work.c

*/

#ifndef PI
#define PI 		3.14159265358979323846	/* from HP math.h */
#endif
#define TWOPI 		(2 * PI)
#define DToR 		(PI / 180)
#define SToR 		(DToR / 3600)		/* seconds of arc to radians */
#define AU 		149597870.66		/* km. Differs from IAU76, see ESAA 92 Table 15.2 */
#define JDJ2000         2451545			/* julian date of epoch J2000 */
#define CEE             299792.458		/* speed of light, km/s */
#define GRAVCONST       6.67259e-11 		/* from Table 15.2 ESAA, m^3 kg^-1 s^-2 */
#define MSUN            1.9891e30 		/* kg */
#define EARTHPOLRADIUS  6356752.0 		/* polar radius of earth, m */
#define EARTHEQARADIUS  6378136.0 		/* equatorial radius of earth, m */
#define MEARTH          5.9742e24 		/* kg */
#define OMEGA           7.292115018e-5 		/* rotational angular velocity of earth, rad/s */
						/* inertial, from Meeus p79, epoch 1989.5 */
#define DABBCON         0.3200 			/* constand of diurnal aberration, arcsec */