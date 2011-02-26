/*******************************************************************************

 File:    Stx.h
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


#ifndef Stx_H
#define Stx_H

#include "sseInterface.h"
#include <string>
using std::string;

class Ifc;

class Stx 
{

 public:

  Stx(Ifc* ifc);
  Stx(Ifc* ifc, const string & filename);
  virtual ~Stx();

  // ===== accessors for tcl =====

  Ifc* ifc();
  const char*  filename() const;
  const char*  filename(const string& name);
  bool simulated();
  bool simulated(bool mode);

  unsigned int histogramLength() const;
  unsigned int histogramLength(int length);

  unsigned int maxTries() const;
  unsigned int maxTries(unsigned int count);

  double tolerance() const;
  double tolerance(double tol);

  double lcpVariance() const;
  double rcpVariance() const;
      
  void setPol(Polarization pol);

  // for debugging
  void setStatus(unsigned int status);
  void setLeftVar(double variance);
  void setRightVar(double variance);

  // ===== commands =====

  const char* start();
  const char* status();

  const char* setvariance(double lcpVariance, double rcpVariance);

 private:

  bool haveGoodRegisterStatus();
  void setSimulatedState();

  string       tclBuffer_; // holds return string for tcl interpreter
  const char*  sendToSocket(const string& message); // send to tcl on socket

  Ifc* ifc_;

  string filename_;
  bool simulated_;
  unsigned int statusReg_;

  int histogramLength_;
  unsigned int maxTries_;

  double tolerance_;
  double lcpVariance_;
  double rcpVariance_;

  Polarization pol_;

  // Disable copy construction & assignment.
  // Don't define these.
  Stx(const Stx& rhs);
  Stx& operator=(const Stx& rhs);

};

#endif // Stx_H