/*******************************************************************************

 File:    packetgen.cpp
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

/*++ packetgen.cpp  Generate packets to multicast or file.
 *
 * Description:\n
 *		
 * packetgen is a testing utility that generates packets in
 * beamformer format or channelizer format. The packet are
 * either broadcast via multicast protocol or written to a file.
*/
#include <fenv.h>
#include <netdb.h>
#include "alarm.h"
#include "packetgen.h"

//using namespace sonata_lib;
//using namespace gauss;

const string version = "1.23";
const char *pgm = "packetgen";
const string beamAddr = "226.1.50.1";
const uint32_t beamPort = 50000;
const string channelAddr = "227.1.1.1";
const uint32_t channelPort = 51000;

string packetFile;
string mcAddr = channelAddr;
uint32_t mcIp;
uint32_t mcPort = channelPort;
uint32_t mcPortX = channelPort;
uint32_t mcPortY = channelPort;
int alarmInterval = 0;				// usecs
int packetBurst = 100;
uint64_t nPackets = 10000;
uint64_t packet = 0;
uint8_t polarization = ATADataPacketHeader::XLINEAR;

bool lazy = false;
bool local = false;
bool beam = false;
bool singlePol = false;
bool xPol = true;
bool swapri = false;
bool invert = false;
int32_t seedX = 1;
int32_t seedY = 7;
uint32_t data_valid = 0;
float32_t usable = 0.75;
float64_t bandwidth = DEFAULT_CHAN_BANDWIDTH;
float64_t cfreq = (float64_t)ATADataPacketHeader::DEFAULT_CENTER_FREQUENCY;
float64_t power = 1.0;
float64_t freq = 0.0;
float64_t drift = 0.0;
float64_t snr = 0.0;
uint8_t sigPol =  ATADataPacketHeader::BOTH;
float64_t pulseStartTime = 0.0;
float64_t pulseTimeOn = 1.0;
float64_t pulseTimeOff = 0.0;

Alarm frameAlarm;
ofstream fout;
ATAPacket *pkt;
ATAPacket *xPkt, *yPkt;
int sequence = 0;
int channel = 0;
int source = ATADataPacketHeader::CHAN_400KHZ;
MCSend *sndX = 0;
MCSend *sndY = 0;
timeval start;

Gaussian genX;
Gaussian genY;
PacketSigList signals;

int
main(int argc, char **argv)
{
	parseArgs(argc, argv);

   if (beam)
      channel = 0;
	sndX = new MCSend(channel, mcPortX+channel, mcAddr);
	sndY = new MCSend(channel, mcPortY+channel, mcAddr);

	/**
	* initialize the sample generators
	*/
	genX.setup(seedX, bandwidth, power);
	genY.setup(seedY, bandwidth, power);
	/**
	* if we're writing to output, open the file
	*/
	if (packetFile.length())
		openFile(packetFile);

	if (beam) {
		pkt = new BeamPacket(source, channel);
		xPkt = new BeamPacket(source, channel);
		yPkt = new BeamPacket(source, channel);
		ATADataPacketHeader hdr = pkt->getHeader();
		complex<float> val(1, 1);
		for (uint32_t i = 0; i < hdr.len; ++i) {
			pkt->putSample(i, val);
			xPkt->putSample(i, val);
			yPkt->putSample(i, val);
		}
		/**
		* send packets with no valid bit to reset Channelizer
		*/
		createPackets(genX, genY, *xPkt, *yPkt);
		sendPackets(*xPkt, *yPkt);
	}
	else {
		pkt = new ChannelPacket(source, channel);
		xPkt = new ChannelPacket(source, channel);
		yPkt = new ChannelPacket(source, channel);
		ATADataPacketHeader hdr = pkt->getHeader();
		complex<float> val(1, 1);
		for (uint32_t i = 0; i < hdr.len; ++i) {
			pkt->putSample(i, val);
			xPkt->putSample(i, val);
			yPkt->putSample(i, val);
		}
	}

	//
	// set the valid flag for all remaining packets
	//
	data_valid = ATADataPacketHeader::DATA_VALID;

	//
	// add signals
	//
	for (PacketSigList::iterator p = signals.begin(); p != signals.end(); ++p) {
		PacketSig sig = *p;
		if (sig.tOff == 0.0)
		{
			if (sig.pol == ATADataPacketHeader::XLINEAR ||
					sig.pol ==  ATADataPacketHeader::BOTH )
						genX.addCwSignal(sig.freq, sig.drift, sig.snr);
			if (sig.pol == ATADataPacketHeader::YLINEAR ||
					sig.pol ==  ATADataPacketHeader::BOTH )
						genY.addCwSignal(sig.freq, sig.drift, sig.snr);
		}
		else
		{
			if (sig.pol == ATADataPacketHeader::XLINEAR ||
					sig.pol ==  ATADataPacketHeader::BOTH )
						genX.addPulseSignal(sig.freq, sig.drift, sig.snr,
							sig.tStart, sig.tOn, sig.tOff);
			if (sig.pol == ATADataPacketHeader::YLINEAR ||
					sig.pol ==  ATADataPacketHeader::BOTH )
						genY.addPulseSignal(sig.freq, sig.drift, sig.snr,
							sig.tStart, sig.tOn, sig.tOff);
		}
	}

	gettimeofday(&start, NULL);
	//
	// now create and send packets at an interval or
	// 	continuously
	//
	if (alarmInterval)
		frameAlarm.set(&alarmHandler, alarmInterval, alarmInterval);
	else
		fullThrottle();

	while (1)
		;
}

