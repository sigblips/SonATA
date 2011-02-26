/*******************************************************************************

 File:    State.cpp
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

//
// Channelizer state class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/State.cpp,v 1.10 2009/06/16 15:14:18 kes Exp $
//
#include <unistd.h>
#include "Args.h"
#include "Dfb.h"
#include "ChErr.h"
#include "ChTypes.h"
#include "State.h"
#include "System.h"
#include "Util.h"

using ssechan::Status;
using namespace sonata_lib;
using namespace dfb;
using sonata_lib::Log;

namespace chan {

State *State::instance = 0;

State *
State::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new State();
	l.unlock();
	return (instance);
}

State::State(): err(0), sLock("stateLock"), beam(0)
{
	init();
}

State::~State()
{
}

/**
 * Initialize the system state.
 *
 * Description:\n
 * 	Initializes the system state, initializing internal variables based
 * 	on startup parameters
 */
void
State::init()
{
	Args *args = Args::getInstance();
	Assert(args);

	beam = Beam::getInstance();
	Assert(beam);

	// set the intrinsics
	strcpy(intrinsics.interfaceVersion, SSE_CHANNELIZER_INTERFACE_VERSION);
	strcpy(intrinsics.name, args->getName().c_str());
	gethostname(intrinsics.host, sizeof(intrinsics.host) - 1);
//	string s = std::string(CHVERSION);
	strcpy(intrinsics.codeVersion, "");

	strcpy(intrinsics.beamBase.addr, args->getInputAddr());
	intrinsics.beamBase.port = args->getInputPort();
	intrinsics.beamId = args->getBeamSrc();
#ifdef notdef
	switch (args->getPol()) {
	case ATADataPacketHeader::RCIRC:
		intrinsics.pol = POL_RIGHTCIRCULAR;
		break;
	case ATADataPacketHeader::LCIRC:
		intrinsics.pol = POL_LEFTCIRCULAR;
		break;
	case ATADataPacketHeader::XLINEAR:
		intrinsics.pol = POL_XLINEAR;
		break;
	case ATADataPacketHeader::YLINEAR:
		intrinsics.pol = POL_YLINEAR;
		break;
	default:
		intrinsics.pol = POL_UNINIT;
		break;
	}
#else
	intrinsics.pol = getPol((ATADataPacketHeader::PolarizationCode)
			args->getPol());
#endif

	strcpy(intrinsics.channelBase.addr, args->getOutputAddr());
	intrinsics.channelBase.port = args->getOutputPort();
	intrinsics.totalChannels = args->getTotalChannels();
	intrinsics.outputChannels = args->getUsableChannels();
	intrinsics.mhzPerChannel = args->getBandwidth() / args->getTotalChannels();

	intrinsics.foldings = args->getFoldings();
	intrinsics.oversampling = args->getOversampling();
	char tmp[MAX_TEXT_STRING];
	strcpy(tmp, args->getFilterFile().c_str());
	strcpy(intrinsics.filterName, basename(tmp));
}

#ifdef notdef
/**
 * Configure the channelizer.
 *
 * Description:\n
 * 	Configure the channelizer for operation.  This involves using
 * 	the configuration information from the SSE to compute the number
 * 	of subchannels and the
 * 	width of each subchannel.  Information provided on the command line
 * 	allows computation of the bin widths and frame times.\n\n
 */
void
State::configure(ChannelizerConfiguration *configuration_)
{
	lock();
	configuration = *configuration_;
	unlock();
}

const ChannelizerConfiguration&
State::getConfiguration()
{
	return (configuration);
}
#endif

/**
 * Get the system intrinsics.
 *
 * Description:\n
 * 	Returns the system intrinsics.
 */
const Intrinsics&
State::getIntrinsics()
{
	return (intrinsics);
}

Status
State::getStatus()
{
	lock();
	Status status;

	GetNssDate(status.timestamp);
	status.state = getState();
	status.startTime = beam->getStartTime();
	status.centerSkyFreqMhz = beam->getFreq();
	unlock();
	return (status);
}

/**
 * Functions to convert between ATADataPacketHeader and SSE versions
 * 	of polarization.
 */
/**
 * Convert a PolarizationCode to a Polarization.
 *
 */
Polarization
State::getPol(ATADataPacketHeader::PolarizationCode polCode)
{
	Polarization pol = POL_UNINIT;

	switch (polCode) {
	case ATADataPacketHeader::RCIRC:
		pol = POL_RIGHTCIRCULAR;
		break;
	case ATADataPacketHeader::XLINEAR:
		pol = POL_XLINEAR;
		break;
	case ATADataPacketHeader::LCIRC:
		pol = POL_LEFTCIRCULAR;
		break;
	case ATADataPacketHeader::YLINEAR:
		pol = POL_YLINEAR;
		break;
	default:
		pol = POL_UNINIT;
		break;
	}
	return (pol);
}

#ifdef notdef
/**
 * Convert a Polarization to a PolarizationCode.
 */

ATADataPacketHeader::PolarizationCode
State::getPolCode(Polarization pol)
{
	ATADataPacketHeader::PolarizationCode polCode =
			(ATADataPacketHeader::PolarizationCode)
			ATADataPacketHeader::UNDEFINED;
	switch (pol) {
	case POL_RIGHTCIRCULAR:
		polCode = ATADataPacketHeader::RCIRC;
		break;
	case POL_LEFTCIRCULAR:
		polCode = ATADataPacketHeader::LCIRC;
		break;
	default:
		break;
	}
	return (polCode);
}
#endif

}