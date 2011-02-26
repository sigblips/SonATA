/*******************************************************************************

 File:    Ifc.i
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

// $Id: Ifc.i,v 1.16 2007/07/06 22:23:28 sse Exp $
// Swig wrapper for Ifc class.

// Original code created by L. R. McFarland 2003

%module Ifc
%{
#include "Ifc.h"
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

class Ifc {

 public:

  Ifc() {};

   %extend {

     // added to un-overload accessors for tcl

     const char* getname() {return(self->name());}
     const char* setname(const char* name) {return(self->name(name));}

     const char* getversion() {return(self->version());}

     // ===== STX configuration =====

     const char* getstxfilename() {return(self->getStx().filename());}
     const char* setstxfilename(const char* name) {
                                            return(self->getStx().filename(name));}

     int gethistlen() {return(self->getStx().histogramLength());}
     int sethistlen(int len) {return(self->getStx().histogramLength(len));}

     double gettol() {return(self->getStx().tolerance());}
     double settol(double tol) {return(self->getStx().tolerance(tol));}

     int getmaxtries() {return(self->getStx().maxTries());}
     int setmaxtries(int count) {return(self->getStx().maxTries(count));}

     const char* stxstart() {return(self->getStx().start());}
     const char* stxstatus() {return(self->getStx().status());}

     const char* stxvariance(double lcpVar, double rcpVar) {
       return(self->getStx().setvariance(lcpVar, rcpVar));
     }

     const char* stxpol(const char *pol) {
       return(self->setStxPol(pol));
     }

     // for debugging
     void setstxstatus(unsigned int status) 
	{ self->getStx().setStatus(status);}

     void setstxleftvar(double variance) 
	 {self->getStx().setLeftVar(variance);}

     void setstxrightvar(double variance)
	 {self->getStx().setRightVar(variance);}



   }

  // ===== GPIB configuration =====

  const char* simscu(const char *mode);
  const char* addressscu(int bus, int addr);
  const char* getscu();

  // ===== commands =====

  const char* attn(int attnl, int attnr);
  const char* gpib(const char* mode);
  const char* help();
  const char* id();
  const char* ifsource(const char* source);
  const char* intrinsics();
  const char* off();
  const char* requestready();
  const char* reset();
  const char* selftest();
  const char* simulate(const char* mode);
  const char* status();

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