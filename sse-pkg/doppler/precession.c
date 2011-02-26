/*******************************************************************************

 File:    precession.c
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

/*++	precession.c
 * 
 *
 * PURPOSE:  A rearrangement of the precession and nutation routines from
 * Sax' implementation of Meeus' algorithms from the source
 * code distributed on diskette with Meeus' book.  Routines
 * slightly modified as noted. Comments clarified a bit.
 * 
 * AUTHOR:  John Dreher
 * HISTORY:
 *         01/16/95 JWD    moved to HP.  Added time stuff
 *
 --*/

/*****************************************************************************/
/*                                                                           */
/*               This program contains software code                         */
/*                Copyright (c) 1991 by Jeffrey Sax                          */
/*              and Distributed by Willmann-Bell, Inc.                       */
/*                         Serial #######                                    */
/*                                                                           */
/*****************************************************************************/

#include <math.h>
#include "ast_const.h"
#include "precession.h"

#define already_in_scslib 1
#ifdef already_in_scslib

/*++ double SiderealTime(double T)
 * Purpose: Calculate mean sidereal time at Greenwich                        
 *****************************************************************************
 * Name:    SiderealTime                                                     *
 * Type:    Function                                                         *
 * Purpose: calculate mean sidereal time at Greenwich                        *
 * Arguments:                                                                *
 *   T : number of Julian centuries since J2000                              *
 * Return value:                                                             *
 *   Greenwich sidereal time in radians                                      *
 *****************************************************************************
 --*/

double SiderealTime(
		    double T)	/* number of Julian centuries since J2000 */
{
   double Theta;
                        /* Degrees per Sidereal Day = 360.98564736629 */
			/* Days per Century = 36525                   */
   Theta = T * (360.98564736629 * 36525 + T * (0.000387933 - T / 38710000));
   return modpi2 ((280.46061837 + Theta) * DToR);
}

/*++ void PrecessFK5(double T0, double T1, double *RA, double *Decl)
 * Purpose: Precess mean equatorial coordinates from one FK5 epoch to another 
 ***************************************************************************** 
 * Name:    PrecessFK5                                                       * 
 * Type:    Procedure                                                        * 
 * Purpose: precess mean equatorial coordinates from one FK5 epoch to another* 
 * Arguments:                                                                * 
 *   T0, T1 : initial and final epochs in centuries since J2000              * 
 *   RA, Decl : coordinates to be converted in radians                       * 
 ***************************************************************************** 
 --*/
void PrecessFK5(
		double T0, 	/* initial epoch in centuries since J2000 */
		double T1, 	/* final epoch in centuries since J2000   */
		double *RA, 	/* coordinates to be converted in radians */
		double *Decl)	/* coordinates to be converted in radians */
{
   double t, zeta, z, theta;
   double A, B, C;

   t = T1 - T0;
   z = 2306.2181 + T0 * (1.39656 - T0 * 0.000139);
   zeta = t * (z + t * ((0.30188 - T0 * 0.000344) + t * 0.017998)) * SToR;
   z = t * (z + t * ((1.09468 + T0 * 0.000066) + t * 0.018203)) * SToR;
   theta = (2004.3109 - T0 * (0.85330 + T0 * 0.000217));
   theta = t * (theta - t * ((0.42665 + T0 * 0.000217) + t * 0.041833)) * SToR;
   A = cos (*Decl) * sin (*RA + zeta);
   B = cos (theta) * cos (*Decl) * cos (*RA + zeta) - sin (theta) * sin (*Decl);
   C = sin (theta) * cos (*Decl) * cos (*RA + zeta) + cos (theta) * sin (*Decl);
   *RA = atan2 (A, B) + z;
   if (*RA < 0) {
      *RA += TWOPI;
   }
   *Decl = asin (C);
}
#endif

/*++ Nutation(double NutLon,double NutObl,double Obl,double *RA,double *Decl)
 * Purpose: Correct right ascension and declination for nutation             * 
 ***************************************************************************** 
 * Name:    Nutation                                                         * 
 * Type:    Procedure                                                        * 
 * Purpose: correct right ascension and declination for nutation             * 
 * Arguments:                                                                * 
 *   NutLon, NutObl : nutation in longitude and obliquity, radians           * 
 *   Obl : mean obliquity of the ecliptic, radian                            * 
 *   RA, Decl : coordinates to be corrected, radians                         * 
 ***************************************************************************** 
 --*/