//---------------------------------------------------------------------
// command line argument handling
//---------------------------------------------------------------------
/**
 * Prints command line options for help or an invalid option
 * Defaults to Channel packets, both Polarizations (X and Y)
 *
 * Command Line Options:\n
 *
 *	packetgen 	[-# seedX] 					Seed for X/LCP random noise generator (1)\n
 *					[-% seedY] 					Seed for Y/RCP random noise Generator (7)\n
 *					[-a ipAddr] 				Multicast broadcast address (227.0.0.1)\n
 *					[-p port] 					Multicast port (51000)\n
 *					[-x] 							Single polarization output X/LCP (false)\n
 *					[-y] 							Single polarization output Y/RCP (false)\n
 *					[-I]						invert the signal by swapping real and imaginary\n
 *					[-f packetFilename]		Output packet filename\n
 *					[-c channel] 				Channel Number (0)\n
 *					[-s source] 				Source: Beam=0,Chan=1 (1)\n
 *					[-C ] 						Circular Polarization (false)\n
 *					[-b] 							Beam Packets (default channel packets)\n
 *					[-l]							Lazy Mode, Do not marshall data (false)\n
 *					[-n numberOfPackets] 	Total packets to be sent (10000)\n
 *					[-i wakeupInterval]		Time interval between sending packets,\n
 *													microseconds (0)\n
 *					[-P noise power] 			Mean noise power (1)\n
 *					[-R Center Frequency]	Center Frequency of bandwidth (1420)\n
 *					[-B bandwidth]				Bandwidth of Channel MHz (0.5461333333)\n
 *					Signal Description options\n
 *					[-X]						 	Signal in X/LCP pol only\n
 *					[-Y]							Signal in Y/RCP pol only\n
 *					[-Z]							Signal in both pols\n
 *					[-t pulse start time]	Time of first pulse, seconds (0.0)\n
 *					[-O pulse time ON]		Pulse ON time, seconds (1.0)\n
 *					[-o pulse time OFF]		Pulse Off time, seconds (0.0)\n
 *													0=>CW signal \n
 *					[-S snr] 					Signal to noise ratio, sigma (0.0)\n
 *					[-D drift] 					Drift rate, Hz/sec (0.0)\n
 *					[-F freq]					Signal Baseband Frequency (0.0)\n
 *													Initiates storing of Signal parameters\n
 */

void
usage()
{
	OUTL("packetgen [-# seedX] [-% seedY] [-I] [-a ipAddr] [-p port] [-x] [-y] [-f packetFilename]\n"
			"			[-c channel] [-s source] [-C ] [-b] [-l]\n "
			"			[-n numberOfPackets] [-i wakeupInterval]\n "
			"			[-P noise power] [-B bandwidth]\n "
			"			[-U usable fraction of bandwidth]\n "
			"			[-X signal in X pol only]\n"
			"			[-Y signal in Y pol only]\n"
			"			[-Z signal in both pols]\n"
			"			[-O pulse time ON]\n"
			"			[-o pulse time OFF]\n"
			"			[-R Center Frequency]\n"
			"			{[-S snr] [-D drift] [-F freq]}");
	exit(-1);
}

