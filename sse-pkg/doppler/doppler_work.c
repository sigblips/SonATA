/*******************************************************************************

 File:    doppler_work.c
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

/* doppler.work.c

 Routines used by doppler.main.c to do the actual nasty stuff.
 get_target_data() is used to get position of a spacecraft, usually
  seen from geocenter and always in J2000 coordinates.
  (Data for stars are entered directly via the user inferface).
 geocentric() does the conversions to geocentric apparent place and calculates
  the various doppler factors.
 topocentric() does the conversions from geocentric to topocentric apparent 
  place and calculates the various doppler factors.
 some utility routines at the end for handling vectors and ra/dec

95 Jan 16 JWD julian day and mean sidereal time.  GMST agrees with AA
  at 0 h UT1 94 Apr 1 to within 1 msec.
95 Jan 17 JWD adding ephemeris stuff
95 Jan 18 JWD ephemeris seems to work, but needs more testing.  Adding parallax.
95 Jan 19 JWD adding aberration and geocentric doppler
95 Jan 20 JWD aberration checks for test case against 'star' app on my Mac
95 Jan 21 JWD tidied up a bit.  Putting in grav and lorentz terms for finf calc, topocentric 
  mod star.parallax so that negative values indicate range in km, so that
  we can test diurnal parallax for spacecraft.  Getting plausible values.
95 Jan 22 JWD increased size of xyz[] to carry acceleration too.  Finished diurnal stuff.
95 Jan 23 JWD accelerations in, seem reasonable.  Mods for spacecraft extensions to cat
95 Jan 24 JWD modified rect_to_radec and radec_to_rect to add range
  added get_target_data, corrected bug in use of dut
95 Jan 25 JWD added vector_mag
  added spacecraft portion of doppler/drift
95 Jan 27 JWD epoch of coords now filled in by get_target_data
  separated geocentric and topocentric routines
  installed app_place structs to return values calculated
  adding topocentric 'curvature' calculation
95 Jan 29 JWD modified stat_coor, now in site_data_type struct, also clock error
  added verbosity control
95 feb 02 JWD brought gecoentric unaberrated RA and Dec to vverbosity 1  
97 may 13 JWD kludged limited precesion fix to add topocentric diurnal 
	parallactic doppler term to topocentric().  Added unit_vector().  
	Calculate relative vel and acc in geocentric() and pass through 
	new memebers in struct app_place

*/

#include "doppler_work.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ast_const.h"
#include "precession.h"
#include "readephem.h"


static int unit_vector( double v[], double n[] );


/*++ int get_target_data(target_data_type *tar,
    civil_time_type time, char *targetfile)
 Purpose: reads a target ephemeris whose stream pointer is given, and
 fills in the appropriate bits of a given target_data struct
 ***************************************************************************** 
 * get_target_data() 
 reads a target ephemeris whose stream pointer is given, and
 fills in the appropriate bits of a given target_data struct
* 
 ***************************************************************************** 
 --*/

#define stateVectLen 12

int get_target_data(
       target_data_type *tar, 
       civil_time_type time, 
       char *targetfile)
 {

 int status;  /* return status from functions */
 long jday;  /* julian day number of obs epoch */
 double jsec;  /* seconds from noon UTC at obs epoch */
    /* this is +- half a day from noon */
 double xyz[stateVectLen];  /* target position (km), vel (km/s), accel (km/s/s) */
 double ra;  /* RA, radians, J2000, unaberrated */
 double dec;  /* Dec, radians, J2000, unaberrated */
 double radius;  /* radius to target, km */

 jday = juldayfromdate(time.year, time.month, time.day);
 jsec = time.sec + 60 * (time.min + 60 * time.hour) - 12*60*60;
		/* should convert to JD ET instead of UTC */
 status = position( targetfile, jday, jsec, xyz ); 
 if ( status == -1 )
  {
  (void)printf("Couldn't find target position!\n");
  return( status );
  }
 rect_to_radec( xyz, &ra, &dec, &radius );
 tar->type = 2;
 tar->ra2000 = ra;
 tar->dec2000 = dec;
 tar->range = radius;
 if( radius != 0.0 ) tar->parallax = SToR * (AU / radius);
 tar->pmra = 0.0;
 tar->pmdec = 0.0;
 tar->epoch = jday + jsec/86400;
 tar->vel[0] = xyz[3]; /* NAIF package just subtracts */
 tar->vel[1] = xyz[4]; /* barycentric from earth barcentric */
 tar->vel[2] = xyz[5]; /* when forming geocentric vels.  Should */
 tar->acc[0] = xyz[6]; /* use relativistic formula */
 tar->acc[1] = xyz[7];
 tar->acc[2] = xyz[8];
 tar->lightTimeSec = xyz[9];
 tar->rangeKm = xyz[10];
 tar->rangeRateKmSec = xyz[11];

 return( 0 );
 } /* end get_target_data() */


/********************************************************************************/
/* geocentric()        */
/*  Corrects a star from catalog position (mean at epoch J2000) to geocentric */
/*  position of date, including aberration and parallax corrections.  */
/*  Calculates doppler shift and derivatives.     */
/*  Uses precession stuff from Meeus, found in precession.c,   */
/*  and which I have tested lightly, ie      */
/*          */
/* For FK5 #464 at 1APR94 transit agrees with Apparent Places to  */
/* 0.01 arcsec, after applying short term nutation to AP   */
/* tabulated value following prescription in AP explanation.  */
/*          */
/*  For Pioneer 11 12 oct 94 20h UTC, I get a predicted frequency (starting */
/*  from the same assumed f0) that agrees with the DSN predicts for Goldstone */
/*  to 14 Hz.  As far as I can tell, the error is theirs: it _looks_ like */
/*  they are using         */
/* (1 + Ve/c)/(1+ Vp11/c) ~ 1 + Ve/c - Vp11/c = 1 + (Ve - Vp11)/c  */
/*  which has an error of order (V/c)^2 ~ 1e-8.     */
/********************************************************************************/

/*++ int geocentric(target_data_type star, civil_time_type time,
 char *ephem_file, app_place_type *stargc, int verbosity )
     Purpose: Corrects a star from catalog position (mean at epoch J2000)
      to geocentric position of date, including aberration and parallax
      corrections. Calculates doppler shift and derivatives.
  --*/
