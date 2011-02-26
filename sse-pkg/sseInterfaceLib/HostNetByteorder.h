/*******************************************************************************

 File:    HostNetByteorder.h
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


// Routines to convert floats & doubles to/from host/network byte order.
// Assume network byte order is same as bigendian.

#ifndef HOSTNETBYTEORDER_H
#define HOSTNETBYTEORDER_H

// config.h header file is supposed to define WORDS_BIGENDIAN
// to indicate host's byte order

#include "machine-dependent.h"

// needed for netinet/in.h
// define macros to let us do net to host conversions in-place
#define NTOHL(x) (x) = ntohl(x)
#define NTOHD(x) (x) = ntohd(x)
#define NTOHF(x) (x) = ntohf(x)

#define HTONL(x) (x) = htonl(x)
#define HTOND(x) (x) = htond(x)
#define HTONF(x) (x) = htonf(x)

// for declaring the real hton[ls] & ntoh[ls]
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>


// if host is bigendian, make the conversion routines
// into no-ops

#ifdef WORDS_BIGENDIAN

#define ntohf(x) (x)
#define ntohd(x) (x)
#define htonf(x) (x)
#define htond(x) (x)

#else


// define some unions to help with byte swapping:

// assumes float & int are same size
// add sizeof check?
union FloatInt
{
    float f;
    int i;
};

// assumes double is twice the size of int
union DoubleIntArray
{
    double d;
    int i[2];
};

// convert a float from host to network byte order
// assumes floats are 4 bytes
inline float htonf(float fvalue)
{
    FloatInt fi;

    // assign the float value, then use htonl to
    // swap the int equivalent
    fi.f = fvalue;
    fi.i = htonl(fi.i);
    return fi.f;
}

// convert a double from host to network byte order
// assumes doubles are 8 bytes long
// and ints are 4 bytes long
inline double htond(double dvalue)
{
    DoubleIntArray di;
    di.d = dvalue;

    // assign the double value, then
    // swap the two ints and use htonl to
    // reorder the bytes within each int
    int temp = di.i[0];
    di.i[0] = htonl(di.i[1]);
    di.i[1] = htonl(temp);
    return di.d;
}

// convert a float from network to host byte order
// assumes floats are 4 bytes
inline float ntohf(float fvalue)
{
    FloatInt fi;
    fi.f = fvalue;
    fi.i = ntohl(fi.i);
    return fi.f;
}

// convert a double from network to host byte order
// assumes doubles are 8 bytes long
// and ints are 4 bytes long
inline double ntohd(double dvalue)
{
    DoubleIntArray di;
    di.d = dvalue;
    int temp = di.i[0];
    di.i[0] = ntohl(di.i[1]);
    di.i[1] = ntohl(temp);
    return di.d;
}


#endif


#endif