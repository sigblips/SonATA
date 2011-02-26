/*******************************************************************************

 File:    Signals.h
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

// ============================================================
// Filename:    Signals.h
// Description: Signal types used by the GPIB instruments
// Authors:     L.R. McFarland
// Created:     20-04-2001
// Language:    C++
// ============================================================


#ifndef _Signals_h
#define _Signals_h

#include <unistd.h>
#include <iosfwd>
#include <string>

using std::string;
using std::ostream;

struct Tone
{
    enum FreqUnits {Hz, kHz, MHz, GHz};
    enum AmpUnits {Vpp, Vrms, dBm}; 

    double     frequency; 
    FreqUnits  freqUnits;
    double     amplitude; 
    AmpUnits   ampUnits;
    bool       output;    // is the output on

    Tone();

    void in_Vpp();
    void in_Vrms();
    void in_dBm();
    void in_Hz();
    void in_kHz();
    void in_MHz();
    void in_GHz();

    double getHz() const;

    friend ostream& operator << (ostream& strm, const Tone& tone);

};

struct DriftingTone
{
  Tone   start;
  double driftRate;
  double duration;
  bool   sweepState;

  DriftingTone();

  friend ostream& operator << (ostream& strm, const DriftingTone& dtone);

};


struct PulseSigParams
{
  double    amplitude;
  double    period;
  double    duration;
  bool      output;   // is the output on

  PulseSigParams();

  friend ostream& operator << (ostream& strm, const PulseSigParams& pulse);

};

struct PulsedTone 
{
    PulseSigParams  pulse;
    DriftingTone driftTone; // what the cw is doing
    
    PulsedTone();

    friend ostream& operator << (ostream& strm, const PulsedTone& pdtone);
};



#endif // _Signals_h
