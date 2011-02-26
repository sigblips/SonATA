// SWIG File for building new tclsh program
// Dave Beazley
// April 25, 1996
//

%{
static char *SSE_INITRC_DEFAULT = "~/.sserc.tcl";
static char *SSE_INITRC_SONATA = "~/sonata_install/scripts/sserc.tcl";
static char *SSE_INITRC_ENV_VAR = "SSE_INITRC";
%}

#ifdef AUTODOC
%subsection "tclsh.i"
%text %{
This module provides the Tcl_AppInit() function needed to build a 
new version of the tclsh executable.   This file should not be used
when using dynamic loading.   To make an interface file work with
both static and dynamic loading, put something like this in your
interface file :

     #ifdef STATIC
     %include tclsh.i
     #endif

A startup file may be specified by defining the symbol SWIG_RcFileName
as follows (this should be included in a code-block) :

     #define SWIG_RcFileName    "~/.myapprc"
%}
#endif
  
%{
 
/* A TCL_AppInit() function that lets you build a new copy
 * of tclsh.
 *
 * The macro SWIG_init contains the name of the initialization
 * function in the wrapper file.
 */

#ifdef MAC_TCL
extern int	MacintoshInit _ANSI_ARGS_((void));
extern int	SetupMainInterp _ANSI_ARGS_((Tcl_Interp *interp));
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
using std::ostream;
int Tcl_AppInit(Tcl_Interp *interp)
{
  if (Tcl_Init(interp) == TCL_ERROR) 
    return TCL_ERROR;

  /* Now initialize our functions */

  if (SWIG_init(interp) == TCL_ERROR)
    return TCL_ERROR;

  // Determine the desired initialization file. 
  // Use the env var if set, otherwise use the default.

#if (TCL_MAJOR_VERSION <= 8) and (TCL_MINOR_VERSION <= 3)
  #define VAR_PREFIX 
#else
  #define VAR_PREFIX const
#endif
char homedir[120];
  strcpy(homedir, getenv("HOME"));
  strcat( homedir,"/.sserc.tcl");
  VAR_PREFIX char *initrcFilename = SSE_INITRC_DEFAULT;
  VAR_PREFIX char *initrcSonATAStr = SSE_INITRC_SONATA;
  VAR_PREFIX char *initrcEnvStr = Tcl_GetVar2(interp,        
        "env", SSE_INITRC_ENV_VAR, TCL_LEAVE_ERR_MSG); 

  struct stat stFileInfo; 
  int intStat = stat(homedir,&stFileInfo);

  if (initrcEnvStr != NULL)
  {
     initrcFilename = initrcEnvStr;
  } 
  else
  {
// Attempt to get the file attributes
  if(intStat != 0) 
        { 
         initrcFilename = initrcSonATAStr;
        }
  }
  cout << "Using " << initrcFilename << endl;
  Tcl_SetVar(interp,"tcl_rcFileName",initrcFilename,TCL_GLOBAL_ONLY);
#ifdef SWIG_RcRsrcName
  Tcl_SetVar(interp,"tcl_rcRsrcName",SWIG_RcRsrcName,TCL_GLOBAL);
#endif

  return TCL_OK;
}

#include <ace/Synch.h>
#include "seeker.h"
#include "Scheduler.h"

int main(int argc, char **argv)
{
#ifdef MAC_TCL
    char *newArgv[2];
    
    if (MacintoshInit()  != TCL_OK)
    {
	Tcl_Exit(1);
    }

    argc = 1;
    newArgv[0] = "tclsh";
    newArgv[1] = NULL;
    argv = newArgv;
#endif

   // used to force seekerStart to progress to a certain point
   // before Tcl_Main is called

   ACE_Thread_Mutex startingSeeker;
   startingSeeker.acquire();
   seekerStart(argc, argv, startingSeeker);
   startingSeeker.acquire();

   Tcl_CreateExitHandler(swigExit, 0);
   Tcl_Main(argc, argv, Tcl_AppInit);

   return(0);
}

%}

