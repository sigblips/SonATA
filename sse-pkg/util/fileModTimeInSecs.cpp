/*******************************************************************************

 File:    fileModTimeInSecs.cpp
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


#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>

/* print the last modification time of a file
 * in seconds since the epoc
 */

using namespace std;

time_t getFileModTime(char *filename);

int main(int argc, char *argv[]) 
{
  char *filename;

  if (argc != 2)
  {
     cerr << "Prints last file mod time in secs" << endl;
     cerr << "Usage: " << argv[0] << " <filename>" << endl;
     exit(1);
  }

  filename = argv[1];

  cout << getFileModTime(filename) << endl;

}


// find the latest file modification time
time_t getFileModTime(char *filename)
{
    struct stat buf;
    if ( stat(filename, &buf) != 0)
    {
       cerr << "error: can't read file time" << endl;
       exit(1);
    }

    return buf.st_mtime;
}

