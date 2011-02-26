/*******************************************************************************

 File:    TestSig.i
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

// SWIG interface for the TestSig class

// Original code created by L. R. McFarland 2003

%module TestSig
%{
#include "TestSig.h"
%}

%{

#include <string.h>

// Replacement for strcmp, since purify is reporting 
// array bounds read errors for the one normally being
// linked in.  Code is from:
// ftp://ftp.openbsd.org/pub/OpenBSD/src/lib/libc/string/strcmp.c

/*
 * Compare strings.
 */
int
strcmp(const char *s1, const char *s2)
{
        while (*s1 == *s2++)
                if (*s1++ == 0)
                        return (0);
        return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

%}


class TestSig {

 public:

  TestSig() {};

   %extend {

     const char* getname()                  {return(self->name());}
     const char* setname(const char* name)  {return(self->name(name));}

     const char* getversion()               {return(self->version());}

     const char* setpulsegensource(const char* pulseGenSource)  {return(self->setPulseGenSource(pulseGenSource));}

     const char* setsiggensource(const char* sigGenSource)  {return(self->setSigGenSource(sigGenSource));}

     const char* getpulsegensource() { return(self->getPulseGenSourceName()); }

   }


  // ===== GPIB configuration =====
  //const char* setsiggensource(const char *mode);
  const char* setsiggensim(const char *mode);
  const char* setsiggenaddress(int bus, int addr);
  const char* getsiggeninfo();
  double setsiglimit(double limit);
  double getsiglimit();

  const char* setpulsegensim(const char *mode);
  const char* setpulsegenaddress(int bus, int addr);
  const char* getpulsegeninfo();
  double setpulselimit(double limit);
  double getpulselimit();

  // ===== commands =====
  const char* gpib(const char* mode);
  const char* help();
  const char* id();
  const char* intrinsics();
  const char* off();
  const char* on();
  const char* quiet();
  const char* requestready();	
  const char* reset();
  const char* simulate(const char* mode);
  const char* selftest();
  const char* status();
  const char* tunesiggen(double frequencyMHz, double amplitudedBm, 
			 double driftRateHz, double duration);
  const char* pulse(double amplitudedBm, double periodSec, double widthSec);

  const char *gpibcmd(const char *cmd, const char *arg1="",
	const char*arg2="", const char *arg3="", const char *arg4="");

};


/*
 make the swig 'defined but not used' warnings go away
 for swig version 1.3.21 and below
*/
#if SWIG_VERSION <= 0x010321

%inline %{
static void dummySwigRoutine()
{
    if (false)
    {	
	SWIG_Tcl_TypeDynamicCast(0, 0);
	SWIG_Tcl_TypeName(0);
	SWIG_Tcl_TypeQuery(0);	
	SWIG_Tcl_PointerTypeFromString(0); 
	SWIG_Tcl_ConvertPacked(0,0,0,0,0,0);
    }
}
%}

#endif