int geocentric(
    target_data_type star,
    civil_time_type time,
    char *ephem_file,
    app_place_type *stargc,
    int verbosity )
 {
 int status;  /* return status from functions */
 long jday;  /* julian day number of obs epoch */
 double jsec;  /* seconds from noon UTC at obs epoch */
    /* this is +- half a day from noon */
 double jsec1;  /* seconds from noon UT1 at obs epoch */
 double xyz[9];  /* Earth position (km), vel (km/s), accel (km/s/s) */
 double  rbt;        /* distance from barycenter to target in km */
 double  vbt;        /* speed of target in barcentric frame, km/s */
 double  tb[3];		/* position vector of target wrt barycenter 
				in J2000 coords, km */
 double  tbv[3];	/* velocity vector of target wrt barycenter 
				in J2000 coords, km/s */
 double  tba[3];	/* accel vector of target wrt barycenter in 
				J2000 coords, km/s/s */
 double  pvct;          /* p*V/c factor for target */
 double dra;  /* correction in RA */
 double ddec;  /* correction in Dec */
 double ra;  /* RA being worked on */
 double  dec;  /* Dec being worked on */
 double tcentury; /* centuries from epoch J2000 */
 double xau;  /* x coordinate of earth in AU */
 double yau;  /* y coordinate of earth in AU */
 double zau;  /* z coordinate of earth in AU */
 double range_gc; /* distance of spacecraft from geocenter, km */
 double re;  /* distance of earth from barycenter */
 double ve;  /* speed of earth */
 double Usun;  /* gravitational potential energy at earth from sun */
 double Uearth;  /* gravitational potential energy at geoid from earth */
 double Ug;  /* gravitational potential energy of observer on geoid */
 double p[3];  /* unit vector towards target, rect equatorial coords */
 double pvc;  /* p*V/c factor for earth */
 double dop_bary_to_geo; /* multiply finf by this factor to get fgeocentric */
 double drift_bary_to_geo; /* Multiply finf by this to get drift rate at geocent Hz/s */
 double drift_bary_to_tar; /* Multiply finf by this to get drift rate at target Hz/s */
 double  dop_bary_to_tar; /* multiply finf by this factor to get f0 */
 double finf_pred;  /* Predicted freq referred to 'infinity' MHz */
 double fgc_pred;  /* Predicted geocentric freq MHz */
 double driftinf_pred;  /* Predicted drift rate referred to 'infinity' Hz/s */
 double driftgc_pred;  /* Predicted geocentric drift rate Hz/s */
 double gamma;   /* Lorentz factor = (1 -v^2/c^2)^(-.5) */
 double pabb[3];  /* abberated position unit vector */
 double ra2000_gc;  /* apparent geocentric ra of date in J2000 coords */
 double dec2000_gc;  /* apparent geocentric dec of date in J2000 coords */
 double ra_mean_gc;  /* geocentric ra in mean coords of date */
 double dec_mean_gc;  /* geocentric dec in mean coordinates of date */
 double dummy;   /* dummy for misc uses */
 int  rahrs;
 int  ramin;
 double  rasec;
 int  decdeg;
 int  decmin;
 double  decsec;
 double gmst;   /* Greenwich Mean Sidereal Time, radians */
 int gsthrs;
 int gstmin;
 double gstsec;
 double nut_in_longitude; /* nutation in longitude, radians */
 double nut_in_obliquity; /* nutation in obliquity, radians */
 double mean_obliquity;  /* mean obliquity of ecliptic, radians */
 double true_obliquity;  /* true obliquity of ecliptic, radians */
 double EQ;  /* Equation of the Equinoxes, radians */
 double gast;  /* Greenwich Apparent Sidereal Time, radians */
 double gaha;  /* Greenwich Apparent Hour Angle, radians */

 /* work out some date and time stuff */

 jday = juldayfromdate(time.year, time.month, time.day);
 jsec = time.sec + 60 * (time.min + 60 * time.hour) - 12*60*60;
 jsec1 = jsec + time.dut; /* correct from UTC to UT1 */
 tcentury = (jday - JDJ2000)/36525.0 + jsec1/(36525.0*(24*60*60));
       /* 36525 days in a Julian century */
 if( verbosity >= 2 )
  {
  (void)printf("\nJulian Day beginning at noon is %12ld\n", jday);
  (void)printf("Seconds from noon, UTC = %12.3f \n", jsec);
  }
 if( verbosity >= 3 )
  {
  (void)printf("Seconds from noon, UT1 = %12.3f \n", jsec1);
  (void)printf("Centuries from J2000 epoch (UT1) = %20.13f \n\n", tcentury);
  }

 if( verbosity >= 1 ) 
  {
  rad_to_hms( star.ra2000, &rahrs, &ramin, &rasec);
  rad_to_dms( star.dec2000, &decdeg, &decmin, &decsec );
  (void)printf(
  "\nBarycentric  RA, Dec at epoch = %12.8f, %12.8f rad\n", star.ra2000, 
	      star.dec2000 );
  (void)printf("   Barycentric RA  is %4d %4d %7.3f (J2000)\n", 
   rahrs, ramin, rasec );
  (void)printf("   Barycentric Dec is %4d %4d %7.3f (J2000)\n", 
   decdeg, decmin, decsec );
  }
 /* correct position of target for proper motion */

 if( star.type == STAR )  /* if target is a star */
  {
  dra = star.pmra * 100 * tcentury; /* pmra is in arcsec per year */
  if( verbosity >= 2 )
   (void)printf("  Proper motion in RA is %10.5f arcsec \n", dra);

  dra = dra * SToR;   /* convert asec to radians */
  if( verbosity >= 3 )
   (void)printf("  Proper motion in RA is %15.9f radians\n", dra);

  ddec = star.pmdec * 100 * tcentury; /* pmdec is in arcsec per year */
  if( verbosity >= 2 )
   (void)printf("  Proper motion in Dec is %10.5f arcsec\n", ddec);

  ddec = ddec * SToR;   /* convert arcsec to radians */
  if( verbosity >= 3 )
   (void)printf("  Proper motion in Dec is %15.9f radians\n\n", ddec);

  ra = star.ra2000 + dra;   /* at epoch, wrt to mean 2000 coodinates */
  dec = star.dec2000 + ddec;  /* at epoch, wrt to mean 2000 coodinates */
  } 					/* end proper motion correction */
 else if( star.type == SPACECRAFT )
  {
  if( fabs( star.epoch - (jday + jsec/86400) ) > 1e-6 )
   (void)printf("\nWARNING, doppler_work: epoch = %20.6f, target epoch = %20.6f\n",
    star.epoch, (jday + jsec/86400) );
  ra = star.ra2000;
  dec = star.dec2000;
  }
 else
  {
  (void)printf("\nWARNING, doppler_work() found illegal star.type = %d\n", star.type);
  return( -1);
  }
 /* find position and velocity of earth with respect to barycenter */

 status = position( ephem_file, jday, jsec, xyz );
 if ( status == -1 )
  {
  (void)printf("Couldn't find earth's position!\n");
  return( status );
  }
 if( verbosity >= 3 )
  {
  (void)printf(
  " Earth position is:     %15.1f %15.1f %15.1f km \n", 
   xyz[0], xyz[1], xyz[2]);
  (void)printf(" Earth velocity is:     %15.7f %15.7f %15.7f km/s \n", 
   xyz[3], xyz[4], xyz[5]);
  (void)printf(" Earth acceleration is: %15.5g %15.5g %15.5g km/s/s \n\n", 
   xyz[6], xyz[7], xyz[8]);
  }

 /* correct position and range of target to geocenter in J2000 coords */

	       /* input was barycentric and target is star */
 if ( ( star.origin == BARYCENTRIC) && (star.type == STAR) ) 
  { /* correct star for annual parallax */
  xau = xyz[0] / AU ; /* convert from km to AU */
  yau = xyz[1] / AU ; /* convert from km to AU */
  zau = xyz[2] / AU ; /* convert from km to AU */
  /* approximate method, see p 125 of ESAA.  Should be accurate enough */
  dra = star.parallax * ( xau*sin(ra) - yau*cos(ra) )/( cos(dec) );
  if (verbosity >= 3)
   (void)printf("  stellar annual parallax in RA = %10.3f arcsec\n", dra);
  dra = dra * SToR; /* arcsec to radians */
  ddec = star.parallax * (xau*cos(ra)*sin(dec) + yau*sin(ra)*sin(dec) - zau*cos(dec));
  if (verbosity >= 3)
   (void)printf("  stellar annual parallax in Dec = %10.3f arcsec\n", ddec);
  ddec = ddec * SToR; /* arcsec to radians */
  ra = ra + dra;
  dec = dec + ddec;
  range_gc = star.range; /* range unchanged for star */
  } /* end correct star for annual parallax */
 /* 
  * input barycentric and target is spacecraft 
  */
 if ( ( star.origin == BARYCENTRIC) && (star.type == SPACECRAFT) ) 
  { /* correct spacecraft for parallax */
  radec_to_rect(ra, dec, star.range, p);
  p[0] = p[0] - xyz[0];
  p[1] = p[1] - xyz[1];
  p[2] = p[2] - xyz[2];
  rect_to_radec( p, &ra, &dec, &range_gc );
  dra = modpi( ra - star.ra2000);
  ddec = modpi( dec - star.dec2000 );
  if( verbosity >= 3)
   {
   (void)printf(
   " target position with respect to geocenter:  %15.1f %15.1f %15.1f km \n",
   p[0], p[1], p[2]);
   printf(
   "  geocentric vector =  %15.1f  %15.1f  %15.1f km \n", p[0], p[1], p[2] );
   (void)printf(" range = %15.1f km \n", range_gc);
   (void)printf("  orbital parallax in RA  = %.6g radian\n", dra);
   (void)printf("  orbital parallax in Dec = %.6g radian\n", ddec);
   }
  } /* end correct spacecraft for parallax */

 if( star.origin == SPACECRAFT )  /* target coords already geocentric */
  range_gc = star.range;

  stargc->unab_ra = ra;
  stargc->unab_dec = dec;
 /* at this point, geocentric unaberrated RA and Dec of date in J2000 coordinates are */
 /* in variables 'ra' and 'dec', range in 'range_gc' */
 
 if( verbosity >= 1 ) 
  {
  rad_to_hms( ra, &rahrs, &ramin, &rasec);
  rad_to_dms( dec, &decdeg, &decmin, &decsec );
  (void)printf(
  "\nGeocentric true (unaberrated) RA, Dec at epoch = %12.8f, %12.8f rad\n", 
   ra, dec );
  (void)printf("   Geocentric true RA  is %4d %4d %7.3f (J2000)\n", 
   rahrs, ramin, rasec );
  (void)printf("   Geocentric true Dec is %4d %4d %7.3f (J2000)\n", 
   decdeg, decmin, decsec );
  }


 /* Find the geocentric doppler shift. The relationship between 
				       the received frequency */
 /* at earth fe and finf, the frequency 'at infinity', 
				ie as it would be measured by an  */
 /* observer far enough away from the solar system to be at 
				a negligble gravitational */
 /* potential energy and at rest with respect to the 
				solar system barycenter (which is  */
 /* NOT the same as the heliocenter or the Local Standard of Rest), is    */
 /*   fe = finf( 1 - U/c^2 + (1/2)(V^2/c^2) + V*p/c )    */
 /* where U is the potential energy of the earth, 
				V is the velocity vector of the earth, */
 /* c is the speed of light, p is the unit vector 
				in the un-aberrated direction of the */
 /* transmitter, and * represents the vector dot product.  This relationship  */
 /* is accurate in the post-Newtonian approximation to General Relativity.   */
 /* The magnitude of the gravitational term at earth orbit is about   */
 /* 1.0e-8 and the time dilation is about 4.9E-9.  
				The eccentricity of the orbit is  */
 /* .016, producing an annual term of about 2e-10(less than 1 Hz).     */
 /* The largest error in this calculation is probably the 
				limited accuracy of the */
 /* tabular ephemerides we use for the Earth position.  
				Direct interface to the DE200 JPL*/
 /* would improve the accuracy.        */

 re = vector_mag( xyz );  /* earth from bayrcenter, km */
 if( verbosity >= 3 )
  (void)printf("\n  earth is %.10e km from barycenter\n", re);
 re = re * 1000.0;  /* convert to meters */
 Usun = -1*GRAVCONST*MSUN/re; /* use radius to barycenter 
				for convenience, error is small */
 if( verbosity >= 3 )
  (void)printf("  potential energy at re from sun is %.5g J/kgm \n", Usun);
 Uearth = -1*GRAVCONST*MEARTH/EARTHPOLRADIUS;
  /* use polar radius of the geoid, 
			Lorentz factor from rotational velocity will just  */
  /* compensate for variation of U when observer is on the geoid, 
				and at pole vrot = 0  */
 if( verbosity >= 3 )
  (void)printf(
  "  potential energy at geoid from earth is %.5g J/kgm \n", Uearth);
 Ug = Usun + Uearth;
 /* 
  * earth orbital speed 
  */
 ve = sqrt( xyz[3]*xyz[3] + xyz[4]*xyz[4] + xyz[5]*xyz[6] ); 
 if( verbosity >= 3 )
  (void)printf("  earth's speed is %20.4f km/s \n", ve);

 radec_to_rect( ra, dec, range_gc, p ); /*get ra, dec, range into vector form*/
 p[0] = p[0]/range_gc;
 p[1] = p[1]/range_gc;
 p[2] = p[2]/range_gc;

 /* at this point, p is a unit vector from geocenter to 
			target without aberration */

 pvc = ( xyz[3]*p[0] + xyz[4]*p[1] + xyz[5]*p[2] )/ CEE;   /* p*V/c factor */
 dop_bary_to_geo = 1.0 - Ug/(CEE*CEE*1e6) + 0.5*ve*ve/(CEE*CEE) + pvc;
 if( verbosity >= 2 )
  {
  (void)printf("\nDoppler factor: multiply finf by %.12f to get fgeocentric.\n", 
   dop_bary_to_geo);
  (void)printf("  -Usun/c^2 term =     %+.5g \n", -1*Usun/(CEE*CEE*1e6) );
  (void)printf("  -Uearth/c^2 term =   %+.5g \n", -1*Uearth/(CEE*CEE*1e6) );
  (void)printf("   (1/2)(V/C)^2 term = %+.5g \n", 0.5*ve*ve/(CEE*CEE) );
  (void)printf("   p*V/c term =        %+.5g \n", pvc );
  }

 /* and the earth's acceleration produces a frequency drift rate */

 drift_bary_to_geo = (xyz[6]*p[0] + xyz[7]*p[1] + xyz[8]*p[2])/CEE;
 if( verbosity >= 2 )
  (void)printf("\nDrift: multiply finf by %+.5g to get drift rate at geocenter.\n", 
   drift_bary_to_geo);

 /* for a spacecraft, we also need to calculate the 
				equivalent finf from f0, the */
 /* transmitter frequency measured in the frame of the craft.  
				This is just the reverse */
 /* of the above calculation we made for the earth.  Also, 
				the acceleration of the */
 /* spacecraft produces another doppler drift term.  From these, 
				we predict the  */
 /* frequency and drift rate seen at the geocenter, which will be 
				further modified */
 /* to produce the topocentric values (actually observed at telescope). */

 if ( star.type == STAR)  /* target is a star */
  {
  fgc_pred = -1.0;  /* this value tells topocentric() not to predict ftopo */
  driftgc_pred = 0.0;  /* just to have a place holder */
   /* need to fill in velocities for star, even though they are zero */

   if( star.origin == BARYCENTRIC )  /* if target coordinates were barycentric (normal) */
  {
   tbv[0] = 0.0;           /* we don't know star vel with enough accuracy  */
   tbv[1] = 0.0;           /* to be useful, so we adopt convention that    */
   tbv[2] = 0.0;           /* star is at rest in barycenter and just give  */
   tba[0] = 0.0;           /* user the freq stuff corrected to barycentric */
   tba[1] = 0.0;
   tba[2] = 0.0;
  }
 else if( star.origin == GEOCENTRIC )  /* target coordinates were geocentric (unusual) */
 { 
  (void)printf("\nDoppler:Warning: Star has geocentric origin specified!\n");
  tbv[0] = xyz[3];
  tbv[1] = xyz[4];
  tbv[2] = xyz[5];
  tba[0] = xyz[6];
  tba[1] = xyz[7];
  tba[2] = xyz[8];
 }
 }
 else if( star.type == SPACECRAFT )  /* target is a spacecraft */
  { /* begin spacecraft doppler stuff */
  
  if( star.origin == BARYCENTRIC )  /* if target coordinates were barycentric */
   {
   rbt = star.range;
   tbv[0] = star.vel[0];
   tbv[1] = star.vel[1];
   tbv[2] = star.vel[2];
   vbt = vector_mag( tbv );
   tba[0] = star.acc[0];
   tba[1] = star.acc[1];
   tba[2] = star.acc[2];
   }
  else if( star.origin == GEOCENTRIC ) /* target coordinates were geocentric */
   {
   radec_to_rect( star.ra2000, star.dec2000, star.range, tb);
   tb[0] = tb[0] + xyz[0];
   tb[1] = tb[1] + xyz[1];
   tb[2] = tb[2] + xyz[2];
   rbt = vector_mag( tb );
   tbv[0] = star.vel[0] + xyz[3]; /* NB, this should really be the */
   tbv[1] = star.vel[1] + xyz[4]; /* relativistic velocity addition */
   tbv[2] = star.vel[2] + xyz[5]; /* formula.  However, NAIF uses this */
   vbt = vector_mag( tbv );
   tba[0] = star.acc[0] + xyz[6];
   tba[1] = star.acc[1] + xyz[7];
   tba[2] = star.acc[2] + xyz[8];
   if( verbosity >= 3 )
    {
    (void)printf("\nTarget barycent position:%15.1f %15.1f %15.1f km\n", 
     tb[0], tb[1], tb[2]);
    (void)printf("Target barycent velocity:%15.7f %15.7f %15.7f km/s\n", 
     tbv[0], tbv[1], tbv[2]);
    (void)printf("Target barycent accel:   %15.5g %15.5g %15.5g km/s/s\n", 
     tba[0], tba[1], tba[2]);
    }
   }
  else
   {
   (void)printf("\nWARNING, doppler_work: star.origin = %d is invalid\n", 
    star.origin);
   return( -1);
   }
  if( verbosity >= 3 )
   {
   (void) printf("  target is %.10e km from barycenter\n", rbt);
   (void)printf("  target's speed is %20.4f km/s in barycenter frame \n", vbt);
   }
  rbt = rbt * 1000.0; /* convert to meters */
         Usun = -1*GRAVCONST*MSUN/rbt;
          /* use radius to barycenter for convenience, error is small */
         if( verbosity >= 3 )
          (void)printf(
	  "  potential energy at rbt from sun is %.5g J/kgm \n", Usun);
  pvct = ( tbv[0]*p[0] + tbv[1]*p[1] + tbv[2]*p[2] )/ CEE; /* p*V/c factor */
         if( verbosity >= 3 )
   (void)printf(
   "  radial velocity wrt earth = %.10g km/s\n", (pvct - pvc) * CEE );

  dop_bary_to_tar = 1.0 - Usun/(CEE*CEE*1e6) + 0.5*vbt*vbt/(CEE*CEE) + pvct;
  if( verbosity >= 3 )
   {
   (void)printf("\nTarget doppler factor: multiply finf by %.12f to get f0.\n", 
    dop_bary_to_tar);
   (void)printf("  -Usun/c^2 term =     %.5g \n", -1*Usun/(CEE*CEE*1e6) );
   (void)printf("   (1/2)(V/C)^2 term = %.5g \n", 0.5*vbt*vbt/(CEE*CEE) );
   (void)printf("   p*V/c term =        %.5g \n", pvct );
   }

  drift_bary_to_tar = (tba[0]*p[0] + tba[1]*p[1] + tba[2]*p[2])/CEE;
  if( verbosity >= 3 )
   (void)printf(
   "\nTarget drift: multiply finf by %+.5g to get drift rate at target.\n", 
    drift_bary_to_tar);

  finf_pred = star.f0 / dop_bary_to_tar;
  if( verbosity >= 3 )
   (void)printf("\n    predicted freq referred to 'infinity' = %15.8f MHz\n", 
    finf_pred);
  fgc_pred = finf_pred * dop_bary_to_geo;
  if( verbosity >= 3 )
   (void)printf("Predicted geocentric freq = %15.8f MHz\n", fgc_pred);
  driftinf_pred = finf_pred * drift_bary_to_tar * 1e6;
  if( verbosity >= 3 )
   printf("    predicted drift rate referred to 'infinity' = %.5g Hz/s \n", 
    driftinf_pred);
  driftgc_pred = (drift_bary_to_geo - drift_bary_to_tar) * finf_pred * 1e6;
  if( verbosity >= 3 )
   printf("Predicted geocentric drift rate = %.5g Hz/s \n", driftgc_pred);
  } /* end spacecraft doppler stuff */

 /* next correct for special relativistic aberration from motion of geocenter */
 /* this ugly formula comes from 3.252-3 on p 129 of the ESAA, revised */

 gamma = 1.0 / sqrt( 1 - ve*ve/(CEE*CEE) );
 pabb[0] = p[0]/gamma + xyz[3]/CEE + pvc*(xyz[3]/CEE)/(1.0 + 1.0/gamma);
 pabb[1] = p[1]/gamma + xyz[4]/CEE + pvc*(xyz[4]/CEE)/(1.0 + 1.0/gamma);
 pabb[2] = p[2]/gamma + xyz[5]/CEE + pvc*(xyz[5]/CEE)/(1.0 + 1.0/gamma);
 pabb[0] = pabb[0]/(1.0 + pvc);
 pabb[1] = pabb[1]/(1.0 + pvc);
 pabb[2] = pabb[2]/(1.0 + pvc);
 rect_to_radec( pabb, &ra2000_gc, &dec2000_gc, &dummy ); 
  /*pabb is unit vector, r = 1, actual range still in range_gc and unaffected */
 /* print out correction */
 dra = ra2000_gc - ra;
 ddec = dec2000_gc - dec;
 if( verbosity >= 2 )
  (void)printf("\nAberration is %+8.3f arcsec, %+8.3f arcsec in RA and Dec\n",
   dra/SToR, ddec/SToR );

 /* print out the resulting apparent geocentic position of date in J2000 coordinates */

 if( verbosity >= 2 )
  {
  rad_to_hms( ra2000_gc, &rahrs, &ramin, &rasec);
  rad_to_dms( dec2000_gc, &decdeg, &decmin, &decsec );
  (void)printf("\nGeocentric RA and Dec at epoch in J2000 frame = %12.8f, %12.8f rad\n", 
   ra2000_gc, dec2000_gc );
  printf("Geocentric RA  is %4d %4d %7.3f (J2000)\n", rahrs, ramin, rasec );
  printf("Geocentric Dec is %4d %4d %7.3f (J2000)\n", decdeg, decmin, decsec );
  printf("Geocentric range = %20.10e km \n", range_gc);
  }

 /* calculate mean sidereal time */

 gmst = SiderealTime(tcentury);
 if( verbosity >= 2 )
  {
  rad_to_hms( gmst, &gsthrs, &gstmin, &gstsec);
  (void)printf("\nGreenwich Mean Sidereal Time = %15.9f radians \n", gmst);
  (void)printf("Greenwich Mean Sidereal Time = %4d %4d %7.3f \n", 
   gsthrs, gstmin, gstsec );
  }

 /* calculate nutation and obliquity of ecliptic */

 NutationConst(tcentury, &nut_in_longitude, &nut_in_obliquity);
 mean_obliquity = Obliquity(tcentury);
 true_obliquity = mean_obliquity + nut_in_obliquity;
 if( verbosity >= 3 )
  {
  (void)printf("\n  nutation, long = %+8.3f arcsec, obl = %+8.3f arcsec\n", 
   nut_in_longitude/SToR, nut_in_obliquity/SToR);
  (void)printf("  Mean obliquity = %15.9f radians \n", mean_obliquity);
  (void)printf("  True obliquity = %15.9f radians \n", true_obliquity);
  }

 /* calculate apparent sidereal time */

 EQ = nut_in_longitude * cos( true_obliquity );
 if( verbosity >= 3 )
  (void)printf("Equation of Equinoxes = %.9g radians \n", EQ);
 if( verbosity >= 2 )
  (void)printf("Equation of Equinoxes = %10.4f sec \n\n", EQ*43200/PI );
 gast = gmst + EQ;
 if( verbosity >= 3 )
  {
  (void)printf("Greenwich Apparent Sidereal Time = %15.9f radians \n", gast);
  rad_to_hms( gast, &gsthrs, &gstmin, &gstsec);
  (void)printf("Greenwich Apparent Sidereal Time = %4d %4d %7.3f \n", 
   gsthrs, gstmin, gstsec );
  }

 /* change apparent RA and Dec from J2000 coords to coordinates of date */

 ra_mean_gc = ra2000_gc;
 dec_mean_gc = dec2000_gc;
 /* Precess on FK5 system from mean equatorial coords at t=0 to t=tcentury (UT1) */
 PrecessFK5( 0.0, tcentury, &ra_mean_gc, &dec_mean_gc ); 
 dra = ra_mean_gc - ra2000_gc;
 ddec = dec_mean_gc - dec2000_gc;
 if( verbosity >= 2 )
  {
  (void)printf("Precession is %10.3f arcsec, %10.3f arcsec in RA and Dec\n",
   dra/SToR, ddec/SToR );
  (void)printf("\nGeocentric RA and Dec at epoch in mean coords of date = %12.8f, %12.8f rad\n", 
   ra_mean_gc, dec_mean_gc );
  }
  
 /* correct for nutation into apparent equatorial coords of date */
 ra = ra_mean_gc;
 dec = dec_mean_gc;
 Nutation( nut_in_longitude, nut_in_obliquity, true_obliquity, &ra, &dec );
 /* ra and dec now contain apparent coordinates of date, range not changed */

 dra = ra - ra_mean_gc;
 ddec = dec - dec_mean_gc;
 if( verbosity >= 2 )
  {
  (void)printf("\nNutation is %10.3f arcsec, %10.3f arcsec in RA and Dec\n",
   dra/SToR, ddec/SToR );
  (void)printf("\nGeocentric RA and Dec in apparent coords of date = %12.8f, %12.8f rad\n", 
   ra, dec );
  rad_to_hms( ra, &rahrs, &ramin, &rasec);
  (void)printf("Geocentric apparent RA of date is  %4d %4d %7.3f \n", 
   rahrs, ramin, rasec );
  rad_to_dms( dec, &decdeg, &decmin, &decsec );
  (void)printf("Geocentric apparent Dec of date is %+4d %4d %7.3f \n", 
   decdeg, decmin, decsec );
  }
 gaha = modpi(gast - ra); /* Greenwich Apparent Hour Angle, radians, -pi to pi */
 if( verbosity >= 3 )
  (void)printf("Greenwich Apparent Hour Angle = %20.9f radians \n", gaha);
 
 /* finally, put results into app_place sturcture */
 
 stargc->view = 2;   /* viewpoint is geocenter */
 stargc->epoch = jday + jsec/86400; /* julian date for which calcs made */
 stargc->gast = gast;
 stargc->ra = ra;
 stargc->dec = dec;
 stargc->range = range_gc;
 stargc->doppler = dop_bary_to_geo;
 stargc->drift = drift_bary_to_geo;
 stargc->curve = 0.0;   /* curvature should be negigible for geocenter */
 stargc->fpred = fgc_pred;
 stargc->dpred = driftgc_pred;
 stargc->ha = gaha;
 stargc->vrel[0] = tbv[0] - xyz[3]; /* need rel vel to do topo right */
 stargc->vrel[1] = tbv[1] - xyz[4];
 stargc->vrel[2] = tbv[2] - xyz[5];
 stargc->arel[0] = tba[0] - xyz[6]; /* need rel accel to do topo right */
 stargc->arel[1] = tba[1] - xyz[7];
 stargc->arel[2] = tba[2] - xyz[8];
 if( verbosity >= 3 )list_app_place( *stargc );
 return(0);
 } 



