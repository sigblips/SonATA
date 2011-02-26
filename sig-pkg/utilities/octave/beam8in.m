################################################################################
#
# File:    beam8in.m
# Project: OpenSonATA
# Authors: The OpenSonATA code is the result of many programmers
#          over many years
#
# Copyright 2011 The SETI Institute
#
# OpenSonATA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSonATA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
# 
# Implementers of this code are requested to include the caption
# "Licensed through SETI" with a link to setiQuest.org.
# 
# For alternate licensing arrangements, please contact
# The SETI Institute at www.seti.org or setiquest.org. 
#
################################################################################

# Program to read a capture file of raw beamformer packets,
# compute the spectrum and plot the result.

# Note: a beam packet has a 64-byte header followed by 2048
# complex 8-bit values, for a total packet length of 4160.

# Modified version of a program by Rob Ackermann.

# Queries for the filename and number of packets to process.
# Reads the file a packet at a time, discards the header
# and performs an FFT on the packet data, summing the power
# in each bin.  When all packets have been read, computes
# the average power in each bin.  A plot is of all the bin
# powers is displayed along with the file name and number
# of packets processed.
name = input("file: ");
packets = input("packets: ");

fd = fopen(name, "rb");

pwr = zeros(1, 2048);
N = packets;
for n = 1:N
	h = fread(fd, 64, "uchar");
	s = fread(fd, 4096, "signed char");
	# swap real and imaginary to invert the frequencies
	cmplx(1:2048) = s(1:2:4096) * i+ s(2:2:4096);
	pwr += abs(fftshift(fft(cmplx)));
endfor
plot(10 * log10(pwr/N));
title(name);
s=sprintf("%d packets", N);
y=max(10 * log10(pwr/N));
text(10, y, s);
