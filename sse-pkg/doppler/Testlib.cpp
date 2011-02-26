/*******************************************************************************

 File:    Testlib.cpp
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


#include "TestRunner.h"
#include "Testlib.h"

#include "doppler.h"
#include <sstream>
#include <cstdio>
#include <string>
#include <cassert>
#include <cmath>
#include <cstring>

static void setStar4023(
    long *starnum,
    target_data_type *target_info)
{
    *starnum = 4023;

    target_info->type= STAR;               /* 1 == STAR, 2 == SPACECRAFT */
    target_info->ra2000=1.952187129382;    /* J2000 Right Ascension radians */
    target_info->dec2000= 0.091205573759;  /* J2000 Declination radians */
    target_info->origin = BARYCENTRIC;     /* 1 == BARYCENTRIC, 2 == GEOCENTRIC */
    target_info->parallax =  0.264400000000;  /* arcsec */
    target_info->pmra =  0.573000000000;   /* proper motion, RA, asec per yr */
    target_info->pmdec = -3.716000000000;  /* proper motion, Dec, asec per yr */
    target_info->range = 1.1670490093e+14; /* range to spacecraft in km */
}


static void setStar4098(
    long *starnum,
    target_data_type *target_info)
{
    *starnum = 4098;

    target_info->type = STAR;              /* 1 == STAR, 2 == SPACECRAFT */
    target_info->ra2000=0.869872000000;    /* J2000 Right Ascension radians */
    target_info->dec2000= 0.058818000000;  /* J2000 Declination radians */
    target_info->origin = BARYCENTRIC;     /* 1 == BARYCENTRIC, 2 == GEOCENTRIC */
    target_info->parallax = 0.109180000000;  /* arcsec */
    target_info->pmra =  0.269345770000;   /* proper motion, RA, asec per yr */
    target_info->pmdec = 0.093530000000;   /* proper motion, Dec, asec per yr */
    target_info->range = 2.8262296947e+14; /* range to spacecraft in km */
}

static void setStarPioneer10(
    long *starnum,
    target_data_type *target_info)
{
    *starnum = 101;  // seti "star" number for this spacecraft

    target_info->type = SPACECRAFT;
    target_info->origin = GEOCENTRIC;
    target_info->f0 = 2295.0;

    // Get the spacecraft ephemeris file.
    // Must ask for it from SRCDIR (rather than just ./)
    // so that when the 'make distcheck' target is run
    // the datafile can be found.

    string spacecraftEphemFile = SRCDIR;
    spacecraftEphemFile += "/pioneer10-ephem-2002.xyz";
    strcpy(target_info->targetfile, spacecraftEphemFile.c_str() );

    // remaining target_info fields are computed by the doppler code
    
}

static void setSiteArecibo(site_data_type *site_info)
{
    /* site info: main site -- Arecibo */

    /* X,Y,Z terrestrial telescope coordinates, m, ITRF system */
    site_info->coor[DOPPLER_SITE_COORD_X] = 2390453.850000000100;  /* X */
    site_info->coor[DOPPLER_SITE_COORD_Y] = -5564816.460000000000; /* y */
    site_info->coor[DOPPLER_SITE_COORD_Z] = 1994663.399999999900; /* z */

    /* height above geoid (mean sea level) in meters, UNUSED */
    site_info->altitude = 0.000000000000;

    /* (station freq standard - true)/true, of order 1e-12 */
    site_info->foffset = 0.000000000001;


}

static void setSiteJodrellBank(site_data_type *site_info)
{
    /* site info: remote site - Jodrell Bank */
    /* X,Y,Z terrestrial telescope coordinates, m, ITRF system */
    site_info->coor[DOPPLER_SITE_COORD_X] =  3822633.000000000000; /* X */
    site_info->coor[DOPPLER_SITE_COORD_Y] =  -154108.799999999990; /* y */
    site_info->coor[DOPPLER_SITE_COORD_Z] =  5086486.269999999600; /* z */

    /* height above geoid (mean sea level) in meters, UNUSED */
    site_info->altitude =  0.000000000000;

    /* (station freq standard - true)/true, of order 1e-12 */
    site_info->foffset =  0.000000000000;

}



void Testlib::setUp ()
{
}

