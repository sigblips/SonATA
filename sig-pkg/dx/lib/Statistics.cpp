/*******************************************************************************

 File:    Statistics.cpp
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
// DxStatistics: statistical functions used by the DX
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/Statistics.cpp,v 1.4 2009/06/05 04:30:28 kes Exp $
//
#include <math.h>
#include "Sonata.h"
#include "System.h"
#include "Statistics.h"

using namespace sonata_lib;

namespace dx {

//
// chiSquare
//
//  computes 1 - the cumulative Chi-squared distribution with 2n degrees
//  of freedom for argument x.
//
float64_t
ChiSquare(int32_t n, float64_t x)
{
	int32_t i;
	float64_t sizeCheck, term, sum, chiSq;

	chiSq = 0;
	if (n < 0 || n % 2 || x < 0)
		return (0.0);
	if (x == 0)
		return (0.0);

	// normalize incoming values
	n /= 2;
	x /= 2;

	// check for overflow
	if (x < n)
		sizeCheck = maxExp((int32_t) x, x);
	else
		sizeCheck = maxExp(n - 1, x);

	if (sizeCheck > MAX_EXP10)
		return (chiSquareLog(n * 2, x * 2));

	term = sum = 1;

	for (i = 1; i < n; i++) {
		term *= x / i;
		sum += term;
	}
	chiSq = log(sum) - x;
	return (chiSq);
}

//
// ChiSquareLog: perform a ChiSquare calculation using logs
//
// Notes:
//      This cannot overflow, but it is slower than the
//      standard version, and thus is used only when overflow is
//      possible.
//      Due to truncation of the exponent, significance in some
//      cases may be higher than actual values.  We haven't been
//      able to avoid this, because the factorial term becomes
//      huge for large n.
//
float64_t
chiSquareLog(int32_t n, float64_t x)
{
	int32_t i;
	float64_t logX, term, sum, chiSq;

	chiSq = 0;
	if (n < 0 || n % 2 || x < 0.0)
 		return (0.0);

	if (x == 0)
		return (0);

	// normalize incoming values
	n /= 2;
	x /= 2;

	term = 0.0;
	sum = 1.0;
	logX = log(x);

	for (i = 1; i < n; i++) {
		term += logX - log((double) i);
		if (term < MAX_EXP)
			sum += exp(term);
		else
			sum += exp(MAX_EXP);
	}
	chiSq = log(sum) - x;
	return (chiSq);
}

//
// gaussDev: return a random gaussian noise value
//
float32_t
GaussDev(int32_t *idum)
{
	static int32_t iset = 0;
	static float64_t gset;
	float64_t fac, r, v1, v2;

	if (iset == 0) {
		do {
			v1 = 2.0 * ran2(idum) - 1.0;
			v2 = 2.0 * ran2(idum) - 1.0;
			r = v1 * v1 + v2 * v2;
	 	} while (r >= 1.0);
	 	fac = sqrt(-2.0 * log(r) / r);
	 	gset = v1 * fac;
	 	iset = 1;
	 	return ((float32_t) (v2 * fac));
	} else {
		iset = 0;
		return ((float32_t) gset);
	}
}

// constants for random number generator (Ran2)
#define M 714025
#define IA 1366
#define IC 150889

//
// ran2: random number generator
//
static float32_t
ran2(int32_t *idum)
{
	static int32_t iy, ir[98];
	static int32_t iff = 0;
	int32_t j;

	if (*idum < 0 || iff == 0) {
		iff = 1;
		if ((*idum = (IC- (*idum)) % M) < 0)
			*idum = -(*idum);
		for (j = 1; j <= 97; j++) {
			*idum = (IA * (*idum) +IC) % M;
			ir[j] = *idum;
		}
		*idum = (IA * (*idum) + IC) % M;
		iy = *idum;
	}
	j = (int32_t) (1 + 97.0 * iy / M);
#ifdef notdef
	if (j > 97 || j < 1)
		Fatal(ERR_R2E, "Ran2: this cannot happen");
#endif
	iy = ir[j];
	*idum = (IA * (*idum) + IC) % M;
	ir[j] = *idum;
	return ((float32_t) iy / M);
}

#undef M
#undef IA
#undef IC

//
// maxExp: estimate the exponent of the largest term in the sum
//		calculated in ChiSquare
//
static float64_t
maxExp(int32_t n, float64_t x)
{
	return ((n * log(x) + n - (n + 0.5) * log(n) - HALFLOG2PI) / M_LOG10E);
}

}
