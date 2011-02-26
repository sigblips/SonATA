/*******************************************************************************

 File:    machine-dependent.h
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

// machine-dependent.h

#ifndef machine_dependent_h
#define machine_dependent_h


#include <config.h>

/* defined so that functionality described in X/Open portabiity guide is
   available, at least for GNU C library
   this includes typedefs for uint32_t, etc */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

/* __EXTENSIONS__ needed to include the definition of struct timeval
   in sys/time.h which is used in sys/resource.h, which is included by
   sseInterfaceLib/HostNetByteorder.h */
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#if HAVE_LIMITS_H || _LIBC
# include <limits.h>
#endif
#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#include <sys/types.h>

/* The following contortions are an attempt to use the C preprocessor
   to determine an unsigned integral type that is 32 bits wide.  An
   alternative approach is to use autoconf's AC_CHECK_SIZEOF macro, but
   doing that would require that the configure script compile and *run*
   the resulting executable.  Locally running cross-compiled executables
   is usually not possible.  */

#if __STDC__
# define UINT_MAX_8_BITS 255U
# define UINT_MAX_16_BITS 65535U
# define UINT_MAX_32_BITS 4294967295U
#else
# define UINT_MAX_8_BITS 0xFF
# define UINT_MAX_16_BITS 0xFFFF
# define UINT_MAX_32_BITS 0xFFFFFFFF
#endif

/* If UINT_MAX isn't defined, assume it's a 32-bit type.
   This should be valid for all systems GNU cares about because
   that doesn't include 16-bit systems, and only modern systems
   (that certainly have <limits.h>) have 64+-bit integral types.  */

#ifndef UINT_MAX
# define UINT_MAX UINT_MAX_32_BITS
#endif

#if UINT_MAX == UINT_MAX_32_BITS
# ifndef HAVE_UINT32_T
typedef unsigned uint32_t;
# endif
# ifndef HAVE_INT32_T
typedef int int32_t;
# endif
#else
# if USHRT_MAX == UINT_MAX_32_BITS
#  ifndef HAVE_UINT32_T
typedef unsigned short uint32_t;
#  endif
#  ifndef HAVE_INT32_T
typedef short int32_t;
#  endif
# else
#  if ULONG_MAX == UINT_MAX_32_BITS
#   ifndef HAVE_UINT32_T
typedef unsigned long uint32_t;
#   endif
#   ifndef HAVE_INT32_T
typedef long int32_t;
#   endif
#  else
  /* The following line is intended to throw an error.  Using #error is
     not portable enough.  */
  "Cannot determine unsigned 32-bit data type."
#  endif
# endif
#endif

#ifndef UCHAR_MAX
# define UCHAR_MAX UINT_MAX_8_BITS
#endif

#if UCHAR_MAX == UINT_MAX_8_BITS
# ifndef HAVE_UINT8_T
typedef unsigned char uint8_t;
# endif
# ifndef HAVE_INT8_T
typedef signed char int8_t;
# endif
#else
  /* The following line is intended to throw an error.  Using #error is
     not portable enough.  */
  "Cannot determine unsigned 8-bit data type."
#  endif

typedef double        float64_t;
typedef float         float32_t;
typedef char          char8_t;

// end machine-dependent typedefs

#endif  // machine_dependent_h 