void Testlib::tearDown()
{
}

void Testlib::testFoo()
{
    cout << "testFoo" << endl;

    // testGetCvsVersionTag()
    string version("$Name:  $");
    cout << "version tag is: " << version << endl;

    //cu_assert(false);  // force failure  
}


// Make double comparisons.  If the expected value is not
// within the tolerance of the actual value, print the 
// values with high precision and return false.

static bool passHighPrecisionDoublesEqualTest(const string &name,
					  double expected, 
					  double actual,
					  double tolerance)
{
   double diff = expected - actual;
   if (fabs (diff) > tolerance) { 
       stringstream results; 
       results.precision(14); // N digits after the decimal
       results 
       << "FAILED HighPrecisionDoublesEqualTest" << endl 
       << " name:      " << name << endl 
       << " expected:  " << expected << endl 
       << " actual:    " << actual << endl 
       << " diff:      " << diff << endl
       << " tolerance: " << tolerance << endl; 
       cout << results.str(); 						  
       return false;
    }

   return true;
}



static void setDopplerParametersTestTolerances(
    doppler_parameters_data_type *toleranceForDopplerParameters)
{
   // From dreher 24 May 2002
   // The error budget for the calculation of the
   // doppler factors was 0.01 Hz at 3 GHz, so the
   // f_ratio tolerance should be (0.01/3e9) ~ 3e-12.

   toleranceForDopplerParameters->f_ratio = 3e-12;   

   // Remaining result tolerances determined by experiment to make
   // sure that all the test cases can be differentiated.
   // Update when more info from dreher is available.

   toleranceForDopplerParameters->drift_offset_factor = 1e-16;
   toleranceForDopplerParameters->curv_main_factor = 1e-20;
   toleranceForDopplerParameters->curv_remote_factor = 1e-20;
}


static bool verifyDopplerResultTolerances(
    doppler_parameters_data_type dopplerParameters,
    doppler_parameters_data_type expectedDopplerParameters)
{
   doppler_parameters_data_type toleranceForDopplerParameters;
   setDopplerParametersTestTolerances(&toleranceForDopplerParameters);

   // Force failures
   //expectedDopplerParameters.f_ratio = 1.0001;  // force failure
   //expectedDopplerParameters.drift_offset_factor = 9.3e-11;
   //expectedDopplerParameters.curv_main_factor = 6.0e-15;
   //expectedDopplerParameters.curv_remote_factor = 2.0e-15;

   if (! passHighPrecisionDoublesEqualTest (
       "f_ratio",
       expectedDopplerParameters.f_ratio,
       dopplerParameters.f_ratio, 
       toleranceForDopplerParameters.f_ratio))
   {
       return false;
   };

   if (! passHighPrecisionDoublesEqualTest (
       "drift_offset_factor",
       expectedDopplerParameters.drift_offset_factor,
       dopplerParameters.drift_offset_factor,
       toleranceForDopplerParameters.drift_offset_factor))
   {
       return false;
   };

   if (! passHighPrecisionDoublesEqualTest (
       "curv_main_factor",
       expectedDopplerParameters.curv_main_factor,
       dopplerParameters.curv_main_factor,
       toleranceForDopplerParameters.curv_main_factor))
   {
       return false;
   };

   
   if (! passHighPrecisionDoublesEqualTest (
       "curv_remote_factor",
       expectedDopplerParameters.curv_remote_factor,
       dopplerParameters.curv_remote_factor,
       toleranceForDopplerParameters.curv_remote_factor))
   {
       return false;
   };


   return true;
}


// compute the doppler for the given star and time.
// return true if the computation matches the expected parameters

