/*******************************************************************************

 File:    edg_trace.h
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

#ifndef _EDG_TRACE_H
#define _EDG_TRACE_H

/*
** This file defines a default version of the two trace macros Tr() and
** TR_DECLARE().  Definitions are designed to allow partial or complete
** overrides in edgc_cfg.h to meet the needs of specific apps, so this
** header file can be used by library functions which need to use tracing,
** without tying them to any specific implementation.
**
** The default implementations are not necessarily the best, but will work
** on any system supporting printf() and getenv(), and are implemented
** completely in the .h file, so no supporting .o is needed.
**
** Each trace flag is enabled dynamically from the environment, by checking
** for the existence of an environment variable TR_modname_flagname
**
** Guidelines for partial overriding:
** - use Tr to control whether static or dynamic binding is used
** - use TR_OK to control how trace flags are dynamically bound to strings
** - use TR_DECLARE to support TR_OK
** - use TR_LOOKUP to specify where dynamic control comes from
** - use TR_PRINTF to specify where final output goes
*/

#ifndef Tr
#define Tr(flag,msg)	(TR_OK(flag) && TR_PRINTF(msg))
#endif

#ifndef TR_OK
#define TR_OK(flag) ((flag < 0) ? \
	(flag = TR_LOOKUP("TR_" EDG_MODNAME "_" #flag)) : (flag))
#endif

#ifndef TR_DECLARE
#define TR_DECLARE(x) static int x = -1
#endif

#ifndef TR_LOOKUP
#include "edg_getenv.h"

static int
TR_LOOKUP(const char *name)
{
	const char *out = GETENV(name);
	return out ? 1 : 0;
}
#endif /* TR_LOOKUP */

#ifndef TR_PRINTF
#include <stdio.h>
#define TR_PRINTF(msg)	(printf msg && printf("\n"))
#endif

#endif /* _EDG_TRACE_H */