void Nutation(
	      double NutLon,	/* nutation in longitude, radians	*/
	      double NutObl, 	/* nutation in obliquity, radians	*/
	      double Obl, 	/* mean obliquity of the ecliptic, radians */
	      double *RA, 	/* RA, radians				*/
	      double *Decl)	/* Declination, radians			*/
{
   double cosObl, sinObl;
   double cosRA, sinRA;

   if (fabs(*Decl) < 1.54) {
      cosObl = cos (Obl);
      sinObl = sin (Obl);
      cosRA = cos (*RA);
      sinRA = sin (*RA);
      *RA += (cosObl + sinObl * sinRA * tan (*Decl)) * NutLon;
      *RA -= (cosRA * tan (*Decl)) * NutObl;
      *Decl += (sinObl * cosRA) * NutLon + (sinRA) * NutObl;
   }
   else {
      EclToEqu (*RA, *Decl, -Obl, RA, Decl);
      EclToEqu (*RA + NutLon, *Decl, Obl + NutObl, RA, Decl);
   }
}

#ifdef already_in_scslib
/*++ double Obliquity(double T)
 * Purpose: Calculate the obliquity of the ecliptic                          
 * NOTES: using polynomial 21.3 from meeus.  This purports to be more
 *	accurate than the IAU standard model.  Near 2000 should agree well
 *	with IAU polynomial.
 * HISTORY:
 *	09/02/94 JWD	removed const declarator for OblPoly,
 *		since the SUN cc compiler
 * 		did not approve of the const declarator in the prototype
 * 		to EvalPoly.  Doesn't look like EvalPoly will change the
 * 		data in any case, but shouldwatch ofr problems.
 ***************************************************************************** 
 * Name:    Obliquity                                                        * 
 * Type:    Procedure                                                        * 
 * Purpose: calculate the obliquity of the ecliptic                          * 
 * Arguments:                                                                * 
 *   T : number of centuries since J2000                                     * 
 * Return value:                                                             * 
 *   the mean obliquity of the ecliptic at the given instant, radians        * 
 ***************************************************************************** 
 --*/
double Obliquity(
		 double T)		/* number of centuries since J2000 */
{
   double u;

   static double OblPoly[11] = {
      84381.448, -4680.93, -1.55, 1999.25, -51.38,
      -249.67, -39.05, 7.12, 27.87, 5.79, 2.45};

   u = T / 100;
   return EvalPoly(OblPoly, 10, u) * SToR;
}
#endif

/* Polynomials page 132 */
static T3POLY POm =    {125.0445222,  -1934.1362608,  0.00207833, 2.22e-6};
static T3POLY PMSun =  {357.5277233,  35999.0503400, -0.0001603, -3.33e-6};
static T3POLY PMMoon = {134.9629814, 477198.8673981,  0.0086972,  1.778e-5};
static T3POLY PF =     { 93.2719103, 483202.0175381, -0.00368250, 3.056e-6};
static T3POLY PD =     {297.8503631, 445267.1114800, -0.0019142,  5.278e-6};

/* Table 27.A                                                          */
/* The coefficients of the angles D, M, M'... are in the same order as */
/* in the Supplement to the Astronomical Almanac of 1984 (pS23-S25),   */
/* which is different from table 27.A.  The order here is M, M', F, D, */
/* Omega.  Arguments are stored in units of 0.0001" and 0.00001"       */
/* respectively for the constant and T-coefficients of the nutation.   */
/* This allows all coefficients to be stored as integers.  The first   */
/* terms are too large and are therefore calculated seperately.        */

#define NUTTABTERMS 62
#define NUTTABSIZE 9

