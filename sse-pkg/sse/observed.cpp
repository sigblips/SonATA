/*******************************************************************************

 File:    observed.cpp
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

// observed.cpp print target selection info

#include <ace/OS.h>
#include <iostream>
#include <list>
#include <map>
#include <getopt.h>
#include <argp.h>
#include <algorithm>
#include "machine-dependent.h" // for float64_t et. al.
#include "mysql.h"
#include "AntennaLimits.h"
#include "Target.h"
#include "getdate.h"
#include "getopt.h"
#include "config.h"
#include "sseVersion.h"
#include "Range.h"
#include "OrderedTargets.h"
#include "SignalMask.h"
#include "ActivityException.h"
#include "PermRfiMaskFilename.h"
#include <iomanip>

using namespace std;

// primary beamsize
const double PrimaryBeamsizeAtOneGhzDeg(3.5);
const double ArcSecPerDeg(3600);

const double PrimaryBeamsizeAtOneGhzArcSec(PrimaryBeamsizeAtOneGhzDeg * 
                                           ArcSecPerDeg);

/*
  Synth beam size:

  Use worse case synth beamsize for determining beam separation.
  For ata42, Peter says it's approx 13.7 x 3.2 arcmin at dec=-33 deg
  at 1420 Mhz.
  13.7 arcmin = 822 arcsec at 1420 Ghz,  which is ~ 1167 arcsec at one ghz.
*/    
const double SynthBeamsizeAtOneGhzArcSec(1167);



/* The name this program was run with. */
char *program_name;

/* Keys for options without short-options. */
#define OPT_DBHOST 1		// --db-host
#define OPT_DBNAME 2		// --db-name

const char *argp_program_version = SSE_VERSION;

const char *argp_program_bug_address =
"<nss-test@seti.org>";

/* Program documentation. */
const static char doc[] =
"Lists targets which need to be observed for Project Phoenix's NSS.";

#define DEFAULT_DBHOST "sol"
#define DEFAULT_DBNAME "janedb"
#define DEFAULT_MAIN_ANTENNA "ATA"
#define DEFAULT_REMOTE_ANTENNA "JODRELL BANK"
static struct argp_option options[] =
{
  {"autorise", 'a', "target selection mode", 0, 
  "Target selection mode\ntrue = autorise, false = auto\nDefault: false"},
  {"dbhost", 'h', "hostname", 0,
   "Connect to the specified host.\nDefault host:  "
   DEFAULT_DBHOST},
  {"dbname", 'n', "dbname", 0,
   "Name of database.\nDefault name:  "
   DEFAULT_DBNAME},
  {"obsdate", 'o', "'date time'", 0, 
  "Date and time of observation, UTC.    Default:currentTime   DateFormat:YYYY-MM-DD or MM/DD/YY # ISO 8601.  TimeFormat:HH:MM [am|pm]"

  },
  {"band", 'b', "bandname", 0, "Name of band to observe.  Default name: S.  \nChoices: L (1195-1755), S (1755-3005)"
  },
  {"mainantenna", 'm', "mainantenna", 0, "Name of main antenna in database, \nDefault:  "
  DEFAULT_MAIN_ANTENNA},
  {"notvisible", 'v', 0 , 0, "Include non-visible targets"
  },
  {"target", 't', "targetId", 0,
   "targetId of current target.  The scheduler will remain with the current target, if possible"
  },
  {"sunavoid", 's', "sun avoidance angle deg", 0,
   "sun avoidance angle in degrees"
  },
  {"obslength", 'l', "observation length (in seconds)", 0,
   "Observation length (in seconds)"
  },
  {"verboselevel", 'D', "'verbose (debug) level [0-2]'", 0, 
  "verbose (debug) level"
  },
  { 0 }
};

struct arguments
{
  arguments(const Range& range);
  const char *dbhost;
  const char *dbname;
  time_t obsDate;
  Range reqRange;
  const char *mainAntenna;
  const char *remoteAntenna;
  bool visibleonly;
  long currentTarget;
  int verboseLevel;
  double observationLength;
  bool autorise;
  double sunAvoidAngleDeg;
};
arguments::arguments(const Range& range):
  reqRange(range),
  currentTarget(0),
  verboseLevel(0),
  observationLength(98),
  autorise(false),
  sunAvoidAngleDeg(0)
{
}