static bool passDopplerComputationTest(
    long starnum,
    target_data_type target_info, 
    time_t obs_date_time,
    double diff_utc_ut1,
    doppler_parameters_data_type expectedDopplerParameters)
{
   doppler_parameters_data_type dopplerParameters;
   site_data_type main_site_info;
   site_data_type remote_site_info;
   char earth_ephem_file[DOPPLER_MAX_EPHEM_FILENAME_LEN];

   printf("obs_date_time is %ld, ctime: %s\n", obs_date_time, ctime(&obs_date_time));

   // SETI Star Number 
   printf( "SETI Star Number: %ld\n", starnum);

   // Get the earth ephemeris file.
   // Must ask for it from SRCDIR (rather than just ./)
   // so that when the 'make distcheck' target is run
   // the datafile can be found.

   string earthdatafile = SRCDIR;
   earthdatafile += "/earth-ephem-2002.xyz";
   // don't overflow the output 
   assert(strlen(earthdatafile.c_str()) < DOPPLER_MAX_EPHEM_FILENAME_LEN-1); 
   strcpy(earth_ephem_file, earthdatafile.c_str());

   /* verbosity level: 0=none 3=most */   
   int verbosity = 3;

   setSiteArecibo(&main_site_info);
   setSiteJodrellBank(&remote_site_info);

   int status = 
       compute_doppler(target_info, main_site_info, remote_site_info, 
		       obs_date_time, diff_utc_ut1, earth_ephem_file, 
		       &dopplerParameters,
		       verbosity);

   // make sure computation itself succeeds without error
   assert(status == 0);

#if 0
   printf( "f_ratio %.12f\nDrift Offset factor %.6e\n",
	       dopplerParameters.f_ratio, dopplerParameters.drift_offset_factor );
   printf( "main curvature %.6e\nremote curvature %.6e\n",
	       dopplerParameters.curv_main_factor, dopplerParameters.curv_remote_factor );
#endif


   // make sure f_ratio is in the expected range
   assert(dopplerParameters.f_ratio > 0.9 && 
	  dopplerParameters.f_ratio < 1.1);

   return verifyDopplerResultTolerances(dopplerParameters,
					expectedDopplerParameters);
}

