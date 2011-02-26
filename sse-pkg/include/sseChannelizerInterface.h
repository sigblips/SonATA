/*******************************************************************************

 File:    sseChannelizerInterface.h
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

/*
 * sseChannelizerInterface.h
 *
 *  Created on: Nov 17, 2009
 *      Authors: kes, tksonata
 */

#ifndef SSE_CHANNELIZER_INTERFACE_H
#define SSE_CHANNELIZER_INTERFACE_H

#include <complex>
#include <iostream>
#include <iosfwd>
#include <machine-dependent.h>
#include <HostNetByteorder.h>
#include <sseInterface.h>
//#include "sseSigStruct.h"

using std::cout;
using std::endl;
using std::ostream;

namespace ssechan 
{
#define CHAN_STATS

   const char *const SSE_CHANNELIZER_INTERFACE_VERSION =
      "SSE-CHAN Interface Version 1.0.3 2010-Jan-14  0:34:13 UTC";

   typedef char8_t InterfaceVersion[MAX_TEXT_STRING];

   enum ChannelizerMessageCode
   {
      CHANNELIZER_MSG_CODE_UNINIT = CHANNELIZER_CODE_RANGE_START,
      REQUEST_INTRINSICS,
      SEND_INTRINSICS,
      REQUEST_STATUS,
      SEND_STATUS,
      START,
      STARTED,
      SEND_MESSAGE,
      STOP,
      SHUTDOWN,
      CHANNELIZER_MSG_CODE_END
   };

   enum ChannelizerState
   {
      STATE_IDLE,
      STATE_PENDING,
      STATE_RUNNING
   };

#ifdef notdef
/**
 * Channelizer beam statistics
 */
   struct BeamStatistics
   {
      NetStatistics netStats;				// network statistics
      SampleStatistics inputStats;		// input sample statistics (beam)
      SampleStatistics outputStats;		// output sample statistics (channels)

      BeamStatistics();
      void reset();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream& strm, const BeamStatistics& beamStats);
   };
#endif

/**
 * Multicast base address struct
 */
   struct BaseAddr 
   {
      IpAddress addr;	
      int32_t port;	

      BaseAddr();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream& strm, const BaseAddr& baseAddr);
   };

/**
 * Channelizer intrinsics
 */
   struct Intrinsics
   {
      InterfaceVersion interfaceVersion;
      char8_t name[MAX_TEXT_STRING];
      char8_t host[MAX_TEXT_STRING];
      char8_t codeVersion[MAX_TEXT_STRING]; 

      // input
      BaseAddr beamBase;	// beam (input) base address
      int32_t beamId;
      Polarization pol;

      // output
      BaseAddr channelBase;	// channel (output) base address
      int32_t totalChannels;		
      int32_t outputChannels;
      float64_t mhzPerChannel;  

      // Digital filter bank (DFB)
      int32_t foldings;
      float32_t oversampling;
      char8_t filterName[MAX_TEXT_STRING];

      // methods
      Intrinsics();
      int32_t getOutputChannels();
      float64_t getMhzPerChannel();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream &strm, const Intrinsics &intrinsics);
   };

   struct Status 
   {
      NssDate timestamp;
      NssDate startTime;
      float64_t centerSkyFreqMhz; 
      ChannelizerState state;
      int32_t alignPad;

      Status();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream &strm, const Status &status);
   };

   struct Start
   {
      NssDate startTime;
      float64_t centerSkyFreqMhz; 

      Start();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream &strm, const Start& start);
   };

   struct Started
   {
      NssDate startTime;

      Started();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream &strm, const Started& started);
   };

#ifdef CHAN_STATS

   struct NetStatistics
   {
	   uint64_t total;				// total number of packets
	   uint64_t invalid;			// packets with invalid data
	   uint64_t wrong;				// packets of wrong type
	   uint64_t missed;				// missing packets
	   uint64_t late;				// late packets

	   NetStatistics();
	   void marshall();
	   void demarshall();
	   friend ostream& operator << (ostream& strm, const NetStatistics& ns);
   };

   typedef std::complex<float64_t> cfloat64;

   struct SampleStatistics
   {
	   uint64_t samples;			// total # of samples
	   float64_t sumSq;				// sum of squares
	   cfloat64 min;				// minimum sample
	   cfloat64 max;				// maximum sample
	   cfloat64 sum;				// sum of samples

	   SampleStatistics();
	   void marshall();
	   void demarshall();
	   friend ostream& operator << (ostream& strm, const SampleStatistics& ss);
   };

   struct BeamStatistics
   {
	   NetStatistics netStats;		// network statistics
	   SampleStatistics inputStats;	// input sample statistics (beam)
	   SampleStatistics outputStats;// output sample statistics (channels)

	   BeamStatistics();
	   void marshall();
	   void demarshall();
	   friend ostream& operator << (ostream& strm, const BeamStatistics& bs);
   };

   struct ChannelizerStatisticsHeader
   {
      BeamStatistics beam;
      SampleStatistics channels;
      int32_t numberOfChannels;
      int32_t pad0;

      ChannelizerStatisticsHeader();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream& strm,
    		  const ChannelizerStatisticsHeader& hdr);
   };

   struct ChannelizerStatistics
   {
      ChannelizerStatisticsHeader header;

      ChannelizerStatistics();
      void marshall();
      void demarshall();
      friend ostream& operator << (ostream& strm,
    		  const ChannelizerStatistics& ChannelizerStatistics);
   };
// ChannelizerStatisticsHeader is followed by this array:
// SampleStatistics stats[numberOfChannels];

#endif

} // namespace


#endif /* SSE_CHANNELIZER_INTERFACE_H */