/**
 * Parse the command line arguments
 *
 */
void
parseArgs(int argc, char **argv)
{
	const char *optstring = "blLxyVCXYZI%:#:f:i:n:a:o:p:t:s:c:R:B:P:F:O:S:D:U:?h:"; // arguments
	extern char *optarg;			// getopt argument value return string
	extern int	optind;				// current argv index
	bool done = false;				// getopt option parsing state
	extern int optopt;
	extern int opterr;
	opterr = 0;
	while (!done)
	{
		switch (getopt(argc,argv,optstring)) {
		case -1:
			done = true;
			break;
		case 'V': 	// Program Version
			cout << "Version " << version << endl;
			break;
		case 'a':	// Multicast Address
			mcAddr = string(optarg);
			break;
		case 'p':	// Multicast Port
			mcPortX = atoi(optarg);
			mcPortY = atoi(optarg);
			if ( beam ) mcPortY++;
			break;
		case '#':	// Seed for Random Number Generator for X pol
			seedX = atoi(optarg);
			break;
		case '%':	// Seed for Random Number Generator for Y pol
			seedY= atoi(optarg);
			break;
		case 'x':	// Single pol output, x only
			singlePol = true;
			xPol = true;
			break;
		case 'y':	// Single pol output, y only
			singlePol = true;
			xPol = false;
			break;
		case 'f':	// Output filename
			packetFile = optarg;
			break;
		case 'i':	// Alarm Interval
			alarmInterval = atoi(optarg);
			break;
		case 'n':	// Total Number of Packets to generate
			nPackets = (uint64_t) atof(optarg);
			break;
		case 'b':	// Beam Mode
			beam = true;
			swapri = true;
			bandwidth = DEFAULT_BEAM_BANDWIDTH;
			usable = .6875;
			source = ATADataPacketHeader::BEAM_104MHZ;
			mcAddr = beamAddr;
			mcPortX = beamPort;
			mcPortY = beamPort+1;
//			channel = 0;
			break;
		case 'I':
			invert = true;
			break;
		case 'l':	// Lazy mode -- no marshalling
			lazy = true;
			break;
		case 'L':
			local = true;
			break;
		case 'C':	// Circular Polarization
			polarization = ATADataPacketHeader::RCIRC;
			break;
		case 'c':	// Channel number (in Channel Mode)
			channel = atoi(optarg);
			break;
		case 's':	// Type of Packet (channel=1, Beam=0)
			source = atoi(optarg);
			break;
		case 'R':	// Center Frequency (MHz)
			cfreq = atof(optarg);
			break;
		case 'B':	// Bandwidth (MHz); actually sampling rate
			bandwidth = atof(optarg);
			break;
		case 'U':	// Fraction of bandwidth which is usable
			usable = (float32_t) atof(optarg);
			if (usable <= 0 || usable > 1.0)
				USAGE("Usable fraction must be > 0 and <= 1");
			break;
		case 'P':		// Signal Power (sigma)
			power = atof(optarg);
			break;
		case 'F':		// Signal Frequency (MHz)
			freq = atof(optarg);
			if (freq >= bandwidth / 2 || freq < -bandwidth / 2)
				USAGE("Freq must be >= -bw/2 and < bw/2");
			if (snr || drift) {
				if ( pulseTimeOff == 0.0)
				{
				PacketSig sig(CwSignal, freq, drift, snr, 
					pulseStartTime, pulseTimeOn, pulseTimeOff, sigPol);
				signals.push_back(sig);
				}
				else
				{
				PacketSig sig(PulseSignal, freq, drift, snr, 
					pulseStartTime, pulseTimeOn, pulseTimeOff, sigPol);
				signals.push_back(sig);
				}
			}
			break;
		case 'D': 	// Signal Drift
			drift = atof(optarg);
			break;
		case 'S':	// Signal to noise Ratio of Signal
			snr = atof(optarg);
			break;
		case 't':	// Pulse Start time
			pulseStartTime = atof(optarg);
			break;
		case 'O':	// Pulse time On
			pulseTimeOn = atof(optarg);
			break;
		case 'o':	// Pulse Time Off
			pulseTimeOff = atof(optarg);
			break;
		case 'X':	// Signal in X pol only
			sigPol = ATADataPacketHeader::XLINEAR;
			break;
		case 'Y':	// Signal in Y pol only
			sigPol = ATADataPacketHeader::YLINEAR;
			break;
		case 'Z':	// Signal in Both Polarizations
			sigPol = ATADataPacketHeader::BOTH;
			break;
		case 'h':	// Help
		case '?':
			usage();
			break;
		default:
			USAGE("Invalid option: " << optopt);
		}
	}

	if (optind < argc)
		USAGE("Too many arguments");
	if (invert)
		swapri = !swapri;
}