static const int NutTab[NUTTABTERMS][NUTTABSIZE] = {
	{ 0, 0, 2,-2, 2,-13187,-16,5736,-31},
	{ 0, 0, 2, 0, 2,-2274,  -2, 977, -5},
	{ 0, 0, 0, 0, 2, 2062,   2,-895,  5},
	{ 0, 1, 0, 0, 0, 1426, -34,  54, -1},
	{ 1, 0, 0, 0, 0, 712, 1,  -7, 0},
	{ 0, 1, 2,-2, 2,-517,12, 224,-6},
	{ 0, 0, 2, 0, 1,-386,-4, 200, 0},
	{ 1, 0, 2, 0, 2,-301, 0, 129,-1},
	{ 0,-1, 2,-2, 2, 217,-5, -95, 3},
	{ 1, 0, 0,-2, 0,-158, 0,   0, 0},
	{ 0, 0, 2,-2, 1, 129, 1, -70, 0},
	{-1, 0, 2, 0, 2, 123, 0, -53, 0},
	{ 0, 0, 0, 2, 0, 63, 0,  0, 0},
	{ 1, 0, 0, 0, 1, 63, 1,-33, 0},
	{-1, 0, 2, 2, 2,-59, 0, 26, 0},
	{-1, 0, 0, 0, 1,-58,-1, 32, 0},
	{ 1, 0, 2, 0, 1,-51, 0, 27, 0},
	{ 2, 0, 0,-2, 0, 48, 0,  0, 0},
	{-2, 0, 2, 0, 1, 46, 0,-24, 0},
	{ 0, 0, 2, 2, 2,-38, 0, 16, 0},
	{ 2, 0, 2, 0, 2,-31, 0, 13, 0},
	{ 2, 0, 0, 0, 0, 29, 0,  0, 0},
	{ 1, 0, 2,-2, 2, 29, 0,-12, 0},
	{ 0, 0, 2, 0, 0, 26, 0,  0, 0},
	{ 0, 0, 2,-2, 0,-22, 0,  0, 0},
	{-1, 0, 2, 0, 1, 21, 0,-10, 0},
	{ 0, 2, 0, 0, 0, 17,-1, 0, 0},
	{-1, 0, 0, 2, 1, 16, 0,-8, 0},
	{ 0, 2, 2,-2, 2,-16, 1, 7, 0},
	{ 0, 1, 0, 0, 1,-15, 0, 9, 0},
	{ 1, 0, 0,-2, 1,-13, 0, 7, 0},
	{ 0,-1, 0, 0, 1,-12, 0, 6, 0},
	{ 2, 0,-2, 0, 0, 11, 0, 0, 0},
	{-1, 0, 2, 2, 1,-10, 0, 5, 0},
	{ 1, 0, 2, 2, 2,-8, 0, 3, 0},
	{ 0, 1, 2, 0, 2, 7, 0,-3, 0},
	{ 1, 1, 0,-2, 0,-7, 0, 0, 0},
	{ 0,-1, 2, 0, 2,-7, 0, 3, 0},
	{ 0, 0, 2, 2, 1,-7, 0, 3, 0},
	{ 1, 0, 0, 2, 0, 6, 0, 0, 0},
	{ 2, 0, 2,-2, 2, 6, 0,-3, 0},
	{ 1, 0, 2,-2, 1, 6, 0,-3, 0},
	{-2, 0, 0, 2, 1,-6, 0, 3, 0},
	{ 0, 0, 0, 2, 1,-6, 0, 3, 0},
	{ 1,-1, 0, 0, 0, 5, 0, 0, 0},
	{ 0,-1, 2,-2, 1,-5, 0, 3, 0},
	{ 0, 0, 0,-2, 1,-5, 0, 3, 0},
	{ 2, 0, 2, 0, 1,-5, 0, 3, 0},
	{-2, 0, 2, 0, 2,-3, 0, 0, 0},
	{ 2, 0, 0,-2, 1, 4, 0, 0, 0},
	{ 0, 1, 2,-2, 1, 4, 0, 0, 0},
	{ 1,-1, 2, 0, 2,-3, 0, 0, 0},
	{-1,-1, 2, 2, 2,-3, 0, 0, 0},
	{ 3, 0, 2, 0, 2,-3, 0, 0, 0},
	{ 0,-1, 2, 2, 2,-3, 0, 0, 0},
	{ 1,-1, 0,-1, 0,-3, 0, 0, 0},
	{ 1, 0, 0,-1, 0,-4, 0, 0, 0},
	{ 0, 1, 0,-2, 0,-4, 0, 0, 0},
	{ 1, 0,-2, 0, 0, 4, 0, 0, 0},
	{ 0, 0, 0, 1, 0,-4, 0, 0, 0},
	{ 1, 1, 0, 0, 0,-3, 0, 0, 0},
	{ 1, 0, 2, 0, 0, 3, 0, 0, 0}
};

