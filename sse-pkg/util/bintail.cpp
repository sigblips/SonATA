/*******************************************************************************

 File:    bintail.cpp
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

/* 
 * Similar to 'tail -f' but for binary files.
 * Takes filename as first arg, sends output to stdout.
 * At EOF, program sleeps until more input is available.
 * If input file is truncated, the program exits.
 * Written in C to get nonblocking reads to work.
 */

#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFF_SIZE 32768

int getFileSize(int filedes)
{
    int filesize = 0;

    struct stat buf;
    if (fstat(filedes, &buf) == 0)
    {
        filesize = buf.st_size;
    }
    else 
    {
       fprintf(stderr, "error finding filesize for filedes %d\n",
	       filedes);
    }

    return filesize;
}


int main(int argc, char *argv[])
{
   char buf[BUFF_SIZE];
   long pos;
   int fd;
   bool debug(false);

   if (argc < 2)
   {
      fprintf(stderr, "usage: prog <filename>\n");
      exit(EXIT_FAILURE);
   }

   char *fname = argv[1];

   if (-1 == (fd = open(fname, O_RDONLY | O_NONBLOCK, 0)))
   {
      fprintf(stderr,"Cannot open file: %s\n", fname);
      exit(EXIT_FAILURE);
   }

   if (-1 == lseek(fd, 0L, SEEK_SET))  // seek to beginning
   {
      fprintf(stderr,"Cannot lseek file: %s\n", fname);
      exit(EXIT_FAILURE);
   }

   if ((pos = lseek(fd, 0L, SEEK_CUR)) == -1)
   {
      fprintf(stderr,"Cannot seek file: %s\n", fname);
      exit(EXIT_FAILURE);
   }


   bool alreadyReadSomeData = false;

   for (;;)
   {
      //fgets(buf, sizeof(buf), fp);
      int readCount = read(fd, buf, sizeof(buf));

      if (debug)
      {
	 fprintf(stderr, "readcount is %d\n", readCount);
      }

      if (readCount == -1)
      {
	 // no data available, sleep

	 if (debug)
	 {
	    fprintf(stderr, "sleeping...\n");
	 }

	 sleep(1); /* give up the cpu to other processes */

	 continue;
      }

      if (readCount == 0)
      {
	 // eof
	 if (debug)
	 {
	    fprintf(stderr, "at eof\n");
	 }

	 if (alreadyReadSomeData & getFileSize(fd) == 0)
	 {
	    if (debug)
	    {
	       fprintf(stderr, "file was truncated\n");
	    }
	    break;
	 }

	 // At EOF, remember this file position, and
	 // sleep, waiting for more data

	 lseek(fd, pos, SEEK_SET);

	 if (debug)
	 {
	    fprintf(stderr, "sleeping...\n");
	 }
	 sleep(1); /* give up the cpu to other processes */

	 continue;

      }

      alreadyReadSomeData = true;

      pos = lseek(fd, 0L, SEEK_CUR);

      if (debug)
      {
	 fprintf(stderr, "read %d\n", readCount);
      }

      write(STDOUT_FILENO, buf, readCount);


   }

}