

/*
 * A command line based tool to run tests.
 * TestRunner prints out a trace as the tests are executed followed by a
 * summary at the end.
 *
 * You can add to the tests that the TestRunner knows about by 
 * making calls to "addTest (...)".
 *
 *
 */

#include <iostream>
#include <vector>
#include "getopt.h"

using namespace std;


#include "TestRunner.h"
#include "TextTestResult.h"



void TestRunner::printUsage(char *progName)
{
            cout << "Run unit tests." << endl;
            cout << "Usage: ";
            cout << progName;
            cout << " [--help] [--list] [--wait] [--all OR [testName [testName] ...]]";
	    cout << endl;
	    cout << "[--help] print this help message." << endl;
	    cout << "[--list] list the names of all the unit tests." << endl;
	    cout << "[--wait] wait for <return> after each test result." << endl;
            cout << "[--all] run all the tests." << endl;
	    cout << "[testName] is the name of a test case class to run.";
            cout << endl;
	    return;
}

/*
 * Run tests and collect their results.
 */

int TestRunner::run (int ac, char **av)
{

        int all = 0;
        int list = 0;
	int wait = 0;
	vector<string> testNames;
	int errflg = 0;
	int help = 0;

	while (1) {
	  static struct option long_options[] = {
	    {"all", no_argument, &all, 1},
	    {"list", no_argument, &list, 1},
	    {"wait", no_argument, &wait, 1},
	    {"help", no_argument, &help, 1},
	    {0, 0, 0, 0}
	  };

	  int option_index = 0;
     
	  int c = getopt_long (ac, av, "",
			   long_options, &option_index);
  
  	  
	  /* Detect the end of the options. */
	  if (c == -1)
	    break;

	  switch (c)
	    {
	    case 0:
	      /* If this option set a flag, do nothing else now. */
	      if (long_options[option_index].flag != 0){
		break;
	      }
	      break;
     
	    case '?':
	      /* `getopt_long' already printed an error message. */
	      errflg++;
	      break;
     
	    default:
	      cerr << "Unrecognized option: - " << av[optind] << endl;
	      errflg++;
	    }
	}
	

	if (errflg > 0){
	  printUsage(av[0]);
	  return -1;
	}

	m_wait = wait;
	
	// If there aren't any arguments, run everything
	if (!list && !help && optind == ac){
	  all = 1;
	}
	
        for (int i = optind; i<ac; i++)
        {
	  testNames.push_back(string(av[i]));
        };


	if (list)
	{
	      listAllTests();
	      return 0;
        }
	else if (help)
        {
	    printUsage(av[0]);
	    return 0;
        }
        else if (all)
	{
	      return runAllTests();
	}
        else
        {
	      return runSelectTests(testNames);
	}

}

int TestRunner::runAllTests ()
{
        Test *testToRun = NULL;

        for (mappings::iterator it = m_mappings.begin ();
                it != m_mappings.end ();
                it++)
        {
                testToRun = (*it).second;
                run (testToRun);
        }

return testFailures() + testErrors();
}


int TestRunner::runSelectTests (vector<string> testNames)
{
    string testCase;

    for (vector<string>::iterator requestedTest = testNames.begin();
          requestedTest != testNames.end(); requestedTest++)
    {
        testCase = *requestedTest;

        Test *testToRun = NULL;

        for (mappings::iterator it = m_mappings.begin ();
                it != m_mappings.end ();
                it++)
        {
            if ((*it).first == testCase)
            {
                testToRun = (*it).second;
                run (testToRun);
            }
        }


        if (!testToRun)
        {
            cout << "Test " << testCase << " not found." << endl;
            return -1;
        } 

    }

return testFailures() + testErrors();
}


void TestRunner::listAllTests ()
{
        for (mappings::iterator it = m_mappings.begin ();
                it != m_mappings.end ();
                it++)
        {
	  cout << (*it).first << endl;
        }

}


/* 
 * Clean up  
 */

TestRunner::~TestRunner ()
{
    for (mappings::iterator it = m_mappings.begin ();
             it != m_mappings.end ();
             it++)
        delete it->second;

}


/*
 * Runs a single test and collects its results.
 */

int TestRunner::run (Test *test)
{
    TextTestResult  result;

    test->run (&result);

    cout << result << endl;

    if (m_wait)
    {
        cout << "<RETURN> to continue" << endl;
        cin.get ();

    }

    m_numberOfFailures += result.testFailures();
    m_numberOfErrors += result.testErrors();

    return result.testFailures() + result.testErrors();
}

int TestRunner::testFailures() const
{
  return m_numberOfFailures;
}

int TestRunner::testErrors() const
{
  return m_numberOfErrors;
}