/*++  void NutationConst(double T, double *NutLon, double *NutObl)
 * PURPOSE:  routine for calculating nutation in longitude and obliquity
 * HISTORY:
 *  09/02/94 JWD    remove const declarator for polynomials, 
 *		 since the c compiler
 *               did not approve of the const declarator in the prototype
 *               to EvalPoly.  Doesn't look like EvalPoly will change the
 *               data in any case, but shouldwatch ofr problems.
 ***************************************************************************** 
 * Name:    NutationConst                                                    * 
 * Type:    Procedure                                                        * 
 * Purpose: calculate the nutation in longitude and obliquity                * 
 * Arguments:                                                                * 
 *   T : number of centuries since J2000                                     * 
 *   NutLon, NutObl : nutation in longitude and obliquit, radians            *
 *****************************************************************************
 --*/
void NutationConst(
		   double T, 		/* number of centuries since J2000 */
		   double *NutLon, 	/* nutation in longitude, radians  */
		   double *NutObl)	/* nutation in obliquity, radians  */
{
   int i, j, k, l;
   int Flag;
   double MS, MM, FF, DD, OM;
   double SinVal, CosVal, ArgSin, ArgCos, Tmp;
   double LonArg, OblArg;
   double SinTab[5][5], CosTab[5][5];

   MM = modpi2 (EvalPoly (PMMoon, 3, T) * DToR);
   MS = modpi2 (EvalPoly (PMSun, 3, T) * DToR);
   FF = modpi2 (EvalPoly (PF, 3, T) * DToR);
   DD = modpi2 (EvalPoly (PD, 3, T) * DToR);
   OM = modpi2 (EvalPoly (POm, 3, T) * DToR);

   CalcSinCosTab (MM, 3, SinTab[0], CosTab[0]);
   CalcSinCosTab (MS, 2, SinTab[1], CosTab[1]);
   CalcSinCosTab (FF, 4, SinTab[2], CosTab[2]);
   CalcSinCosTab (DD, 4, SinTab[3], CosTab[3]);
   CalcSinCosTab (OM, 2, SinTab[4], CosTab[4]);

   /* the first terms are too big for the table : */
   *NutLon = (-0.01742 * T - 17.1996) * SinTab[4][1];    /* sin(OM) */
   *NutObl = ( 0.00089 * T +  9.2025) * CosTab[4][1];      /* cos(OM) */

   for (i = 0 ; i < NUTTABTERMS ; i++) {
      Flag = 1;
      /* Find first non-zero coefficient of one of the five anlges */
      for (j = 0 ; j < 5 ; j++) {
	 k = NutTab[i][j];
	 if (k) {
	    if (k < 0) {
	       l = -k;
	    }
	    else {
	       l = k;
	    }
	    SinVal = SinTab[j][l];
	    if (k < 0) {
	       SinVal = -SinVal;
	    }
	    CosVal = CosTab[j][l];
	    if (Flag) {
	       ArgSin = SinVal;
	       ArgCos = CosVal;
	       Flag = 0;
	    }
	    else {
	       Tmp = ArgSin * CosVal + ArgCos * SinVal;
	       ArgCos = ArgCos * CosVal - ArgSin * SinVal;
	       ArgSin = Tmp;
	    }
	 }
      }
      OblArg = 0.0;
      LonArg = NutTab[i][5] * 0.0001; /* constant coefficient of sine */
      k = NutTab[i][6];   /* T-coefficient of sine */
      if (k) {
	 LonArg += 0.00001 * T * k;
      }
      k = NutTab[i][7];   /* constant coefficient of cosine */
      if (k) {
	 OblArg = 0.0001 * k;
	 k = NutTab[i][8]; /* T-coefficient of cosine */
	 if (k) {
	    OblArg += 0.00001 * T * k;
	 }
      }
      *NutLon += LonArg * ArgSin;
      *NutObl += OblArg * ArgCos;
   }
   *NutLon *= SToR;
   *NutObl *= SToR;
}

/*++ void EclToEqu(double l, double b, double Obl, double *RA, double *Decl)
 * Purpose: convert ecliptic coordinates to equatorial coordinates             
 ***************************************************************************** 
 * Name:    EclToEqu                                                         * 
 * Type:    Procedure                                                        * 
 * Purpose: convert ecliptic coordinates to equatorial coordinates           * 
 * Arguments:                                                                * 
 *   l, b : ecliptical coordinates to be converted                           * 
 *   Obl : obliquity of the ecliptic                                         * 
 *   RA, Decl : the converted equatorial coordinates                         * 
 ***************************************************************************** 
 --*/