Range LBand(1200, 1750);
Range SBand(1755, 3000);
Range TestBand(1400, 1480);

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the INPUT argument from `argp_parse', which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = static_cast<struct arguments *>(state->input);
     
  switch (key)
    {
    case 'a':
      arguments->autorise = arg;
      break;

    case 'h':
      arguments->dbhost = arg;
      break;

    case 'n':
      arguments->dbname = arg;
      break;

    case 'o':
      {
	time_t temp_date = get_date(arg, NULL);
	if (temp_date == -1) {	/* upon error return, keep previous date */
	  argp_error(state, "date \"%s\" is not parseable\n", arg);
	} else {
	  arguments->obsDate = temp_date;
	}
      }
      break;

    case 'b':
      if (strcmp(arg, "L") == 0){
	arguments->reqRange = LBand;
      } else if (strcmp(arg, "S") == 0){
	arguments->reqRange = SBand;
      } else if (strcmp(arg, "T") == 0){
	arguments->reqRange = TestBand;
      } else {
	argp_error(state, "band \"%s\" is not recognized\n", arg);
      }
      break;

    case 'm':
      arguments->mainAntenna = arg;
      break;
      
    case 'r':
      arguments->remoteAntenna = arg;
      break;
      
    case 'v':
      arguments->visibleonly = false;
      break;

    case 's':
      arguments->sunAvoidAngleDeg = strtol(arg, 0, 10);
      break;

    case 't':
      arguments->currentTarget = strtol(arg, 0, 10);
      break;

    case 'l':
      arguments->observationLength = strtol(arg, 0 , 10);
      break;

    case 'D':
       arguments->verboseLevel = strtol(arg, 0, 10);
      break;

    case ARGP_KEY_NO_ARGS:
      //  arguments->script_file = arg;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* A description of the additional arguments we accept. */
static char args_doc[] = "";

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };


/*  main() Print statistics for scheduled targets */
int main(int argc, char *argv[])
{
  putenv("TZ=UTC");
  struct arguments *arglist = new struct arguments(SBand);

  /* Default values. */
  arglist->dbhost = DEFAULT_DBHOST;
  arglist->dbname = DEFAULT_DBNAME;
  arglist->mainAntenna = DEFAULT_MAIN_ANTENNA;
  // TODO:  use remoteAntenna
  arglist->remoteAntenna = DEFAULT_REMOTE_ANTENNA;
  arglist->visibleonly = true;
  arglist->autorise = false;
  arglist->sunAvoidAngleDeg = 0;
  time(&arglist->obsDate);

  program_name = argv[0];

  /* Parse our arguments; every option seen by `parse_opt' will be
     reflected in `arguments'. */
  argp_parse (&argp, argc, argv, 0, 0, arglist);


  cout << "Date:  " << ctime(&arglist->obsDate);
  cout << "Time zone:  " << *tzname << endl;
  
  MYSQL* db = mysql_init(0);
  if (!mysql_real_connect(db, 
			  arglist->dbhost, 
			  "", 
			  "",
			  arglist->dbname,
			  0, 
			  0,
			  0)) {
      
    cerr << "connect:  MySQL error: " << mysql_error(db) << endl;
  }

  try {

  const double MaxDistLightYears = 225;    
  int obsPadding = 12;
  bool waitTargetComplete = true;

  PermRfiMask permRfiMask(PermRfiMaskFilename, "permRfiMask", 
			  arglist->verboseLevel);
  
  vector<FrequencyBand> permRfiBands(permRfiMask.getFreqBands());

  double bandwidthOfSmallestDxMhz(2.1);
  double minAcceptableRemainingBandMhz(7.5);
  double maxDxTuningSpreadMhz(50);
  double autoRiseTimeCutoffMinutes(10);
  bool useRaHourAngleLimits(false);
  double hourAngleRiseLimitHours(1);
  double hourAngleSetLimitHours(1);
  double decLowerLimitDeg(-34);
  double decUpperLimitDeg(90);
  int primaryTargetIdCountCutoff(120);
  int priorityCatalogSizeCutoff(20000);
  double moonAvoidAngleDeg(0);
  
  OrderedTargets orderedTargets(db,
				arglist->verboseLevel,
				obsPadding,
				arglist->mainAntenna,
				bandwidthOfSmallestDxMhz,
				minAcceptableRemainingBandMhz,
                                maxDxTuningSpreadMhz,
				arglist->autorise, // autorise
				arglist->observationLength,
				MaxDistLightYears,
				arglist->sunAvoidAngleDeg,
                                moonAvoidAngleDeg,
				autoRiseTimeCutoffMinutes,
				waitTargetComplete,
				useRaHourAngleLimits,
				hourAngleRiseLimitHours,
				hourAngleSetLimitHours,
				decLowerLimitDeg,
				decUpperLimitDeg,
                                primaryTargetIdCountCutoff,
                                priorityCatalogSizeCutoff,
				arglist->reqRange,
				permRfiBands,
                                PrimaryBeamsizeAtOneGhzArcSec,
                                SynthBeamsizeAtOneGhzArcSec
     );
  TargetId bestTargetId;
  ObsRange bestTargetObsRange;

  cout << "******* BestTarget ";
  if (arglist->autorise) cout << "Autorise " << endl;
  else
    cout << "Auto " << endl;

    orderedTargets.chooseTarget(
       arglist->obsDate,
       arglist->currentTarget,
       bestTargetId,
       bestTargetObsRange
       );
     //cout << "Next target is " << bestTargetId << endl << "Unobserved frequencies are :  " << bestTargetObsRange << endl;
      
      
      cout << " Id   "
	   << setw(4) << "Type"
	   << setw(7) << "RA" << setw(11) << "Dec"
	   << " " << setw(9) << "Dist"
	   << " " << setw(8) << "Merit"
	   << "  " << setw(8) << "HA Rise"
	  //<< " " << setw(7) << " HA Set"
	   << " " << setw(7) << "Cur HA"
	   << "   UTC Rise/Set  "
	   << "    Max Freq"
	   << endl;
      cout << orderedTargets;
      
  }
  catch (ActivityStartWaitingForTargetComplete &except) {
    cout << "cannot continue observing current target\n";
  }
  catch (SseException &except) {
     cout << except << endl;
  }
  catch (...) {
      cout << "caught unexpected exception" << endl;
  }  

}