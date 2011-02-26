/*******************************************************************************

 File:    Dadd.cpp
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
// SonATA DADD library class definition
//
#include <fftw3.h>
#include "Dadd.h"
#include "Report.h"

using std::endl;

namespace dadd {

Dadd::Dadd(): spectrumBins(0), totalBins(0), spectra(0),
		threshold(0), nBands(0), bandBins(0), badBandLimit(0), type(TDDadd),
		bands(0)
{
}

Dadd::~Dadd()
{
}

/**
* Set up the library for specific DADD parameters
*
* Description:\n
*	Records the operating parameters and initializes recording of
*	statistics.\n\n
* Notes:\n
*	Must be called before initiating DADD processing for a given
*	activity.
*
* @param	bins_, the width of the input spectrum in bins.
* @param	totalBins
* @param	spectra_, the number of spectra.
* @param	threshold_, the pathsum false-alarm threshold.
* @param	badBandLimit_, maximum # of hits in a band before it is
*			logged as a bad band.
* @param	accumulatorSize, the width of the accumulators in bytes.
* @param	type_, the type of DADD to perform (top down or cache efficient).
*/
void
Dadd::setup(int32_t spectra_, int32_t spectrumBins_, int32_t totalBins_,
			int32_t threshold_, int32_t bandBins_, int32_t badBandLimit_,
			DaddType type_, bool reportBinStats_)
{
	spectra = spectra_;
	spectrumBins = spectrumBins_;
	totalBins = totalBins_;
	threshold = threshold_;
	bandBins = bandBins_;
	badBandLimit = badBandLimit_;
	type = type_;
	reportBinStats = reportBinStats_;
	initBands();
	stats.reset();
}

/**
 * Reset the dadd.
 *
 * Description:\n
 * 	Initializes the dadd library for the next polarization.  Simply resets
 * 	the band array and the statistics.
 */
void
Dadd::reset()
{
	initBands();
	stats.reset();
}

/**
 * Initialize the band arrays.
 *
 * Description:\n
 * 	Reallocate the band array if necessary, then reset all bands.\n\n
 * Notes:\n
 * 	There is one band array consisting of n DaddBand structures,
 * 	where n is computed by
 * 	dividing the total bandwidth in bins by the band bandwidth in bins.  The
 * 	array is used to record the total number of hits for each band as well
 * 	as the maximum power path in the band.  If the number of hits in a single
 * 	band exceeds the limit, the band will be recorded as bad.\n
 * 	IMPORTANT NOTE: if multiple polarizations are being processed,
 * 	reportBadBands() must be called after each polarization (both slopes)
 * 	has been processed, then dadd.reset must be called to initialize
 * 	the array for the next polarization.
 */
void
Dadd::initBands()
{
	int32_t n = spectrumBins / bandBins;
	if (spectrumBins % bandBins)
		++n;
	if (nBands < n && bands) {
		fftwf_free(bands);
		bands = 0;
	}
	if (!bands) {
		// allocate a new band array
		size_t size = n * sizeof(DaddBand);
		bands = static_cast<DaddBand *> (fftwf_malloc(size));
		DaddAssert(bands);
	}
	// initialize the band array
	for (int32_t i = 0; i < n; ++i) {
		bands[i].reset();
		bands[i].bin = i * bandBins;
		bands[i].width = bandBins;
		bands[i].limit = badBandLimit;
	}
	nBands = n;
}

/**
* Execute DADD on a buffer of data.
*
* Description:\n
*	Runs DADD on the specified buffer according to the parameters
*	to setup.  Hits are reported as found, as are bad bands.
*
* @param	pol, the polarization of the buffer data.
* @param	baseBin, initial (leftmost) bin of the slice.
* @param	detectionBuf, the buffer containing the bin data.  This is
*			also the buffer used for path sums.
* @param	slope, the sign of the slope.  Positive or negative.
* @param	reportHit, a callback function called when a pathsum exceeds
*			threshold.
* @param	reportBadBand, a callback function called when the maximum
*			number of hits in a band is exceeded.
*/
void
Dadd::execute(Polarization pol, DaddSlope slope, DaddAccum *data,
		ReportHit *reportHit)
{
#if (DADD_TIMING)
	uint64_t t0 = getticks();
#endif
	stats.pol = pol;
	if (reportBinStats && slope == Positive)
		computeBinStatistics(pol, data);
#if (DADD_TIMING)
	uint64_t t1 = getticks();
#endif
	topDown(spectra, totalBins, data);
#if (DADD_TIMING)
	uint64_t t2 = getticks();
#endif
	thresholdData(data);
#if (DADD_TIMING)
	uint64_t t3 = getticks();
#endif
	reportHits(pol, slope, data, reportHit);
#if (DADD_TIMING)
	uint64_t t4 = getticks();
	++timing.dadd.dadds;
	timing.dadd.computeBinStats += elapsed(t1, t0);
	timing.dadd.topDown += elapsed(t2, t1);
	timing.dadd.threshold += elapsed(t3, t2);
	timing.dadd.reportHits += elapsed(t4, t3);
	timing.dadd.total += elapsed(t4, t0);
#endif
}

