

#include "ExampleTestCase.h"
#include "TestTest.h"
#include "MulticasterTest.h"

#include "TestRunner.h"

/* 
 * Driver 
 */

int main (int ac, char **av)
{
    TestRunner runner;
    runner.addTest ("ExampleTestCase", ExampleTestCase::suite ());
    runner.addTest ("MulticasterTest", MulticasterTest::suite ());
    runner.addTest ("TestTest", TestTest::suite ());
    return runner.run (ac, av);
}
