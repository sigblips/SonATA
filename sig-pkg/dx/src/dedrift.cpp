/*******************************************************************************

 File:    dedrift.cpp
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
// Dedrift: functions to handle dedrifting of time domain data
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/dedrift.cpp,v 1.7 2009/06/05 04:38:24 kes Exp $
//
#include "dedrift.h"
#include "DxErr.h"

namespace dx {

//
// DedriftFTPlane: dedrift the frequency-time plane
//
// Notes:
//		[The following explanation is from Rick Stauduhar and was written
//		for the original FUDD dedrift code.]
//
//		In the complex array sftp we have time sampled data, which can be
//		thought of as arranged in a rectangular array of dimensions
//		BinsPerSpec by spec.  The samples are assumed to be of an exponential
//		of the form exp(i*(ak+bk^2)), with k running from totalcount to
//		totalcount + spec*BinsPerSpec.  a = shiftcon, and b = driftcon/2 .
//		We want to remove the advancing phase from the data, leaving a
//		signal approximately constant in phase. Because the quantity in the
//		exponent is quadratic, we can generate the sequence of values in the
//		exponent by building second differences.  First we increment the
//		exponent, then we increment the difference.  The "increments" are
//		complex multiplies by complex exponentials, which effect addition
//		in the exponent.
//
void
DedriftFTPlane(ComplexFloat32 *cftp, ComplexFloat32 *sftp,
		int32_t blks, int32_t samplesPerBlk, int32_t firstSample,
		float64_t shiftConst, float64_t driftConst, float32_t *power)
{
	float32_t temp1, temp2, temp3, totalPower = 0;

#ifdef notdef
	// compute the power in the spectrum
	for (int32_t sample = 0; sample < blks * samplesPerBlk; sample++)
		totalPower += norm(cftp[sample]);
	DxDebug(DEBUG_NEVER, totalPower, "totalPower before dedrift");
	totalPower = 0;
#endif

	shiftConst *= 2 * M_PI;
	driftConst *= M_PI;

	temp1 = 2 * driftConst;
	ComplexFloat64 ei2b(cos(temp1), -sin(temp1));

	// to minimize computational rounding errors, reinitialize
	// the complex exponentials for each spectrum
	for (int32_t blk = 0; blk < blks; blk++, firstSample += samplesPerBlk) {
		temp2 = (2 * firstSample + 1) * driftConst + shiftConst;
		ComplexFloat64 ei2tbp1pa(cos(temp2), -sin(temp2));

		temp3 = firstSample * (firstSample * driftConst + shiftConst);
		ComplexFloat64 eitsq(cos(temp3), -sin(temp3));
		ComplexFloat64 totalSum(0, 0);
		for (int sample = 0; sample < samplesPerBlk; sample++, cftp++) {
			ComplexFloat64 dedrift(cftp->real(), cftp->imag());
			dedrift *= eitsq;
			totalSum += dedrift;
			totalPower += norm(dedrift);
			eitsq *= ei2tbp1pa;
			ei2tbp1pa *= ei2b;
		}
		*sftp = ComplexFloat32(totalSum.real(), totalSum.imag());
		sftp++;
	}
	if (power)
		*power += totalPower;
#ifdef notdef
	// print the power in the extracted channel
	totalPower = 0;
	for (int32_t blk = 0; blk < blks; blk++)
		totalPower += norm(isftp[blk]);
	DxDebug(DEBUG_NEVER, totalPower, "totalPower after dedrift");
#endif
}

}