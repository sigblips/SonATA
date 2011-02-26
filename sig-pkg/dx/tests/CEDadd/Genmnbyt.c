/*******************************************************************************

 File:    Genmnbyt.c
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

#include <utility.h>
#include <stdio.h>
#include "gendadd8.h"
#include "daddsub8.c"
#define NROWS 384
#define NCOLS 2400 

//extern "C" void topdowndaddpair(short length,short drift,signed char *inrowlower,
//			       signed char *inrowupper);
//extern "C" void topdownsinglesum(short length,short height,
//			signed char *lower,signed char *upper);
				 

char test1[NROWS][NCOLS],test3[NROWS][NCOLS],tst[NROWS][NCOLS];
char shield[4];
signed char *test11 = (signed char *) &(test1[0][0]);
signed char *test33 = (signed char *) &(test3[0][0]);
signed char *testtemp = (signed char *) &(tst[0][0]);
/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
    THE FOLLOWING THREE SUBROUTINES CONSTITUTE THE GENERAL DADD ALGORITHM. THE
    ALGORITHM IS GENERAL IN THE SENSE THAT PATH LENGTHS ARE NOT RESTRICTED TO
    BE A POWER OF TWO.
   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
*/ 



#define rowadrs(index,length,dataptr) (char *) dataptr+index*length    


/* This is the Cache Efficient version of general length DADD.




   This version of dadd is not restricted to power_of_two length blocks.
   It splits the incoming data block into equal length subblocks if m is
   even, or nearly equal subblocks if m is odd, runs dadd on the subblocks,
   then sums the resulting vectors. 
   The argument m is the actual row count of the data block, not the base 2
   log of the row count.

   Note that when subblocks are unequal, the upper subblock is always taken
   as the larger.

*/


void gendivnconq(long rowcount,long rowlength,char *darray)
    {long i;
#ifdef  DEBUG    
     printf("divnconq called rowcount = %ld  rlen = %ld \n",rowcount,rowlength);
	 getchar();
#endif     
    if((rowcount > CACHEROWS)&&((rowcount % CACHEROWS)==0))
	 {for(i=0;i<CACHEROWS;i++) //cut the block into subblocks and run dadd
				   //on each subblock     
	     gendivnconq(rowcount/CACHEROWS,rowlength,
		       rowadrs(i*rowcount/CACHEROWS,rowlength,darray));

	  for(i=0;i<rowcount/CACHEROWS;i++) // run dadd on the set of drift i vectors
	     genconquer(CACHEROWS,rowlength,rowcount/CACHEROWS,i,
		     rowadrs(getposition(i,rowcount/CACHEROWS),rowlength,darray));
	 }
       //rowcount is too small to divide, so just conquer
     else genconquer(rowcount,rowlength,1,0,darray);
    }




void genconquer(long m,long length,long rgap,long groupdrift,char *darray)
    {
     char *lowbase,*highbase;
     long i,ibitrev1,ibitrev2,mover2,mstar;
#ifdef DEBUG
	  printf("genconquer called rows = %d gap = %d groupdrift = %d \n",m,rgap,groupdrift);
#endif	  
     if(m == 1) return;
     else 
	  {mover2 = m/2;
	   mstar = m - mover2; //if blocks are unequal, upper one is bigger

			     //run dadd on the two subblocks
	   genconquer(mover2,length,rgap,groupdrift,darray);
	   genconquer(mstar,length,rgap,groupdrift,rowadrs(mover2*rgap,length,darray));
	   }
			  //now combine the paths from the subblocks

       if(mstar > mover2) //if the subblocks are unequal,do leftover case first
	 {lowbase = rowadrs((mover2-1)*rgap,length,darray);
	  highbase = rowadrs((mover2+mover2)*rgap,length,darray);
	  topdownsinglesum(length,mstar,lowbase,highbase);
	  }



     for(i=0;i<mover2;i++) //add the matched pairs of drifts from subblocks
	 {ibitrev1 = dvnqposition(i,mover2);  //index of drift i row in lower blk
	  ibitrev2 = dvnqposition(i,mstar);   // ditto for upper block
	  lowbase = rowadrs(ibitrev1*rgap,length,darray); //set pointer to drift i vector in lower blk
	  highbase = rowadrs((ibitrev2 + mover2)*rgap,length,darray); // ditto for upper block

	  topdowndaddpair(length,mover2*groupdrift+i,lowbase,highbase);
	  }

     }


