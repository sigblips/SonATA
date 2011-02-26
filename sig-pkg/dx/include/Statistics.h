/*******************************************************************************

 File:    Statistics.h
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

//
// Statistical functions
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/Statistics.h,v 1.2 2009/02/22 04:37:45 kes Exp $
//
#ifndef _StatisticsH
#define _StatisticsH

namespace dx {

// function prototypes
float64_t ChiSquare(int32_t n, float64_t x);
float32_t GaussDev(int32_t *idum);

static float64_t chiSquareLog(int32_t n, float64_t x);
static float32_t ran2(int32_t *idum);
static float64_t maxExp(int32_t n, float64_t x);

}

#endif