/************************************************************************/
/*  topocentric        */
/*  calculates the topocentric (or 'diurnal') corrections to position */
/*  and doppler frequency, drift, and acceleration, using geocentric */
/*  apparent inputs.  state_coor contains the terrestrial coordinates */
/*  of the observer (telescope) in the ITRF system, in meters.          */
/*         */
/*  results are placed into app_place struct 'topo'   */
/************************************************************************/

/*++ int topocentric( app_place_type geo, site_data_type stat, 
          app_place_type *topo, int verbosity ) 
      Purpose: calculates the topocentric (or 'diurnal') corrections 
      to position and doppler frequency, drift, and acceleration, 
      using geocentric apparent inputs.  state_coor contains the 
      terrestrial coordinates of the observer (telescope) in 
      the ITRF system, in meters 
 --*/ 
int topocentric( 
      app_place_type geo, 
      site_data_type stat, 
      app_place_type *topo, 
      int verbosity )
 {
 double gast; /* Greenwich Apparent Siderial Time, radians */
 double  gx; /* x coord of observer in equatorial coords of date, m */
 double  gy; /* y coord of observer in equatorial coords of date, m */
 double  gz; /* z coord of observer in equatorial coords of date, m */
 double  gvx; /* x coord of obs velocity, m/s */
 double  gvy; /* y coord of obs velocity, m/s */
 double  gax; /* x coord of obs acceleration, m/s/s */
 double  gay; /* y coord of obs acceleration, m/s/s */
 double  gjx; /* x coord of obs jerk, m/s/s/s */
 double  gjy; /* y coord of obs jerk, m/s/s/s */
 double  longitude;  /* longitude of observer, radians from -pi to pi */
 double pgc[3];   /* geocentric target position vector, coords of date */
 double range_topo;  /* topocentric target range, km */
 double  ngc[3];    /* geocentric target position unit vector, coords of date */
 double ptopo[3];  /* topocentric target position vector, coords of date */
 double pvct;   /* topocentric p*V/c factor for doppler */
 double  ntopo[3]; /* topocentric target position unit vector, coords of date */
 double ratopo;   /* topocentric ra of date, radians */
 int rathrs, ratmin;  /* topocentric ra of date, hours, minutes */
 double ratsec;   /* topocentric ra of date, seconds of time */
 double dectopo;  /* topocentric dec of date, radians */
 int dectdeg, dectmin; /* topocentric dec of date, degreess, arcmin */
 double dectsec;  /* topocentric dec of date, arcsec */
 double secdec;   /* sec(dec) */
 double dop_geo_to_topo; /* topocentric doppler factor */
 double drift_geo_to_topo; /* topocentric doppler drift factor */
 double curvature;  /* topocentric doppler track curvature factor */
 double ftopo_pred;  /* predicted spaceraft received frequency, MHz */
 double drifttopo_pred;  /* predicted spaceraft drift rate, Hz/s */
 double curve_pred;  /* predicted spacecraft track curvature, Hz/s/s */
 double laha;   /* topocentric apparent hour angle */
 int hathrs, hatmin;  /* topocentric HA of date, hours, min */
 double hatsec;   /* topocentric HA of date, seconds of time */
double  dop;                    /* v/c term */
double  ddop;                   /* a/c term */


 /* Calculate where observer is, in equatorial coordinattes of date.  */
 /* We ignore polar motion for now; error will be up to 15m in position,  */
 /* but for stations a few hundred km apart, differential is <1 m and acceptable.*/
 /* Correct procedure is        */
 /*      gr = R3(theta)R1(y)R2(x)r    */
 /* where gr is the observer position vector in the frame of the true equator */
 /* and equinox of date, theta is the apparent siderial time, x and y are the  */
 /* coordinates of the actual pole of date (from IERS Bulletins), r is the ITRF */
 /* position vector, and R()is the appropriate rotation matrix.  ESAA p 140 */

 gast = geo.gast;
 gx = stat.coor[0]*cos( gast ) - stat.coor[1]*sin( gast );
 gy = stat.coor[0]*sin( gast ) + stat.coor[1]*cos( gast );
 gz = stat.coor[2];
 if( verbosity >= 3 )
  {
  (void)printf("\n  observer ITRF coords = %+13.3f %+13.3f %+13.3f m\n",
   stat.coor[0], stat.coor[1], stat.coor[2] );
  (void)printf("  observer equat coords of date = %+13.3f %+13.3f %+13.3f m\n",
   gx, gy, gz );
  }

 /* find the velocity, accel, and jerk of the observer caused by the earth's rotation */
 /* coodinates are aligned with earth's axis at epcoh of observation, z terms are thus all 0 */

 gvx = OMEGA * -1.0 * gy; /* actually, OMEGA varies slightly with time */
 gvy = OMEGA * gx;
 gax = -1.0 * OMEGA * OMEGA * gx;
 gay = -1.0 * OMEGA * OMEGA * gy;
 gjx = OMEGA * OMEGA * OMEGA * gy;
 gjy = -1.0 * OMEGA * OMEGA * OMEGA * gx;
 if( verbosity >= 2 )
  {
  (void)printf("Observer velocity =     %.3f %.3f 0.000  m/s \n", gvx, gvy );
  (void)printf("Observer acceleration = %.5g %.5g 0.000  m/s/s \n", gax, gay );
  (void)printf("Observer jerk =         %.5g %.5g 0.000 m/s/s/s \n", gjx, gjy );
  }

 /* put apparent geocentric target position into rectangular form,
	  frame of date */

 radec_to_rect( geo.ra, geo.dec, geo.range, pgc );
 (void) unit_vector( pgc, ngc );         /* should check return, really */

 /* now diurnal parallax correction for apparent position */

 if( geo.range < 1e13 ) /* if close enough to matter (spacecraft)
	(1E13 km = 1e5 AU = 0.5 pc) */
  {
  if( verbosity >= 2 )(void)printf("\nShifting pgc to ptopo\n");
  ptopo[0] = pgc[0] - gx/1000.0; /* shift origin to topocenter */
  ptopo[1] = pgc[1] - gy/1000.0; /* in km */
  ptopo[2] = pgc[2] - gz/1000.0;
  }
 else     /* diurnal parallax negligible (stars) */
  {
  ptopo[0] = pgc[0];
  ptopo[1] = pgc[1];
  ptopo[2] = pgc[2];
  }
 (void)unit_vector( ptopo, ntopo );      /* should check return, really */
 rect_to_radec( ptopo, &ratopo, &dectopo, &range_topo );

 if( verbosity >= 2 )
  (void)printf("\nDiurnal parallax correction = %.3f %.3f arcsec\n",
   (ratopo - geo.ra)/SToR, (dectopo - geo.dec)/SToR );
  
 /* Next we need to correct the doppler caused by the relative velocity of */
 /* the target and the geocenter for the difference in the projection angle */
 /* between the lines of sight from the geocenter and the topocenter.  This */
 /* correction would be needed even if the earth did not rotate. */
 /* Here I do something very ugly.  I use the velocity and accel vectors */
 /* expressed in components in the coordinate frame J2000, but with the */
 /* displacement vector for the topocenter in the coordinate frame of the */
 /* epoch of observation.  This is a kludge to provide a quick fix (shudder). */
 /* The frame of date precesses by roughly 1e-4 radian per year, so the */
 /* fractional error for the decade befoe and after J2000 will be of order */
 /* 1e-3 radian.  Since the frequency shift produced by this term is of order */
 /* 1 Hz, the error caused by the frame mismatch will be of order 1 mHz */
 /* I need to clean this up when I convert all the coordinate manipulations */
 /* to vector/matrix form!!! */

        dop =     (ntopo[0] - ngc[0])*geo.vrel[0]
		+ (ntopo[1] - ngc[1])*geo.vrel[1]
		+ (ntopo[2] - ngc[2])*geo.vrel[2];
	dop = -1.0 * dop / CEE;         /* doppler factor, vrel is in km/s */
	dop_geo_to_topo = 1.0 + dop;
	ddop =    (ntopo[0] - ngc[0])*geo.arel[0]
		+ (ntopo[1] - ngc[1])*geo.arel[1]
		+ (ntopo[2] - ngc[2])*geo.arel[2];
	ddop = -1.0 * ddop / CEE;     /* doppler drift factor, arel in km/s/s */
	drift_geo_to_topo = ddop;

 if( verbosity >= 2 )
	 {
	 (void)printf(
	 "\nDiurnal paralactic doppler: multiply fgc by %.12f to get ftopo.\n",
			dop_geo_to_topo);

	 (void)printf(
"Diurnal paralactic drift: multiply fgc by %+.5g to get drift rate at scope.\n", 
		drift_geo_to_topo);
	 if( geo.fpred > 0.0  )          /* if we have a predicted gc freq */
	    { 
	     (void)printf(
	     "  diurnal paralactic delta freq  = %+.3f Hz.\n",
			1e6 * geo.fpred * dop);
	     (void)printf(
	     "  diurnal paralactic delta drift = %+.6f Hz/s.\n", 
			1e6 * geo.fpred * ddop);
	    }         
	 }
 /* Doppler caused by rotation of the earth. */
 /* We have already taken care of the potential at the geoid, as well */
 /* as the Lorentz factor on the geoid.  All we need is the V/c term and the */
 /* gravitational term for observer height above the geoid */
 /* For now, I ignore the height, since it's calculation from ITRF coords is */
 /* fairly messy. (ESAA p206ff). The error introduced will be < 1e-12. */
 /* have added height to site_data, but not yet implemented gH/c^2 correction */

 pvct = (gvx*ntopo[0] + gvy*ntopo[1])/(1000.0 * CEE );
		 /* gvz is 0, CEE in km/s */
 dop_geo_to_topo = (1.0 + pvct)*dop_geo_to_topo;
 drift_geo_to_topo = drift_geo_to_topo
		+ (gax*ntopo[0] + gay*ntopo[1] )/(1000.0 * CEE );
	/* gaz is 0, CEE in km/s */
 curvature = (gjx*ptopo[0] + gjy*ptopo[1] )/(1000.0 * CEE * range_topo);
 if( verbosity >= 3 )
  {
  (void)printf("\nDiurnal doppler: multiply fgc by %.12f to get ftopo.\n", 
   dop_geo_to_topo);
  (void)printf("Diurnal drift: multiply fgc by %+.5g to get drift rate at telescope.\n", 
   drift_geo_to_topo);
  (void)printf("Track curvature: multiply fgc by %+.5g to get curvature at telescope.\n", 
   curvature);
  }
 
 if( geo.fpred > 0.0  )  /* target is spacecraft, so we generated predicted gc freq */
  {
  ftopo_pred = geo.fpred * dop_geo_to_topo;
  if( verbosity >= 3 )
   (void)printf("\nPredicted topocentric true freq = %15.8f MHz\n", ftopo_pred);
  if( stat.foffset != 0.0 ) /* if we know the site clock rate error */
   { /* correct to the frequency that will be reported */
   ftopo_pred = ftopo_pred * (1.0 - stat.foffset);
   if( verbosity >= 3 )
    (void)printf("\nPredicted topocentric apparent freq = %15.8f MHz\n", 
     ftopo_pred); /* correction of order 1e-12 fractionally */
   }
  /* sum topocentric drift rate with geocentric for total drift */
  drifttopo_pred = geo.dpred + geo.fpred * drift_geo_to_topo * 1e6;
  curve_pred = geo.fpred * curvature * 1e6; 
   /* curvature from orbital motion can be neglected */
  if( verbosity >= 3 )
   {
   (void)printf("Predicted total drift rate = %.5g Hz/s \n", drifttopo_pred);
   (void)printf("Predicted track curvature = %.5g Hz/s/s \n", curve_pred);
   }
  } 

 /* calculate local apparent hour angle. */
 
 longitude = modpi( atan2( stat.coor[1], stat.coor[0]) );
 if( verbosity >= 2 )
  (void)printf("\nObserver longitude = %+10.8f radian, east positive (IAU convention)\n", 
   longitude );

 laha = modpi( gast - ratopo + longitude ); /* longitude measured to east */
 if( verbosity >= 3 )
  (void)printf("Topocentric apparent hour angle is %12.8f radians\n", laha);
 if( verbosity >= 3 )
  { 
  rad_to_hms( laha, &hathrs, &hatmin, &hatsec);
  (void)printf("Topcentric apparent HA is %4d %4d %7.3f \n", 
    hathrs, hatmin, hatsec );
  }

 /* and finally diurnal aberration, for both stars and craft, using a crude */
 /* approximation, but adequate given that the whole effect is < 1 arcsec */

 secdec = cos( dectopo );
 if( secdec > 1e-6 )
  {
  double dra;
  double ddec;

  secdec = 1.0/secdec;
  dra = DABBCON * (sqrt(gx*gx + gy*gy) / EARTHEQARADIUS) * cos(laha) * secdec; 
  ddec = DABBCON * (sqrt(gx*gx + gy*gy) / EARTHEQARADIUS) * sin(laha) * sin(geo.dec);
  if( verbosity >= 2 )
   (void)printf("\nDiurnal aberration = %.3g %.3g arcsec\n\n", dra, ddec);
  ratopo = ratopo + dra*SToR;
  dectopo = dectopo +ddec*SToR;
  }
 else
  {
  (void)printf("Topocentric: can't apply diurnal aberration, too near pole!\n");
  }

 if( verbosity >= 3 )
  (void)printf("Topocentric RA and Dec in apparent coords of date = %12.8f, %12.8f rad\n", 
   ratopo, dectopo );
 if( verbosity >= 3 )
  {
  rad_to_hms( ratopo, &rathrs, &ratmin, &ratsec);
  (void)printf("Topcentric apparent RA is %4d %4d %7.3f \n", 
   rathrs, ratmin, ratsec );
  rad_to_dms( dectopo, &dectdeg, &dectmin, &dectsec );
  (void)printf("Topocentric apparent Dec is %4d %4d %7.3f \n", 
   dectdeg, dectmin, dectsec);
  }
 
 /* finally, put results into app_place sturcture */
 
 topo->view = 3;   /* viewpoint is topocenter */
 topo->epoch = geo.epoch; /* julian date for which calcs made */
 topo->gast = geo.gast;
 topo->ra = ratopo;
 topo->dec = dectopo;
 topo->range = range_topo;
 topo->doppler = dop_geo_to_topo;
 topo->drift = drift_geo_to_topo;
 topo->curve = curvature;
 topo->fpred = ftopo_pred;
 topo->dpred = drifttopo_pred;
 topo->ha = laha;

 if( verbosity >= 3 )list_app_place( *topo );
 return( 0 );
 } /* end topocentric() */


