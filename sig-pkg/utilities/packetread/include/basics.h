/*******************************************************************************

 File:    basics.h
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

#ifndef BASICS_H_
#define BASICS_H_

#include <iostream>
#include <sstream>
#include <string>

//------------------------------------------------------------------
// convenience macros for output
//------------------------------------------------------------------
// single line to stderr
#define OUTL(msg) \
	do { \
		std::cerr << msg << std::endl; \
	} while (0)
// dump and label
#define DUMP(exp) " " << #exp << ":" << (exp)
// dump in hex and label
#define HDUMP(exp) " " << #exp << ":" << ((void *)(exp))

#define USAGE(exp) \
	do { \
		OUTL("ERROR:" << exp); \
		usage(); \
	} while (0)



//------------------------------------------------------------------
// tests
// - code under test should use ASSERT; this is a lot like standard 
//   assert(), but has a hook that allows tests to verify that an
//   assertion gets hit without terminating the program.
// - tests should generally use the CONFIRM macro to verify behavior;
//   this currently just asserts, but may do more later
// - tests can use CONFIRM_ASSERT to verify that an assertion is hit.
//   (the first parameter is an expression to execute; the second is
//   a substring of the expected assertion; a null or empty substring
//   accepts any assertion)
//------------------------------------------------------------------
extern bool ThrowOnFailure;

extern void Fail();
extern void OnFail(void (*function)(void));

#define CODE_LOC " at line " << __LINE__ << ", " << __FILE__

#define FAIL(msg) \
	do { \
		std::stringstream tmp; \
		tmp << msg << CODE_LOC; \
		if (ThrowOnFailure) throw(tmp.str()); \
		OUTL(tmp.str()); \
		Fail(); \
	} while (0)

#define ASSERT(exp) \
	do { \
		if (!(exp)) \
			FAIL("FAILED assertion '" << #exp << "'"); \
	} while (0)

#define CONFIRM(exp) \
	do { \
		if (!(exp)) \
			FAIL("FAILED to confirm '" << #exp << "'"); \
		else \
			OUTL("OK" << CODE_LOC); \
	} while (0)


#define CONFIRM_ASSERT(exp,match) \
	do { \
		bool stack = ThrowOnFailure; \
		try { \
			ThrowOnFailure = true; \
			exp; \
			ThrowOnFailure = stack; \
			FAIL("no assertion"); \
		} \
		catch(std::string &msg) { \
			ThrowOnFailure = stack; \
			CONFIRM(strstr(msg.c_str(),match)); \
		} \
	} while(0)

//------------------------------------------------------------------
// DEBUG facility
//------------------------------------------------------------------
#define DEBUG(flag,exp) \
	do { \
		if (DEBUG_##flag) \
			OUTL("DEBUG(" << #flag << "): " << exp); \
	} while (0)

#define DEBUG_ALWAYS true
#define DEBUG_NEVER false

#endif /* BASICS_H_ */