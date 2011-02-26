/*******************************************************************************

 File:    GPIBDevice.cpp
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

// Original code by L.R. McFarland
// ============================================================

#include <GPIBDevice.h>
#include <unistd.h>

using namespace std;

GPIBDevice::GPIBDevice(int bufferSize) :
    verbose_(false), simulated_(false), 
    function_("function not set"), id_("ID not set"),
    bus_(-1), address_(-1),  bufferSize_(bufferSize)
{
    buffer_ = new char[bufferSize_];
    buffer_[0] = '\0';

#if !defined(HAVE_LIBGPIB)
    simulated_ = true;
    id_ = "simulated";
#endif

}

GPIBDevice::GPIBDevice(int bus, int address, int bufferSize) :  
  verbose_(true), simulated_(false),
  function_("function not set"), id_("ID not set"), 
  bus_(bus), address_(address), bufferSize_(bufferSize)
{
  buffer_ = new char[bufferSize_];
  buffer_[0] = '\0';

#if !defined(HAVE_LIBGPIB)
  simulated_ = true;
  id_ = "simulated";
#endif

}

GPIBDevice::~GPIBDevice()
{
  delete [] buffer_;
}

void GPIBDevice::sendIFC()
{
  if (0) // too verbose because it is now called every send/recv?
  {
    cout << "SendIFC() called." << endl;
  }

#if defined(HAVE_LIBGPIB)
  if (!simulated())
  {
    SendIFC(bus()); // this is NI's IFC, no relation to our IFC
  }
#endif
}

string GPIBDevice::getModelName() const
{
    return "";  // let subclasses override this method & fill it in
}

void GPIBDevice::send(const char* command)
{
  if (verbose())
  {
    if (simulated())
    {
      cout << "Send simulated " << function() << endl;
    }
    else
    {
      cout << "Send " << function() << endl;
    }

    cout << "  GPIB(" << bus() << ", " << address() << "): " 
	 << command << endl;
  }

#if defined(HAVE_LIBGPIB)

  if (!simulated())
  {
    if (address() < 0)  // TBD check this 
    {
      throw GPIBBusError(function(), getModelName(), bus(), 
			 address(), ibsta, iberr, 
			 "Send() invalid address.");
    }

    if (address() > 32) // TBD check this 
    {
      throw GPIBBusError(function(), getModelName(), bus(), 
			 address(), ibsta, iberr, 
			 "Send() invalid address.");
    }

    sendIFC();
    Send(bus(), address(), const_cast<char *>(command), strlen(command),
	 NLend);

    if (ibsta & ERR)
    { 
      throw GPIBBusError(function(), getModelName(), bus(), 
			 address(), ibsta, iberr, "Send()");
    }
  }

#endif
}

void GPIBDevice::send(const string& command)
{
  send(command.c_str());
}

void GPIBDevice::recv(char* result, int size)
{

#if defined(HAVE_LIBGPIB)

  if (!simulated())
  {
    if (address() < 0)  // TBD check this 
    {
	throw GPIBBusError(function(), getModelName(), bus(),
			 address(), ibsta, iberr, 
			 "Receive() bogus address.");
    }

    if (address() > 32) // TBD check this 
    {
	throw GPIBBusError(function(), getModelName(), bus(),
			   address(), ibsta, iberr, 
			   "Receive() bogus address.");
    }

    sendIFC();
    Receive(bus(), address(), result, size-1, STOPend);
    if (ibsta & ERR) 
    {
	throw GPIBBusError(function(), getModelName(), bus(), 
			   address(), ibsta, iberr, 
			   "Receive()");
    }

    result[ibcnt-1] = '\0';
  }

#endif

  if (verbose())
  {
      cout << "Recv " << function() << " GPIB(" 
	   << bus() << ", " << address() << "): " 
	   << result << endl;
  }

}

void GPIBDevice::recv(string& result)
{
  recv(buffer_, bufferSize_);
  result = buffer_;
}

void GPIBDevice::identify()
{
  string result;
  send("*IDN?");
  if (!simulated()) recv(result);
  id(result.c_str());
}

void GPIBDevice::reset()
{
  send("*RST");
}

void GPIBDevice::selftest(string& result)
{
  send("*TST?");
  if (simulated())
  {
      result = "0";  // Test succeeded
  } 
  else
  {
    sleep(10); // TBD self test sleep time
    recv(result);
  }
}

ostream& operator << (ostream& strm, const GPIBDevice& dev)
{
  strm << "GPIBDevice:"
       << "\tfunction(" << dev.function() << "), " << endl
       << "\tmodel(" << dev.getModelName() << "), " << endl
       << "\tid(" << dev.id() << "), " << endl
       << "\tbus(" << dev.bus() << "), " << endl
       << "\taddr(" << dev.address() << "), " << endl
       << "\tbsize(" << dev.bufferSize_ << "), " << endl
       << "\tbuffer(" << dev.buffer_ << "), " << endl
       << "\tverbose(" << dev.verbose() << "), " << endl
       << "\tsimulated(" << dev.simulated() << "), " << endl
       << endl;

  return strm;
}



// ----- GPIB Error -----
GPIBBusError::GPIBBusError(string function, string modelName, int bus,
			   int addr, int ibstatus, int iberror, 
			   const char* msg) : 
  GPIBError(msg), bus_(bus), addr_(addr), 
  ibstatus_(ibstatus), iberror_(iberror)
{
  stringstream strm;

  strm 
       << "Device: " << function << ". "
       << "  Model: " << modelName << "."
       << "  Msg: '" << message() << "'." 
       << "  GPIB bus: " << bus_ << "."
       << "  Device Addr: " << addr_ << ".";

  // Ref: NI-488.2M Software Reference Manual, Feb 1996 Ed.
  // Ch 3: Understanding the NI-488.2M Software 
  // Ch 4: Software Characteristics and Routines, pp. 4-42,43

#if defined(HAVE_LIBGPIB)

  strm << "  IB Error: " << (dec) << iberror_ << " ";

  if (iberror_ == EDVR) strm << "EDVR (UNIX error, code=" << ibcnt << ")";
  if (iberror_ == ECIC) strm << "ECIC (Not Controller In Charge)";
  if (iberror_ == ENOL) strm << "ENOL (Can't talk to device)";
  if (iberror_ == EADR) strm << "EADR (Address error)";
  if (iberror_ == EARG) strm << "EARG (Invalid argument)";
  if (iberror_ == ESAC) strm << "ESAC (GPIB Board not System Controller)";
  if (iberror_ == EABO) strm << "EABO (I/O operation aborted (timeout))";
  if (iberror_ == ENEB) strm << "ENEB (Non-existent GPIB board)";
  if (iberror_ == EDMA) strm << "EDMA (DMA hardware problem)";
  if (iberror_ == EBTO) strm << "EBTO (DMA hardware bus timeout)";
  if (iberror_ == ECAP) strm << "ECAP (No capability for operation)";
  if (iberror_ == EFSO) strm << "EFSO (File system error)";
  if (iberror_ == EBUS) strm << "EBUS (GPIB bus error)";
  if (iberror_ == ESTB) strm << "ESTB (Status byte queue overflow)";
  if (iberror_ == ESRQ) strm << "ESRQ (SRQ stuck on)";
  if (iberror_ == ETAB) strm << "ETAB (Table Overflow)";

  strm << "  IB Status: ";
  strm << "0x" << (hex) << ibstatus_ << ".";

/*
  // Too much detail, not useful enough

  if (ibstatus_ & ERR)  strm << "ERR (GPIB error)  ";
  if (ibstatus_ & TIMO) strm << "TIMO (timeout)  ";
  if (ibstatus_ & END)  strm << "END (END or EOS detected)  ";
  if (ibstatus_ & SRQI) strm << "SRQI (Service request interrupt received)  ";
  if (ibstatus_ & RQS)  strm << "RQS (Device requesting service)  ";
  if (ibstatus_ & CMPL) strm << "CMPL (I/O completed)  ";
  if (ibstatus_ & LOK)  strm << "LOK (Lockout state)  ";
  if (ibstatus_ & REM)  strm << "REM (Remote state)  ";
  if (ibstatus_ & CIC)  strm << "CIC (Controller-In-Charge)  ";
  if (ibstatus_ & ATN)  strm << "ATN (Attention is asserted)  ";
  if (ibstatus_ & TACS) strm << "TACS (Talker)  ";
  if (ibstatus_ & LACS) strm << "LACS (Listener)  ";
  if (ibstatus_ & DTAS) strm << "DTAS (Device trigger state)  ";
  if (ibstatus_ & DCAS) strm << "DCAS (Device clear state)  ";
*/

#endif

  message_ = strm.str();
}