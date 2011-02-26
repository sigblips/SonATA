/*******************************************************************************

 File:    TuneDxs.h
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

/*****************************************************************
 * TuneDxs.h - declaration of functions defined in TuneDxs.h
 * tunes dxs uses preferred strategy
 * PURPOSE:  
 *****************************************************************/

#ifndef TUNEDXS_H
#define TUNEDXS_H

#include "machine-dependent.h"
#include "Range.h"
#include "DxList.h"
#include <mysql.h>

class Round;
class DxProxy;

class TuneDxs 
{
public:
  TuneDxs(int verboseLevel);
  virtual ~TuneDxs();
  virtual void tune(DxList &dxList, float64_t maxDxTuneSeparationMhz,
		    float64_t maxAllowedDxSkyFreqMhz) = 0;
  virtual void tune(DxList &dxList, int32_t totalChannels,
		float64_t mhzPerChannel) = 0;
  virtual bool moreActivitiesToRun() const = 0;
  virtual int getVerboseLevel() const;
  virtual int setVerboseLevel(int level);

  static void tuneDxsFromPrevActInDatabase(
      int oldActivityId, MYSQL *db, DxList &dxList);

protected:

  static void markDxAsNotInUse(DxProxy *dxProxy);

  int verboseLevel_;
};

// uses manual tuning
class TuneDxsUser : public TuneDxs
{
public:
  TuneDxsUser(int verboseLevel);
  virtual ~TuneDxsUser();
  virtual void tune(DxList &dxList, float64_t maxDxTuneSeparationMhz,
		   float64_t maxAllowedDxSkyFreqMhz);
  virtual void tune(DxList &dxList, int32_t totalChannels,
		float64_t mhzPerChannel);
  virtual bool moreActivitiesToRun() const;
};

class TuneDxsAuto : public TuneDxs
{
public:
  TuneDxsAuto(int verboseLevel);
  TuneDxsAuto(int verboseLevel,
	       float64_t overlap,
	       float64_t tuningTolerance,
	       Round* round);
  virtual ~TuneDxsAuto();
protected:
  virtual float64_t overlap(float64_t skyFreq) const;
  virtual float64_t tuningTolerance() const;
  float64_t overlap_;
  float64_t tuningTolerance_;
  Round* round_;
};


// tunes dxs from start to end
class TuneDxsRange : public TuneDxsAuto
{
public:
  TuneDxsRange(int verboseLevel,
		const Range& range);
  TuneDxsRange(int verboseLevel,
		const Range& range,
		float64_t overlap,
		float64_t tuningTolerance,
		Round* round);
  TuneDxsRange(int verboseLevel,
		const Range& range,
		float64_t overlap,
		float64_t tuningTolerance);
  virtual ~TuneDxsRange();
  virtual void tune(DxList &dxList, float64_t maxDxTuneSeparationMhz,
		    float64_t maxAllowedDxSkyFreqMhz);
  virtual void tune(DxList &dxList, int32_t totalChannels,
		float64_t mhzPerChannel);
  virtual bool moreActivitiesToRun() const;

protected:
  virtual float64_t calculateCenterFreq(DxProxy *dxProxy);
  const Range range_;
  float64_t nextFreq_;
};

// tunes dxs from start to end, but first center frequency will be equal
// to start, rather than having the lower bound of the first dx be equal
// to start
class TuneDxsRangeCenter : public TuneDxsRange 
{
public:
  TuneDxsRangeCenter(int verboseLevel,
		      const Range& range);
  TuneDxsRangeCenter(int verboseLevel,
		      const Range& range,
		      float64_t overlap,
		      float64_t tuningTolerance,
		      Round* round);
  TuneDxsRangeCenter(int verboseLevel,
		      const Range& range,
		      float64_t overlap, float64_t tuningTolerance);
  virtual ~TuneDxsRangeCenter();
protected:
  virtual float64_t calculateCenterFreq(DxProxy *dxProxy);
};


class TuneDxsRangeCenterRound : public TuneDxsRangeCenter
{
public:
  TuneDxsRangeCenterRound(int verboseLevel,
			   const Range& range,
			   float64_t roundValue,
			   float64_t overlap,
			   float64_t tuningTolerance);
};


// uses default dx values, repeated forever
class TuneDxsForever : public TuneDxs
{
public:
  TuneDxsForever(int verboseLevel);
  virtual ~TuneDxsForever();
  virtual void tune(DxList &dxList, float64_t maxDxTuneSeparationMhz,
		    float64_t maxAllowedDxSkyFreqMhz);
  virtual void tune(DxList &dxList, int32_t totalChannels,
		float64_t mhzPerChannel);

  virtual bool moreActivitiesToRun() const;
};

class TuneDxsObsRange : public TuneDxsAuto
{
public:
  TuneDxsObsRange(int verboseLevel,
		   const ObsRange& obsRange);
  TuneDxsObsRange(int verboseLevel,
		   const ObsRange& obsRange,
		   float64_t roundValue,
		   float64_t overlap,
		   float64_t tuningTolerance);
  virtual ~TuneDxsObsRange();

  virtual void tune(DxList &dxList, float64_t maxDxTuneSeparationMhz,
		    float64_t maxAllowedDxSkyFreqMhz);
  virtual void tune(DxList &dxList, int32_t totalChannels,
		float64_t mhzPerChannel);
  virtual bool moreActivitiesToRun() const;

protected:
  virtual float64_t calculateCenterFreq(DxProxy *dxProxy);

  const ObsRange obsRange_;
  float64_t nextLeftEdgeFreq_;

};


#endif /* TUNEDXS_H */