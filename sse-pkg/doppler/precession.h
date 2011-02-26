/*******************************************************************************

 File:    precession.h
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
 * precession.h - declaration of functions defined in precession.h
 * PURPOSE:  
 *****************************************************************/

#ifndef PRECESSION_H
#define PRECESSION_H

#ifdef __cplusplus
extern "C" {
#endif

/* module precesssion.h

     $Header: /home/cvs/nss/sse-pkg/doppler/precession.h,v 1.2 2002/09/26 21:58:59 kevin Exp $

     $Log: precession.h,v $
     Revision 1.2  2002/09/26 21:58:59  kevin
     * doppler/precession.h: extern "C" added to file.

     initial port from TSS

     Revision 5.0  1996/08/28 11:13:21  scs
     no entry

 * Revision 1.1  95/02/06  11:55:47  11:55:47  jane (Jane Jordan)
 * Initial revision
 * 
constants and declarations for the precession.c module

09/02/94 JWD	Updated constants to higher precision, eliminated physical ephem stuff
		mod EvalPoly prototype to remove const declaration for P.  For aa.h
09/19/94 JWD	reduced aa.h to precess.h  Assumed ANSI compiler.
01/16/95 JWD	moved to HP.  Added time stuff
01/19/95 JWD	changed define for pi2 to TWOPI

*/

/*****************************************************************************/
/*                                                                           */
/*            This program contains software code                            */
/*             Copyright (c) 1991 by Jeffrey Sax                             */
/*           and Distributed by Willmann-Bell, Inc.                          */
/*                      Serial #######                                       */
/*                                                                           */
/*****************************************************************************/

/* First, some general constants and functions */
/* Note that doubles on Sparc have 15 decimal digits of precision */
/* moved to ast_const.h */


typedef double T2POLY[3];
typedef double T3POLY[4];
typedef double T4POLY[5];

/*************************\
* Basic support functions *
\*************************/

double modpi2(double x);
double modpi(double x);
void CalcSinCosTab(double X, int Degree, double *SinTab, double *CosTab);
double EvalPoly( double *P, int n, double x);	/* const ahead of *P made acc mad */

/***********************\
* Time calculations     *
\***********************/

double SiderealTime(double T);

/***********************\
* Position calculations *
\***********************/

void EclToEqu(double l, double b, double Obl, double *RA, double *Decl);
void NutationConst(double T, double *NutLon, double *NutObl);
void Nutation(double NutLon, double NutObl, double Obl, double *RA, double *Decl);
double Obliquity(double T);
void PrecessFK5(double T0, double T1, double *RA, double *Decl);

#ifdef __cplusplus
}
#endif

#endif /* PRECESSION_H */