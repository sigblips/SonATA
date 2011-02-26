/*******************************************************************************

 File:    ConfirmationChannel.cpp
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
// ConfirmationChannel: confirmation channel handling base class
//
// Notes:
//		The confirmation channel is assembled from several (nominally
//		16) adjacent subchannels.  The center subchannel is the
//		subchannel which contains the candidate signal at the middle of
//		the observation.
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ConfirmationChannel.cpp,v 1.6 2009/05/24 22:43:59 kes Exp $
//
#include "dedrift.h"
#include "System.h"
#include "ConfirmationChannel.h"
#include "Signal.h"
#include "TransformWidth.h"

namespace dx {

ConfirmationChannel::ConfirmationChannel(): subchannels(0), fftPlan(0)
{
}

ConfirmationChannel::~ConfirmationChannel()
{
	if (fftPlan)
		fftwf_destroy_plan(fftPlan);
}

/**
 * Compute confirmation channel parameters.
 *
 * Description:\n
 * 	Creates the FFT for synthesis of the channel from several subchannels.
 * 	Also computes the layout of the subchannel data in the CD buffer so
 * 	that data can be retrieved for assembly prior to synthesis.\n\n
 */
void
ConfirmationChannel::computeChannelParams(Activity *act_,
		SignalDescription *sig_, int32_t subchannels_)
{
	int32_t startSubchannel, centerSubchannel;

	// record the activity and signal
	activity = act_;
	sig = sig_;

	Channel *channel = activity->getChannel();

	// create new fft plans if necessary, after destroying the old ones
	if (subchannels != subchannels_) {
		if (fftPlan)
			fftwf_destroy_plan(fftPlan);
		ComplexFloat32 in[subchannels_], out[subchannels_];
		fftPlan = fftwf_plan_dft_1d(subchannels_, (fftwf_complex *) in,
				(fftwf_complex *) out, FFTW_BACKWARD, FFTW_MEASURE);
	}

	subchannels = subchannels_;

	// store the number of half frames, just for convenience
	halfFrames = activity->getHalfFrames();

	// compute the number of spectra in both the original subchannel
	// data and the number of samples in the synthesized channel
	subchannelSpectra = halfFrames
			* channel->getSamplesPerSubchannelHalfFrame();
	stride = subchannelSpectra * sizeof(ComplexPair);
	samples = subchannelSpectra * subchannels;

	// compute the frequency of the candidate signal in the middle
	// of data collection; this is used to compute the subchannel
	// containing that frequency.  The set of subchannels
	// around the signal is then extracted to create the confirmation
	// channel
	float32_t sigDrift = sig->path.drift;
	float64_t sigFreq = sig->path.rfFreq;
	float64_t sigCenterFreq = sigFreq + activity->getDataCollectionTime() / 2
			* HZ_TO_MHZ(sigDrift);

	// compute the set of subchannels to extract
	centerSubchannel = channel->getSubchannel(sigCenterFreq);
	startSubchannel = centerSubchannel - subchannels / 2;
	if (startSubchannel < 0)
		startSubchannel = 0;
	if (startSubchannel + subchannels > activity->getUsableSubchannels())
		startSubchannel = activity->getUsableSubchannels() - subchannels;

	// we may have shifted the channel, so recompute the center
	centerSubchannel = startSubchannel + subchannels / 2;
	centerFreq = channel->getSubchannelCenterFreq(centerSubchannel);
}

#ifdef notdef
void
ConfirmationChannel::getRetrievalSpec(XferSpec& xfer)
{
	// compute the retrieval parameters for retrieving the data
	// from disk
	xfer.blkLen = blkLen;

	// initialize counters used during the transfer
	xfer.pass = 0;
	xfer.src.blk = 0;

	// we do one pass through the disk buffer for the signal, extracting
	// all data at once to a single host buffer
	xfer.passes = 1;

	// source is the disk; the offset is the location of the first
	// block of the first subchannel to extract
	xfer.src.ofs = startSubchannel * subchannelBlkLen;

	// the # of blocks is the number of host buffers written to disk
	// during data collection, which depends on the number of half
	// frames in the observation
	xfer.src.blks = blks;

	// stride by one full host buffer between blocks
	xfer.src.stride = bufLayout.blkStride;
	xfer.src.passStride = 0;			// only one pass

	// destination is the confirmation subchannel buffer; the data is
	// packed by block/subchannel contiguously in this buffer;
	// it will be sorted out during creation of the confirmation
	// channel after retrieval
	xfer.dest.blk = 0;
	xfer.dest.base = 0;					// inserted before retrieval
	xfer.dest.ofs = 0;
	xfer.dest.blks = xfer.src.blks;
	xfer.dest.stride = xfer.blkLen;
	xfer.dest.passStride = 0;

	LogInfo(DXE_NE, -1, "hf %d, src ofs 0x%x, blks %d, len %d",
			halfFrames, xfer.src.ofs, xfer.src.blks, xfer.blkLen);
}

