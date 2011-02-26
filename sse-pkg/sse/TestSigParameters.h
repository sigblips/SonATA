/*******************************************************************************

 File:    TestSigParameters.h
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

// Original code created 2001-09-11 by L.R. McFarland

#ifndef TESTSIG_PARAMETERS_H
#define TESTSIG_PARAMETERS_H

#include "machine-dependent.h" // for float64_t et. al.
#include "SeekerParameterGroup.h"
#include "sseInterface.h"
#include <mysql.h>

class TestSigParametersInternal;

class TestSigParameters : public SeekerParameterGroup
{
 public:

  enum SignalType {CW, PULSE};

  TestSigParameters(string command);
  TestSigParameters(const TestSigParameters& rhs);
  TestSigParameters& operator=(const TestSigParameters& rhs);
  ~TestSigParameters();

  bool getGenerate() const;
  bool getEnable() const;

  SignalType getSignalType() const;

  float64_t getFrequency() const;
  float64_t getFreqTol() const;
  float64_t getDriftRate() const;
  float64_t getDriftTol() const;
  float64_t getSweepTime() const;
  float64_t getWidth() const;
  float64_t getWidthTol() const;
  float64_t getSnr() const;
  float64_t getSnrTol() const;
  float64_t getCwAmplitude() const;
  float64_t getPulseAmplitude() const;
  float64_t getPulsePeriod() const;
  float64_t getPulseWidth() const;

  Polarization getPol() const;

  bool getCheckForTestSignal() const;
  bool getCheckFreq() const;
  bool getCheckDrift() const;
  bool getCheckWidth() const;
  bool getCheckSnr() const;

protected:
  virtual void addParameters();

private:
  TestSigParametersInternal *internal_;


};

#endif