/**
 * Report over-threshold paths.
 *
 * Description:\n
 * 	Searches the thresholded pathsum buffer, looking for bins which
 * 	are non-zero, which means they are over the threshold.  Reports
 * 	the bin, drift and power of paths found.\n
 * Notes:\n
 * 	If the number of hits exceeds the maximum, it is declared a bad
 * 	band.\n
 * 	For negative-slope paths, the calculation must take into account
 * 	the fact that the data has been loaded into the buffer in mirror
 * 	image order.
 *
 */
void
Dadd::reportHits(Polarization pol, DaddSlope slope, DaddAccum *data,
		ReportHit *reportHit)
{
	// scan each row
	DaddPath maxPath;
	for (int32_t drift = 0; drift < spectra; ++drift) {
		int32_t row = getPosition(drift, spectra);

		DaddAccum *dp = data + row * totalBins;
		for (int32_t bin = 0; bin < spectrumBins; ++bin) {
			if (dp[bin] && (bin + drift) < spectrumBins) {
				int32_t power = dp[bin] + threshold;
				// adjust bin and negate the drift
				int32_t actualBin = 0;
				int32_t actualDrift = 0;
				if (slope != Negative) {
					actualBin = bin;
					actualDrift = drift;
				}
				else if (drift) {
					// report only non-zero drift hits for negative slope
					// bins are mirror-imaged in buffer
					actualBin = (totalBins - 1) - bin;
					actualDrift = -drift;
				}
				else
					continue;

				if (actualBin >= 0) {
					// keep track of the maximum power in this band; reported
					// only if it is a bad band
					int32_t i = actualBin / bandBins;
					DaddBand& b = bands[i];
					if (power > b.maxPath.power) {
						b.maxPath.pol = pol;
						b.maxPath.bin = actualBin;
						b.maxPath.drift = actualDrift;
						b.maxPath.power = power;
					}
					// if we just passed the bad band limit, record this
					// slice as a bad band
					if (b.hits++ > b.limit)
						b.bad = true;
					else if (reportHit) {
						DaddPath path(pol, actualBin, actualDrift, power);
						reportHit->report(path);
					}
				}
			}
		}
	}
}

/**
 * Report bad bands.
 *
 * Description:\n
 * 	Reports all the bad bands found for the given polarization.  Bad
 * 	bands are reported via a callback function provided by the caller.\n\n
 * Notes:\n
 * 	This function should be called once, after both slopes for each
 * 	polarization have been processed;
 */
void
Dadd::reportBadBands(ReportBadBand *reportBadBand)
//		void (*reportBadBand)(Polarization, int32_t, const DaddBand&))
{
#if (DADD_TIMING)
	uint64_t t0 = getticks();
#endif
	DaddHitStatistics& h = stats.hitStats;
	h.reset();
	for (int32_t i = 0; i < nBands; ++i) {
		h.hits += bands[i].hits;
		if (bands[i].maxPath.power > h.maxPath.power)
			h.maxPath = bands[i].maxPath;
		if (bands[i].bad) {
			++h.badBands;
			if (reportBadBand)
				reportBadBand->report(bands[i]);
		}
	}
#if (DADD_TIMING)
	uint64_t t1 = getticks();
	++timing.badBand.reports;
	timing.badBand.total += elapsed(t1, t0);
#endif
}

/**
 * Compute the bin statistics for a given polarization.
 *
 * Description:\n
 * 	Called on the input before computing any pathsums, this function
 * 	computes the bin statistics:
 */
void
Dadd::computeBinStatistics(Polarization pol, DaddAccum *data)
{
	DaddBinStatistics& b = stats.binStats;
	b.reset();
	for (int32_t i = 0; i < spectra; ++i) {
		DaddAccum *spectrum = data + i * totalBins;
		for (int32_t j = 0; j < spectrumBins; ++j)
			++b.bins[spectrum[j]];
	}
}

/**
 * Compute the hit statistics for a polarization.
 *
 * Description:\n
 * 	Computes the hit statistics for the specified polarization.\n\n
 * Notes:\n
 * 	Should not be called until both slopes have been done.
 */
void
Dadd::computeHitStatistics(Polarization pol)
{
	DaddHitStatistics& h = stats.hitStats;
	for (int32_t i = 0; i < nBands; ++i) {
		h.hits += bands[i].hits;
		if (bands[i].bad)
			++h.badBands;
		if (bands[i].maxPath.power > h.maxPath.power)
			h.maxPath = bands[i].maxPath;
	}
}

}

