/*******************************************************************************

 File:    MinMaxBandwidth.h
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
 * define functions that determine the min and max bandwidths 
 * for a list of components which support the getBandwidthInMHz()
 * method
 *****************************************************************/

#ifndef MINMAXBANDWIDTH_H
#define MINMAXBANDWIDTH_H

#include <algorithm>

template <class Component>
bool BandwidthLessThan(const Component* a, const Component* b)
{
    if (a->getBandwidthInMHz() < b->getBandwidthInMHz())
    {
	return true;
    }
    
    return false;
}

template <class ForwardIter, class Component>
float64_t MinBandwidth(ForwardIter first, ForwardIter last)
{
  float64_t minBandwidth = 0;

  ForwardIter iter = 
      min_element(first, last, BandwidthLessThan<Component>);
  if (iter != last)
  {
    minBandwidth = (*iter)->getBandwidthInMHz();
  }

  return minBandwidth;
}

template <class ForwardIter, class Component>
float64_t MaxBandwidth(ForwardIter first, ForwardIter last)
{
  float64_t maxBandwidth = 0;

  ForwardIter iter =
      max_element(first, last, BandwidthLessThan<Component>);
  if (iter != last)
  {
    maxBandwidth = (*iter)->getBandwidthInMHz();
  }

  return maxBandwidth;
}


#endif /* MINMAXBANDWIDTH_H */