/************************************************************************/
/* juldayfromdate                                                       */
/* Adapted from http://en.wikipedia.org/wiki/Julian_day formula.        */
/************************************************************************/
long juldayfromdate( int yr, int mn, int dy)
{
    int a = (14 - mn)/12;
    int y = yr + 4800 - a;
    int m = mn + 12*a - 3;
    long jdn = dy + ((153*m+2)/5) + 365*y + y/4 - y/100 + y/400 -32045;
    return jdn;
}

/************************************************************************/
/*  vector_mag         */
/*  returns magnitude of a vector      */
/************************************************************************/

/*++ double vector_mag( double v[] )
    returns magnitude of a vector                                       *
 --*/
double vector_mag( double v[] )
 {
 return( sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] ) );
 } /* end radec_to_rect() */

/************************************************************************/
/*  unit_vector                                                         */
/*  puts unit vector into second arg                                    */
/************************************************************************/

static int unit_vector( double v[], double n[] )
   {
    double mag;

    mag = vector_mag(v);
    if( mag <= 1e-6 )
	    {
           fprintf(stderr,
	       "\nunit_mag: ERROR, vector has mag = %.5g <= 1e-6\n", mag);
           return( -1 );
            }

     n[0] = v[0]/mag;
     n[1] = v[1]/mag;
     n[2] = v[2]/mag;
     return( 0 );
   }  /* end unit_vector() */

