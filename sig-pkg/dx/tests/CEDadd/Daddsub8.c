/*******************************************************************************

 File:    Daddsub8.c
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

#include <ansi_c.h>
#include "gendadd8.h"

void printrow(short n,char *a)
    {short i,temp;
     for(i=0;i<n/2;i++)
	 {temp = a[i+n/2];
	  printf("%ld ",temp);
	  }
      printf("\n");
      getchar();
      }






/* When the DADD algorithm is run in its own memory space, the results appear 
   not in their natural order.  In the case of power of 2 DADD, the row containing 
   sums of drift k appears in row bitreverse(k). When the number of rows is
   not a power of two, the correspondence is more complicated.  "getposition"
    returns the index of the row containing pathsums with total drift
    = driftrate, in a block of size blocksize. This generalizes the 
    bit reversal calculation in a power of 2 dadd computation.

*/

short getposition(short driftrate,short blocksize)
    {short rval, bov2;

     if((driftrate == 0) || (driftrate == (blocksize -1)))
	 return driftrate;

     bov2 = blocksize/2;

     if(driftrate%2 == 1)  //drift is odd
	 return bov2 + getposition(driftrate/2,blocksize - bov2);

     else                  //drift is even   
	 return getposition(driftrate/2,bov2);

      }

/* WHY THE ABOVE CALCULATION WORKS:  By induction, one can see that in 
   an m row block, the drift 0 row is always the bottom row, and the
   drift m-1 row is always the top row.  Because of the way subblocks 
   are combined, even drifts will lie in the lower subblock, and odd 
   drifts will lie in the upper subblock, with the possible exception of
   the maximum drift row, which lies at the top of the upper subblock 
   independent of the parity of its index.
   Thus, if driftrate is even, the row in question will be placed in the lower
   subblock, in the position previously occupied by the row of
   drift = driftrate/2 in that subblock, and if driftrate is odd, then the row 
   in question will lie in the upper subblock in the position previously 
   occupied by the row of driftrate = drift/2 in that subblock.

*/

long dvnqposition(long drift, long rowcount)
    {long whereset,inset,index;
      if((rowcount <= CACHEROWS)||(rowcount%CACHEROWS != 0))
	   return getposition(drift,rowcount);
      else
	   {whereset = dvnqposition(drift/CACHEROWS,rowcount/CACHEROWS);
            inset = getposition(drift%CACHEROWS,CACHEROWS);
            index = whereset +inset*(rowcount/CACHEROWS);
            return index;
            }
      }


void dvnqunscramble(short rows,short cols,char *data,char *tempdata)
    {short m,temp,i,j,ibitrev;

     for(i=0;i<rows;i++)
	 {ibitrev = dvnqposition(i,rows);
	  printf(" i   %d   ibitrev   %d  \n",i,ibitrev);
	      for(j=0;j<cols;j++)
		   tempdata[i*cols+j] = data[ibitrev*cols+j];		    
	   }
     for(i=0;i<rows;i++)
	      for(j=0;j<cols;j++)
		  data[i*cols+j] = tempdata[i*cols+j];


      }




/* this does unscrambling of rows using the general (i.e non
   power-of-2) row index calculator getposition, which 
   generalizes the power-of-2 routine bitreverser.
*/

void generalunscramble(short rows,short cols,char *data,char *tempdata)
    {short m,temp,i,j,ibitrev;

     for(i=0;i<rows;i++)
	 {ibitrev = getposition(i,rows);
	      for(j=0;j<cols;j++)
		   tempdata[i*cols+j] = data[ibitrev*cols+j];
	   }
     for(i=0;i<rows;i++)
	      for(j=0;j<cols;j++)
		  data[i*cols+j] = tempdata[i*cols+j];


      }


/* copy right half of array rightgood to right half of array leftgood */

void mergehalves(short rowcount,short colcount,char *rightgood,char *leftgood)
{long i,j,index;
 for(i=0;i<rowcount;i++)
   {index = i*colcount;
    for(j=colcount/2;j<colcount;j++)
      leftgood[index+j] = rightgood[index+j];
  }
}


void comparrays(short rowcount,short colcount,char *ar1,char *ar2)
{long i,j,index,character;
 
 for(i=0;i<rowcount;i++)
   {index = i*colcount;
    for(j=0;j<colcount;j++)
      if(ar1[index+j] !=  ar2[index+j])
	 {printf("array compare failed at index pair (%ld , %ld)\n",i,j);
	   character = getchar();  
	   exit(1);
	}
  }
      printf("arrays compared perfectly!!\n");
       
}

 void printarray(short rowcount,short colcount,char *a)
	{long i,j,index,temp,character;
	  for(i=0;i<rowcount;i++)
	      {for(j=colcount/2;j<colcount;j++)
		  {index = colcount*i + j;
		   temp = a[index];
		   printf("%ld ",temp);
		  }
	   printf("\n");
	   }
	 character = getchar();
	}

/* inar and outar are two-dimensional arrays.
    This routine sets each row of outar to the reverse
    of the corresponding row of inar.  If inar and outar
    point to the same array the routine will work.
*/

void dreverse(short rowcount,short colcount,char *inar,char *
	      outar)
    {long i,j,indx;
     short temp;

     for(i=0;i<rowcount;i++)
	 {indx = colcount*i;
	  for(j=0;j<(colcount+1)/2;j++)
	      {temp = inar[indx + j];
	       outar[indx +j] = inar[indx+colcount-1-j];
	       outar[indx+colcount-1-j] = temp;
	       }
	   }
      }



     
short odd(short m)
{return (m % 2);
 }

/*
void timediff(struct timeb *starttime,struct timeb *endtime)
    {float tdiff;
     long stsec,etsec;
     float stmil,etmil;


     printf(" %ld  %ld  %d  %d  \n",starttime->time,endtime->time, 
			      starttime->millitm,endtime->millitm);
     stsec =  starttime->time;
     etsec =  endtime->time;
     stmil = (float) starttime->millitm;
     etmil =  (float) endtime->millitm;

     tdiff =((float) (etsec - stsec)) + (etmil - stmil)/1000.;
     
     printf("timediff is %f  seconds\n",tdiff);
     }
  */
void daddshift(short shiftcount,long qwordcount,char *a)
    {short smallmask,character;
      long i,j,mediummask,qwc8;
      long *temp0,*temp1;
//      double largemask;
      
//      largemask = 0;
      smallmask = (0xff) >> shiftcount;
      mediummask = smallmask + 256*smallmask +(256*256)*smallmask + (256*256*256)*smallmask;
      qwc8 = 8*(qwordcount/8);
            for(i=0;i<qwc8;i=i+8)  //set up unrolled loop to do 8 qwords at a time.
	    for(j=0;j<8;j++)
                  {temp0 = (long *) &a[i+j];
                   temp1 = temp0 + 1;
                    *temp0 = (*temp0 >> shiftcount) & mediummask;
                    *temp1 = (*temp1 >> shiftcount) & mediummask;
           //         printf(" %p  %p  %x  %x \n",temp0,temp1,*temp0,*temp1); 
           //         character = getchar();

                   }
        for(i=qwc8; i<qwordcount;i++)
              {temp0 = (long *) &a[i]  ;
                temp1 = temp0 + 1;
                    *temp0 = (*temp0 >> shiftcount) & mediummask;
                    *temp1 = (*temp1 >> shiftcount) & mediummask;
              }
     }




























