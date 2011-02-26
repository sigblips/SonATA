/*******************************************************************************

 File:    Stx.cpp
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



#include "Stx.h" 
#include "Ifc.h"
#include "SseMsg.h"
#include "Assert.h"
#include <sstream>
#include <unistd.h>

// get the HAVE_STX macro from the configuration file
#include "config.h"

using namespace std;

#ifdef HAVE_STX

#ifdef __linux
#include <sys/ioctl.h>
#include "../../hardware/stx/LinuxDriver/stxio.h" // TBD where to keep this?
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif

/* Good STX status words for left, right, and both pols */
static const unsigned int GoodStxStatusBothPols = 0x6f0;

// RCP DLL Lock Err, L&R Hist sync Err
static const unsigned int GoodStxStatusLeftPolOnly = 0x104f0; // TBD verify

// LCP DLL Lock Err, L&R Hist sync Err
static const unsigned int GoodStxStatus1RightPolOnly = 0x102f0; 

static const unsigned int GoodStxStatus2RightPolOnly = 0x002f0; 


static const double GoodVariance = 16;

Stx::Stx(Ifc* ifc) : 
    ifc_(ifc),
    filename_("/dev/stx0"), // default value
    simulated_(false),
    statusReg_(0),
    histogramLength_(0),
    maxTries_(100),
    tolerance_(2),
    lcpVariance_(0),
    rcpVariance_(0),
    pol_(POL_BOTH)
{
}

Stx::Stx(Ifc* ifc, const string &filename) : 
    ifc_(ifc),
    filename_(filename),
    simulated_(false),
    statusReg_(0),
    histogramLength_(0),
    maxTries_(100),
    tolerance_(2),
    lcpVariance_(0),
    rcpVariance_(0),
    pol_(POL_BOTH)
{
}

Stx::~Stx()
{
}

  // ===== accessors for tcl =====

Ifc* Stx::ifc()
{
    return(ifc_);
}

const char* Stx::filename() const
{
    return(filename_.c_str());
}

const char* Stx::filename(const string& name)
{
    filename_ = name;
    return(filename());
}

bool Stx::simulated() 
{
    return(simulated_);
}

void Stx::setSimulatedState()
{
    // set good stx status & variance for testing
    setStatus(GoodStxStatusBothPols);  
    setLeftVar(GoodVariance);
    setRightVar(GoodVariance);
}

bool Stx::simulated(bool mode)
{
    simulated_ = mode;
    if (simulated_)
    {
        setSimulatedState();
    }
    return(simulated());
}

unsigned int Stx::histogramLength() const 
{
    return(histogramLength_);
}

unsigned int Stx::histogramLength(int length) 
{
    histogramLength_ = length;
    return(histogramLength());
}

unsigned int Stx::maxTries() const
{
    return(maxTries_);
}

unsigned int Stx::maxTries(unsigned int count)
{
    maxTries_ = count; return(maxTries());
}

double Stx::tolerance() const
{
    return(tolerance_);
}

double Stx::tolerance(double tol)
{
    tolerance_ = tol;
    return(tolerance());
}

double Stx::lcpVariance() const 
{
    return(lcpVariance_);
}

double Stx::rcpVariance() const
{
    return(rcpVariance_);
}

void Stx::setPol(Polarization pol)
{
    pol_ = pol;
}


// ===== private methods =====

void Stx::setStatus(unsigned int status)
{
    statusReg_ = status;
}

void Stx::setLeftVar(double variance)
{
    lcpVariance_ = variance;
}

void Stx::setRightVar(double variance)
{
    rcpVariance_ = variance;    
}



const char* Stx::sendToSocket(const string& message)
{
    tclBuffer_ = message;
    return(tclBuffer_.c_str());
}

// ===== public methods =====

