/*******************************************************************************

 File:    readephem.c
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

/*++	readephem.c
 * 
	Purpose: this module handles reading the xyz ascii ephemeris files
	produced by scephm using the naif toolkit from the JPL
	ephemerides.  It also handles the interpolation of
	the data. With 1 day tabulations and 4th order Besselian
	interpolation, earth position is good to about 1 km,
	velocity to better than 1 cm/sec, compared to naif
	results.

 *	95 jan 16 JWD	starting from scratch
 *	95 jan 18 JWD	seems to work OK, needs some testing
 *	95 jan 22 JWD	adding 4th order, acceleration stuff.  appears to work
 *	95 jan 26 JWD	improved error reporting for step not constant
 *	95 jan 29 JWD	commented out some print statements

 --*/
dummy()
 {
 }

#define	ASTEP	1e4	/* step size for acceleration calc.  Longer than 1e4 sec will */
			/* increase errors due to finite step, less will bring out errors */
			/* due to approximate nature of interpolator.  */

#include <stdio.h>
#include <math.h>
#include "readephem.h"

#define MaxLineSize 400

/*++ long skipheader( FILE *ephemfile )
	Purpose: Read header and discard
 --*/
long skipheader( 
   FILE *ephemfile )			/* Pointer to file descriptor */
{
   char 	line[MaxLineSize];		/* buffer for header lines */
   int 	lines_read;
   char 	*p;			/* for return from fgets */

   for( lines_read = 0; lines_read<LINESINHEADER; ++lines_read)
   {
      p = fgets( line, MaxLineSize, ephemfile);
      if( p == NULL )
      {
         (void)fprintf(stderr, 
                       "\nFATAL error reading ephemeris file: can't skip header, linenumber %d.\nInvalid ephemeris file format.\n", 
                       lines_read);
         return(-1);
      }
   }
   return(1);
}	/* end skipheader() */

/*++ int readxyztable( FILE *ephemfile, double tbl[][ARGSINROW] )
      Purpose: Read coordinate table.
 --*/
int readxyztable( 
   FILE *ephemfile, 			/* pointer to File Descriptor 	*/
   double tbl[][ARGSINROW] )		/* Coordinate Table		*/
{
   int 	n_rows;		/* number of rows found in table */
   int 	n_args;		/* number of args found in row */
   char 	line[MaxLineSize];	/* buffer for lines */
   char 	*p;			/* for return from fgets */

   for( n_rows = 0; n_rows < MAXROWS; )
   {
      p = fgets( line, MaxLineSize, ephemfile);
      if (p == NULL)
      {
         if ( feof(ephemfile) == 0 )
         {
            (void)fprintf(stderr,
                          "\nFATAL error reading ephemeris file , I/O error reading ephem table.\nInvalid ephemeris file format.");
            return(-1);
         }
         return( n_rows );	/* normal return */
      }
      n_args = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
                      &tbl[n_rows][0],
                      &tbl[n_rows][1], &tbl[n_rows][2], &tbl[n_rows][3],
                      &tbl[n_rows][4], &tbl[n_rows][5], &tbl[n_rows][6],
                      &tbl[n_rows][7], &tbl[n_rows][8], &tbl[n_rows][9]);
      ++n_rows;
				
      if (n_args != ARGSINROW)	
      {
         (void)fprintf(stderr,
                       "\nFATAL error reading ephemeris file: readxyztable found %d args in row %d!\nInvalid ephemeris file format.\n", 
                       n_args, n_rows);
         return(-1);
      }
   }

   (void)printf(
      "\nEphemeris WARNING , readxyztable found more than %d rows!\n", 
      n_rows);
   (void)fprintf( stderr,
                  "\nEphemeris WARNING , readxyztable found more than %d rows!\n", 
                  n_rows);
   return( n_rows );
}	/* end readxyztable() */

/*++ 
	Purpose: Print Coordinate Table
 --*/
