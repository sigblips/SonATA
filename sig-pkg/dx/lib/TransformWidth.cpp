/*******************************************************************************

 File:    TransformWidth.cpp
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
// Transform width class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/TransformWidth.cpp,v 1.2 2009/02/22 04:41:42 kes Exp $
//
#include "TransformWidth.h"

namespace dx {

struct TransformSpec {
	float64_t sigChanWidth;				// * subchannels to get length of block
	int32_t sigSamplesPerSpectrum;
};

static TransformSpec transformWidth[] = {
	// CWD and PD resolutions
	{ 32, 32 },					// 1 Hz
	{ 16, 32 },					// 2 Hz
	{ 16, 16 },					// 4 Hz
	// PD-only resolutions
	{ 16, 8 },					// 8 Hz
	{ 8, 8 },					// 16 Hz
	{ 4, 8 },					// 32 Hz
	{ 2, 8 },					// 64 Hz
	{ 1, 8 },					// 128 Hz
	{ .5, 8},					// 256 Hz
	{ .25, 8 },					// 512 Hz
	{ .25, 4 }					// 1KHz
};

TransformWidth *TransformWidth::instance = 0;

TransformWidth *
TransformWidth::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new TransformWidth();
	l.unlock();
	return (instance);
}

TransformWidth::TransformWidth()
{
}

TransformWidth::~TransformWidth()
{
}

/**
 * Get the signal channel width in bins for a given resolution.
 */
float64_t
TransformWidth::getSigChanWidth(Resolution res)
{
	return (transformWidth[res].sigChanWidth);
}

int32_t
TransformWidth::getSigSamplesPerSpectrum(Resolution res)
{
	return (transformWidth[res].sigSamplesPerSpectrum);
}

}