/*******************************************************************************

 File:    TestSig.h
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

// Test Signal Generator control

#ifndef TestSig_H
#define TestSig_H

#include "SignalGenerator.h"
#include <string>

class TestSig
{

 public:

  enum SigGenSource { SigGenAt33250, SigGenAt4400 };

  enum PulseGenSource { PulseGenAt33250, PulseGenAt4400, 
			SigGenInternal };

  TestSig();
  virtual ~TestSig();

  // ===== accessors for tcl =====

  const char* name() const;
  const char* name(const string& newName);
  const char* version() const;

  bool isSimulated();

  const char* setSigGenSource(const char* sigGenSourceName);
  const char* getSigGenSourceName();

  const char* setPulseGenSource(const char* pulseGenSourceName);
  const char* getPulseGenSourceName();

  // ===== GPIB configuration =====

  const char* setsiggensim(const char *mode);
  const char* setsiggenaddress(int bus, int addr);
  const char* getsiggeninfo();
  double setsiglimit(double limit);
  double getsiglimit();

  const char* setpulsegensim(const char *mode);
  const char* setpulsegenaddress(int bus, int addr);
  const char* getpulsegeninfo();
  double setpulselimit(double limit);
  double getpulselimit();

  // ===== commands =====

  const char* gpib(const char* mode);
  const char* help();
  const char* id();
  const char* intrinsics();
  const char* off();
  const char* on();
  const char* quiet();
  const char* requestready();
  const char* reset();
  const char* selftest();
  const char* simulate(const char* mode);
  const char* status();

  const char* tunesiggen(double frequencyMHz, double amplitudedBm,
			 double driftRateHzSec, double durationSecs);

  const char* pulse(double amplitudedBm, double periodSec, double widthSec);

  const char *gpibcmd(const char *cmd, const char *arg1="",
	const char*arg2="", const char *arg3="", const char *arg4="");

 private:
  const char* simulate(GPIBDevice *device, const char *mode);
  bool setSimulated(bool mode);
  SignalGenerator* getSigGen();
  SignalGenerator* getPulseGen();

  // ---- tcl interface ----

  string tclBuffer_; // holds return string for tcl interpreter
  const char*  sendToSocket(const string& message); // send to tcl on socket

  // ===== TestSig members =====

  string name_;
  string version_;
  bool simulated_;
  bool usingPulseGen_;
  PulseGenSource pulseGenSource_;
  SigGenSource sigGenSource_;

  SignalGenerator* siggen_;
  SignalGenerator* pulsegen_;

};

#endif // TestSig_H
