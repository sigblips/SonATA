/*******************************************************************************

 File:    Ifc.h
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

// NSS IF control

// Original code created by L. R. McFarland 2003

#ifndef IFC_H
#define IFC_H

#include <string>
#include <list>
#include <SignalGenerator.h>
#include <SCU.h>
#include "Stx.h"
#include "IfcAttnScu.h"

class Ifc 
{

 public:

   Ifc();
   virtual ~Ifc();

   // ===== accessors for tcl =====

   const char* name() const;
   const char* name(string& newName);
   const char* name(const char* newName);

   const char* version() const;

   bool simulated();
   bool simulated(bool mode);

   Stx & getStx();
   IfcAttnScu & getAttnScu();

   // ===== GPIB configuration =====
   const char* simscu(const char* mode);
   const char* addressscu(int bus, int addr);
   const char* getscu();

   // ===== commands =====

   const char* attn(int attnDbLeft, int attnDbRight);
   const char* gpib(const char* mode);
   const char* help();
   const char* id();
   const char* ifsource(const char* source);
   const char* intrinsics();
   const char* off();

   const char* reset();
   const char* requestready();
   const char* selftest();
   const char* simulate(const char* mode);
   const char* status();

   const char* setStxPol(const char* pol);

 private:

   const char* simulate(GPIBDevice *device, const char *mode);

   // ---- tcl interface ----

   string tclBuffer_; // holds return string for tcl interpreter
   const char* sendToSocket(const string& message); // send to tcl on socket

   // ===== Ifc members =====

   string name_;
   string version_;
   bool simulated_;

   IfcAttnScu scu_;
   Stx stx_;

};


#endif // IFC_H
