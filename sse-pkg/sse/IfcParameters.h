/*******************************************************************************

 File:    IfcParameters.h
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

 
#ifndef IFC_PARAMETERS_H
#define IFC_PARAMETERS_H

#include "machine-dependent.h" // for float64_t et. al.
#include "SeekerParameterGroup.h"

class IfcParametersInternal;

class IfcParameters : public SeekerParameterGroup
{
 public:
  
  IfcParameters(const string &command);
  IfcParameters(const IfcParameters& rhs);
  IfcParameters& operator=(const IfcParameters& rhs);
  virtual ~IfcParameters();

  // ----- immediate commands -----
  void stxvariance(const string& ifcName);

  // ----- misc -----
  int32_t getAttnl() const;
  int32_t getAttnr() const;

  float64_t getVarLeft() const;
  float64_t getVarRight() const;

  float64_t getStxTolerance() const;
  int32_t getStxLength() const;

  bool useAutoAttnCtrl() const;
  int32_t getHistogramLength() const;

  const bool ifSourceIsTest();
  const bool ifSourceIsSky();

protected:

  virtual void addParameters();
  virtual void addAllImmedCmdHelp();

private:

  IfcParametersInternal *internal_;

};

#endif