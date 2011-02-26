/*******************************************************************************

 File:    Statistics.cpp
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

/**
 * Statistics.cpp
*/
#include "sseChannelizerInterfaceLib.h"

namespace ssechan {

NetStatistics::NetStatistics()
{
	total = wrong = invalid = missed = late = 0;
}

void
NetStatistics::marshall()
{
	HTOND(total);
	HTOND(wrong);
	HTOND(invalid);
	HTOND(missed);
	HTOND(late);
}

void
NetStatistics::demarshall()
{
	marshall();
}

ostream& operator << (ostream& s, const NetStatistics& ns)
{
   s << "total: " << ns.total << ", wrong: " << ns.wrong
		   << ", invalid: " << ns.invalid << ", missed: " << ns.missed
		   << ", late: " << ns.late << endl;
   return (s);
}

SampleStatistics::SampleStatistics()
{
	samples = 0;
	sumSq = 0;
	min = max = sum = cfloat64(0, 0);
}

void
marshall_cfloat64(cfloat64& val)
{
	val = cfloat64(HTOND(val.real()), HTOND(val.imag()));
}

void
SampleStatistics::marshall()
{
	HTOND(samples);
	HTOND(sumSq);
	marshall_cfloat64(min);
	marshall_cfloat64(max);
	marshall_cfloat64(sum);
}

void
SampleStatistics::demarshall()
{
	marshall();
}

ostream& operator << (ostream& s, const SampleStatistics& ss)
{
   s << "samples: " << ss.samples << ", sumSq: " << ss.sumSq << endl
		   << "min: " << ss.min << ", max: " << ss.max
		   << ", sum: " << ss.sum << endl;
    return (s);
}

BeamStatistics::BeamStatistics()
{
}

void
BeamStatistics::marshall()
{
	netStats.marshall();
	inputStats.marshall();
	outputStats.marshall();
}

void
BeamStatistics::demarshall()
{
	marshall();
}

ostream& operator << (ostream& s, const BeamStatistics& bs)
{
   s << "Beam statistics:" << endl
		   << "Net statistics:" << endl << bs.netStats
		   << "Input statistics:" << endl << bs.inputStats
		   << "Output statistics" << endl << bs.outputStats << endl;
    return (s);
}

ChannelizerStatisticsHeader::ChannelizerStatisticsHeader(): numberOfChannels(0),
		pad0(0)
{
}

void
ChannelizerStatisticsHeader::marshall()
{
	beam.marshall();
	channels.marshall();
	HTONL(numberOfChannels);
	HTONL(pad0);
}

void
ChannelizerStatisticsHeader::demarshall()
{
	marshall();
}

ostream& operator << (ostream& s, const ChannelizerStatisticsHeader& h)
{
   s << "Channelizer statistics header:" << endl
		   << "Beam statistics:" << endl << h.beam << endl
		   << "Channel statistics:" << endl << h.channels << endl
		   << "Number of channels: " << h.numberOfChannels
		   << "pad " << h.pad0 << endl;
    return (s);
}

ChannelizerStatistics::ChannelizerStatistics()
{
}

void
ChannelizerStatistics::marshall()
{
	SampleStatistics *sample = (SampleStatistics *) (&header + 1);
	for (int i = 0; i < header.numberOfChannels; ++i)
		sample[i].marshall();
	header.marshall();
}

void
ChannelizerStatistics::demarshall()
{
	header.marshall();
	SampleStatistics *sample = (SampleStatistics *) (&header + 1);
	for (int i = 0; i < header.numberOfChannels; ++i)
		sample[i].demarshall();
}

ostream& operator << (ostream& s, const ChannelizerStatistics& cs)
{
   s << "Channelizer statistics:" << endl
		   << cs.header << endl;
   SampleStatistics *sample = (SampleStatistics *) (&cs.header + 1);
   for (int i = 0; i < cs.header.numberOfChannels; ++i)
	   s << sample[i];
    return (s);
}

}

