/*******************************************************************************

 File:    test.cpp
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

#include <iostream>
#include <iomanip>
#include <string.h>
#include "Gaussian.h"
#include "userSignals.h"

using namespace gauss;
using std::cout;
using std::dec;
using std::hex;
using std::setfill;
using std::setw;
using std::endl;

const int32_t SAMPLES = 1000000;

void printStats(const ComplexFloat64 *samples, int32_t len);
void printStats(const ComplexFloat4 *samples, int32_t len);

int
main(int argc, char **argv)
{
	ComplexFloat64 *samples = new ComplexFloat64[SAMPLES];
	ComplexFloat4 *f4Samples = new ComplexFloat4[SAMPLES];

	// create a sample generator
	Gaussian gen;
	cout << "Gaussian library version ";
	cout << hex << setfill('0') << setw(4) << gen.getVersion();
	cout << ", I/F version " << setw(4) << gen.getIfVersion() << endl;;
	cout << dec << SAMPLES << " samples generated" << endl;

	gen.setup(1, 1, 1);
	gen.getSamples(samples, SAMPLES);
	ComplexFloat64 power = gen.getPower();
	ComplexFloat64 sum = gen.getSum();
	cout << "ComplexFloat64, noise only, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	gen.setup(1, 1, 1);
	gen.getSamples(f4Samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << "ComplexFloat4, noise only, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(f4Samples, SAMPLES);

	gen.setup(1, 1, 0);
	gen.addCwSignal(0, 1, 1);
	gen.getSamples(samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl << "ComplexFloat64, cw signal only, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	gen.setup(1, 1, 0);
	gen.addCwSignal(0, 1, 1);
	gen.getSamples(f4Samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl << "ComplexFloat4, cw signal only, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(f4Samples, SAMPLES);

	gen.setup(1, 1, 0);
	gen.addPulseSignal(.125, 0, 1, 0, .000001, .000001);
	gen.getSamples(samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl << "pulse signal only, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	gen.setup(0, 1, 1);
	gen.addCwSignal(.125, 0, 1e6);
	gen.getSamples(samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl << "noise with cw, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	gen.setup(0, 1, 1);
	gen.addPulseSignal(.5, 0, 1e6, 0, 0.000001, .000001);
	gen.getSamples(samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl << "noise with power, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	gen.setup(0, 1, 1);
	gen.addCwSignal(.125, 0, 1e6);
	gen.addPulseSignal(.5, 0, 1e6, 0, 0.000001, .000001);
	gen.getSamples(samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl << "noise with cw and pulse, power = 1" << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	gen.setup(0, 1, 1);
	BasicSig *sig = new BasicSig(gen.getBandwidth(), gen.getNoisePower(), .125,
			0, 1e6);
	gen.addUserSignal(sig, scaleSignal);
	gen.getSamples(samples, SAMPLES);
	power = gen.getPower();
	sum = gen.getSum();
	cout << endl;
	cout << "noise with user signal scaling sample by sqrt(2), power = 1";
	cout << endl;
	cout << "power = (" << power.real() << ", " << power.imag() << "), avg = ";
	cout << (power.real() + power.imag()) / SAMPLES << endl;
	cout << "sum = (" << sum.real() << ", " << sum.imag() << ")" << endl;
	sum /= SAMPLES;
	printStats(samples, SAMPLES);

	return (0);
}

/**
 * Compute statistics for the sample buffer.
 *
 * Description:\n
 * 	Computes the power statistics for the sample buffer to determine whether
 * 	the complex components conform to a Gaussian distribution.
 */
void
printStats(const ComplexFloat64 *samples, int32_t len)
{
	ComplexFloat64 sum(0, 0);
	for (int32_t i = 0; i < len; ++i)
		sum += samples[i];
	ComplexFloat64 mean = sum / (float64_t) len;
	// Compute the variance
	ComplexFloat64 variance(0, 0);
	for (int32_t i = 0; i < len; ++i) {
		ComplexFloat64 diff = samples[i] - mean;
		variance.real() += diff.real() * diff.real();
		variance.imag() += diff.imag() * diff.imag();
	}
	variance /= len - 1;
	ComplexFloat64 sd = ComplexFloat64(sqrt(variance.real()),
			sqrt(variance.imag()));
//	cout << endl;
	cout << "mean = (" << mean.real() << ", " << mean.imag() << ")" << endl;
	cout << "sd = (" << sd.real() << ", " << sd.imag() << ")" << endl;
}

/**
 * Compute statistics for the sample buffer.
 *
 * Description:\n
 * 	Computes the power statistics for the sample buffer to determine whether
 * 	the complex components conform to a Gaussian distribution.
 */
void
printStats(const ComplexFloat4 *samples, int32_t len)
{
	ComplexFloat64 sum(0, 0);
	for (int32_t i = 0; i < len; ++i) {
		ComplexFloat64 v((float4_t) samples[i].real(),
				(float4_t) samples[i].imag());
		sum += v;
	}
	ComplexFloat64 mean = sum / (float64_t) len;
	// Compute the variance
	ComplexFloat64 variance(0, 0);
	for (int32_t i = 0; i < len; ++i) {
		ComplexFloat64 v((float4_t) samples[i].real(),
				(float4_t) samples[i].imag());
		ComplexFloat64 diff = v - mean;
		variance.real() += diff.real() * diff.real();
		variance.imag() += diff.imag() * diff.imag();
	}
	variance /= len - 1;
	ComplexFloat64 sd = ComplexFloat64(sqrt(variance.real()),
			sqrt(variance.imag()));
//	cout << endl;
	cout << "mean = (" << mean.real() << ", " << mean.imag() << ")" << endl;
	cout << "sd = (" << sd.real() << ", " << sd.imag() << ")" << endl;
}