void mov8(char *dest,char *src)
    {dest[0] = src[0];
     dest[1] = src[1];
     dest[2] = src[2];
     dest[3] = src[3];
     dest[4] = src[4];
     dest[5] = src[5];
     dest[6] = src[6];
     dest[7] = src[7];

     }
// simulate 8 simultaneous bytewide adds

void addeight(char *in1,char *in2,char *out)
   {short i;
    out[0] = in1[0] + in2[0];
    out[1] = in1[1] + in2[1];
    out[2] = in1[2] + in2[2];
    out[3] = in1[3] + in2[3];
    out[4] = in1[4] + in2[4];
    out[5] = in1[5] + in2[5];
    out[6] = in1[6] + in2[6];
    out[7] = in1[7] + in2[7];

    }


void topdowndaddpair(short length,short drift,char *inrowlower,char *inrowupper)

    
    {short i,lminusd,lmdm1,lmdm1ov8,lmdm1mod8;
     char temp1;
     double junk;  //alignment for temp
     char temp[8];
#ifdef DEBUG     
	 printf("daddpair  drift = %d low = %p  up = %p \n",drift,inrowlower,inrowupper);
	 getchar();
#endif	 
     lminusd = length - drift;
     lmdm1 = lminusd -1;
     lmdm1ov8 = lmdm1/8;
     lmdm1mod8 = lmdm1 % 8;
    for(i=0;i<8*lmdm1ov8;i=i+8)
       {mov8(temp,&inrowlower[i]);
	addeight(temp,&inrowupper[i+drift],&inrowlower[i]);
	addeight(temp,&inrowupper[i+drift+1],&inrowupper[i]);
	}
    if(lmdm1mod8 > 0)
	for(i=8*lmdm1ov8;i<lmdm1;i++)
	    {temp1 = inrowlower[i];
	     inrowlower[i] = temp1 + inrowupper[i+drift];
	     inrowupper[i] = temp1 + inrowupper[i+drift+1];
	     }
    
    temp1 = inrowlower[lminusd-1];
    if(lminusd>0)
    inrowlower[lminusd-1] = temp1 + inrowupper[lminusd-1+drift];

     for(i=lminusd; i<length;i++)
	 inrowupper[i] = inrowlower[i];
	
    inrowupper[lminusd-1] = temp1;
    }



/* this handles the eccentric case which arises when adding an even block
   of rows to an odd block of rows in the non-power-of-2 DADD algorithm.
   It sums the maximum drift from the larger (upper) block with the maximum
   drift from the lower (smaller) block.  This is the only case where paths of
   different total drift are combined.

*/

void topdownsinglesum(short length,short height,char *lower,char *upper)
    {long i;
     short smallheight;
#ifdef DEBUG     
     printf("singlesum called height = %d  low = %p  up = %p \n",height,lower,upper);
#endif     
     smallheight = height -1; 
     for(i=0;i<length-smallheight;i++)
	 upper[i] = lower[i]+upper[i+smallheight];
     for(i=length-smallheight;i<length;i++)     
	 upper[i] = lower[i];
     }
     
  


/*  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    THE FOLLOWING FOUR SUBROUTINES CONSTITUTE THE GENERAL INVERSE DADD
    ALGORITHM. THE INVERSE ALGORITHM IS PASSED AN ARRAY OF PATH SUMS, FROM 
    WHICH IT COMPUTES THE ARRAY OF POWERS WHICH GAVE RISE TO THE PATH SUMS.
    
    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
*/ 





 /* lvec points to a row of pathsums of drift 2k.
   uvec points to a row of pathsums of drift 2k+1.
   drift = 2k.
   length = length of row.
   This routine solves the system of equations which created the
   two rows from a pair of drift k rows, and replaces lvec and uvec
   with the two drift k rows. 
*/

void splitpath(short length,short drift,signed char *lvec,signed char *uvec)
    {long i;
     short temp;
#ifdef DEBUG     
	 printf(" split path args drift = %d low %p high %p \n",drift,lvec,uvec);
	 printf(" input rows \n");
	 printrow(length,(char *) lvec);
	  printrow(length,(char *) uvec); 
#endif	  
     temp = uvec[length - drift -1];
     
     for(i=length-drift-1; i > -1;i--)
	 {uvec[i+drift] = lvec[i] - temp;
	  lvec[i] = temp;
	  temp = uvec[i-1] - uvec[i+drift];
	  }
#ifdef DEBUG	  
	  printrow(length,(char *) lvec);
	  printrow(length,(char *) uvec);
#endif	  
	  
     }