/**************************************************************************/
/*  radec_to_rect         */
/*  converts RA and Dec in radians and range to x, y, z                   */
/**************************************************************************/

/*++ void radec_to_rect( double ra, double dec, double r, double vector[] )
    converts RA and Dec in radians and range to x, y, z                   *
 --*/
void radec_to_rect( 
     double ra, 
     double dec, 
     double r, 
     double vector[] )
 {
 vector[0] = r*cos(dec)*cos(ra);
 vector[1] = r*cos(dec)*sin(ra);
 vector[2] = r*sin(dec);
 return;
 } /* end radec_to_rect() */

/**************************************************************************/
/*  rect_to_radec         */
/*  converts x, y, z to RA and Dec in radians and range     */
/**************************************************************************/

/*++ int rect_to_radec( double vector[], double *ra, double *dec, double *r )
    converts x, y, z to RA and Dec in radians and range                   *
  --*/
int rect_to_radec( 
 double vector[], 
 double *ra, 
 double *dec, 
 double *r )
 {

 *r = vector_mag( vector );
 if( *r < 1e-6 )
  {
  (void)printf("\nERROR, rect_to_radec given a zero length vector!\n");
  return( -1 );
  }
 *ra = atan2( vector[1], vector[0] );
 if ( *ra < 0.0 ) *ra = *ra + TWOPI; /* ra by convention is 0 to two PI */
 *dec = asin( vector[2]/(*r) );
 return( 0 );
 } /* end rect_to_radec() */

