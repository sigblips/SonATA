/*******************************************************************************

 File:    packetsqimport.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "ATADataPacketHeader.h"

/*
 * 2010-08  Robert Ackermann
 *
 * program packetsqimport recombines split header and data files
 * from setiQuest observations into packets compatible with SonATA
 * channelizer input (64 byte headers with 4096 byte 8-bit data).
 *
 * there are a number of assumptions: little endian processor, single
 * pol. packets of 8-bit coefficients, ...  if setiQuest observations
 * advance to such things as dual-pol. and wider bandwidths the
 * associated utilities must become more sophisticated. 
 */

using namespace std;

extern char *optarg;
extern int optind, opterr;

// hard-coded for 64-byte header
static struct sqextendedhdrfmt {
  char header[64];
  char filler[8];
  uint64_t offset;
} sqextendedhdr;

static struct sqdatafmt {
  signed char coeffs[1024 * 2];
} sqdata;

static struct sonatapktfmt {
  ATADataPacketHeader hdr;
  signed char first_coeffs[1024 * 2];
  signed char second_coeffs[1024 * 2];
} sonatapkt;

uint32_t sequence_num = 1;

void recombine(ifstream& hdrstrm, ifstream& datastrm);
void usage();
void mylog(const char *msg);
void mylog(ostringstream& ossmsg);
 
int main(int argc, char **argv) {
  int c;
  static char optstring[] = "";  // no options for this version 

  // prepare for inevitable options
  while ((c = getopt(argc, argv, optstring)) != -1) {
    switch (c) {
    }
  }   

  // test for arguments header file and at least one data file
  if ((argc - optind) < 2) {
    usage();
    exit(EXIT_FAILURE);
  }

  ifstream hdrstrm, datastrm;

  ostringstream hdropenoss;
  hdropenoss << "opening header file \"" << argv[optind] << "\" ...";
  mylog(hdropenoss);
  // open setiQuest header file
  hdrstrm.open(argv[optind], ios::binary);
  if (!(hdrstrm)) {
    mylog("unable to open header file!");
    exit(EXIT_FAILURE);
  } 
  for (int datafi = optind + 1; datafi < argc; datafi++) {
    ostringstream dataopenoss;
    dataopenoss << "opening data file \"" << argv[datafi] << "\" ..."; 
    mylog(dataopenoss);
    // open first or next setiQuest data file
    datastrm.open(argv[datafi], ios::binary);
    if (!(datastrm)) {
      mylog("unable to open data file!");
      if (datafi > (optind + 1)) break;  // keep previously processed data 
      exit(EXIT_FAILURE);
    } 
    // merge setiQuest headers and data.  Write recombined SonATA
    // channelizer input compatible packets to stdout 
    recombine(hdrstrm, datastrm);
    mylog("closing data file");
    datastrm.close();
  }
  mylog("closing header file");
  hdrstrm.close();

  return (EXIT_SUCCESS);
}

void recombine(ifstream& hdrstrm, ifstream& datastrm) {
  ostringstream ossmsg;

  for (;;) {
    datastrm.read((char *) &sqdata, sizeof(sqdatafmt));  
    if (datastrm.gcount() != sizeof(sqdatafmt)) {
      if (datastrm.eof()) return;  // EOF should occur on this read 
      ossmsg << "(1) unexpected data file read count " <<
        dec << datastrm.gcount();
      mylog(ossmsg);
      exit(EXIT_FAILURE);
    }
    memcpy(&sonatapkt.first_coeffs, &sqdata.coeffs,
      sizeof(sonatapkt.first_coeffs));
    datastrm.read((char *) &sqdata, sizeof(sqdatafmt));  
    if (datastrm.gcount() != sizeof(sqdatafmt)) {
      ossmsg << "(2) unexpected data file read count " <<
        dec << datastrm.gcount();
      mylog(ossmsg);
      exit(EXIT_FAILURE);
    }
    memcpy(&sonatapkt.second_coeffs, &sqdata.coeffs,
      sizeof(sonatapkt.second_coeffs));
    hdrstrm.read((char *) &sqextendedhdr, sizeof(sqextendedhdrfmt)); 
    if (hdrstrm.gcount() != sizeof(sqextendedhdrfmt)) {
      ossmsg << "(1) unexpected header file read count " <<
        dec << hdrstrm.gcount();
      mylog(ossmsg);
      exit(EXIT_FAILURE);
    }
    memcpy(&sonatapkt.hdr, &sqextendedhdr.header, sizeof(sonatapkt.hdr));
    hdrstrm.read((char *) &sqextendedhdr, sizeof(sqextendedhdrfmt)); 
    // don't copy this header.  Every other header thrown out.
    if (hdrstrm.gcount() != sizeof(sqextendedhdrfmt)) {
      ossmsg << "(2) unexpected header file read count " <<
        dec << hdrstrm.gcount();
      mylog(ossmsg);
      exit(EXIT_FAILURE);
    }
    // sanity check for 0xaabbccdd endian order value
    if (sonatapkt.hdr.order != 0xaabbccdd) {
      mylog("output packet header does not contain 0xaabbccdd endian value");
      exit(EXIT_FAILURE);
    } 
    // fix some of the fields to make packets look like they came
    // from the beamformer
    sonatapkt.hdr.src = ATADataPacketHeader::BEAM_104MHZ;  // not really 
    sonatapkt.hdr.chan = 1;
    sonatapkt.hdr.seq = sequence_num++;
    sonatapkt.hdr.len = 2048;
    // write the SonATA channelizer compatible packet to stdout
    cout.write((char *) &sonatapkt, sizeof(sonatapktfmt)); 
  }
}

void usage() {
  cerr << "usage: packetsqimport [options] sqheaderfile sqdatafiles... ";
  cerr << "> sonatapacketfile" << endl;
}

// should be in a separate file or class
void mylog(const char *msg) {
  ostringstream ossmsg(msg);
  mylog(ossmsg);
}

// should be in a separate file or class
void mylog(ostringstream& ossmsg) {
  timeval tv;
  time_t current_seconds;
  tm bdt;

  gettimeofday(&tv, NULL);
  current_seconds = tv.tv_sec;
  // not thread safe (but no threads used in this utility)
  memcpy(&bdt, gmtime(&current_seconds), sizeof(tm)); 
  
  cerr << setfill('0');
  cerr << dec << setw(4) << (bdt.tm_year + 1900) << "-" << setw(2) <<
    (bdt.tm_mon + 1) << "-" << setw(2) << bdt.tm_mday << "T" <<
    setw(2) << bdt.tm_hour << ":" << setw(2) << bdt.tm_min << ":" <<
    setw(2) << bdt.tm_sec << "." << setw(3) << (tv.tv_usec / 1000) <<      
    "Z" << " | " << ossmsg.str() << endl;
}