/*******************************************************************************

 File:    Dadd.h
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

// SonATA DADD library header file

#ifndef _DaddH
#define _DaddH

#include <assert.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <sseInterface.h>
#include "cycle.h"
#include "DaddVersion.h"

namespace dadd {

class ReportHit;
class ReportBadBand;

#define DADD_TIMING			(true)

// macros
#define DaddAssert(exp)		assert(exp)
#define DADD_ALIGNED(add)	((((uint64_t) addr) & 0xf) == 0)

typedef uint16_t DaddAccum;			// dadd accumulator

const int32_t VECTOR_LEN = 8;
const int32_t DADD_ACC_BITS = 16;
const int32_t NPOLS = 3;

/**
* vectors
*/
typedef uint8_t v16bu __attribute__ ((vector_size(16)));
typedef uint16_t v8wu __attribute__ ((vector_size(16)));
typedef uint16_t v4wu __attribute__ ((vector_size(8)));
typedef uint32_t v4du __attribute__ ((vector_size(16)));
typedef char v16qi __attribute__ ((vector_size(16)));
typedef int16_t  v8wi __attribute__ ((vector_size(16)));
typedef long long int v2di __attribute__ ((vector_size(16)));

enum DaddType {
	TDDadd,
	CEDadd
};

enum DaddSlope {
	Positive,
	Negative
};

/**
* Dadd path structure
*/
struct DaddPath {
	Polarization pol;
	int32_t bin;
	int32_t drift;
	int32_t power;

	DaddPath() { reset(); }
	DaddPath(Polarization pol_, int32_t bin_, int32_t drift_,
			int32_t power_): pol(pol_), bin(bin_), drift(drift_),
			power(power_) {}
	void reset() { pol = POL_UNINIT; bin = drift = power = 0; }

	friend ostream& operator << (ostream& s, const DaddPath& daddPath);
};

/**
 * Band data structure.
 */
struct DaddBand {
	bool bad;						// bad band or not
	int32_t bin;					// starting bin of the band
	int32_t width;					// width of the band in bins
	int32_t hits;					// total # of hits in the band
	int32_t limit;					// bad band limit
	DaddPath maxPath;				// maximum path if bad band

	DaddBand() { reset(); }
	void reset() { bad = false; bin = width = hits = 0; maxPath.reset(); }

	friend ostream& operator << (ostream& s, const DaddBand& daddBand);
};

/**
 * Input bin statistics.
 *
 * Description:\n
 * 	Records a histogram of the
 */
struct DaddBinStatistics {
	int32_t bins[4];				// histogram of input bins

	DaddBinStatistics() { reset(); }
	void reset() { bins[0] = bins[1] = bins[2] = bins[3] = 0; }

	friend ostream& operator << (ostream& s, const DaddBinStatistics& binStats);
};

struct DaddHitStatistics {
	int32_t hits;					// total # of hits
	int32_t badBands;				// total # of bad bands
	DaddPath maxPath;				// maximum power path seen

	DaddHitStatistics() { reset(); }
	void reset() { hits = badBands = 0; maxPath.reset(); }

	friend ostream& operator << (ostream& s, const DaddHitStatistics& hitStats);
};

/**
* Statistics reporting structure
*/
struct DaddStatistics {
	Polarization pol;				// polarization
	DaddBinStatistics binStats;		// bin statistics
	DaddHitStatistics hitStats;		// hit statistics

	DaddStatistics() { reset(); }

	void reset() { pol = POL_UNINIT; binStats.reset(); hitStats.reset(); }

	friend ostream& operator << (ostream& s, const DaddStatistics& stats);
};

/**
 * Timing structure.
 */
struct DaddTiming {
	struct d {
		uint64_t dadds;
		float32_t computeBinStats;
		float32_t topDown;
		float32_t threshold;
		float32_t reportHits;
		float32_t total;

		d(): dadds(0), computeBinStats(0), topDown(0), 	threshold(0),
				reportHits(0), total(0) {}
		friend ostream& operator << (ostream& s, const d& d);
	} dadd;
	struct b {
		uint64_t reports;
		float32_t total;

		b(): reports(0), total(0) {}
		friend ostream& operator << (ostream& s, const b& b);
	} badBand;
	struct p {
		uint64_t pairSums;
		float32_t total;

		p(): pairSums(0), total(0) {}
		friend ostream& operator << (ostream& s, const p& p);
	} sum;

	friend ostream& operator << (ostream& s, const DaddTiming &timing);
};

class Dadd {
public:
	Dadd();
	~Dadd();

	uint32_t getVersion() { return (DADD_VERSION); }
	uint32_t getIfVersion() { return (DADD_IFVERSION); }
	int32_t getAccumulatorSize() { return (accumulatorSize); }
	void setup(int32_t spectra_, int32_t bins_, int32_t totalBins_,
			int32_t threshold_, int32_t bandBins_, int32_t badBandLimit_,
			DaddType type_ = TDDadd, bool reportBinStats_ = false);
	void reset();
	void execute(Polarization pol, DaddSlope slope, DaddAccum *data,
			ReportHit *reportHit);
	void reportBadBands(ReportBadBand *reportBadBand);
	const DaddStatistics& getStatistics() { return (stats); }
	const DaddTiming& getTiming() { return (timing); }

	void topDown(int32_t rows, int32_t bins, DaddAccum *data);
	void pairSum(int32_t drift, int32_t bins, DaddAccum *lower,
			DaddAccum *upper);
	void singleSum(int32_t drift, int32_t bins, DaddAccum *lower,
			DaddAccum *upper);
	void thresholdData(DaddAccum *data);

private:
	bool reportBinStats;
	int32_t accumulatorSize;
	int32_t spectrumBins;
	int32_t totalBins;
	int32_t spectra;
	int32_t threshold;
	int32_t nBands;						// number of bands
	int32_t bandBins;					// number of bins in a band
	int32_t badBandLimit;				// maximum hit count for a band
	DaddType type;
	DaddBand *bands;					// bands
	DaddStatistics stats;				// stats: one pol
	DaddTiming timing;					// timing structure

	void initBands();
	void reportHits(Polarization pol, DaddSlope slope, DaddAccum *data,
			ReportHit *reportHit);
	void computeBinStatistics(Polarization pol, DaddAccum *data);
	void computeHitStatistics(Polarization pol);
	void reportBinStatistics(Polarization pol, DaddSlope slope);
	int32_t getPosition(int32_t drift, int32_t blocksize);

//	int32_t convertPolToIndex(Polarization pol);
};

}

#endif