/************************************************************************/
/*  rad_to_dms        */
/*  converts angle theta in radians to deg min arcsec   */
/************************************************************************/

/*++ void rad_to_dms( double theta, int *deg, int *arcmin, double *arcsec)
    converts angle theta in radians to deg min arcsec                   *
 --*/
void rad_to_dms( 
     double theta, 
     int *deg, 
     int *arcmin, 
     double *arcsec)
 {
 int sign; /* sign of theta */

 sign = 1;
 if( theta < 0.0 )
  {
  sign = -1;
  theta = -1.0 * theta;
  }
 theta = theta/DToR; /* convert theta to degrees */
 *deg = (int)theta;
 theta = 60.0 * (theta - *deg); /* remainder in arcmin */
 *arcmin = (int)theta;
 *arcsec = 60.0 * (theta - *arcmin); /* remainder in arcsec */
 *deg = sign * (*deg);   /* pass sign to deg part */

 return;
 } /* end rad_to_dms() */

/************************************************************************/
/*  rad_to_hms        */
/*  converts angle theta in radians to hours, minutes, seconds  */
/************************************************************************/

/*++ void rad_to_hms( double theta, int *hrs, int *min, double *sec)
    converts angle theta in radians to hours, minutes, seconds          *
 --*/
void rad_to_hms( 
    double theta, 
    int *hrs, 
    int *min, 
    double *sec)
 {
 theta = theta/15;   /* 15 deg = 1 hr, 15 arcmin = 1 minute etc */
 rad_to_dms( theta, hrs, min, sec);
 return;
 }
