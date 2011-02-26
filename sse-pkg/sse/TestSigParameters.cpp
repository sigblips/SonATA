/*******************************************************************************

 File:    TestSigParameters.cpp
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


#include <ace/OS.h>            // for SwigScheduler.h
#include <TestSigParameters.h>
#include <TestSigProxy.h>
#include "Site.h"
#include "Assert.h"
#include "SseMsg.h"
#include "RangeParameter.h"
#include "ChoiceParameter.h"

// TBD rename these tables to match the class
static const char *DbTableNameSuffix="Parameters";
static const char *IdColNameInActsTableSuffix="ParametersId";

static const char *ChoiceOn = "on";
static const char *ChoiceOff = "off";

static const char *ChoicePulse = "pulse";
static const char *ChoiceCw = "cw";

static const char *ChoiceLeft = "left";
static const char *ChoiceRight = "right";
static const char *ChoiceBoth = "both";

struct TestSigParametersInternal
{
    ChoiceParameter<string> generate;   // on | off
    ChoiceParameter<string> enable;   // on | off

    ChoiceParameter<string> signalType; // cw or pulse

    RangeParameter<float64_t> frequency;
    RangeParameter<float64_t> freqTol;

    RangeParameter<float64_t> driftRate;
    RangeParameter<float64_t> driftTol;

    RangeParameter<float64_t> sweepTime;

    RangeParameter<float64_t> width;
    RangeParameter<float64_t> widthTol;

    RangeParameter<float64_t> cwamp;

    RangeParameter<float64_t> pulseamp;
    RangeParameter<float64_t> pulseper;
    RangeParameter<float64_t> pulsewidth;

    ChoiceParameter<string> polarization;

    ChoiceParameter<string> checksig;
    ChoiceParameter<string> checkfreq;
    ChoiceParameter<string> checkdrift;
    ChoiceParameter<string> checkwidth;

    mutable string outString; // string buffer for tcl

    TestSigParametersInternal();

};

TestSigParametersInternal::TestSigParametersInternal()
    :
    generate("gen", "", "generate a test signal", ChoiceOn), 
    enable("enable", "", "enable control of test signal settings", ChoiceOn), 

    signalType("sigtype", "", "signal type", ChoiceCw),

    frequency("freq", "MHz", "Test signal frequency", 25.800100, 0, 80),
    freqTol("freqtol", "MHz", "Frequency tolerance", 0.000050, 0, 100),

    driftRate("drift", "Hz/sec", "Frequency drift rate", 0.1, -1, 1),
    driftTol("drifttol", "Hz/sec", "Drift rate tolerance", 0.2, 0, 1),

    sweepTime("sweeptime", "sec", "Frequency sweep time", 500, 0, 500),

    width("width", "Hz", "signal width", 1, 0.1, 10),
    widthTol("widthtol", "Hz", "signal width tolerance", 0.1, 0, 1),

    cwamp("cwamp", "dBm", "CW Test Signal Amplitude", -16.0, -36.0, 20.0),

    pulseamp("pulseamp", "dBm", "Pulse amplitude", 15.0, -36.0, 20.0 ),
    pulseper("pulseper", "sec", "Pulse period", 10, 0.0001, 100),
    pulsewidth("pulsewidth", "sec", "Pulse width", 1.0, 0.00001, 100),

    polarization("pol", "", "polarization", ChoiceBoth),

    checksig("checksig", "", "check for test signal", ChoiceOff),
    checkfreq("checkfreq", "", "compare freq tolerance", ChoiceOff),
    checkdrift("checkdrift", "", "compare drift tolerance", ChoiceOff),
    checkwidth("checkwidth", "", "compare width tolerance", ChoiceOff)
{

}

TestSigParameters::TestSigParameters(string command) : 
    SeekerParameterGroup(command, string(command + DbTableNameSuffix), 
		string(command +  IdColNameInActsTableSuffix)),
    internal_(new TestSigParametersInternal())
{
      internal_->generate.addChoice(ChoiceOn);
      internal_->generate.addChoice(ChoiceOff);

      internal_->enable.addChoice(ChoiceOn);
      internal_->enable.addChoice(ChoiceOff);

      internal_->signalType.addChoice(ChoicePulse);
      internal_->signalType.addChoice(ChoiceCw);

      internal_->polarization.addChoice(ChoiceLeft);
      internal_->polarization.addChoice(ChoiceRight);
      internal_->polarization.addChoice(ChoiceBoth);

      internal_->checksig.addChoice(ChoiceOn);
      internal_->checksig.addChoice(ChoiceOff);

      internal_->checkfreq.addChoice(ChoiceOn);
      internal_->checkfreq.addChoice(ChoiceOff);

      internal_->checkdrift.addChoice(ChoiceOn);
      internal_->checkdrift.addChoice(ChoiceOff);

      internal_->checkwidth.addChoice(ChoiceOn);
      internal_->checkwidth.addChoice(ChoiceOff);

      addParameters();
}

TestSigParameters::TestSigParameters(const TestSigParameters& rhs):
  SeekerParameterGroup(rhs.getCommand(), rhs.getDbTableName(), 
		       rhs.getIdColNameInActsTable()),
  internal_(new TestSigParametersInternal(*rhs.internal_))
{
  setSite(rhs.getSite());
  addParameters();
}


TestSigParameters& TestSigParameters::operator=(const TestSigParameters& rhs)
{
  if (this == &rhs)
  {
    return *this;
  }
  
  setCommand(rhs.getCommand());
  eraseParamList();
  setSite(rhs.getSite());
  delete internal_;
  internal_ = new TestSigParametersInternal(*rhs.internal_);
  addParameters();
  return *this;
}

TestSigParameters::~TestSigParameters()
{
    delete internal_;
}


void TestSigParameters::addParameters()
{
      addParam(internal_->generate);
      addParam(internal_->enable);

      addParam(internal_->signalType);

      addParam(internal_->frequency);
      addParam(internal_->freqTol);

      addParam(internal_->driftRate);
      addParam(internal_->driftTol);

      addParam(internal_->sweepTime);

      addParam(internal_->width);
      addParam(internal_->widthTol);

      addParam(internal_->cwamp);
      addParam(internal_->pulseamp);
      addParam(internal_->pulseper);
      addParam(internal_->pulsewidth);

      addParam(internal_->polarization);
      addParam(internal_->checksig);
      addParam(internal_->checkfreq);
      addParam(internal_->checkdrift);
      addParam(internal_->checkwidth);

      sort();
}

// convert from parameter to signal type value 
static TestSigParameters::SignalType
convertSignalType(ChoiceParameter<string> &param)
{
    AssertMsg(param.getNumberOfChoices() == 2, param.getName());
    AssertMsg(param.isValid(ChoiceCw), param.getName());
    AssertMsg(param.isValid(ChoicePulse), param.getName());

    TestSigParameters::SignalType signalType = 
	TestSigParameters::CW;
    if (param.getCurrent() == ChoicePulse)
    {
	signalType = TestSigParameters::PULSE;
    }

    return signalType;
}



bool TestSigParameters::getGenerate() const
{
  return convertOnOffToBool(internal_->generate);
}

bool TestSigParameters::getEnable() const
{
  return convertOnOffToBool(internal_->enable);
}

TestSigParameters::SignalType TestSigParameters::getSignalType() const
{
  return convertSignalType(internal_->signalType);
}

float64_t TestSigParameters::getFrequency() const
{
  return internal_->frequency.getCurrent();
}

float64_t TestSigParameters::getFreqTol() const
{
  return internal_->freqTol.getCurrent();
}

float64_t TestSigParameters::getDriftRate() const
{
  return internal_->driftRate.getCurrent();
}

float64_t TestSigParameters::getDriftTol() const
{
  return internal_->driftTol.getCurrent();
}

float64_t TestSigParameters::getSweepTime() const
{
  return internal_->sweepTime.getCurrent();
}

float64_t TestSigParameters::getWidth() const
{
  return internal_->width.getCurrent();
}

float64_t TestSigParameters::getWidthTol() const
{
  return internal_->widthTol.getCurrent();
}

float64_t TestSigParameters::getCwAmplitude() const
{
  return internal_->cwamp.getCurrent();
}

float64_t TestSigParameters::getPulseAmplitude() const
{
  return internal_->pulseamp.getCurrent();
}

float64_t TestSigParameters::getPulsePeriod() const
{
  return internal_->pulseper.getCurrent();
}

float64_t TestSigParameters::getPulseWidth() const
{
  return internal_->pulsewidth.getCurrent();
}

Polarization TestSigParameters::getPol() const
{
  return convertPolarization(internal_->polarization);
}

bool TestSigParameters::getCheckForTestSignal() const
{
  return convertOnOffToBool(internal_->checksig);
}

bool TestSigParameters::getCheckFreq() const
{
  return convertOnOffToBool(internal_->checkfreq);
}

bool TestSigParameters::getCheckDrift() const
{
  return convertOnOffToBool(internal_->checkdrift);
}

bool TestSigParameters::getCheckWidth() const
{
  return convertOnOffToBool(internal_->checkwidth);
}

