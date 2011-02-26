/*******************************************************************************

 File:    SignalGenerator.h
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


#ifndef _SYNTHESIZERS_h
#define _SYNTHESIZERS_h

#include <GPIBDevice.h>

using std::cerr;
using std::endl;

class SignalGenerator : public GPIBDevice
{

 public:

   SignalGenerator(int bufferSize = DefaultBufferSize);
   virtual ~SignalGenerator();

   virtual void safetyAmp(float64_t limit);
   virtual float64_t safetyAmp();

   // ----- commands -----

   virtual void reset();

   virtual float64_t maxFrequency() = 0;
   virtual float64_t minFrequency() = 0;

   virtual float64_t maxAmplitude() = 0;
   virtual float64_t minAmplitude() = 0;

   virtual void frequency(float64_t freqHz) = 0;
   virtual float64_t frequency() = 0;

   virtual void amplitude(float64_t ampVpp) = 0;
   virtual float64_t amplitude() = 0;

   virtual void RF(bool state);
   virtual bool RF();

   virtual void sweep(float64_t amplitudeVpp, float64_t startHz, 
		      float64_t driftRateHzSec, float64_t durationSec);

   virtual float64_t startFrequency();
   virtual float64_t stopFrequency();
   virtual float64_t sweepTime();
   virtual bool sweepState();

   virtual void pulse(float64_t amplitudeVpp, float64_t periodSec,
		      float64_t widthSec);

   virtual float64_t period();
   virtual float64_t width();

   virtual void enableExternalPulseModulation();
   virtual void disablePulseModulation();

 protected:

   float64_t safetyAmp_; // amplitude safety limit

   // To remember commanded values:
   float64_t freqHz_;  
   float64_t amp_;  //  dBm or volts   
   bool on_;    

   float64_t startFrequencyHz_;
   float64_t stopFrequencyHz_;
   float64_t sweepTime_;
   bool sweepState_;

   float64_t pulsePeriod_;
   float64_t pulseWidth_;

};


class AT33250 : public SignalGenerator
{

 public:

   AT33250(int bufferSize = DefaultBufferSize);
   ~AT33250();

   float64_t maxFrequency();
   float64_t minFrequency();

   float64_t maxAmplitude();
   float64_t minAmplitude();

   float64_t maxSweepTime();
   float64_t minSweepTime();

   float64_t period();
   float64_t width();

   // ----- commands -----

   virtual void amplitude(float64_t amp_Vpp);
   virtual float64_t amplitude();

   virtual void frequency(float64_t freq_Hz);
   virtual float64_t frequency();

   virtual void sweep(float64_t amplitudeVpp, float64_t startHz, 
		      float64_t driftRateHzSec, float64_t durationSec);
  
   virtual float64_t startFrequency();
   virtual float64_t stopFrequency();
   virtual bool sweepState();
   virtual float64_t sweepTime();

   void pulse(float64_t amplitudeVpp, float64_t periodSec, float64_t widthSec);

   virtual string getModelName() const;

 private:
   static const float64_t max_frequency = 80e6;    // Hz, Service Guide p. 14
   static const float64_t min_frequency = 1e-6;    // Hz, Service Guide p. 14

   static const float64_t max_amplitude = 20;      // dBm, Service Guide p. 15
   static const float64_t min_amplitude = -36.0;   // dBm, Service Guide p. 15
  
   static const float64_t min_sweep_time = 0.001; 
   static const float64_t max_sweep_time = 500;   

   static const float64_t default_Vpp = -5; // ZFSWHA control switching level

   static const int high_limit =  2047; // User's guide, p. 200
   static const int low_limit  = -2047; // User's guide, p. 200

   float64_t periodSec_;   // saved value for status
   float64_t widthSec_; // saved value for status


};


class AT4400 : public SignalGenerator
{
 public:

   AT4400(int bufferSize = DefaultBufferSize);
   ~AT4400();

   virtual void amplitude(float64_t amp_dBm);
   virtual float64_t amplitude();

   virtual void frequency(float64_t freq_Hz);
   virtual float64_t frequency();

   virtual float64_t startFrequency();
   virtual float64_t stopFrequency();
   virtual bool sweepState();
   virtual float64_t sweepTime();

   float64_t maxAmplitude();
   float64_t minAmplitude();

   float64_t maxFrequency();
   float64_t minFrequency();

   float64_t maxSweepDwell();
   float64_t minSweepDwell();

   float64_t maxSweepPoints();
   float64_t minSweepPoints();

   float64_t maxPulsePeriodSec();

   virtual void enableExternalPulseModulation();
   virtual void disablePulseModulation();

   virtual void sweep(float64_t amplitudedBm, float64_t startHz,
			float64_t driftRateHzSec, float64_t durationSec);

   virtual void pulse(float64_t amplitudeVpp, float64_t periodSec,
		      float64_t widthSec);

   virtual string getModelName() const;

 private:
   static const float64_t max_frequency = 1e9;    // Hz, 4400 front panel
   static const float64_t min_frequency = 250e3;  // Hz, 4400 front panel

   static const float64_t max_amplitude = 20;     // dBm, spin dial
   static const float64_t min_amplitude = -136;   // dBm, spin dial

   static const float64_t min_sweep_dwell = 0.001; 
   static const float64_t max_sweep_dwell = 60;   

   static const float64_t min_sweep_points = 2; 
   static const float64_t max_sweep_points = 401;   

   static const float64_t maxPulsePeriodSec_ = 30;

   float64_t sweepTime_;
};



class HP3325B : public SignalGenerator
{

 public:

   HP3325B(int bufferSize = DefaultBufferSize) : 
      SignalGenerator(bufferSize)
      {}

   ~HP3325B() {}

   float64_t maxFrequency() {return(max_frequency);}
   float64_t minFrequency() {return(min_frequency);}

   float64_t maxAmplitude() {return(max_amplitude);}
   float64_t minAmplitude() {return(min_amplitude);}

   float64_t maxSweepTime() {return(max_sweep_time);}
   float64_t minSweepTime() {return(min_sweep_time);}

   // ----- commands -----

   virtual void amplitude(float64_t amp_Vpp);
   virtual float64_t amplitude();

   virtual void amplitude_dBm(float64_t amp_dBm);
   virtual float64_t amplitude_dBm();

   virtual void frequency(float64_t freq_Hz);
   virtual float64_t frequency();

   virtual void sweep(float64_t amplitudeVpp, float64_t startHzSec, 
		      float64_t driftRateHzSec, float64_t durationSec);
  
   virtual float64_t startFrequency();
   virtual float64_t stopFrequency();
   virtual bool sweepState();
   virtual float64_t sweepTime();

   virtual void selftest(string& result);

   virtual void RF(bool state);
   virtual bool RF();

   virtual float64_t period()
   {
      cerr << "HP3325B::period() not yet implemented for this device." 
	      << endl;
      return(-1);
   };
   
   virtual float64_t width()
   {
      cerr << "HP3325B::duration() not yet implemented for this device." 
	   << endl;
      return(-1);
   };
   
   virtual string getModelName() const;

 private:
   static const float64_t max_frequency = 20.999999e6; // Hz
   static const float64_t min_frequency = 0.1e-6; // Hz, TBD (from 3326A manual)

   static const float64_t max_amplitude = 5;      // Vpp, TBD (from 3326A manual)
   static const float64_t min_amplitude = -55;    // dBm
  
   static const float64_t min_sweep_time = 0.005; // sec, TBD(from 3326A manual)
   static const float64_t max_sweep_time = 1000;  // sec, TBD(from 3326A manual)

   double    parseDouble(string source);



};

class DS340 : public SignalGenerator
{

 public:

   enum Waveform_t {SINE, SQUARE, TRIANGLE, RAMP, NOISE, ARBITRARY};

   DS340(int bufferSize = DefaultBufferSize) : 
      SignalGenerator(bufferSize), 
      period_(0), 
      width_(0), 
      duration_(0), 
      samples_(0), 
      wavefm_(SINE) 
      {}

   ~DS340() {}

   float64_t maxFrequency()  {return(max_frequency);}
   float64_t minFrequency()  {return(min_frequency);}

   float64_t maxAmplitude()  {return(max_amplitude);}
   float64_t minAmplitude()  {return(min_amplitude);}

   float64_t defaultVpp()    {return(default_Vpp);}

   float64_t period()        {return(period_);}
   float64_t width()         {return(width_);}
   float64_t duration()      {return(duration_);}
   int       samples()       {return(samples_);}

   // ----- commands -----

   void         amplitude(float64_t amp_Vpp);
   float64_t    amplitude();

   void         frequency(float64_t freq_Hz);
   float64_t    frequency();

   void         waveform(Waveform_t wavefm);
   Waveform_t   waveform();

#ifdef DS340_PULSE
   void         pulse(float64_t amplitude, float64_t period,
		      float64_t duration, int samples);
#endif

   virtual void sweep(float64_t amplitudeVpp, float64_t startHz, 
		      float64_t driftRateHzSec, float64_t durationSec);
   virtual float64_t startFrequency();
   virtual float64_t stopFrequency();
   virtual bool sweepState();
   virtual float64_t sweepTime();
   virtual string getModelName() const;

 private:

   // sine limits, TBD check also spqare, ramp, triangle
   static const float64_t max_frequency = 15.1e6; // Hz, Manual, p. vii
   static const float64_t min_frequency = 1e-6;   // Hz, TBD

   static const float64_t max_amplitude = 10;    // Vpp,Operating Manual, p. vii
   static const float64_t min_amplitude = 50e-3; // Vpp,Operating Manual, p. vii

   static const float64_t default_Vpp = -6; // ZFSWHA control switching level
   static const int       min_waveform_length = 8;
   static const int       max_waveform_length = 16300; // TBD

   static const int       ARB_high_limit = 2047; // Operating Manual, p. 3-6
   static const int       ARB_low_limit = -2048; // Operating Manual, p. 3-6

   float64_t period_;   // saved value for status
   float64_t width_; // saved value for status
   float64_t duration_; // saved value for status
   int       samples_;  // saved value for status
   Waveform_t wavefm_;

};


class HP83711 : public SignalGenerator
{

 public:

   HP83711(int bufferSize = DefaultBufferSize) : SignalGenerator(bufferSize) {}
   ~HP83711() {}

   virtual void      amplitude(float64_t amp_dBm);
   virtual float64_t amplitude();

   virtual void      frequency(float64_t freq_Hz);
   virtual float64_t frequency();

   float64_t         maxAmplitude() {return(max_amplitude);}
   float64_t         minAmplitude() {return(min_amplitude);}

   float64_t         maxFrequency() {return(max_frequency);}
   float64_t         minFrequency() {return(min_frequency);}

   virtual string getModelName() const;

 private:

   static const float64_t max_frequency = 20.0e9; // Hz,  User's guide, p. 4-6
   static const float64_t min_frequency = 1.0e9; // Hz,  User's guide, p. 4-6

   static const float64_t max_amplitude = 11;     // dBm, User's guide, p. 4-7
   static const float64_t min_amplitude = -90;    // dBm, User's guide, p. 4-7


};

class HP8662 : public SignalGenerator
{

 public:

   HP8662(int bufferSize = DefaultBufferSize) : SignalGenerator(bufferSize) {}
   ~HP8662() {}

   virtual void      amplitude(float64_t amp_dBm);
   virtual float64_t amplitude();

   virtual void      frequency(float64_t freq_Hz);
   virtual float64_t frequency();

   float64_t         maxAmplitude() {return(max_amplitude);}
   float64_t         minAmplitude() {return(min_amplitude);}

   float64_t         maxFrequency() {return(max_frequency);}
   float64_t         minFrequency() {return(min_frequency);}

   virtual void      RF(bool state);
   virtual bool      RF();

   // IEEE 488.2 common commands won't work with this machine

   virtual void      identify(); // from machine
   virtual void      reset();
   virtual void      selftest(string& result);

   virtual string getModelName() const;

 private:
   static const float64_t max_frequency = 1280e6; // Hz, from manual 1-1
   static const float64_t min_frequency = 10e3;   // Hz, from manual 1-1

   static const float64_t max_amplitude = 13;     // dBm, from manual 1-2
   static const float64_t min_amplitude = -30.0;  // dBm, front pannel

   static const float64_t freq_res_break = 640e6; // Hz, from manual 1-4
   static const float64_t freq_resolutionA = 0.1; // Hz, below 640 MHz
   static const float64_t freq_resolutionB = 0.2; // Hz, above 640 MHz

   static const float64_t amp_resolution = 0.1;   // dBm, from manual 1-4


};

class HP8644A : public SignalGenerator
{
 public:

   HP8644A(int bufferSize = DefaultBufferSize) : SignalGenerator(bufferSize) {}
   ~HP8644A() {}

   virtual void      amplitude(float64_t amp_dBm);
   virtual float64_t amplitude();

   virtual void      frequency(float64_t freq_Hz);
   virtual float64_t frequency();

   float64_t         maxAmplitude() {return(max_amplitude);}
   float64_t         minAmplitude() {return(min_amplitude);}

   float64_t         maxFrequency() {return(max_frequency);}
   float64_t         minFrequency() {return(min_frequency);}

   virtual void      RF(bool state);
   virtual bool      RF();

   virtual void      selftest(string& result);

   virtual string getModelName() const;

 private:
   static const float64_t max_frequency = 1030e6; // Hz, from cal manual 1-2
   static const float64_t min_frequency = 251e3;  // Hz, from cal manual 1-2

   static const float64_t max_amplitude = 16;     // dBm, from cal manual 1-2
   static const float64_t min_amplitude = -137.0; // dBm, from cal manual 1-2

};

#endif //
