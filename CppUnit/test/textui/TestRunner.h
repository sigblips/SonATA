#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <iostream>
#include <vector>

using namespace std;

#include "Test.h"

typedef pair<string, Test *>           mapping;
typedef vector<pair<string,Test *> >   mappings;

class TestRunner
{
protected:
    bool                                m_wait;
    vector<pair<string,Test *> >   m_mappings;

public:
                TestRunner ():
		  m_wait(false),
		  m_numberOfFailures(0),
		  m_numberOfErrors(0)
    {}
                ~TestRunner ();

    int        run (int ac, char **av);
    void        addTest (string name, Test *test)
    { m_mappings.push_back (mapping (name, test)); }
    int testFailures() const;		// number of failures
    int testErrors() const;		// number of errors

protected:
    int         run (Test *test);
    int         runAllTests();
    int         runSelectTests(vector<string> testNames);
    void        printUsage(char *progName);
    void        listAllTests();

private:
    int m_numberOfFailures;
    int m_numberOfErrors;
};

#endif