//
// retrieveCDData: retrieve the confirmation data from disk
//
// Synopsis:
//		dxError retrieveCDData(req);
//		dxDiskIORequest *req;			I/O request structure
// Notes:
//		The data is retrieved a block at a time to minimize the
//		lock time for the disk, since it is also used for storage
//		during data collection.
//
Error
ConfirmationChannel::retrieveCDData(DiskIORequest *req)
{
	// make a local copy of the disk I/O request
	DiskIORequest ioReq = *req;
	ioReq.xfer.src.blks = 1;

//	Debug(DEBUG_CONFIRM, (int32_t) req->xfer.blkLen, "blklen");

#ifdef notdef
	cout << "buffer " << ioReq.buffer << endl;
	cout << "blklen " << ioReq.xfer.blkLen << endl;
	cout << "src ofs " << ioReq.xfer.src.ofs << endl;
	cout << "dest base " << ioReq.xfer.dest.base << endl;
	cout << "dest ofs " << ioReq.xfer.dest.ofs << endl;
#endif

	// read a block at a time, relinquishing the disk between blocks
	// so that storage can occur
	for (int32_t blk = 0; blk < req->xfer.src.blks; blk++)
		ReadDisk(&ioReq);
}
#endif

//
// assembleConfData: assemble the confirmation data into a spectrum
//		of SUBCHANNELS_PER_CONF_CHANNEL subchannel bins
//
// Notes:
//		This method is called one spectrum at a time; it rearranges
//		the time-domain bins from several subchannels into a single
//		frequency-domain spectrum which can be inverse transformed
//		to create a wider channel.
//
void
ConfirmationChannel::assembleConfData(ComplexPair *cdData,
		ComplexFloat32 *confData, int32_t spectrum)
{
	ComplexPair *data = &cdData[spectrum];
	for (int32_t i = subchannels / 2; i < subchannels - 1; ++i) {
		uint8_t val = (data + i * stride)->pair;
		uint8_t re = val >> 4;
		re |= re & 0x08 ? 0xf0 : 0;
		uint8_t im = val & 0x0f;
		im |= im & 0x08 ? 0xf0 : 0;
		*confData++ = ComplexFloat32(re, im);
	}
	// then place the subchannels left of center in the right half
	for (int32_t i = 0; i < subchannels / 2; ++i) {
		// extract the data for a single subchannel bin
		uint8_t val = (data + i * stride)->pair;
		uint8_t re = val >> 4;
		re |= re & 0x08 ? 0xf0 : 0;
		uint8_t im = val & 0x0f;
		im |= im & 0x08 ? 0xf0 : 0;
		*confData++ = ComplexFloat32(re, im);
	}
}

//
// synthesizeConfChannel: create a single block of time samples for the
//		wide confirmation channel
//
void
ConfirmationChannel::synthesizeConfChannel(ComplexFloat32 *confData,
		ComplexFloat32 *confTDData, float32_t *basePower)
{
	fftwf_execute_dft(fftPlan, (fftwf_complex *) confData,
			(fftwf_complex *) confTDData);

	if (basePower) {
		for (int32_t bin = 0; bin < subchannels; bin++)
			*basePower += std::norm(confTDData[bin]);
	}
}

#ifdef nodef
//
// dumpSubchannelData: dump the subchannel data buffer to a file
//
Error
ConfirmationChannel::dumpSubchannelData(Buffer *buf, const string *diskFile)
{
	int8_t re, im;
	int32_t blk, subchannel, bin, bins, val;
	string file(DEFAULT_CONF_SUBCHANNEL_FILE);
	ComplexPair *cdData, *data;
	FILE *fp;

	// use the file name if specified
	if (diskFile)
		file = *diskFile;

	fp = fopen(file.c_str(), "w");

	// dump the data in the file in text format; the data is organized
	// in blocks, but the last block will usually be a partial one,
	// so we must keep track of the number of spectra actually
	// printed
	cdData = static_cast<ComplexPair *> (buf->getBuf());
	for (blk = 0; blk < blks; blk++) {
		data = cdData + blk * blkLen;
		bins = subchannelSpectra - blk * subchannelBinsPerBlk;
		if (bins > subchannelBinsPerBlk)
			bins = subchannelBinsPerBlk;
		for (subchannel = 0; subchannel < subchannels; subchannel++) {
			data = cdData + blk * blkLen + subchannel * subchannelBlkLen;
			for (bin = 0; bin < bins; bin++, data++) {
				val = data->pair;
				re = val >> 4;
				re |= re & 0x08 ? 0xf0 : 0;
				im = val & 0x0f;
				im |= im & 0x08 ? 0xf0 : 0;
				fprintf(fp, "%04d:%04d (%d, %d) (0x%02x)\n",
						blk * subchannelBinsPerBlk + bin, subchannel, re, im,
						(uint8_t) (re << 4 | im));
			}
		}
	}
	fclose(fp);
	return (0);
}
#endif
}