const char* Stx::start() {

  if (simulated())
    return(sendToSocket("Simulated Stx started."));

#ifdef HAVE_STX

  int          stxfd(0);

  STXioctrl stxioctrl;
  stxioctrl.host_addr = &statusReg_;
  stxioctrl.stx_addr  = 0x0;
  stxioctrl.count     = sizeof(statusReg_);

  stxfd = open(filename_.c_str(), O_RDWR);

  if (stxfd == -1) 
  {
    stringstream errmsg;
    errmsg << "Error Stx::start() unable to open stx file: " 
	   << filename() << endl;
    return(sendToSocket(errmsg.str()));
  }

  if (ioctl(stxfd, STXIO_START_DC, &stxioctrl)) 
  {
    stringstream errmsg;
    errmsg << "Error Stx::start() failed: " << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));
  }

  close(stxfd);

  return(sendToSocket("Stx started."));

#else

  stringstream stxmsg;
  stxmsg << "Error Stx::start() "
	 << "is not implemented for this architecture." << endl;
  return(sendToSocket(stxmsg.str()));

#endif

}

const char* Stx::setvariance(double lcpVariance, double rcpVariance)
{
  stringstream msg;

#ifdef HAVE_STX

  const int maxAttn(11); // TBD user parameter?
  const int min_attn(0);   // minimum attenuation
  const int max_attn(11);  // maximum attenuation

  try {

    // tune left and right separately to simplify exit logic

    IfcAttnScu & scu =ifc()->getAttnScu();

    // left
    for (int i = 0; i < maxAttn; i++)
    {
      
      status();
      
      if (fabs(lcpVariance_ - lcpVariance) < lcpVariance * 0.01*tolerance_)
        break;
      
      if (lcpVariance_ > lcpVariance)
	{
	  if (scu.getAttnLeft() == max_attn)
	    break;
	
	  scu.setAttnLeft(scu.getAttnLeft() + 1);
	}
      else
	{
	  if (scu.getAttnLeft() == min_attn)
	    break;
	  
	  scu.setAttnLeft(scu.getAttnLeft() - 1);
      }

      sleep(1);

    }


    // right
    for (int j = 0; j < maxAttn; j++)
    {

      status();
      
      if (fabs(rcpVariance_ - rcpVariance) < rcpVariance * 0.01*tolerance_)
        break;
      
      if (rcpVariance_ > rcpVariance)
	{
        if (scu.getAttnRight() == max_attn)
	  break;
	
	scu.setAttnRight(scu.getAttnRight() + 1);
      }
      else 
	{
	  if ( scu.getAttnRight() == min_attn )
	    break;

	  scu.setAttnRight(scu.getAttnRight() - 1);
      }

      sleep(1);

    }

  }

  catch (GPIBError gpib_err) {
    msg  << "Error Stx::setvariance(): " << gpib_err;
    return(sendToSocket(msg.str()));
  }

  msg << "Stx::setvariance() complete." << endl;
  return(sendToSocket(msg.str()));

#else

  msg << "Error STX::setvariance() "
      << "is not implemented for this architecture." << endl;
  return(sendToSocket(msg.str()));

#endif


}

bool Stx::haveGoodRegisterStatus()
{
   switch (pol_)
   {
   case POL_LEFTCIRCULAR:
       return (statusReg_ == GoodStxStatusLeftPolOnly);
       break;
       
   case POL_RIGHTCIRCULAR:
       return (statusReg_ == GoodStxStatus1RightPolOnly || 
               statusReg_ == GoodStxStatus2RightPolOnly);
       break;
       
   case POL_BOTH:
       return (statusReg_ == GoodStxStatusBothPols);
       break;
       
   default:
       // TBD error reporting
       return false;
       break;
   }

   // shouldn't get here
   Assert(0);

}