/************************************************************************/

/*++ void list_target_data( target_data_type cat )
 --*/
void list_target_data( target_data_type cat )
{
    printf("\nRA and Dec (J2000) = %12.8f, %12.8f rad\n", cat.ra2000, cat.dec2000);
    if( cat.origin == 1 )
     printf("   Origin at barycenter.\n");
    else if ( cat.origin == 2 )
     printf("   Origin at geocenter.\n");
    else printf("\n  Illegal origin = %d \n", cat.origin);
    printf("Proper motion in RA and Dec = ");
    printf("%+7.3f asec/yr, %+7.3f asec/yr\n", cat.pmra, cat.pmdec);
    if( cat.type == 1 )
     printf("Stellar Parallax = %7.3f arcsec\n", cat.parallax);
    else if( cat.type == 2 )
 {
 printf("At JD %15.5f \n", cat.epoch );
 printf("  Spacecraft Range = %.10g km\n", cat.range);
 printf("  Velocity =     %+11.6f %+11.6f %+11.6f km/s J2000\n",
  cat.vel[0], cat.vel[1], cat.vel[2] );
   printf("  Acceleration = %+.5g %+.5g %+.5g km/s/s J2000\n",
  cat.acc[0], cat.acc[1], cat.acc[2] );
   printf("  Transmitter at %13.8f MHz in its own frame.\n", cat.f0);
 }
    return;
} /* end list_catalog_data */

/************************************************************************/

/*++ void list_civil_time( civil_time_type time )
 --*/
void list_civil_time( civil_time_type time )
{
    printf("UTC date is ");
    printf("%5d %3d %3d\n", time.year, time.month, time.day );
    printf("  UTC time = ");
    printf("%3d %3d %7.3f\n", time.hour, time.min, time.sec);
    printf("  UT1 - UTC = %10.4f sec\n", time.dut );
return;
} /* end list_civil_time */

/************************************************************************/

/*++ void list_app_place( app_place_type app )
 --*/
void list_app_place( app_place_type app )
 {
 int h, m; /* hours, minutes, or degrees, arcminutes */
 double s; /* seconds, or arc seconds */
 
 rad_to_hms( app.gast, &h, &m, &s );
 (void)printf("GAST = %15.8f rad, %+2dh %2dm %6.3fs \n", app.gast, h, m, s );
 if( app.view == 2 )
  {
  (void)printf("Apparent place, at JD %15.5f, viewed from geocenter: \n",
   app.epoch );
  rad_to_hms( app.ha, &h, &m, &s );
  (void)printf("  GAHA = %15.8f rad, %+2dh %2dm %6.3fs \n", app.ha, h, m, s );
  }
 else if ( app.view == 3 )
  {
  (void)printf("Apparent place, at JD %15.5f, viewed from topocenter: \n",
   app.epoch );
  rad_to_hms( app.ha, &h, &m, &s );
  (void)printf("  LAHA = %15.8f rad, %+2dh %2dm %6.3fs \n", app.ha, h, m, s );
  }
 else
  (void)printf("Illegal 'place', view = %d \n", app.view );
 rad_to_hms( app.ra, &h, &m, &s );
 (void)printf("  RA   = %15.8f rad, %+2dh %2dm %6.3fs \n", app.ra, h, m, s );
 rad_to_dms( app.dec, &h, &m, &s );
 (void)printf("  Dec  = %15.8f rad, %+2dd %2d' %6.3f\" \n", app.dec, h, m, s );
 (void)printf("  range= %.5g km\n", app.range );
 (void)printf("  doppler factor = %.13g \n", app.doppler );
 (void)printf("  drift factor = %+.5g 1/sec \n", app.drift );
 (void)printf("  curvature factor = %+.5g 1/sec/sec \n", app.curve );
 if( app.fpred >= 0.0 ) /* if we calculated a predicted freq */
  {
  (void)printf("  predicted frequency = %.8f MHz \n", app.fpred );
  (void)printf("  predicted drift rate = %.5g Hz/sec \n", app.dpred );
  }
 
 return;
 } /* end list_app_place() */

/****************************************************************************/

/*++ void list_site_data( site_data_type site)
 --*/
void list_site_data( site_data_type site)
 {
 (void)printf("\nSite data: \n");
 (void)printf("  X = %+15.3f m, ITRF \n", site.coor[0] );
 (void)printf("  Y = %+15.3f m, ITRF \n", site.coor[1] ); 
 (void)printf("  Z = %+15.3f m, ITRF \n", site.coor[2] );
 (void)printf("  h = %15.3f m above geoid \n", site.altitude );
 if( site.foffset != 0.0 )
  (void)printf("  frequency standard relative error = %.5g \n",
   site.foffset );
 return;
 } /* end list_site_data() */
 

/*++ unaberated_ra_dec()
 --*/

