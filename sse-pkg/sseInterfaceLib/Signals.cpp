/*******************************************************************************

 File:    Signals.cpp
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



#include "Signals.h"
#include <sstream>
#include <iostream>

using namespace std;

static string boolToOnOffString(bool value)
{
    if (value) 
        return "on";
    else
        return "off";
}



Tone::Tone()
    : 
    frequency(0), freqUnits(Hz), 
    amplitude(0), ampUnits(dBm), 
    output(false)
{}


void Tone::in_Vpp()   {ampUnits  = Vpp;}
void Tone::in_Vrms()  {ampUnits  = Vrms;}
void Tone::in_dBm()   {ampUnits  = dBm;}
void Tone::in_Hz()    {freqUnits = Hz;}
void Tone::in_kHz()   {freqUnits = kHz;}
void Tone::in_MHz()   {freqUnits = MHz;}
void Tone::in_GHz()   {freqUnits = GHz;}

double Tone::getHz() const
{
    if (freqUnits == kHz) return(1.0e3*frequency);
    if (freqUnits == MHz) return(1.0e6*frequency);
    if (freqUnits == GHz) return(1.0e9*frequency);
    return(frequency);
}

ostream& operator << (ostream& strm, const Tone& tone)
{
    strm << "    Tone: \n";

    strm << "      Frequency: " << tone.frequency;
    if (tone.freqUnits == Tone::GHz)
	strm << " GHz" << endl;
    else if (tone.freqUnits == Tone::MHz)
	strm << " MHz" << endl;
    else if (tone.freqUnits == Tone::kHz)
	strm << " kHz" << endl;
    else
	strm << " Hz" << endl;

    strm << "      Amplitude: " << tone.amplitude;
    if (tone.ampUnits == Tone::Vpp)
	strm << " Vpp" << endl;
    else if (tone.ampUnits == Tone::Vrms)
	strm << " Vrms" << endl;
    else
	strm << " dBm" << endl;
    strm << "      Output:    " << boolToOnOffString(tone.output) << endl;

    return strm;
}


// -- Drifting Tone
DriftingTone::DriftingTone() 
    : driftRate(0), duration(0), sweepState(false)
{}

ostream& operator << (ostream& strm, const DriftingTone& dtone) {
    strm << dtone.start
	 << "    Drift Rate:  "  << dtone.driftRate  << " Hz/sec" << endl
	 << "    Duration:    "    << dtone.duration    << " sec" << endl
	 << "    Sweep State: " << boolToOnOffString(dtone.sweepState)
	 << endl;
    return strm;
}



// -- PulsedTone

PulseSigParams::PulseSigParams() :
    amplitude(0), period(0), duration(0), output(false) {};

ostream& operator << (ostream& strm, const PulseSigParams& pulse)
{
    strm << "    Pulse: " << endl
	 << "      Amplitude: " << pulse.amplitude << " dBm " << endl
	 << "      Period:    " << pulse.period << " sec " << endl
	 << "      Duration:  " << pulse.duration << " sec " << endl
	 << "      Output:    " << boolToOnOffString(pulse.output)
	 << endl;
    return strm;
}


PulsedTone::PulsedTone() 
{}

ostream& operator << (ostream& strm, const PulsedTone& pdtone)
{
    strm << pdtone.driftTone
	 << pdtone.pulse;

    return strm;
}