const char* Stx::status()
{

  stringstream msg;

  if (simulated())
  {

    msg << "STX status: " << endl
	<< "STX filename = "  << filename() << endl
	<< "STX simulated = " << simulated() << endl
#ifdef HAVE_STX
	<< "HAVE_STX = yes" << endl
#else
	<< "HAVE_STX = no " << endl
#endif
	<< "Histogram Length = " << histogramLength() << endl
	<< "Max Tries = " << maxTries() << endl
	<< "STX Pol = " << SseMsg::polarizationToString(pol_) << endl
	<< "Status Register = " << statusReg_ << endl

	<< "GoodStatus = " << haveGoodRegisterStatus() << endl 

	<< "LCP count = 0"  << endl
	<< "LCP mean = 0.0" << endl
	<< "LCP variance = " << lcpVariance_  << endl
	<< "RCP count = 0" << endl
	<< "RCP mean = 0.0" << endl
	<< "RCP variance = " << rcpVariance_ << endl

	<< endl;
    return(sendToSocket(msg.str()));
  }


#ifdef HAVE_STX

  msg << "STX status: "  << endl
      << "STX filename = "  << filename() << endl
      << "STX simulated = " << simulated() << endl
      << "HAVE_STX = " << "yes" << endl
      << "Histogram Length = " << histogramLength() << endl
      << "Max Tries = " << maxTries() << endl
      << "STX Pol = " << SseMsg::polarizationToString(pol_) << endl;

  int          stxfd(0);

  STXioctrl stxioctrl;
  stxioctrl.host_addr = &statusReg_;
  stxioctrl.stx_addr  = 0x0; // status register offset
  stxioctrl.count     = sizeof(statusReg_);

  stxfd = open(filename_.c_str(), O_RDWR);

  if (stxfd == -1) {
    stringstream errmsg;
    errmsg << "Error Stx::status() unable to open stx file: " 
	   << filename() << endl;
    return(sendToSocket(errmsg.str()));
  }

  // ----- read status register -----

  if (ioctl(stxfd, STXIO_RD_STATUS, &stxioctrl)) {
    stringstream errmsg;
    errmsg << "Error Stx::status() read status register failed: " 
	   << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));
  }

  msg << "Status Register = " << statusReg_ << endl;

  msg << "GoodStatus = " << haveGoodRegisterStatus() << endl;


  // ----- variance calculation -----

  // all histograms are 8 bit data = 0x100 bins x 2 pols
  const    int  datasz(0x200);
  unsigned int* data = new u_int32_t [datasz];

  // ----- init histogram -----

  if (ioctl(stxfd, STXIO_HIST_INIT, &stxioctrl)) {
    stringstream errmsg;
    errmsg << "Error Stx::status() histogram init failed: " 
	   << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));
  }

  // ----- set length -----

  unsigned int length(histogramLength());
  stxioctrl.host_addr = &length;
  stxioctrl.count     = sizeof(length);

  if (ioctl(stxfd, STXIO_HIST_LENGTH, &stxioctrl)) {
    stringstream errmsg;
    errmsg << "Error Stx::status() histogram length failed: " 
	   << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));
  }

  // ----- start the histogram -----
  if (ioctl(stxfd, STXIO_HIST_START, &stxioctrl)) {
    stringstream errmsg;
    errmsg << "Error Stx::status() histogram start failed: " 
	   << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));
  }

  // ----- check status if ready -----

  stxioctrl.host_addr = &statusReg_;
  stxioctrl.count     = sizeof(statusReg_);

  // WARNING: assumes 0x0000a000 is histogram ready bits

  for (unsigned int i = 0; i < maxTries(); i++)
  {
    ioctl(stxfd, STXIO_RD_STATUS, &stxioctrl);

    if (statusReg_ & 0x0000a000) break;

    if (i >= maxTries())
    {
      stringstream errmsg;
      errmsg << "Error Stx::status() max tries (" << maxTries()
	     << ") exceeded: "
	     << filename() << endl;
      close(stxfd);
      return(sendToSocket(errmsg.str()));
    }

    // give the histogram a chance to run
    sleep(1);

  }

  // ----- stop histogram -----

  if (ioctl(stxfd, STXIO_HIST_STOP, &stxioctrl)) {
    stringstream errmsg;
    errmsg << "Error Stx::status() histogram stop failed: " 
	   << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));
  }


  // ----- read array -----

  int ret = read(stxfd, data, datasz*sizeof(data[0]));

  if (ret == -1) {
    stringstream errmsg;
    errmsg << "Error Stx::status() read histogram array failed: " 
	   << filename() << endl;
    close(stxfd);
    return(sendToSocket(errmsg.str()));

  } else {

    // ----- calculate variance -----

    int lcpData[datasz/2];
    int rcpData[datasz/2];

    // ----- sort into lcp/rcp data -----
    for (int i = 0, j = datasz/2; i < datasz/2; i++, j++) {
      lcpData[i] = data[i];
      rcpData[i] = data[j];
    }


   // ----- wrap 128 -----

    int lcpDataWrap128[datasz/2];
    int rcpDataWrap128[datasz/2];

    for (int i = datasz/4, j = 0; i < datasz/2; i++, j++) {
      lcpDataWrap128[j] = lcpData[i];
      rcpDataWrap128[j] = rcpData[i];
    }

    for (int i = 0, j = datasz/4; i < datasz/4; i++, j++) {
      lcpDataWrap128[j] = lcpData[i];
      rcpDataWrap128[j] = rcpData[i];
    }

    // ---- statistics -----

    long lcpSumW(0);
    long rcpSumW(0);

    long lcpSumWX(0);
    long rcpSumWX(0);

    long lcpSumWX2(0);
    long rcpSumWX2(0);

    for (int i = 0; i < datasz/2; i++) {

      lcpSumW   += lcpDataWrap128[i];
      rcpSumW   += rcpDataWrap128[i];

      lcpSumWX  += lcpDataWrap128[i] * (i - datasz/4);
      rcpSumWX  += rcpDataWrap128[i] * (i - datasz/4);

      lcpSumWX2 += lcpDataWrap128[i] * (i - datasz/4) * (i - datasz/4);
      rcpSumWX2 += rcpDataWrap128[i] * (i - datasz/4) * (i - datasz/4);

    }

    double lcpAve(0);
    double rcpAve(0);

    if (lcpSumW > 0) {
	lcpAve = double(lcpSumWX)/double(lcpSumW);
    }
    if (rcpSumW > 0) {
	rcpAve = double(rcpSumWX)/double(rcpSumW);
    }

    double lcpDiffMeanSum(0);
    double rcpDiffMeanSum(0);
    
    double lcpMeanX(0);
    double rcpMeanX(0);
    
    int index(0);
    
    for (int i = 0; i < datasz/2; i++) {
	
        index = i - datasz/4; // i is unsigned here?
	
        lcpMeanX = index - lcpAve;
        rcpMeanX = index - rcpAve;
	
        lcpDiffMeanSum += double(lcpDataWrap128[i]) * lcpMeanX * lcpMeanX;
        rcpDiffMeanSum += double(rcpDataWrap128[i]) * rcpMeanX * rcpMeanX;
	
    }

    if (lcpSumW > 0) {
	lcpVariance_ = lcpDiffMeanSum / double(lcpSumW);
    }
    if (rcpSumW > 0) {
	rcpVariance_ = rcpDiffMeanSum / double(rcpSumW);
    }

    msg << "LCP count = "    << lcpSumW      << endl
	<< "LCP mean = "     << lcpAve       << endl
	<< "LCP variance = " << lcpVariance_ << endl
	
	<< "RCP count = "    << rcpSumW      << endl
	<< "RCP mean = "     << rcpAve       << endl
	<< "RCP variance = " << rcpVariance_ << endl
	<< endl;
  }

  close(stxfd);

  return(sendToSocket(msg.str()));

#else

  msg << "Error Stx::status() "
      << "is not implemented for this architecture." 
      << endl;
  return(sendToSocket(msg.str()));

#endif

}



#if 0
Stx::Stx()
{
}

Stx::~Stx()
{
}
#endif