int unaberated_ra_dec(
    target_data_type star,
    civil_time_type time,
    char *ephem_file,
    app_place_type *stargc,
    int verbosity )
 {
 int status;  /* return status from functions */
 long jday;  /* julian day number of obs epoch */
 double jsec;  /* seconds from noon UTC at obs epoch */
    /* this is +- half a day from noon */
 double jsec1;  /* seconds from noon UT1 at obs epoch */
 double xyz[9];  /* Earth position (km), vel (km/s), accel (km/s/s) */
 double dra;  /* correction in RA */
 double ddec;  /* correction in Dec */
 double ra;  /* RA being worked on */
 double  dec;  /* Dec being worked on */
 double tcentury; /* centuries from epoch J2000 */
 double xau;  /* x coordinate of earth in AU */
 double yau;  /* y coordinate of earth in AU */
 double zau;  /* z coordinate of earth in AU */
 double range_gc; /* distance of spacecraft from geocenter, km */
 double re;  /* distance of earth from barycenter */
 double ve;  /* speed of earth */
 double Usun;  /* gravitational potential energy at earth from sun */
 double Uearth;  /* gravitational potential energy at geoid from earth */
 double Ug;  /* gravitational potential energy of observer on geoid */
 double p[3];  /* unit vector towards target, rect equatorial coords */
 double pvc;  /* p*V/c factor for earth */
 double dop_bary_to_geo; /* multiply finf by this factor to get fgeocentric */
 double drift_bary_to_geo; /* Multiply finf by this to get drift rate at geocent Hz/s */
 double drift_bary_to_tar; /* Multiply finf by this to get drift rate at target Hz/s */
 double  dop_bary_to_tar; /* multiply finf by this factor to get f0 */
 double finf_pred;  /* Predicted freq referred to 'infinity' MHz */
 double fgc_pred;  /* Predicted geocentric freq MHz */
 double driftinf_pred;  /* Predicted drift rate referred to 'infinity' Hz/s */
 double driftgc_pred;  /* Predicted geocentric drift rate Hz/s */
 double gamma;   /* Lorentz factor = (1 -v^2/c^2)^(-.5) */
 double pabb[3];  /* abberated position unit vector */
 double ra2000_gc;  /* apparent geocentric ra of date in J2000 coords */
 double dec2000_gc;  /* apparent geocentric dec of date in J2000 coords */
 double ra_mean_gc;  /* geocentric ra in mean coords of date */
 double dec_mean_gc;  /* geocentric dec in mean coordinates of date */
 double dummy;   /* dummy for misc uses */
 int  rahrs;
 int  ramin;
 double  rasec;
 int  decdeg;
 int  decmin;
 double  decsec;
 double gmst;   /* Greenwich Mean Sidereal Time, radians */
 int gsthrs;
 int gstmin;
 double gstsec;
 double nut_in_longitude; /* nutation in longitude, radians */
 double nut_in_obliquity; /* nutation in obliquity, radians */
 double mean_obliquity;  /* mean obliquity of ecliptic, radians */
 double true_obliquity;  /* true obliquity of ecliptic, radians */
 double EQ;  /* Equation of the Equinoxes, radians */
 double gast;  /* Greenwich Apparent Sidereal Time, radians */
 double gaha;  /* Greenwich Apparent Hour Angle, radians */

 /* work out some date and time stuff */

 jday = juldayfromdate(time.year, time.month, time.day);
 jsec = time.sec + 60 * (time.min + 60 * time.hour) - 12*60*60;
 jsec1 = jsec + time.dut; /* correct from UTC to UT1 */
 tcentury = (jday - JDJ2000)/36525.0 + jsec1/(36525.0*(24*60*60));
       /* 36525 days in a Julian century */
 if( verbosity >= 2 )
  {
  (void)printf("\nJulian Day beginning at noon is %12ld\n", jday);
  (void)printf("Seconds from noon, UTC = %12.3f \n", jsec);
  }
 if( verbosity >= 3 )
  {
  (void)printf("Seconds from noon, UT1 = %12.3f \n", jsec1);
  (void)printf("Centuries from J2000 epoch (UT1) = %20.13f \n\n", tcentury);
  }

 if( verbosity >= 1 ) 
  {
  rad_to_hms( star.ra2000, &rahrs, &ramin, &rasec);
  rad_to_dms( star.dec2000, &decdeg, &decmin, &decsec );
  (void)printf(
  "\nBarycentric  RA, Dec at epoch = %12.8f, %12.8f rad\n", star.ra2000, 
	      star.dec2000 );
  (void)printf("   Barycentric RA  is %4d %4d %7.3f (J2000)\n", 
   rahrs, ramin, rasec );
  (void)printf("   Barycentric Dec is %4d %4d %7.3f (J2000)\n", 
   decdeg, decmin, decsec );
  }
 /* correct position of target for proper motion */

 if( star.type == STAR )  /* if target is a star */
  {
  dra = star.pmra * 100 * tcentury; /* pmra is in arcsec per year */
  if( verbosity >= 2 )
   (void)printf("  Proper motion in RA is %10.5f arcsec \n", dra);

  dra = dra * SToR;   /* convert asec to radians */
  if( verbosity >= 3 )
   (void)printf("  Proper motion in RA is %15.9f radians\n", dra);

  ddec = star.pmdec * 100 * tcentury; /* pmdec is in arcsec per year */
  if( verbosity >= 2 )
   (void)printf("  Proper motion in Dec is %10.5f arcsec\n", ddec);

  ddec = ddec * SToR;   /* convert arcsec to radians */
  if( verbosity >= 3 )
   (void)printf("  Proper motion in Dec is %15.9f radians\n\n", ddec);

  ra = star.ra2000 + dra;   /* at epoch, wrt to mean 2000 coodinates */
  dec = star.dec2000 + ddec;  /* at epoch, wrt to mean 2000 coodinates */
  } 					/* end proper motion correction */
 else if( star.type == SPACECRAFT )
  {
  if( fabs( star.epoch - (jday + jsec/86400) ) > 1e-6 )
   (void)printf("\nWARNING, doppler_work: epoch = %20.6f, target epoch = %20.6f\n",
    star.epoch, (jday + jsec/86400) );
  ra = star.ra2000;
  dec = star.dec2000;
  }
 else
  {
  (void)printf("\nWARNING, doppler_work() found illegal star.type = %d\n", star.type);
  return( -1);
  }
 /* find position and velocity of earth with respect to barycenter */

 status = position( ephem_file, jday, jsec, xyz );
 if ( status == -1 )
  {
  (void)printf("Couldn't find earth's position!\n");
  return( status );
  }
 if( verbosity >= 3 )
  {
  (void)printf(
  " Earth position is:     %15.1f %15.1f %15.1f km \n", 
   xyz[0], xyz[1], xyz[2]);
  (void)printf(" Earth velocity is:     %15.7f %15.7f %15.7f km/s \n", 
   xyz[3], xyz[4], xyz[5]);
  (void)printf(" Earth acceleration is: %15.5g %15.5g %15.5g km/s/s \n\n", 
   xyz[6], xyz[7], xyz[8]);
  }

 /* correct position and range of target to geocenter in J2000 coords */

	       /* input was barycentric and target is star */
 if ( ( star.origin == BARYCENTRIC) && (star.type == STAR) ) 
  { /* correct star for annual parallax */
  xau = xyz[0] / AU ; /* convert from km to AU */
  yau = xyz[1] / AU ; /* convert from km to AU */
  zau = xyz[2] / AU ; /* convert from km to AU */
  /* approximate method, see p 125 of ESAA.  Should be accurate enough */
  dra = star.parallax * ( xau*sin(ra) - yau*cos(ra) )/( cos(dec) );
  if (verbosity >= 3)
   (void)printf("  stellar annual parallax in RA = %10.3f arcsec\n", dra);
  dra = dra * SToR; /* arcsec to radians */
  ddec = star.parallax * (xau*cos(ra)*sin(dec) + yau*sin(ra)*sin(dec) - zau*cos(dec));
  if (verbosity >= 3)
   (void)printf("  stellar annual parallax in Dec = %10.3f arcsec\n", ddec);
  ddec = ddec * SToR; /* arcsec to radians */
  ra = ra + dra;
  dec = dec + ddec;
  range_gc = star.range; /* range unchanged for star */
  } /* end correct star for annual parallax */
 /* 
  * input barycentric and target is spacecraft 
  */
 if ( ( star.origin == BARYCENTRIC) && (star.type == SPACECRAFT) ) 
  { /* correct spacecraft for parallax */
  radec_to_rect(ra, dec, star.range, p);
  p[0] = p[0] - xyz[0];
  p[1] = p[1] - xyz[1];
  p[2] = p[2] - xyz[2];
  rect_to_radec( p, &ra, &dec, &range_gc );
  dra = modpi( ra - star.ra2000);
  ddec = modpi( dec - star.dec2000 );
  if( verbosity >= 3)
   {
   (void)printf(
   " target position with respect to geocenter:  %15.1f %15.1f %15.1f km \n",
   p[0], p[1], p[2]);
   printf(
   "  geocentric vector =  %15.1f  %15.1f  %15.1f km \n", p[0], p[1], p[2] );
   (void)printf(" range = %15.1f km \n", range_gc);
   (void)printf("  orbital parallax in RA  = %.6g radian\n", dra);
   (void)printf("  orbital parallax in Dec = %.6g radian\n", ddec);
   }
  } /* end correct spacecraft for parallax */

 if( star.origin == SPACECRAFT )  /* target coords already geocentric */
  range_gc = star.range;

  stargc->unab_ra = ra;
  stargc->unab_dec = dec;
 /* at this point, geocentric unaberrated RA and Dec of date in 
  * J2000 coordinates are in variables 'ra' and 'dec', range in 'range_gc' 
  */
 
 if( verbosity >= 1 ) 
  {
  rad_to_hms( ra, &rahrs, &ramin, &rasec);
  rad_to_dms( dec, &decdeg, &decmin, &decsec );
  (void)printf(
  "\nGeocentric true (unaberrated) RA, Dec at epoch = %12.8f, %12.8f rad\n", 
   ra, dec );
  (void)printf("   Geocentric true RA  is %4d %4d %7.3f (J2000)\n", 
   rahrs, ramin, rasec );
  (void)printf("   Geocentric true Dec is %4d %4d %7.3f (J2000)\n", 
   decdeg, decmin, decsec );
  }

  return 0;
}