/**
* Open the output file
*/
void
openFile(const string& file)
{
	fout.open(file.c_str(), ios::binary);
}

/**
* Create LCP and RCP packets with different noise and signals
*
*/
void
createPackets(Gaussian &xGen, Gaussian &yGen, 
			ATAPacket &xPkt, ATAPacket &yPkt)
{
	//
	// we're reusing the packets, so make sure they're in the right order
	//
	xPkt.putPacket(pkt->getPacket());
	yPkt.putPacket(pkt->getPacket());

	ATADataPacketHeader& hdr = xPkt.getHeader();

	//
	// the following converts the timeval unix time into a 64-bit sec.frac
	// format; this requires converting tv_usec to a 32-bit fraction
	// of the form usec / 1e6, then conve[Brting it back to a 32-bit unsigned
	// integer by multiplying by 2^32.
	//
	timeval tp;
	gettimeofday(&tp, NULL);
	hdr.absTime = ATADataPacketHeader::timevalToAbsTime(tp);

	hdr.chan = channel;
	hdr.freq = cfreq;
//	hdr.sampleRate = bandwidth;
//	hdr.usableFraction = usable;
	hdr.seq = sequence++;
	hdr.flags |= data_valid;
	static ComplexFloat64 *samples = 0;
//   		Copy the whole packet to get the header
	memcpy(&yPkt, &xPkt, xPkt.getSize());  
//		Now fill in the samples
	if (!samples)
		samples = new ComplexFloat64[hdr.len];
	if (!local) {
//
// 	Only create samples for the pols to be sent
//
		if ( !singlePol || (singlePol && xPol))
		{
			xGen.getSamples(samples, hdr.len);
		//
		// store the samples in the  X packet
		//
			for (uint32_t i = 0; i < hdr.len; ++i) {
				ComplexFloat32 s(samples[i]);
				if (swapri)
					s = ComplexFloat32(s.imag(), s.real());
				xPkt.putSample(i, s);
			}
		}
		if ( !singlePol || (singlePol && !xPol))
		{
			yGen.getSamples(samples, hdr.len);
		//
		// store the samples in the Y packet 
		//
			for (uint32_t i = 0; i < hdr.len; ++i) {
				ComplexFloat32 s(samples[i]);
				if (swapri)
					s = ComplexFloat32(s.imag(), s.real());
				yPkt.putSample(i, s);
			}
		}
	}
	switch (polarization) {
	case ATADataPacketHeader::XLINEAR:
		xPkt.getHeader().polCode = ATADataPacketHeader::XLINEAR;
		yPkt.getHeader().polCode = ATADataPacketHeader::YLINEAR;
		break;
	case ATADataPacketHeader::RCIRC:
	default:
		xPkt.getHeader().polCode = ATADataPacketHeader::RCIRC;
		yPkt.getHeader().polCode = ATADataPacketHeader::LCIRC;
	}
}

