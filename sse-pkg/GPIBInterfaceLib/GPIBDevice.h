/*******************************************************************************

 File:    GPIBDevice.h
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
// Filename:    GPIBDevice.h
// Description: GPIB Device class
// Authors:     L.R. McFarland
// Created:     16-02-2001
// Language:    C++
// ============================================================

#ifndef _GPIBDevice_h
#define _GPIBDevice_h

#include <config.h>

#include <string>
#include <sstream>
#include <iostream>

#include "machine-dependent.h"

using std::string;
using std::ostream;

#if defined(HAVE_LIBGPIB)
extern "C" {

/* define _ANSI_C to use the full function prototypes in ugpib.h */
#ifndef _ANSI_C_
#define _ANSI_C_
#endif

#include <ugpib.h>

}
#endif


class GPIBDevice {

 public:

  static const int DefaultBufferSize = 256; // default buffer size

  GPIBDevice(int bufferSize = DefaultBufferSize);
  GPIBDevice(int bus, int address, int bufferSize = DefaultBufferSize);
  virtual ~GPIBDevice();

  const string& function() const          {return(function_);}
  void          function(const char* description)   {function_ = description;}

  const string& id() const                {return(id_);}
  void          id(const char* id)         {id_ = id;}

  const  int    bus() const               {return bus_;}
  void          bus(int bus)                {bus_ = bus;}

  const  int    address() const           {return address_;}
  void          address(int addr)            {address_ = addr;}

  const  bool   verbose() const           {return(verbose_);}
  void          verbose(bool mode)           {verbose_ = mode;}

  const  bool   simulated() const         {return(simulated_);}
  void          simulated(bool mode)         {simulated_ = mode;}

  virtual string getModelName() const;
  void          sendIFC();

  void          send(const char* command);
  void          send(const string& command);

  void          recv(char* result, int size);
  void          recv(string& result);

  // IEEE 488.2 common commands

  virtual void identify(); // from machine
  virtual void reset();
  virtual void selftest(string& result);

  // TBD wait() ?
  // TBD save() ?
  // TBD recall() ?
  // TBD operation complete

  friend ostream& operator << (ostream& strm, const GPIBDevice& msg);

 protected:

  bool   verbose_;
  bool   simulated_;

 private:
  string function_;
  string id_;
  int    bus_;
  int    address_;
  
  int    bufferSize_;
  char*  buffer_;

};

class GPIBError {

 public:

  GPIBError(const char* msg = "") : message_(msg) {};
  GPIBError(const string msg) : message_(msg) {}

  string message() const {return message_;}

  friend ostream& operator << (ostream& strm, const GPIBError& err) {
    strm << "Error GPIB: " << err.message() << std::endl;
    return(strm);
  };


 protected:

  string message_;


};

class GPIBBusError : public GPIBError {

 public:

  GPIBBusError(string function, string modelName, int bus, int addr, 
	       int ibstatus, int iberror, const char* msg);

  friend ostream& operator << (ostream& strm, const GPIBBusError& gerr) {
    strm << gerr.message();
    return strm;
  };

 private:
  int    bus_;
  int    addr_;
  int    ibstatus_;
  int    iberror_;


};

#endif // _GPIBDevice_h