void Testlib::testDopplerCase1()
{
    cout << "=========== testDopplerCase1 ============" << endl;

    long starnum;

    target_data_type target_info;
    setStar4023(&starnum, &target_info);

    time_t obs_date_time=1021504629;  /* May 15 2002 23:17:09 UTC */
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results:  (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 1.000000143839;
    expectedDopplerParameters.drift_offset_factor = 9.830678e-11;
    expectedDopplerParameters.curv_main_factor = 5.435059e-15;
    expectedDopplerParameters.curv_remote_factor = 4.670194e-15;

    cu_assert(passDopplerComputationTest(
	starnum, target_info, obs_date_time, diff_utc_ut1,
	expectedDopplerParameters));
    

}

void Testlib::testDopplerCase2()
{

    cout << "=========== testDopplerCase2 ============" << endl;

   // same star as Case1, but different obs date (several months earlier)

    long starnum;
    target_data_type target_info;
    setStar4023(&starnum, &target_info);

    time_t obs_date_time=1011136629; // Jan 15 2002  23:17:09 UTC
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 0.999998744659;
    expectedDopplerParameters.drift_offset_factor = -3.734225e-11;
    expectedDopplerParameters.curv_main_factor = -7.503407e-15;
    expectedDopplerParameters.curv_remote_factor = -8.281420e-16;

    cu_assert (passDopplerComputationTest(
	starnum, target_info, obs_date_time, diff_utc_ut1,
	expectedDopplerParameters));

}

void Testlib::testDopplerCase3()
{
    cout << "=========== testDopplerCase3 ============" << endl;

   // same star as Case2, but different obs date (one day later)

    long starnum;
    target_data_type target_info;
    setStar4023(&starnum, &target_info);

    time_t obs_date_time=1011223029; // Jan 16 2002  23:17:09 UTC  
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 0.999998736036;
    expectedDopplerParameters.drift_offset_factor =-3.576208e-11;
    expectedDopplerParameters.curv_main_factor = -7.465578e-15;
    expectedDopplerParameters.curv_remote_factor = -7.444600e-16;

    cu_assert (passDopplerComputationTest(
	starnum, target_info, obs_date_time, diff_utc_ut1,
	expectedDopplerParameters));

}

void Testlib::testDopplerCase4()
{
    cout << "=========== testDopplerCase4 ============" << endl;

    // same star & obs date as case3, but different obs time ( 1 hour later)

    long starnum;
    target_data_type target_info;
    setStar4023(&starnum, &target_info);

    time_t obs_date_time=1011226629; // Jan 17 2002 00:17:09 UTC
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 0.999998652070;
    expectedDopplerParameters.drift_offset_factor = -1.061780e-11;
    expectedDopplerParameters.curv_main_factor = -6.622432e-15;
    expectedDopplerParameters.curv_remote_factor = 5.451806e-16;

    cu_assert (passDopplerComputationTest(
	starnum, target_info, obs_date_time, diff_utc_ut1,
	expectedDopplerParameters));

}

void Testlib::testDopplerCase5()
{
    cout << "=========== testDopplerCase5 ============" << endl;

    // same star & obs date as case3, but different obs time (5 minutes later)

    long starnum;
    target_data_type target_info;
    setStar4023(&starnum, &target_info);

    time_t obs_date_time=1011223329; // Jan 16 2002  23:22:09 UTC
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 0.999998725611;
    expectedDopplerParameters.drift_offset_factor = -3.373735e-11;
    expectedDopplerParameters.curv_main_factor = -7.414281e-15;
    expectedDopplerParameters.curv_remote_factor = -6.377260e-16;

    cu_assert (passDopplerComputationTest(
	starnum, target_info, obs_date_time, diff_utc_ut1,
	expectedDopplerParameters));

}

void Testlib::testDopplerCase6()
{
    cout << "=========== testDopplerCase6 ============" << endl;

    // same star & obs date as case3, but different obs time (1 minute later)

    long starnum;
    target_data_type target_info;
    setStar4023(&starnum, &target_info);

    time_t obs_date_time=1011223029 + 60; // Jan 16 2002  23:18:09 UTC
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 0.999998733903;
    expectedDopplerParameters.drift_offset_factor = -3.535848e-11;
    expectedDopplerParameters.curv_main_factor = -7.455604e-15;
    expectedDopplerParameters.curv_remote_factor = -7.231400e-16;

    cu_assert (passDopplerComputationTest(
	starnum, target_info, obs_date_time,  diff_utc_ut1,
	expectedDopplerParameters));

}


void Testlib::testDopplerCase7()
{
    cout << "=========== testDopplerCase7 ============" << endl;

    // different star 

    long starnum;

    target_data_type target_info;
    setStar4098(&starnum, &target_info);

    time_t obs_date_time=1021504629;  /* May 15 2002 23:17:09 UTC  */
    double diff_utc_ut1 = 0.0;  

    // expected doppler output results:  (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 1.000001261028;
    expectedDopplerParameters.drift_offset_factor = 3.695624e-11;
    expectedDopplerParameters.curv_main_factor = 7.510388e-15;
    expectedDopplerParameters.curv_remote_factor = 8.048766e-16;

    cu_assert(passDopplerComputationTest(
	starnum, target_info, obs_date_time,  diff_utc_ut1,
	expectedDopplerParameters));


}

void Testlib::testDopplerCase8()
{
    cout << "=========== testDopplerCase8 ============" << endl;

    // same as test7 but different diff_utc_ut1 value

    long starnum;

    target_data_type target_info;
    setStar4098(&starnum, &target_info);

    time_t obs_date_time=1021504629;  /* May 15 2002 23:17:09 UTC  */
    double diff_utc_ut1 = 0.1;  

    // expected doppler output results:  (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 1.000001261032;
    expectedDopplerParameters.drift_offset_factor = 3.695557e-11;
    expectedDopplerParameters.curv_main_factor = 7.510372e-15;
    expectedDopplerParameters.curv_remote_factor = 8.048410e-16;

    cu_assert(passDopplerComputationTest(
	starnum, target_info, obs_date_time,  diff_utc_ut1,
	expectedDopplerParameters));



#if 0

    // more exhaustive testing
    // walk through 24 hours of time, 360 degress (2pi radians) of RA,
    // and 60 degrees of dec (-0.5 to 0.5 radians)

    // +- 30 degrees dec
    for (double decRad = -0.5; decRad < +0.5; decRad += 0.10)
    {
	cout << "DDD decRad = " << decRad << endl;
	target_info.dec2000 = decRad;

	for (double raRad = 0.0; raRad < 6.28; raRad += 0.5)
	{
	    target_info.ra2000 = raRad;

	    cout << "YYY raRad = " << raRad << endl;
	    for (int i=0; i<24; i++)
	    {
		obs_date_time += 3600;

		cout <<" XXX obs_date_time =" << obs_date_time << " = " 
		     << ctime(&obs_date_time) << endl;

		cu_assert(passDopplerComputationTest(
		    starnum, target_info, obs_date_time,  diff_utc_ut1,
		    expectedDopplerParameters));

	    }
	}

    }

#endif

}

void Testlib::testDopplerCase9()
{
    cout << "=========== testDopplerCase9 ============" << endl;

    // spacecraft (pioneer 10) test

    long starnum;

    target_data_type target_info;
    setStarPioneer10(&starnum, &target_info);

#if 0

// use current date/time
#include <sys/types.h>
#include <time.h>

    cout << "=== using current date === " << endl;
    time_t obs_date_time;
    time(&obs_date_time);

#else
    // use canned date/time
    time_t obs_date_time=1021504629;  /* May 15 2002 23:17:09 UTC */

#endif

    double diff_utc_ut1 = 0.0;  

    // tbd factor this into its own method
    char earth_ephem_file[DOPPLER_MAX_EPHEM_FILENAME_LEN];
    string earthdatafile = SRCDIR;
    earthdatafile += "/earth-ephem-2002.xyz";
    assert(strlen(earthdatafile.c_str()) < DOPPLER_MAX_EPHEM_FILENAME_LEN-1); 
    strcpy(earth_ephem_file, earthdatafile.c_str());

    cout << "calculating spacecraft position" << endl;

    cu_assert(calculateSpacecraftPosition(&target_info,
				       obs_date_time,
				       diff_utc_ut1,
				       earth_ephem_file) == 0);

    // check expected coordinate position
    const double DEG_PER_RADIAN = 57.29577951;
    const double DEG_PER_HOUR = 15;
    cout << "ra: " << target_info.ra2000 << " radians " << endl;
    cout << "dec: " << target_info.dec2000 << " radians " << endl;

    cout << "ra: " << target_info.ra2000 * DEG_PER_RADIAN / DEG_PER_HOUR << " hours" << endl;
    cout << "dec: " << target_info.dec2000 * DEG_PER_RADIAN << " degrees " << endl;

    double expectedRaRadians=1.320432;
    double raRadiansTolerance=0.000001;
    cu_assert(passHighPrecisionDoublesEqualTest("spacecraft position ra radians",
				      expectedRaRadians,
				      target_info.ra2000,
				      raRadiansTolerance));

    double expectedDecRadians=0.450202;
    double decRadiansTolerance=0.000001;
    cu_assert(passHighPrecisionDoublesEqualTest("spacecraft position dec radians",
				      expectedDecRadians,
				      target_info.dec2000,
				      decRadiansTolerance));


    // expected doppler output results:  (from running original TSS code)
    doppler_parameters_data_type expectedDopplerParameters;
    expectedDopplerParameters.f_ratio = 1.000000824685;
    expectedDopplerParameters.drift_offset_factor = 6.612478e-11;
    expectedDopplerParameters.curv_main_factor = 6.953299e-15;
    expectedDopplerParameters.curv_remote_factor = 2.567895e-15;

    cu_assert(passDopplerComputationTest(
	starnum, target_info, obs_date_time, diff_utc_ut1,
	expectedDopplerParameters));
    

}



/*
static string readFileIntoString(const string &filename)
{
    ifstream ifile(filename.c_str());
    assert(ifile); // make sure it opened

    ostringstream buf;
    char ch;
    while (buf && ifile.get(ch))
    {
	buf.put(ch);
    }
    
    return buf.str();
}
*/

Test *Testlib::suite ()
{
    TestSuite *testSuite = new TestSuite("Testlib");
    
    testSuite->addTest (new TestCaller <Testlib> ("testFoo", &Testlib::testFoo));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase1", &Testlib::testDopplerCase1));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase2", &Testlib::testDopplerCase2));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase3", &Testlib::testDopplerCase3));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase4", &Testlib::testDopplerCase4));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase5", &Testlib::testDopplerCase5));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase6", &Testlib::testDopplerCase6));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase7", &Testlib::testDopplerCase7));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase8", &Testlib::testDopplerCase8));
    testSuite->addTest (new TestCaller <Testlib> (
	"testDopplerCase9", &Testlib::testDopplerCase9));

    return testSuite;
}