/**
* sendPackets() Send a pair of packets.
*
* Description:\n
*	Sends the pair of packets to either the multicast socket or the\n
*	specified output file.\n
* Notes:\n
*	If a single polarization has been specified, X only is sent.\n
*	Packets are marshalled only when being transmitted on the\n
*	network; packets stored in a file are left in natural order.\n
*/
void
sendPackets(ATAPacket& xPkt, ATAPacket& yPkt)
{
	// marshall into network order
	if (lazy) {
		xPkt.marshall(ATADataPacketHeader::LAZY);
		yPkt.marshall(ATADataPacketHeader::LAZY);
	}
	else {
		xPkt.marshall(ATADataPacketHeader::FORCE_BIG_ENDIAN);
		yPkt.marshall(ATADataPacketHeader::FORCE_BIG_ENDIAN);
	}
	if (fout.is_open()) {
		if (!singlePol) {
			fout.write((char *) xPkt.getPacket(), xPkt.getPacketSize());
			fout.write((char *) yPkt.getPacket(), yPkt.getPacketSize());
		}
		else if (xPol)
			fout.write((char *) xPkt.getPacket(), xPkt.getPacketSize());
		else
			fout.write((char *) yPkt.getPacket(), yPkt.getPacketSize());
	}
	else {
		if (!singlePol) {
			sndX->send((const void *) xPkt.getPacket(), xPkt.getPacketSize());
			sndY->send((const void *) yPkt.getPacket(), yPkt.getPacketSize());
		}
		else if (xPol)
			sndX->send((const void *) xPkt.getPacket(), xPkt.getPacketSize());
		else
			sndY->send((const void *) yPkt.getPacket(), yPkt.getPacketSize());
	}
}

void
printSummary(ComplexInt8 *data, int n)
{
	float64_t power = 0, amplitude = 0;
	for (int i = 0; i < n; ++i) {
		power += std::norm(data[i]);
		amplitude += fabs(data[i].real()) + fabs(data[i].imag());
	}
	power /= n;
	amplitude /= 2 * n;
	cout << "power = " << power << ", amplitude = " << amplitude << endl;
}

void
fPrintSummary(ComplexFloat64 *data, int n)
{
	float64_t power = 0, amplitude = 0;
	for (int i = 0; i < n; ++i) {
		power += std::norm(data[i]);
		amplitude += fabs(data[i].real()) + fabs(data[i].imag());
	}
	power /= n;
	amplitude /= 2 * n;
	cout << "power = " << power << ", amplitude = " << amplitude << endl;
}

/**
 * Print Statistics for the generated Packets
 *
 * Description:\n
 * Prints Total number of packets sent, the elapsed time,
 * the mean power, mean real and imaginary values for the samples
 * for both LCP and RCP
 *
 */