//Solves for the top row in an odd length block										 
void topsolve(long length,short smallerblock,signed char *lvec,signed char *uvec)
    {long i;
#ifdef DEBUG    
     printf("topsolve called smalblk = %d  lvec = %p  uvec = %p \n",smallerblock,lvec,uvec);
#endif     
     for(i=length-smallerblock-1; i>=0;i--)
	 uvec[i+smallerblock] = uvec[i]-lvec[i];
     }

void geninv(long rowcount,long length,long blocklength,long driftbase,signed char *data)
    {long i,lowercount,uppercount,offsetwithinblock;
     signed char *x2d,*x2dp1;
#ifdef DEBUG     
     printf("geninv called rowcount %d blocksize %d driftbase %d \n",rowcount,blocklength,driftbase);
#endif     
     if(rowcount == 1) return;
     lowercount = rowcount/2;
     uppercount = rowcount - lowercount;

     for(i=0;i<lowercount;i++) //set pointers to rows of drift 2 i and 2 i + 1
	 {offsetwithinblock = length*getposition(driftbase,blocklength);
	  x2d = data + blocklength*length*getposition(i,lowercount)+offsetwithinblock;
	  x2dp1 = x2d +blocklength*length*lowercount;

	  splitpath(length,lowercount*driftbase+i,x2d,x2dp1); //solve for the drift i rows in 
					 // the lower and upper blocksets
	 }

     if(lowercount != uppercount)  //if blocksets are different sizes, solve for
				   //top row of upper blockset  
	 {x2dp1 = data + (rowcount-1)*length*blocklength+offsetwithinblock;
	topsolve(length,lowercount,x2d,x2dp1);
       }

     geninv(lowercount,length,blocklength,driftbase,data);  //invert lower blockset sums
     geninv(uppercount,length,blocklength,driftbase,data+length*blocklength*lowercount);//invert upperset block sums
 
   }    

void geninvdvcq(long rowcount,long rowlength,signed char *darray)
  {long i,blockcount,blocksize;
#ifdef DEBUG  
   printf("invdvcq called rowcount = %ld  rlen = %ld \n",rowcount,rowlength);
   getchar();
#endif   
   if((rowcount > CACHEROWS)&&((rowcount % CACHEROWS) == 0))
       {blocksize = rowcount/CACHEROWS; //solve for toplevel block contents
        blockcount = CACHEROWS;
        for(i=0;i<blocksize;i++)
            geninv(blockcount,rowlength,blocksize,i,darray); //solve for drift i row in each block
        
        for(i=0;i<blockcount;i++)
            geninvdvcq(blocksize,rowlength,darray + blocksize*rowlength*i);
       }
    else geninv(rowcount,rowlength,1,0,darray);
  }  



int main()
    {
     int i,j;											 
     int character;
    

    i= NROWS;
    j=NCOLS;
    printf(" NROWS =  %d  NCOLS =  %d  \n",i,j);

                      	//build test array
     for(i=0;i<NROWS;i++)
	  for(j=0;j<NCOLS;j++)
	     {  test11[i*NCOLS + j] = (i+j)%256;
	    //   test2[i][j] = 0.0;
	    //   if(i==4) test1[i][j] = 1.0;
	     }
    dreverse(NROWS,NCOLS,&test1[0][0],&test3[0][0]);  //reverse for negative slopes
    printf("start forward dadd \n");
    
    gendivnconq((short) NROWS,(short) NCOLS,&test1[0][0]); //positive slopes
    gendivnconq((short) NROWS,(short) NCOLS,&test3[0][0]); //negative slopes
    
    printf("end forward dadd \n");
    printf("start inverse dadd \n");
    
    geninvdvcq((long) NROWS,(long) NCOLS,test11); //invert positive slopes
    geninvdvcq((long) NROWS,(long) NCOLS,test33); //invert negative slopes
   
    printf("end inverse dadd \n");   
    getchar();
    
    dreverse(NROWS,NCOLS,&test3[0][0],&test3[0][0]);

    mergehalves(NROWS,NCOLS,&test1[0][0],&test3[0][0]); //reconstruct data array


     for(i=0;i<NROWS;i++) //  rebuild test array
	  for(j=0;j<NCOLS;j++)
	     { test11[i*NCOLS + j] =(i+j)%256;// ((i %8) == 0);
	     }
	     
//	test11[3*NCOLS+30] = 4; 
	 
   comparrays(NROWS,NCOLS,&test1[0][0],&test3[0][0]);
   getchar();
   }




























