/*******************************************************************************

 File:    Gendadd8.h
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

//#define DEBUG  1
#define CACHEROWS  32

void dvnqunscramble(short rows,short cols,char *data,char *tempdata);
short getposition(short driftrate,short blocksize);
long dvnqposition(long rowcount, long drift);
void genconquer(long m,long length,long rgap,long groupdrift,char *darray);
void splitpath(short length,short drift,signed char *lvec,signed char *uvec);
void topsolve(long length,short smallerblock,signed char *lvec,signed char *uvec);
void geninvdadd(short rowcount,short length,signed char *data);
void topdowndaddpair(short length,short drift,
		     char *inrowlower,char *inrowupper);
void topdownsinglesum(short length,short height,char *lower,char *upper);
			  