void EclToEqu(
	      double l, 	/* ecliptical coordinates to be converted */
	      double b, 	/* ecliptical coordinates to be converted */
	      double Obl,	/* obliquity of the ecliptic		  */
	      double *RA, 	/* the converted equatorial coordinates	  */
	      double *Decl)	/* the converted equatorial coordinates	  */
{
   double sinObl, cosObl;
   double sinl, cosl;
   double sinb, cosb;

   sinObl = sin (Obl);
   cosObl = cos (Obl);
   sinl = sin (l);
   cosl = cos (l);
   sinb = sin (b);
   cosb = cos (b);
   *RA = atan2 (cosb * sinl * cosObl - sinb * sinObl, cosb * cosl);
   if (*RA < 0) {
      *RA += TWOPI;
   }
   *Decl = asin (sinb * cosObl + cosb * sinObl * sinl);
}

#ifdef already_in_scslib
/*++ double EvalPoly( double *P, int n, double x)
 * Purpose: evaluate a polynomial.
 * HISTORY:
 *	09/02/94 JWD	removed const declarator from EvalPoly,
 *			made cc mad on Sun
 ***************************************************************************** 
 * Name:    EvalPoly                                                         * 
 * Type:    function                                                         * 
 * Purpose: evaluate a polynomial.                                           * 
 * Arguments:                                                                * 
 *   P : the coefficients of the polynomial : P[0] is constant term, etc.    * 
 *   n : the degree of the polynomial                                        * 
 *   x : the point at which to evaluate the polynomial                       * 
 ***************************************************************************** 
 --*/

double EvalPoly(
		double *P,  /* the coefficients of the polynomial   */
		int n,	    /* the degree of the polynomial		*/
		double x)   /* the point at which to evaluate the polynomial */
{
   double tmp = P[n];

   while (n--) {
      tmp = tmp * x + P[n];
   }
   return tmp;
}

/*++ double modpi2(double x)  reduce an angle to the interval (0, 2pi).
 ***************************************************************************** 
 * Name:    modpi2                                                           * 
 * Type:    function                                                         * 
 * Purpose: reduce an angle to the interval (0, 2pi).                        * 
 ***************************************************************************** 
 --*/
double modpi2(
	      double x)			/* Angle in radians	*/
{
   return x - floor (x / TWOPI) * TWOPI;
}
#endif

/*++ double modpi(double x)	reduce an angle to the interval (-pi, pi). 
 ***************************************************************************** 
 * Name:    modpi                                                            * 
 * Type:    function                                                         * 
 * Purpose: reduce an angle to the interval (-pi, pi).                       * 
 ***************************************************************************** 
 --*/
double modpi(
	     double x)			/* Angle in Radians	*/
{
   return x - floor(x / TWOPI + 0.5) * TWOPI;
}

/*++ void CalcSinCosTab(double X, int Degree, double *SinTab, double *CosTab)
 * Purpose: calculate a table of sines and cosines of multiples of an angle
 ***************************************************************************** 
 * Name:    CalcSinCosTab                                                    * 
 * Type:    procedure                                                        * 
 * Purpose: calculate a table of sines and cosines of multiples of an anlge. * 
 * Arguments:                                                                * 
 *   X : the base angle                                                      * 
 *   Degree : the highest multiple for which to calculate sine and cosine    * 
 *   SinTab, CosTab : arrays to hold the return values                       * 
 ***************************************************************************** 
 --*/
void CalcSinCosTab(
		   double X, 		/* the base angle		   */
		   int Degree,		/* the highest multiple for which
					   to calc sin and cos */
		   double *SinTab,	/* arrays to hold the return values */
		   double *CosTab)	/* arrays to hold the return values */
{
   double SinVal, CosVal;
   int i;

   SinVal = sin(X);
   SinTab[1] = SinVal;
   CosVal = cos(X);
   CosTab[1] = CosVal;
   for (i = 2 ; i <= Degree ; i++) {
      SinTab[i] = SinTab[i - 1] * CosVal + CosTab[i - 1]*SinVal;
      CosTab[i] = CosTab[i - 1] * CosVal - SinTab[i - 1]*SinVal;
   }
}