/*******************************************************************************

 File:    SignalMask.h
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


#ifndef SignalMask_H
#define SignalMask_H

// A base class that loads a signal mask and
// sends it to a dx.  Subclasses override the forwardMaskToDx
// method to select which type of mask is sent (ie,
// which specific dx sendmask method is called)

#include "sseDxInterface.h"
#include <string>
#include <vector>

class DxProxy;
class ObsRange;

using std::string;
using std::vector;

class SignalMask
{
 public:
    SignalMask(const string &maskFilename, const string &maskType, 
	       int verboseLevel);

    SignalMask(FrequencyMaskHeader & maskHeader,
	       vector <FrequencyBand> & maskList,
	       const string &maskType,
	       int verboseLevel);

    vector <FrequencyBand> getFreqBands();

    virtual ~SignalMask();

protected:
 
    FrequencyMaskHeader maskHeader_;
    vector <FrequencyBand> maskList_;
    string maskType_;
    int verboseLevel_;

    // Disable copy construction & assignment.
    // Don't define these.
    SignalMask(const SignalMask& rhs);
    SignalMask& operator=(const SignalMask& rhs);

};

class DxSignalMask : public SignalMask
{
public:
    DxSignalMask(const string &maskFilename, const string &maskType, 
		  int verboseLevel);
    virtual void sendMaskToDx(DxProxy *proxy);
protected:

    // subclasses override this
    virtual void forwardMaskToDx(DxProxy *proxy, 
				  const FrequencyMaskHeader &maskHeader,
				  const FrequencyBand freqBandArray[]) = 0;
private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxSignalMask(const DxSignalMask& rhs);
    DxSignalMask& operator=(const DxSignalMask& rhs);
  
};


class PermRfiMask : public DxSignalMask 
{
public:
    PermRfiMask(const string &maskFilename, const string &maskType, 
		int verboseLevel);
protected:
    virtual void forwardMaskToDx(DxProxy *proxy, 
				  const FrequencyMaskHeader &maskHeader,
				  const FrequencyBand freqBandArray[]);
};

#endif // SignalMask_H