void printxyztable( 
   int n_rows, 			/* Number of rows	*/
   double tbl[][ARGSINROW] )		/* Coordinate Table	*/
{
   int i;

   (void)printf( "       JD              X               Y              "
                 "Z           VX        VY        VZ\n");
   for ( i = 0; i < n_rows; ++i )
      (void)printf("%16.7f %15.1f %15.1f %15.1f %10.5f %10.5f %10.5f\n",
                   tbl[i][0], tbl[i][1], tbl[i][2],
                   tbl[i][3], tbl[i][4], tbl[i][5], tbl[i][6]);
   return;
}

/*++
      Purpose: 
 --*/
int position( 
   char *filename, 
   long jday, 
   double jsec, 
   double statevect[STATE_VECT_LEN] )
{
   FILE *p_file;
   int 	nrows;
   int 	n;
   int 	i;
   double	epoch;		/* epoch for requested position, Julian UTC */
   double	epochlo;	/* epoch - ASTEP for accel calc */
   double	epochhi;	/* epoch - ASTEP for accel calc */
   double	table[MAXROWS][ARGSINROW];
   double	span;		/* time interval covered by table */
   double	dt;		/* time step in table */
   double	p;		/* interpolating factor */
   double	b1;		/* 1st Bessel coefficient */
   double	b2;		/* 2nd Bessel coefficient */
   double	b3;		/* 3rd Bessel coefficient */
   double	b4;		/* 4th Bessel coefficient */
   double	vhi[3];		/* velocity at epoch + ASTEP */
   double	vlo[3];		/* velocity at epoch - ASTEP */

   p_file = fopen( filename, "r" );
   if ( (p_file) == NULL )
   {
      perror( filename );
      printf( "Error opening ephemeris file: %s\n", filename );
      return(-1);
   }
   if ( skipheader( p_file ) == -1 )
   {
      fclose(p_file);
      return(-1);
   }
   if ( (nrows = readxyztable( p_file, table )) == -1 )
   {
      fclose(p_file);
      return(-1);
   }
   fclose(p_file);
   /*(void)printf("readxyztable found %d rows\n", nrows);*/

   /* now check whether table is OK for requested epoch and seems sane */

   if ( nrows < 5 )	/* need at least 5 entries for interpolator */
   {
      (void)printf("\nEphemeris WARNING, less than 5 entries in xyz table!\n");
      (void)fprintf( stderr,"\nEphemeris WARNING, less than 5 entries in xyz table!\n");
      return ( -1 );
   }
   epoch = (double)jday + jsec/86400;	/* fractional julian days */
				/* prior time for acceleration calc */
   epochlo = (double)jday + (jsec - ASTEP)/86400;	
				/* later time for acceleration calc */
   epochhi = (double)jday + (jsec + ASTEP)/86400;
   /*(void)printf("Target Epoch = %15.8f days\n", epoch);*/
				/* need two earlier entries for interp */
   if ( epochlo < table[2][0] )
   {
      (void)printf(
         "\nEphemeris WARNING, epoch %10.1f earlier than 3rd table entry %10.1f\n",
         epoch, table[1][0]);
      (void)fprintf( stderr,
                     "\nEphemeris WARNING, epoch %10.1f earlier than 3rd table entry %10.1f\n",
                     epoch, table[1][0]);
      return ( -1 );
   }
				/* need two later entries for interp */
   if ( epochhi > table[nrows-3][0] )
   {
      (void)printf(
         "\nEphemeris WARNING, epoch %10.1f later than 2nd to last table entry %10.1f\n",
         epoch, table[nrows-2][0]);
      (void)fprintf( stderr,
                     "\nEphemeris WARNING, epoch %10.1f later than 2nd to last table entry %10.1f\n",
                     epoch, table[nrows-2][0]);
      return ( -1 );
   }
				/* time interval covered by table */
   span = table[nrows-1][0] - table[0][0];	
   dt = span / (nrows - 1);		/* time step of table */
   /*(void)printf("Table interval = %20.10f days, step %20.10f days\n",
     span, dt);*/
				/* row of table with time just < epoch */
   n = (epoch - table[0][0]) / dt;	
				/* a quick sanity check */
   if( fabs(table[n][0] - table[0][0] - n * dt) >= 1e-6 )
   {
      (void)printf("\nWARNING, table step seems not to be a constant\n");
      (void)printf("table[%4d ][0] = %20.10f\n", n, table[n][0] );
      (void)printf("table[   0 ][0] = %20.10f\n", table[0][0] );
      (void)printf("    %4d * step = %20.10f\n", n, n*dt );
   }

   /* now do the interpolation on x, y, z, vx, vy, and vz */
   /* method from p 546ff of Explanatory Supplement to the */
   /* American Ephemeris, revised ed */

   p = (epoch - table[n][0])/dt;		/* the interpolating factor */
   b1 = p;					/* besselian coefficients */
   b2 = p*(p-1)/4;
   b3 = p*(p-1)*(p-0.5)/6;
   b4 = (p+1)*p*(p-1)*(p-2)/48;
   for( i=1; i<7; ++i)		/* bessel 4th order interpolation */
      statevect[i-1] = table[n][i] + b1 * ( table[n+1][i] - table[n][i] )
         + b2 * ( table[n+2][i] - table[n+1][i] - 
                  table[n][i] + table[n-1][i])
         + b3 * ( table[n+2][i] - 3*table[n+1][i] + 
                  3*table[n][i] - table[n-1][i])
         + b4 * ( table[n+3][i] - 3*table[n+2][i] + 
                  2*table[n+1][i] + 2*table[n][i]
                  - 3*table[n-1][i] + table[n-2][i] );

   /* now code, in a very clumsy fashion, the acceleration calcs */
   /* really should replace this junk with proper derivative of  */
   /* above interpolator */

				/* row of table with time just < epochhi */
   n = (epochhi - table[0][0]) / dt;
				
   p = (epochhi - table[n][0])/dt;	/* the interpolating factor */
   b1 = p;				/* besselian coefficients */
   b2 = p*(p-1)/4;
   b3 = p*(p-1)*(p-0.5)/6;
   b4 = (p+1)*p*(p-1)*(p-2)/48;
   for( i=4; i<7; ++i)		/* bessel 4th order interpolation */
      vhi[i-4] = table[n][i]
         + b1 * ( table[n+1][i] - table[n][i] )
         + b2 * ( table[n+2][i] - table[n+1][i] - 
                  table[n][i] + table[n-1][i])
         + b3 * ( table[n+2][i] - 3*table[n+1][i] + 
                  3*table[n][i] - table[n-1][i])
         + b4 * ( table[n+3][i] - 3*table[n+2][i] + 
                  2*table[n+1][i] + 2*table[n][i]
                  - 3*table[n-1][i] + table[n-2][i] );
	
   /* row of table with time just < epochhi */
   n = (epochlo - table[0][0]) / dt;
   p = (epochlo - table[n][0])/dt;		/* the interpolating factor */
   b1 = p;					/* besselian coefficients */
   b2 = p*(p-1)/4;
   b3 = p*(p-1)*(p-0.5)/6;
   b4 = (p+1)*p*(p-1)*(p-2)/48;
   for( i=4; i<7; ++i)		/* bessel 4th order interpolation */
      vlo[i-4] = table[n][i]
         + b1 * ( table[n+1][i] - table[n][i] )
         + b2 * ( table[n+2][i] - table[n+1][i] - 
                  table[n][i] + table[n-1][i])
         + b3 * ( table[n+2][i] - 3*table[n+1][i] + 
                  3*table[n][i] - table[n-1][i])
         + b4 * ( table[n+3][i] - 3*table[n+2][i] + 
                  2*table[n+1][i] + 2*table[n][i]
                  - 3*table[n-1][i] + table[n-2][i] );

   for( i=6; i<9; ++i )
   {
      statevect[i] = (vhi[i-6] - vlo[i-6])/(2.0 * ASTEP);
   }
   
   // lighttime, range, range_rate
   // No interpolation, just use nearest row.
   int tblOffset = 2;
   for (i=9; i<12; ++i)
   {
      statevect[i] = table[n][i-tblOffset];
   }

   return( 1 );
}	/* end position() */