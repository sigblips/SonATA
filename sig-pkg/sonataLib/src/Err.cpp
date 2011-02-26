/*******************************************************************************

 File:    Err.cpp
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
// dummy error functions
//
#include <iostream>
#include <pthread.h>
#include "Sonata.h"
#include "Err.h"

using std::cout;
using std::dec;
using std::hex;
using std::endl;

namespace sonata_lib {

static DebugMask debugMask;

void
FatalMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str)
{
	pthread_t tid = pthread_self();

	cout << "Fatal error " << file << "(" << (dec) << line << ")[";
	cout << func << "]: " << (dec) << err << "(0x" << (hex);
	cout << err << ")" << " tid (" << (dec) << tid << ") ";
	if (str)
		cout << str;
	cout << endl;
	exit(1);
}

void
ErrorMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str)
{
	pthread_t tid = pthread_self();

	cout << "Error " << file << "(" << (dec) << line << ")[";
	cout << func << "]: " << (dec) << err << "(0x" << (hex);
	cout << err << ")" << " tid (" << (dec) << tid << ") ";
	if (str)
		cout << " " << str;
	cout << endl;
}

void
WarningMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str)
{
	cout << "Warning " << file << "(" << (dec) << line << ")[" << func
			<< "]: " << (dec) << err << "(0x" << (hex) << err << ")";
	if (str)
		cout << " " << str;
	cout << endl;
}

void
InfoMsg(const char *file, const char *func, int32_t line,
		Error err, const char *str)
{
	cout << "Info " << file << "(" << (dec) << line << ")[" << func
			<< "]: " << (dec) << err << "(0x" << (hex) << err << ")";
	if (str)
		cout << " " << str;
	cout << endl;
}

void
DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, Error err, const char *str)
{
	if (debugMask.test(mask)) {
		cout << "Debug " << file << "(" << (dec) << line << ")[" << func
				<< "]: ";
		if (str)
			cout << str << " ";
		cout << (dec) << err << " (0x" << (hex) << err << ")" << endl;
	}
}

void
DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, void *p, const char *str)
{
	if (debugMask.test(mask)) {
		cout << "Debug " << file << "(" << (dec) << line << ")[" << func
				<< "]: ";
		if (str)
			cout << str << " ";
		cout << p << (dec) << p << endl;
	}
}

void
DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, float64_t val, const char *str)
{
	if (debugMask.test(mask)) {
		char vstr[MAX_STR_LEN];
		sprintf(vstr, "%.12lf", val);

		cout << "Debug " << file << "(" << (dec) << line << ")[" << func
				<< "]: ";
		if (str)
			cout << str << " ";
		cout << vstr << endl;
	}
}

void
DebugMsg(const char *file, const char *func, int32_t line,
		int32_t mask, ComplexFloat32 *val, const char *str)
{
	if (debugMask.test(mask)) {
		char re[MAX_STR_LEN], im[MAX_STR_LEN];
		sprintf(re, "%.12lf", val->real());
		sprintf(im, "%.12lf", val->imag());

		cout << "Debug " << file << "(" << (dec) << line << ")[" << func
				<< "]: ";
		if (str)
			cout << str << " ";
		cout << "(" << re << ", " << im << ")" << endl;
	}
}

void
SetDebugMask(int32_t mask)
{
	debugMask.set(mask);
}

void
ResetDebugMask(int32_t mask)
{
	debugMask.reset(mask);
}

}