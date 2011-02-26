/*******************************************************************************

 File:    SignalGenerator.cpp
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

// Based on original code by L.R. McFarland

#include "SignalGenerator.h"
#include "Assert.h"
#include <vector>
#include <math.h>
#include <unistd.h>

using namespace std;

static const int PrintPrecision(9);

const bool UseCommandedValuesForStatus = true;

SignalGenerator::SignalGenerator(int bufferSize) : 
   GPIBDevice(bufferSize), 
   safetyAmp_(0), 
   freqHz_(0), 
   amp_(0), 
   on_(false), 
   startFrequencyHz_(0),
   stopFrequencyHz_(0),
   sweepTime_(0),
   sweepState_(false),
   pulsePeriod_(0),
   pulseWidth_(0)
{}


SignalGenerator::~SignalGenerator()
{}

void SignalGenerator::safetyAmp(float64_t limit)
{
   safetyAmp_ = limit;
}

float64_t SignalGenerator::safetyAmp()
{
   return(safetyAmp_);
}


void SignalGenerator::RF(bool state)
{
   on_ = state;

   if (simulated())
   {
      return;
   }
  
   if (state)
   {
      send("OUTP ON");
   }
   else
   {
      send("OUTP OFF");
   }
}

bool SignalGenerator::RF()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(on_);
   }

   string rf;
   send("OUTP?");
   recv(rf);
   return(atoi(rf.c_str()));
}

void SignalGenerator::reset()
{
   freqHz_ = 0;
   amp_ = 0;
   on_ = false;

   GPIBDevice::reset();
}


void SignalGenerator::sweep(float64_t amplitude_Vpp, float64_t start_Hz, 
			    float64_t driftRate, float64_t duration)
{
   cerr << "sweep() not implemented for " << getModelName() << endl;

}


float64_t SignalGenerator::startFrequency()
{
   cerr << "startFrequency() not implemented for " << getModelName() << endl;

   return -1;
}

float64_t SignalGenerator::stopFrequency()
{
   cerr << "stopFrequency() not implemented for " << getModelName() << endl;

   return -1;
}

float64_t SignalGenerator::sweepTime()
{
   cerr << "sweepTime() not implemented for " << getModelName() << endl;

   return -1;
}

bool SignalGenerator::sweepState()
{
   cerr << "sweepState() not implemented for " << getModelName() << endl;
    
   return false;
}

void SignalGenerator::pulse(float64_t amplitudeVpp, float64_t periodSec,
			    float64_t widthSec)
{
   cerr << "pulse() not implemented for " << getModelName() << endl;
}


float64_t SignalGenerator::period()
{
   cerr << "period() not implemented for " << getModelName() << endl;

   return -1;
}

float64_t SignalGenerator::width()
{
   cerr << "width() not implemented for " << getModelName() << endl;

   return -1;
}

void SignalGenerator::enableExternalPulseModulation()
{
   cerr << "enableExternalPulseModulation() not implemented for "
	<< getModelName() << endl;
}

void SignalGenerator::disablePulseModulation()
{
   cerr << "disablePulseModulation() not implemented for " 
	<< getModelName() << endl;
}


// -------------------
// ----- AT33250 -----
// -------------------

AT33250::AT33250(int bufferSize) : 
   SignalGenerator(bufferSize),
   periodSec_(0), 
   widthSec_(0)
{}

AT33250::~AT33250()
{}

float64_t AT33250::maxFrequency() {return(max_frequency);}
float64_t AT33250::minFrequency() {return(min_frequency);}

float64_t AT33250::maxAmplitude() {return(max_amplitude);}
float64_t AT33250::minAmplitude() {return(min_amplitude);}

float64_t AT33250::maxSweepTime() {return(max_sweep_time);}
float64_t AT33250::minSweepTime() {return(min_sweep_time);}

float64_t AT33250::period()       {return(periodSec_);}
float64_t AT33250::width()        {return(widthSec_);}


string AT33250::getModelName() const
{
   return "AT33250";  
}


void AT33250::amplitude(float64_t amp_dBm)
{
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_dBm > safetyAmp())
   {
      strm << function() << " " << getModelName() 
	   << " safety amplitude (" << safetyAmp()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }

   if (amp_dBm > maxAmplitude())
   {
      strm << function() << " " << getModelName() 
	   << " maximum amplitude (" << maxAmplitude()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }

   if (amp_dBm < minAmplitude()) {
      strm << function() << " " << getModelName() 
	   << " minimum amplitude (" << minAmplitude()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }

   send("VOLTAGE:UNIT DBM");
   strm << "VOLTAGE " << amp_dBm << " DBM";
   send(strm.str());

   amp_ = amp_dBm;

}

float64_t AT33250::amplitude()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(amp_);
   }

   string amp_dBm;
   send("VOLTAGE?");
   recv(amp_dBm);
   
   return(atof(amp_dBm.c_str()));
}

void AT33250::frequency(float64_t freqHz)
{
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency()) {
      strm << function() << " " << getModelName() << " maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   if (freqHz < minFrequency()) {
      strm << function() << " " << getModelName() << " minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   strm << "FREQ " << freqHz << " HZ";
   send(strm.str());

   freqHz_ = freqHz;
}

float64_t AT33250::frequency() 
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(freqHz_);
   }

   string freqHz;
   send("FREQ?"); // returns Hz
   recv(freqHz);

   return(atof(freqHz.c_str()));
}

void AT33250::sweep(float64_t amplitudedBm, float64_t startHz,
		    float64_t driftRateHzSec, float64_t durationSec)
{
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   amplitude(amplitudedBm);

   if (fabs(driftRateHzSec) < 0.0001)
   {
      // nondrifting CW tone
      send("SWEEP:STATE OFF");
      sweepState_ = false;
      frequency(startHz);
   } 
   else
   {
      float64_t stopHz = startHz + driftRateHzSec * durationSec;

      if (startHz > maxFrequency())
      {
	 strm << function() << " " << getModelName() 
	      << " attempt to sweep past maximum frequency ("
	      << maxFrequency() << " Hz) = " << startHz;
	 throw GPIBError(strm.str());
      }

      if (startHz < minFrequency()) {
	 strm << function() << " " << getModelName() 
	      << " attempt to sweep past minimum frequency ("
	      << minFrequency() << " Hz) = " << startHz;
	 throw GPIBError(strm.str());
      }

      if (stopHz > maxFrequency()) {
	 strm << function() << " " << getModelName() 
	      << " attempt to sweep past maximum frequency ("
	      << maxFrequency() << " Hz) = " << stopHz;
	 throw GPIBError(strm.str());
      }

      if (stopHz < minFrequency()) {
	 strm << function() << " " << getModelName() 
	      << " attempt to sweep past minimum frequency ("
	      << minFrequency() << " Hz) = " << stopHz;
	 throw GPIBError(strm.str());
      }

      if (durationSec < minSweepTime()) {
	 strm << function() << " " << getModelName()
	      << " attempt to set sweep time less than minimum ("
	      << minSweepTime() << " sec) = " << durationSec;
	 throw GPIBError(strm.str());
      }

      if (durationSec > maxSweepTime()) {
	 strm << function() << " " << getModelName() 
	      << " attempt to set sweep time greater than maximum ("
	      << maxSweepTime() << " sec) = " << durationSec;
	 throw GPIBError(strm.str());
      }

      strm << "FREQ:START " << startHz;
      send(strm.str());
      strm.str("");
      startFrequencyHz_ = startHz;

      strm << "FREQ:STOP "  << stopHz; 
      send(strm.str());
      strm.str("");
      stopFrequencyHz_ = stopHz;

      strm << "SWEEP:TIME " << durationSec;
      send(strm.str());
      strm.str("");
      sweepTime_ = durationSec;

      send("SWEEP:STATE ON");
      sweepState_ = true;

   }

}

void AT33250::pulse(float64_t ampdBm, float64_t periodSec, float64_t widthSec)
{
   stringstream widthstrm;
   stringstream periodstrm;
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (periodSec <= 0) 
   {
      strm << function() << " " 
	   << getModelName() 
	   << " given a pulse period less than or equal to zero: " 
	   << periodSec;
      throw GPIBError(strm.str());
   }

   if (widthSec > periodSec)
   {
      strm << function() << " " << getModelName() 
	   << " given a pulse width greater than its "
	   << "period. ";
      throw GPIBError(strm.str());
   }

   // set the amplitude, to update the status & get the
   // safety checks, even though the amplitude will be set again
   // in the command below

   amplitude(ampdBm);

   // for status
   periodSec_ = periodSec; 
   widthSec_  = widthSec;  

   //strm << "APPLY:PULSE " << 1/periodSec << ", "
//	<< ampdBm << ", "
//	<< ampdBm/2.0;   // apply positive offset to drive test LO AT4400
//   strm << "APPLY:PULSE " << 1/periodSec << ", "
//	<< ampdBm << ", "
//	<< "-2.5 V";     // Offset required for the gate
 //  send(strm.str()); strm.str("");

   RF(false);
   send("VOLTAGE:OFFSET -2.5 V");
   
   widthstrm << "PULSE:WIDTH " << widthSec;
   send(widthstrm.str()); 

   periodstrm << "PULSE:PERIOD " << periodSec;
   send(periodstrm.str()); 
   send("FUNC PULSE");
   RF(true);
}

float64_t AT33250::startFrequency()
{
   // ASSUMES: frequency in Hz

   if (simulated() || UseCommandedValuesForStatus) 
   {
      return(startFrequencyHz_);
   }

   string buffer;
   send("FREQ:START?");
   recv(buffer);

   return(atof(buffer.c_str()));
}

float64_t AT33250::stopFrequency()
{
   // ASSUMES: frequency in Hz

   if (simulated() || UseCommandedValuesForStatus)
   {
      return(stopFrequencyHz_);
   }

   string buffer;
   send("FREQ:STOP?");
   recv(buffer);

   return(atof(buffer.c_str()));
}

bool AT33250::sweepState()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(sweepState_);
   }

   string buffer;
   send("SWEEP:STATE?");
   recv(buffer);
    
   if (buffer == "1")
   {
      return(true);
   }
   else
   {
      return(false);
   }
   // TBD check for bad buffer value

}

float64_t AT33250::sweepTime() 
{
   // ASSUMES: time in sec
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(sweepTime_);
   }

   string buffer;
   send("SWEEP:TIME?");
   recv(buffer);

   return(atof(buffer.c_str()));
}




// ------------------
// ----- AT4400 -----
// ------------------

AT4400::AT4400(int bufferSize) :
   SignalGenerator(bufferSize),
   sweepTime_(0)
{}

AT4400::~AT4400() 
{}

float64_t AT4400::maxAmplitude() {return max_amplitude;}
float64_t AT4400::minAmplitude() {return min_amplitude;}

float64_t AT4400::maxFrequency() {return max_frequency;}
float64_t AT4400::minFrequency() {return min_frequency;}

float64_t AT4400::maxSweepDwell() {return(max_sweep_dwell);}
float64_t AT4400::minSweepDwell() {return(min_sweep_dwell);}

float64_t AT4400::maxSweepPoints() {return(max_sweep_points);}
float64_t AT4400::minSweepPoints() {return(min_sweep_points);}

float64_t AT4400::maxPulsePeriodSec() {return maxPulsePeriodSec_;}

string AT4400::getModelName() const
{
   return "AT4400";  
}

void AT4400::amplitude(float64_t amp_dBm)
{
   // ASSUMES: amplitude in dBm
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_dBm > safetyAmp())
   {
      strm << function() << " " << getModelName() 
	   << " safety amplitude (" << safetyAmp()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }

   if (amp_dBm > maxAmplitude()) 
   {
      strm << function() << " " << getModelName() 
	   << " maximum amplitude (" << maxAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }

   if (amp_dBm < minAmplitude())
   {
      strm << function() << " " << getModelName() 
	   << " minimum amplitude (" << minAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }

   strm << ":POWER:AMPL " << amp_dBm;
   send(strm.str());

   amp_ = amp_dBm;

}

float64_t AT4400::amplitude()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(amp_);
   }

   string amp_dBm;
   send(":POWER:AMPL?"); // returns dBm
   recv(amp_dBm);

   return(atof(amp_dBm.c_str()));
}

void AT4400::frequency(float64_t freqHz)
{
   // ASSUMES: frequency in Hz
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency())
   {
      strm << function() << " " << getModelName()
	   << " maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }

   if (freqHz < minFrequency())
   {
      strm << function() << " " << getModelName() 
	   << " minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }

   strm << "FREQ " << freqHz;
   send(strm.str());

   freqHz_ = freqHz;
}

float64_t AT4400::frequency()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(freqHz_);
   }

   string freqHz;
   send("FREQ?");  // returns Hz
   recv(freqHz);

   return(atof(freqHz.c_str()));
}

bool AT4400::sweepState()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(sweepState_);
   }

   // TBD access device.  For now always assume it's on
   // so that the start/stop freqs are assumed to be valid

   return true;
}

float64_t AT4400::sweepTime()
{
   return sweepTime_;
}



float64_t AT4400::startFrequency()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(startFrequencyHz_);
   }

   string freqHz;
   send("FREQ:START?");  // returns Hz
   recv(freqHz);
   return(atof(freqHz.c_str()));
}

float64_t AT4400::stopFrequency()
{
   if (simulated() || UseCommandedValuesForStatus)
   {
      return(stopFrequencyHz_);
   }

   string freqHz;
   send("FREQ:STOP?");  // returns Hz
   recv(freqHz);
   return(atof(freqHz.c_str()));
}

void AT4400::enableExternalPulseModulation()
{
   // select External Source 2 as the pulse source
   send(":PULM:SOURCE EXT2");

    // select pulse modulation
   send(":PULM:STATE ON");

   // enable RF output modulation
   send(":OUTPUT:MOD ON");

}

void AT4400::disablePulseModulation()
{
   // disable RF output modulation
   send(":OUTPUT:MOD OFF");

    // deselect pulse modulation
   send(":PULM:STATE OFF");

}

void AT4400::sweep(float64_t amplitudedBm, float64_t startHz,
		    float64_t driftRateHzSec, float64_t durationSec)
{
   stringstream strm;
   strm.precision(PrintPrecision);  // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   float64_t sweepDwell = durationSec/maxSweepPoints();
   float64_t sweepPoints = maxSweepPoints();

   send("*RST");
   send("*CLS");
   send(":FREQ:MODE LIST");
   send(":LIST:TYPE STEP"); 


   float64_t stopHz = startHz + driftRateHzSec * durationSec;

   if (startHz > maxFrequency())
   {
      strm << function() << " " << getModelName() 
	   << " attempt to sweep past maximum frequency ("
	   << maxFrequency() << " Hz) = " << startHz;
      throw GPIBError(strm.str());
   }

   if (startHz < minFrequency()) 
   {
      strm << function() << " " << getModelName() 
	   << " attempt to sweep past minimum frequency ("
	   << minFrequency() << " Hz) = " << startHz;
      throw GPIBError(strm.str());
   }

   if (stopHz > maxFrequency())
   {
      strm << function() << " " 
	   << getModelName() 
	   << " attempt to sweep past maximum frequency ("
	   << maxFrequency() << " Hz) = " << stopHz;
      throw GPIBError(strm.str());
   }

   if (stopHz < minFrequency())
   {
      strm << function() << " " << getModelName() 
	   << " attempt to sweep past minimum frequency ("
	   << minFrequency() << " Hz) = " << stopHz;
      throw GPIBError(strm.str());
   }

   if (sweepDwell < minSweepDwell()) 
   {
      strm << function() << " " << getModelName() 
	   << " attempt to set dwell time less than minimum ("
	   << minSweepDwell() << " sec) = " << sweepDwell;
      throw GPIBError(strm.str());
   }

   if (sweepDwell > maxSweepDwell())
   {
      strm << function() << " " << getModelName() 
	   << " attempt to set dwell time greater than maximum ("
	   << maxSweepDwell() << " sec) = " << sweepDwell;
      throw GPIBError(strm.str());
   }

   strm << ":FREQ:START " << startHz << " Hz"; 
   send(strm.str());
   strm.str("");
   startFrequencyHz_ = startHz;
      
   strm << ":FREQ:STOP " << stopHz << " Hz"; 
   send(strm.str());
   strm.str("");
   stopFrequencyHz_ = stopHz;

   strm << ":SWEEP:POINTS " << sweepPoints;
   send(strm.str());
   strm.str("");

   strm << ":SWEEP:DWELL " << sweepDwell << " S";
   send(strm.str());
   strm.str("");

   sweepTime_ = durationSec;

   send(":INIT");
   send(":INIT:CONT ON");
   sweepState_ = true;

   amplitude(amplitudedBm);

   RF(true);

}

void AT4400::pulse(float64_t amplitudedBm, float64_t periodSec,
		   float64_t widthSec)
{
   stringstream strm;
   strm.precision(PrintPrecision); // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (periodSec <= 0.0) 
   {
      strm << function() << " " 
	   << getModelName() << " pulse period of "
	   << periodSec << " sec is <= zero";
      throw GPIBError(strm.str());
   }

   if (widthSec > periodSec)
   {
      strm << function() << " " << getModelName() 
	   << " pulse width of " << widthSec 
	   << " sec is greater than pulse period of " 
	   << periodSec << " sec";
      throw GPIBError(strm.str());
   }

   if (periodSec > maxPulsePeriodSec())
   {
      strm << function() << " " << getModelName() 
	   << " pulse period of " << periodSec
	   << " sec exceeds max period of " 
	   << maxPulsePeriodSec() << " sec";

      throw GPIBError(strm.str());
   }

   // select internal pulse source
   send(":PULM:SOURCE INT");

   // set pulse period
   stringstream periodCmd;
   periodCmd << ":PULM:INT:PER " << periodSec << " sec";
   send(periodCmd.str());
   pulsePeriod_ = periodSec;

   // set pulse width
   stringstream widthCmd;
   widthCmd << ":PULM:INT:PWID " << widthSec << " sec";
   send(widthCmd.str());
   pulseWidth_ = widthSec;

   // select pulse modulation
   send(":PULM:STATE ON");

   // enable RF output modulation
   send(":OUTPUT:MOD ON");
}


// -------------------
// ----- HP3325B -----
// -------------------

string HP3325B::getModelName() const
{
   return "HP3325B";  
}

void HP3325B::amplitude(float64_t amp_Vpp) {
   // ASSUMES: amplitude in Vpp into 50 Ohms
   float64_t amp_rms   = amp_Vpp/(2.0*sqrt(2.0));
   float64_t power     = amp_rms*amp_rms/50.0;
   float64_t power_dBm = 10.0*log10(power) + 30.0;
   amplitude_dBm(power_dBm);
}

float64_t HP3325B::amplitude() {
   // ASSUMES: amplitude in Vpp into 50 Ohms
   float64_t power_dBm = amplitude_dBm();
   float64_t power     = pow(10.0, (power_dBm - 30.0)/10.0);
   float64_t amp_Vpp    = sqrt(50.0*power) * 2.0 * sqrt(2.0);
   return(amp_Vpp);
}

void HP3325B::amplitude_dBm(float64_t amp_dBm) {

   // ASSUMES: amplitude in dBm

   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_dBm > safetyAmp()) {
      strm << function() << " " << getModelName() << " safety amplitude (" << safetyAmp()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }
   if (amp_dBm > maxAmplitude()) {
      strm << function() << " " << getModelName() << " maximum amplitude (" << maxAmplitude()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }
   if (amp_dBm < minAmplitude()) {
      strm << function() << " " << getModelName() << " minimum amplitude (" << minAmplitude()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }

   strm << "AM " << amp_dBm << " DB ";
   send(strm.str());
   if (simulated()) amp_ = amp_dBm;

}

float64_t HP3325B::amplitude_dBm() {
   send("AM?");   // returns "AM0000000.020DB", chop bits
   string ampBuffer;
   recv(ampBuffer);
   if (simulated()) return(amp_);
   return(parseDouble(ampBuffer));
}

void HP3325B::frequency(float64_t freqHz) {
   // ASSUMES: frequency in Hz
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency()) {
      strm << function() << " " << getModelName() << " maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   if (freqHz < minFrequency()) {
      strm << function() << " " << getModelName() << " minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   strm << "FR " << freqHz << " HZ"; // TBD test this
   send(strm.str());
   if (simulated()) freqHz_ = freqHz;
}

float64_t HP3325B::frequency() {
   send("FR?"); // returns Hz, e.g. "FR00100000.00HZ"
   string freqBuffer;
   recv(freqBuffer);
   if (simulated()) return(freqHz_);
   return(parseDouble(freqBuffer));
}

void HP3325B::sweep(float64_t amplitudeVpp, float64_t start_Hz, 
		    float64_t driftRate, float64_t duration) {

   // ASSUMES: amplitude in Vpp, frequency in Hz, drift rate in Hz/sec,
   // ASSUMES: duration in sec

   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   amplitude(amplitudeVpp);

   send("FU 1"); // sine wave form
   send("MA 0"); // modulation off

   if (fabs(driftRate) < 0.0001) { // TBD drift rate threshold?

      // CW tone
      frequency(start_Hz);
      sweepState_ = false;

   } else {

      float64_t stop_Hz = start_Hz + driftRate * duration;

      if (start_Hz > maxFrequency()) {
	 strm << function() << " " 
	      << getModelName() << " attempt to sweep past maximum frequency ("
	      << maxFrequency() << " Hz) = " << start_Hz;
	 throw GPIBError(strm.str());
      }

      if (start_Hz < minFrequency()) {
	 strm << function() << " " 
	      << getModelName() << " attempt to sweep past minimum frequency ("
	      << minFrequency() << " Hz) = " << start_Hz;
	 throw GPIBError(strm.str());
      }

      if (stop_Hz > maxFrequency()) {
	 strm << function() << " " 
	      << getModelName() << " attempt to sweep past maximum frequency ("
	      << maxFrequency() << " Hz) = " << stop_Hz;
	 throw GPIBError(strm.str());
      }

      if (stop_Hz < minFrequency()) {
	 strm << function() << " " 
	      << getModelName() << " attempt to sweep past minimum frequency ("
	      << minFrequency() << " Hz) = " << stop_Hz;
	 throw GPIBError(strm.str());
      }

      if (duration < minSweepTime()) {
	 strm << function() << " " 
	      << getModelName() << " attempt to set sweep time less than minimum ("
	      << minSweepTime() << " sec) = " << duration;
	 throw GPIBError(strm.str());
      }

      if (duration > maxSweepTime()) {
	 strm << function() << " " 
	      << getModelName() << " attempt to set sweep time greater than maximum ("
	      << maxSweepTime() << " sec) = " << duration;
	 throw GPIBError(strm.str());
      }

      strm << "ST " << start_Hz  << " HZ";  send(strm.str()); strm.str("");
      startFrequencyHz_ = start_Hz;

      strm << "SP " << stop_Hz   << " HZ";  send(strm.str()); strm.str("");
      stopFrequencyHz_ = stop_Hz;

      strm << "TI " << duration << " SE "; send(strm.str()); strm.str("");
      sweepTime_ = duration;

      sweepState_ = true;

   }

}

double HP3325B::parseDouble(string source) {

   // This is a problem: I think there are some non-printing characters
   // in the raw string that mess up normal parsing
   // the only way I could get this to work
   // tried this too, but to no success
   // double foo(0);
   // foo = SseUtil::strToDouble(freq); core dumps?
   // istringstream iss(amp);
   // iss >> foo;

   string token(" ");

   source.insert(2, token);
   source.insert(source.length() - 3, token);

   vector<string> words;

   string::size_type start = 0;
   string::size_type stop = 0;

   while ((start = source.find_first_not_of(token, start)) != string::npos) {

      stop = source.find_first_of(token, start);

      if (stop == string::npos)
	 stop = source.size();

      words.push_back(source.substr(start, stop - start));

      start = stop;

   }

   return atof(words[1].c_str());
}

float64_t HP3325B::startFrequency() {
   string buffer;
   send("ST?");
   recv(buffer);
   if (simulated()) return(startFrequencyHz_);
   return(parseDouble(buffer.c_str()));
}

float64_t HP3325B::stopFrequency() {
   string buffer;
   send("SP?");
   recv(buffer);
   if (simulated()) return(stopFrequencyHz_);
   return(parseDouble(buffer.c_str()));
}

bool HP3325B::sweepState() {
   // TBD query command for this?
   return(sweepState_);
}

float64_t HP3325B::sweepTime() {
   string    buffer;
   float64_t duration;

   send("TI?");
   recv(buffer);

   if (simulated()) {
      return(sweepTime_);
   } else {
      duration = parseDouble(buffer.c_str());
      if (fabs(duration) < 0.0000001) {
	 stringstream err;
	 err << function() << " " << getModelName() << " found tone duration == 0.";
	 throw GPIBError(err.str());
      }
      return(duration);
   }
}

void HP3325B::RF(bool state) {
   if (state) {
      if (simulated()) on_ = true;
      // TBD send("OUTP ON");
   } else {
      if (simulated()) on_ = false;
      // TBD send("OUTP OFF");
   }
}

bool  HP3325B::RF() {
   string rf;
   // TBD send("OUTP?");
   // TBD recv(rf);
   if (simulated()) return(on_);
   return(atoi(rf.c_str()));
}

void HP3325B::selftest(string& result) {
   send("TST"); // TBD test this
   if (simulated()) result = "0";
   else {
      sleep(10); // TBD self test sleep time
      recv(result);
   }
};

// -----------------
// ----- DS340 -----
// -----------------

string DS340::getModelName() const
{
   return "DS340";  
}


void DS340::amplitude(float64_t amp_Vpp) {
   // ASSUMES: amplitude in Vpp
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_Vpp > safetyAmp()) {
      strm << function() << " " << getModelName() << " safety amplitude (" << safetyAmp()
	   << " Vpp) exceeded: " << amp_Vpp;
      throw GPIBError(strm.str());
   }
   if (amp_Vpp > maxAmplitude()) {
      strm << function() << " " << getModelName() << " maximum amplitude (" << maxAmplitude()
	   << " Vpp) exceeded: " << amp_Vpp;
      throw GPIBError(strm.str());
   }
   if (amp_Vpp < minAmplitude()) {
      strm << function() << " " << getModelName() << " minimum amplitude (" << minAmplitude()
	   << " Vpp) exceeded: " << amp_Vpp;
      throw GPIBError(strm.str());
   }
   strm << "AMPL " << amp_Vpp << " VP"; // TBD 2.0 factor for Vpp?
   send(strm.str());
   if (simulated()) amp_ = amp_Vpp;
}

float64_t DS340::amplitude() {
   string amp_Vpp;
   send("AMPL?");
   recv(amp_Vpp);
   if (simulated()) return(amp_);
   return(atof(amp_Vpp.c_str()));
}

void DS340::frequency(float64_t freqHz) {
   // ASSUMES: frequency in Hz
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency()) {
      strm << function() << " " << getModelName() << " maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   if (freqHz < minFrequency()) {
      strm << function() << " " << getModelName() << " minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   strm << "FREQ " << freqHz;
   send(strm.str());
   if (simulated()) freqHz_ = freqHz;
}

float64_t DS340::frequency() {
   string freqHz;
   send("FREQ?"); // returns Hz
   recv(freqHz);
   if (simulated()) return(freqHz_);
   return(atof(freqHz.c_str()));
}

void DS340::waveform(Waveform_t wavefm) {
   stringstream strm;
   strm << "FUNC " << wavefm;
   send(strm.str());
   if (simulated()) wavefm_ = wavefm;
}

DS340::Waveform_t DS340::waveform() {
   string wavefm;
   send("FUNC?");
   recv(wavefm);
   if (simulated()) return(wavefm_);

   switch (atoi(wavefm.c_str())) {
   case 0:
      return SINE;
   case 1:
      return SQUARE;
   case 2:
      return TRIANGLE;
   case 3:
      return RAMP;
   case 4:
      return NOISE;
   case 5:
      return ARBITRARY;
   }

   return SINE; // to keep compiler quiet. should never get here
}

#ifdef DS340_PULSE

void DS340::pulse(float64_t amp, float64_t period, float64_t duration, 
		  int samples) {

   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (samples < min_waveform_length) {
      strm << function() << " " 
	   << getModelName() << " given a pulse samples ("
	   << samples << ") per second less than limit: " 
	   << min_waveform_length;
      throw GPIBError(strm.str());
   }

   if (samples > max_waveform_length) {
      strm << function() << " " 
	   << getModelName() << " given a pulse samples (" << samples 
	   << ") per second greater than limit: " 
	   << max_waveform_length;
      throw GPIBError(strm.str());
   }

   if (period <= 0) {
      strm << function() << " " 
	   << getModelName() << " given a pulse period less than or equal to zero: " 
	   << period;
      throw GPIBError(strm.str());
   }

   period_   = period;   // for status
   duration_ = duration; // for status
   samples_  = samples;  // for status

   float64_t duty_cycle = duration/period;

   if (duty_cycle > 1) {
      strm << function() << " " << getModelName() << " given a duty cycle greater than 100%";
      throw GPIBError(strm.str());
   }

   // TBD send("TSRC 2"); // set trigger to 1 pps input

   waveform(ARBITRARY);

   amplitude(fabs(amp));

   strm << "LDWA " << samples;
   send(strm.str()); strm.str("");

   for (int i = 0; i < samples; i++) {
      if (i < samples * duty_cycle) {
	 if (amp > 0) {
	    strm << ARB_high_limit; send(strm.str()); strm.str("");
	 } else {
	    strm << ARB_low_limit; send(strm.str()); strm.str("");
	 }
      } else {
	 strm << 0; send(strm.str()); strm.str("");
      }
   }

   float64_t guess_sampling_frequency = samples/period;

   float64_t sampling_frequency(40e6);

   // gcc3.3 error: call of overloaded `pow(int, int)' is ambiguous
   // tbd use defined consts for the magic numbers here
   for (int i = 1; i < pow(static_cast<double>(2), 34) - 1; i++) {
      sampling_frequency = 40e6/i;
      if (sampling_frequency < guess_sampling_frequency)
	 break;
   }

   strm << "FSMP " << sampling_frequency;
   send(strm.str());

}

#endif

void DS340::sweep(float64_t amplitudeVpp, float64_t startHz, 
		  float64_t driftRate, float64_t duration) 
{
   cerr << "DS340::sweep() not yet implemented for this device." << endl;
};

float64_t DS340::startFrequency()
{
   cerr << "DS340::startFrequency() not yet implemented for this device." 
	<< endl;
   return(-1);
};

float64_t DS340::stopFrequency()
{
   cerr << "DS340::stopFrequency() not yet implemented for this device." 
	<< endl;
   return(-1);
};

bool DS340::sweepState()
{
   cerr << "DS340::sweepState() not yet implemented for this device." << endl;
   return(-1);
};

float64_t DS340::sweepTime()
{
   cerr << "DS340::sweepTime() not yet implemented for this device." << endl;
   return(-1);
};



// -------------------
// ----- HP83711 -----
// -------------------

string HP83711::getModelName() const
{
   return "HP83711";  
}


void HP83711::amplitude(float64_t amp_dBm) {
   // ASSUMES: amplitude in dBm
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_dBm > safetyAmp()) {
      strm << function() << " " << getModelName() << " safety amplitude (" << safetyAmp()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }
   if (amp_dBm > maxAmplitude()) {
      strm << function() << " " << getModelName() << " maximum amplitude (" << maxAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }
   if (amp_dBm < minAmplitude()) {
      strm << function() << " " << getModelName() << " minimum amplitude (" << minAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }
   strm << ":POWER:AMPL " << amp_dBm;
   send(strm.str());
   if (simulated()) amp_ = amp_dBm;
}

float64_t HP83711::amplitude() {
   string amp_dBm;
   send(":POWER:AMPL?"); // returns dBm
   recv(amp_dBm);
   if (simulated()) return(amp_);
   return(atof(amp_dBm.c_str()));
}

void HP83711::frequency(float64_t freqHz) {
   // ASSUMES: frequency in Hz
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency()) {
      strm << function() << " " << getModelName() << " maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   if (freqHz < minFrequency()) {
      strm << function() << " " << getModelName() << " minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   strm << "FREQ " << freqHz;
   send(strm.str());
   if (simulated()) freqHz_ = freqHz;
}

float64_t HP83711::frequency() {
   string freqHz;
   send("FREQ?");  // returns Hz
   recv(freqHz);
   if (simulated()) return(freqHz_);
   return(atof(freqHz.c_str()));
}

// ------------------
// ----- HP8662 -----
// ------------------


string HP8662::getModelName() const
{
   return "HP8662";  
}


void HP8662::amplitude(float64_t amp_dBm) {
   // ASSUMES: amplitude in dBm
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_dBm > safetyAmp()) {
      strm << function() << " " << " (HP8662) safety amplitude (" << safetyAmp()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }
   if (amp_dBm > maxAmplitude()) {
      strm << function() << " " << " (HP8662) maximum amplitude (" << maxAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }
   if (amp_dBm < minAmplitude()) {
      strm << function() << " " << " (HP8662) minimum amplitude (" << minAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }
   strm << "AP " << amp_dBm << " DM";
   send(strm.str());

   // Since this seems to be a read only instrument, save amplitude
   // TBD verify this is correct
   amp_ = amp_resolution * floor( (amp_dBm/amp_resolution) + 0.5 );

}

float64_t HP8662::amplitude() {
   return(amp_); // TBD notify user simulated() amplitude
}

void  HP8662::identify() {
   stringstream strm;
   strm << "HP8662::identify(): not supported.";
   cerr << strm.str() << endl; // TBD
   id("HP8662");
   // TBD throw GPIBError(strm.str());
};

void HP8662::frequency(float64_t freqHz) {
   // ASSUMES: frequency in Hz
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency()) {
      strm << function() << " " << " (HP8662) maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   if (freqHz < minFrequency()) {
      strm << function() << " " << " (HP8662) minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }

   strm << "FR " << freqHz/1.0e6 << " MZ";  // ASSUMES: freq in Hz
  
   send(strm.str());

   // Since this seems to be a read only instrument, save frequency
   // TBD verify this is correct
   if (freqHz < freq_res_break)
      freqHz_ = freq_resolutionA * floor( (freqHz/freq_resolutionA) + 0.5 );
   else
      freqHz_ = freq_resolutionB * floor( (freqHz/freq_resolutionB) + 0.5 );
}

float64_t HP8662::frequency() {
   return(freqHz_);
}

void  HP8662::reset() {
   send("SP 00");
   send("W1"); // sweep off
   send("MO"); // modulation off
   send("AO"); // amplitude off  
};

void  HP8662::RF(bool state) {
   if (state) {
      if (simulated()) on_ = true;
      send("AMPLITUDE:STATE ON");
   } else {
      if (simulated()) on_ = false;
      send("AO");
   }
}

bool  HP8662::RF() {
   string rf;
   send("AMPLITUDE:STATE?"); // TBD test this
   recv(rf);
   if (simulated()) return(on_);
   return(atoi(rf.c_str()));
}

void  HP8662::selftest(string& result) {
   stringstream strm;
   strm << "HP8662::selftest(): not supported.";
   cerr << strm.str() << endl;
   result = "no self test with this device.";
   // TBD throw GPIBError(strm.str());
};

// -------------------
// ----- HP8644A -----
// -------------------

string HP8644A::getModelName() const
{
   return "HP8644A";  
}



void HP8644A::amplitude(float64_t amp_dBm) {
   // ASSUMES: amplitude in dBm
   stringstream strm;
   strm.precision(PrintPrecision);            // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (amp_dBm > safetyAmp()) {
      strm << function() << " " << " (HP8644A) safety amplitude (" << safetyAmp()
	   << " dBm) exceeded: " << amp_dBm;
      throw GPIBError(strm.str());
   }
   if (amp_dBm > maxAmplitude()) {
      strm << function() << " " << " (HP8644A) maximum amplitude (" << maxAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }
   if (amp_dBm < minAmplitude()) {
      strm << function() << " " << " (HP8644A) minimum amplitude (" << minAmplitude()
	   << " dBm) exceeded: " << amp_dBm; // TBD units?
      throw GPIBError(strm.str());
   }
   strm << "AMPLITUDE " << amp_dBm << "DBM"; // TBD test this
   send(strm.str());
   if (simulated()) amp_ = amp_dBm;

}

float64_t HP8644A::amplitude() {
   string amp_dBm;
   send("AMPLITUDE?"); // TBD test this
   recv(amp_dBm);
   if (simulated()) return(amp_);
   return(atof(amp_dBm.c_str()));
}

void HP8644A::frequency(float64_t freqHz) {
   // ASSUMES: frequency in Hz
   stringstream strm;
   strm.precision(PrintPrecision);  // show N places after the decimal 
   strm.setf(std::ios::fixed);   // show all decimal places up to precision 

   if (freqHz > maxFrequency()) {
      strm << function() << " " << " (HP8644A) maximum frequency (" << maxFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   if (freqHz < minFrequency()) {
      strm << function() << " " << " (HP8644A) minimum frequency (" << minFrequency()
	   << " Hz) exceeded: " << freqHz;
      throw GPIBError(strm.str());
   }
   // ASSUMES: freq in Hz
   strm << "FREQ " << freqHz/1e6 << "MHZ"; // TBD test this
   send(strm.str());
   if (simulated()) freqHz_ = freqHz;
}

float64_t HP8644A::frequency() {
   string freqHz;
   send("FREQ?"); // returns Hz
   recv(freqHz);
   if (simulated()) return(freqHz_);
   return(atof(freqHz.c_str()));
}

void  HP8644A::RF(bool state) {
   if (state) {
      if (simulated()) on_ = true;
      send("AMPLITUDE:STATE ON"); // TBD test this
   } else {
      if (simulated()) on_ = false;
      send("AMPLITUDE:STATE OFF"); // TBD test this
   }
}

bool  HP8644A::RF() {
   string rf;
   send("AMPLITUDE:STATE?"); // TBD test this
   recv(rf);
   if (simulated()) return(on_);
   return(atoi(rf.c_str()));
}

void  HP8644A::selftest(string& result) {
   send("*TST?");
   if (simulated()) result = "0";
   else {
      sleep(330); // This self test takes 5 minutes
      recv(result);
   }

};
