/*******************************************************************************

 File:    TestCmdLineParser.cpp
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
#include "TestCmdLineParser.h"
#include "CmdLineParser.h"
#include "ArrayLength.h"
#include "SseUtil.h"
#include <sstream>

using namespace std;

void TestCmdLineParser::setUp()
{
}

void TestCmdLineParser::tearDown()
{
}

void TestCmdLineParser::testFoo()
{
    cout << "testFoo" << endl;

    //cu_assert(false);
}

void TestCmdLineParser::testFlag()
{
    cout << "testFlag" << endl;

    CmdLineParser parser;

    // set a flag, get it back

    string argFoobar("-foobar");
    parser.addFlagOption(argFoobar);

    const char * cmdLine[] = { "progname", argFoobar.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine), 
                                   const_cast<char **>(cmdLine));
    cu_assert(validParse);

    cu_assert(parser.getFlagOption(argFoobar));
}


void TestCmdLineParser::testStringOption()
{
    cout << "testStringOption" << endl;

    CmdLineParser parser;

    string optName("-color");
    string defaultValue("blue");
    parser.addStringOption(optName, defaultValue);

    string requestedValue("green");
    const char * cmdLine[] = { "progname", optName.c_str(), 
                               requestedValue.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine),
                                   const_cast<char **>(cmdLine));
    cu_assert(validParse);

#if 0
    cerr << "for " << optName << " got value: " 
         << parser.getStringOption(optName) << endl;
#endif
    cu_assert(parser.getStringOption(optName) == requestedValue);

}

void TestCmdLineParser::testDoubleOption()
{
    cout << "testDoubleOption" << endl;

    CmdLineParser parser;

    string optName("-length");
    double defaultValue(5.5);

    parser.addDoubleOption(optName, defaultValue);

    string requestedValue("8.2");
    const char * cmdLine[] = { "progname", optName.c_str(), 
                               requestedValue.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine),
                                   const_cast<char **>(cmdLine));
    cu_assert(validParse);

    double expectedValue(SseUtil::strToDouble(requestedValue));
    double tol(0.01);
    assertDoublesEqual(expectedValue,
                       parser.getDoubleOption(optName), tol);

}

void TestCmdLineParser::testIntOption()
{
    cout << "testIntOption" << endl;

    CmdLineParser parser;

    string optName("-length");
    int defaultValue(5);

    parser.addIntOption(optName, defaultValue);

    string requestedValue("8");
    const char * cmdLine[] = { "progname", optName.c_str(), 
                               requestedValue.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine),
                                   const_cast<char **>(cmdLine));
    cu_assert(validParse);

    int expectedValue(SseUtil::strToInt(requestedValue));
    cu_assert(expectedValue == parser.getIntOption(optName));
}

void TestCmdLineParser::testDoubleInvalidNumber()
{
    cout << "testDoubleInvalidNumber" << endl;

    CmdLineParser parser;

    string optName("-length");
    double defaultValue(5.5);
    parser.addDoubleOption(optName, defaultValue);

    string requestedValue("badnumber");
    const char * cmdLine[] = { "progname", optName.c_str(), 
                               requestedValue.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine),
                                   const_cast<char **>(cmdLine));
    cu_assert(! validParse);

    string errorText(parser.getErrorText());

    //cout << "errorText: " << errorText << endl;

    string expectedErrorText("'badnumber' is not a valid number");
    cu_assert(errorText.find(expectedErrorText) != 
              string::npos);
}

void TestCmdLineParser::testIntInvalidNumber()
{
    cout << "testIntInvalidNumber" << endl;

    CmdLineParser parser;

    string optName("-length");
    double defaultValue(5);
    parser.addDoubleOption(optName, defaultValue);

    string requestedValue("badintnumber");
    const char * cmdLine[] = { "progname", optName.c_str(), 
                               requestedValue.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine),
                                   const_cast<char **>(cmdLine));
    cu_assert(! validParse);

    string errorText(parser.getErrorText());

    //cout << "errorText: " << errorText << endl;

    string expectedErrorText("'badintnumber' is not a valid number");
    cu_assert(errorText.find(expectedErrorText) != 
              string::npos);
}

void TestCmdLineParser::testInvalidOption()
{
    cout << "testInvalidOption" << endl;

    CmdLineParser parser;

    string optName("-color");
    string defaultValue("blue");
    parser.addStringOption(optName, defaultValue);

    string requestedOpt("-badoption");
    const char * cmdLine[] = { "progname", requestedOpt.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine), 
                                   const_cast<char **>(cmdLine));
    cu_assert(! validParse);

    string errorText(parser.getErrorText());

    //cout << "errorText: " << errorText << endl;

    string expectedErrorText("invalid option: " + requestedOpt);
    cu_assert(errorText.find(expectedErrorText) != 
              string::npos);
}


void TestCmdLineParser::testMissingValue()
{
    cout << "testMissingValue" << endl;

    CmdLineParser parser;

    string optName("-color");
    string defaultValue("blue");
    parser.addStringOption(optName, defaultValue);

    const char * cmdLine[] = { "progname", optName.c_str() };

    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine),
                                   const_cast<char **>(cmdLine));
    cu_assert(! validParse);

    string errorText(parser.getErrorText());

    //cout << "errorText: " << errorText << endl;

    string expectedErrorText("no value supplied for -color option");
    cu_assert(errorText.find(expectedErrorText) != 
              string::npos);
}

void TestCmdLineParser::testDefaults()
{
    cout << "testDefaults" << endl;

    // send in empty command line, all options
    // should have default values

    CmdLineParser parser;

    string opt1Name("-length");
    double opt1Default(5.5);
    parser.addDoubleOption(opt1Name, opt1Default);

    string opt2Name("-batch");
    parser.addFlagOption(opt2Name);

    string opt3Name("-color");
    string opt3Default("red");
    parser.addStringOption(opt3Name, opt3Default);

    const char * cmdLine[] = { "progname" };
    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine), 
                                   const_cast<char **>(cmdLine));
    cu_assert(validParse);

    double tol(0.01);
    assertDoublesEqual(opt1Default, parser.getDoubleOption(opt1Name),
                       tol);

    cu_assert(! parser.getFlagOption(opt2Name));

    cu_assert(parser.getStringOption(opt3Name) == opt3Default);
}

void TestCmdLineParser::testUsage()
{
    cout << "testUsage" << endl;

    // send in empty command line, all options
    // should have default values

    CmdLineParser parser;

    string opt1Name("-length");
    double opt1Default(5.5);
    string opt1Help("how long it should be");
    parser.addDoubleOption(opt1Name, opt1Default, opt1Help);

    string opt2Name("-batch");
    string opt2Help("run without GUI");
    parser.addFlagOption(opt2Name, opt2Help);

    string opt3Name("-color");
    string opt3Default("red");
    string opt3Help("desired tint");
    parser.addStringOption(opt3Name, opt3Default, opt3Help);

    const char * cmdLine[] = { "progname" };
    bool validParse = parser.parse(ARRAY_LENGTH(cmdLine), 
                                   const_cast<char **>(cmdLine));
    cu_assert(validParse);

    string usageText(parser.getUsage());

    cerr << usageText << endl;

    cu_assert(usageText.find(opt1Help) != string::npos);
    cu_assert(usageText.find(opt2Help) != string::npos);
    cu_assert(usageText.find(opt3Help) != string::npos);

}


Test *TestCmdLineParser::suite()
{
   TestSuite *testSuite = new TestSuite("TestCmdLineParser");
   
   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testFoo", &TestCmdLineParser::testFoo));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testFlag", &TestCmdLineParser::testFlag));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testStringOption", &TestCmdLineParser::testStringOption));
   
   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testDoubleOption", &TestCmdLineParser::testDoubleOption));
   
   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testDoubleInvalidNumber", &TestCmdLineParser::testDoubleInvalidNumber));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testIntOption", &TestCmdLineParser::testIntOption));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testIntInvalidNumber", &TestCmdLineParser::testIntInvalidNumber));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testInvalidOption", &TestCmdLineParser::testInvalidOption));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testMissingValue", &TestCmdLineParser::testMissingValue));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testDefaults", &TestCmdLineParser::testDefaults));

   testSuite->addTest(new TestCaller<TestCmdLineParser>(
      "testUsage", &TestCmdLineParser::testUsage));

   
   return testSuite;
}