void
printStatistics()
{
	timeval end;
	gettimeofday(&end, NULL);

	int32_t sec = end.tv_sec - start.tv_sec;
	int32_t usec = end.tv_usec - start.tv_usec;
	if (usec < 0) {
		usec += 1000000;
		--sec;
	}
	cout << packet << " packets sent in "
		<< sec << "." << usec << " seconds" << endl;
	int32_t maxQueue = sndX->getSendHWM();
	cout << "maximum output queue X " << maxQueue << endl;
	maxQueue = sndY->getSendHWM();
	cout << "maximum output queue Y " << maxQueue << endl;

	int64_t samples;
	float64_t total;
	ComplexFloat64 power;
	ComplexFloat64 fSum;
	ComplexInt64 iSum;
/**
*	 X pol Statistics
*/
	samples = genX.getSampleCnt();
	power = genX.getPower();
	total = power.real() + power.imag();
	cout << "X pol" << endl;
	cout << samples << " samples, average (float) sample power = ";
	cout << total / samples << endl;
	cout << "power (float): re = " << power.real() << ", im = " << power.imag();
	cout << ", total = " << total << endl;
#ifdef notdef
	power = genX.getIntegerPower();
	total = power.real() + power.imag();
	cout << samples << " samples, average (short) sample power = ";
	cout << total / samples << endl;
	cout << "power (integer): re = " << power.real() << ", im = ";
	cout << power.imag() << ", total = " << total << endl;
#endif
	fSum = genX.getSum();
	cout << "sum of amplitudes (float64) = (" << fSum.real();
	cout << ", " << fSum.imag() << ")" << endl;
	cout << "avg of amplitudes (float64) = (" << fSum.real() / samples;
	cout << ", " << fSum.imag() / samples << ")" << endl;
#ifdef notdef
	iSum = genX.getIntegerSum();
	cout << "sum of amplitudes (short) = (" << iSum.real();
	cout << ", " << iSum.imag() << ")" << endl;
	cout << "avg of amplitudes (short) = (";
	cout << (float64_t) iSum.real() / samples;
	cout << ", " << (float64_t) iSum.imag() / samples << ")" << endl;
#endif

/**
*	 Y pol Statistics
*/
	samples = genY.getSampleCnt();
	power = genY.getPower();
	total = power.real() + power.imag();
	cout << "Y pol" << endl;
	cout << samples << " samples, average (float) sample power = ";
	cout << total / samples << endl;
	cout << "power (float): re = " << power.real() << ", im = " << power.imag();
	cout << ", total = " << total << endl;
#ifdef notdef
	power = genY.getIntegerPower();
	total = power.real() + power.imag();
	cout << samples << " samples, average (short) sample power = ";
	cout << total / samples << endl;
	cout << "power (integer): re = " << power.real() << ", im = ";
	cout << power.imag() << ", total = " << total << endl;
#endif
	fSum = genY.getSum();
	cout << "sum of amplitudes (float64) = (" << fSum.real();
	cout << ", " << fSum.imag() << ")" << endl;
	cout << "avg of amplitudes (float64) = (" << fSum.real() / samples;
	cout << ", " << fSum.imag() / samples << ")" << endl;
#ifdef notdef
	iSum = genY.getIntegerSum();
	cout << "sum of amplitudes (short) = (" << iSum.real();
	cout << ", " << iSum.imag() << ")" << endl;
	cout << "avg of amplitudes (short) = (";
	cout << (float64_t) iSum.real() / samples;
	cout << ", " << (float64_t) iSum.imag() / samples << ")" << endl;
#endif
}

/**
* alarmHandler()  Create and send packets at a given interval.
*
* Description:\n
*	At each alarm, creates a pair of LCP/RCP gaussian noise packets\n
*	and sends them to the network or a file.\n
*/
void
alarmHandler(int signal)
{
	if (packet < nPackets) {
			*xPkt = *pkt;
			*yPkt = *pkt;
			createPackets(genX, genY, *xPkt, *yPkt);
			sendPackets(*xPkt, *yPkt);
			packet++;
	}
	else {
		printStatistics();
		fout.close();
		exit(0);
	}
}

/**
* fullThrottle() Create and send packets as fast as they can be generated.
*
* Description:\n
*	Creates noise packets with optional signal(s) injected, then sends\n
* LCP and RCP signals.\n
*/
void
fullThrottle()
{
	for (packet = 0; packet < nPackets; ++packet) {
		*xPkt = *pkt;
		*yPkt = *pkt;
		createPackets(genX, genY, *xPkt, *yPkt);
		sendPackets(*xPkt, *yPkt);
	}
	printStatistics();
	fout.close();
	exit(0);
}

/**
* MCSend()  Multicast send task
*/
MCSend::MCSend(int channel, uint32_t mcPort, string mcAddr )
	: sendHWM(0)
{
	sockFd = socket(PF_INET, SOCK_DGRAM, 0);
	ASSERT(sockFd >= 0);

	//
	// Pick an arbitrary port, and select one of the multicast
	// IP numbers reserved for local assignment.	The receiver
	// uses the same convention.
	//
	hostent *host = gethostbyname(mcAddr.c_str());
	mcIp = *((uint32_t *) host->h_addr);

	memset(&mcastAddr,0,sizeof(mcastAddr));
	mcastAddr.sin_family = AF_INET;
	mcastAddr.sin_port = htons(mcPort);
	memcpy(&mcastAddr.sin_addr,&mcIp,sizeof(mcIp));

}

void
MCSend::send(const void *buf, size_t size)
{
	int result = sendto(sockFd, buf, size, 0,
			(sockaddr *)&mcastAddr, sizeof(mcastAddr));
	ASSERT(result == (ssize_t) size);

	int value;
	result = ioctl(sockFd, SIOCOUTQ, &value);
	if (result == 0 && value > sendHWM)
		sendHWM = value;
}

