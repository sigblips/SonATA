/*******************************************************************************

 File:    RecentRfiMask.h
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


#ifndef RecentRfiMask_H
#define RecentRfiMask_H

#include <vector>

using std::vector;

class RecentRfiMask
{
 public:

    static void createMask(
       const vector<double> & signalFreqMhz, 
       double minMaskElementWidthMhz,
       vector<double> & maskCenterFreqMhz, 
       vector<double> & maskWidthMhz);

 private:

    // don't allow this class to be instantiated
    RecentRfiMask();
    virtual ~RecentRfiMask();

    // Disable copy construction & assignment.
    // Don't define these.
    RecentRfiMask(const RecentRfiMask& rhs);
    RecentRfiMask& operator=(const RecentRfiMask& rhs);

};

#endif // RecentRfiMask_H