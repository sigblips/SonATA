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
#include "SseUtil.h"
#include "RedirectStreamToFile.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "Parameter.h"
#include "RangeParameter.h"
#include "ChoiceParameter.h"
#include "MultiChoiceParameter.h"
#include "ParameterGroup.h"
#include "AnyValueParameter.h"
#include "ArrayLength.h"
#include "Interpolate.h"

#include <sstream>
#include <stdio.h>
#include <math.h>
#include <vector>

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

    // force an invalid container access
    vector<int> values;
    values.push_back(8);
    values.push_back(9);
    values.push_back(10);

    cout << "values size is:" << values.size() << endl;

    cout << "values[0]=" << values[0] << endl;

    // this is invalid (off the end)
    //cout << "values[4]=" << values[4] << endl;

}


void Testlib::testLoadFileIntoStringVector()
{
    cout << "testLoadFileIntoStringVector" << endl;

    string filename("/tmp/testLoadFileIntoStringVector.txt");
    remove(filename.c_str());     // make sure it's not already there

    // open an output text stream attached to a file
    ofstream strm;
    strm.open(filename.c_str(), (ios::app));
    cu_assert(strm.is_open());

    // send text to the file
    vector<string> outgoing;
    outgoing.push_back("this is line1");
    outgoing.push_back("this is line2");

    for (size_t i=0; i<outgoing.size(); ++i)
    {
	strm << outgoing[i] << endl;
    }
    strm.close();


    // read it back
    vector<string> incoming = SseUtil::loadFileIntoStringVector(filename);

    // verify that what we wrote is what came back
    cu_assert(outgoing.size() == incoming.size());

    for (size_t i=0; i<outgoing.size(); ++i)
    {
	cu_assert(outgoing[i] == incoming[i]);
    }

    // clean up
    remove(filename.c_str());    

}

void Testlib::testUtils()
{
    cout << "testUtils" << endl;

    // verify int to string conversion
    const int testVal = 20;
    const string testValStr("20");
    string convertValStr(SseUtil::intToStr(testVal));
    cu_assert(testValStr == convertValStr);

    // verify string to int conversion
    int valueBack(SseUtil::strToInt(testValStr));
    cu_assert(valueBack == testVal);

    // force an error with an invalid input string
    try {
	int value2;
	value2 = SseUtil::strToInt("abc");

	cu_assert(0);  // shouldn't get here
    }
    catch (SseException & except)
    {
	cout << "caught expected strToInt error:" << endl
	     << except << endl;
    }

    // verify string to double conversion
    const double testValDouble = 20.1234;
    const string testValStrDouble("20.1234");
    double valueBackDouble(SseUtil::strToDouble(testValStrDouble));
    double tolDouble=0.00001;
    assertDoublesEqual(valueBackDouble, testValDouble, tolDouble);


    // force an error with an invalid input string
    try {
	double value3;
	value3 = SseUtil::strToDouble("abc");
    }
    catch (SseException except)
    {
	cout << "caught expected strToDouble exception:" << endl
	     << except;
    }

    cout << "iso date time of the epoch (UTC): ";
    time_t testTime = 0;   // time of the epoch
    string isoEpochTime = SseUtil::isoDateTime(testTime);
    cout << isoEpochTime << endl;
    cu_assert(isoEpochTime == "1970-01-01 00:00:00 UTC");


    // iso date time suitable for filenames
    cout << "iso date time suitable for filenames" << endl;
    string isoEpochTimeSuitableForFilename = 
	SseUtil::isoDateTimeSuitableForFilename(testTime);
    cout << isoEpochTimeSuitableForFilename << endl;
    cu_assert(isoEpochTimeSuitableForFilename == "1970-01-01_00-00-00_UTC");


    cout << "iso date time without timezone" << endl;
    string isoDateTimeWithoutTimezone =
	SseUtil::isoDateTimeWithoutTimezone(testTime);
    cout << isoDateTimeWithoutTimezone << endl;
    cu_assert(isoDateTimeWithoutTimezone == "1970-01-01 00:00:00");

    cout << "iso time without timezone" << endl;
    string isoTimeWithoutTimezone =
	SseUtil::isoTimeWithoutTimezone(testTime);
    cout << isoTimeWithoutTimezone << endl;
    cu_assert(isoTimeWithoutTimezone == "00:00:00");


    cout << "current iso date time: ";
    cout << SseUtil::currentIsoDateTime() << endl;

    cout << "current iso date: ";
    cout << SseUtil::currentIsoDate() << endl;

    cout << "current iso date time suitable for filenames: ";
    cout << SseUtil::currentIsoDateTimeSuitableForFilename() << endl;

    // test getHostname
    cout << "hostname: " <<  SseUtil::getHostname() << endl;

    // test getenv 
    // try a bogus env var, make sure it comes back as empty string
    string env1 = SseUtil::getEnvString("ABCDEF12345");
    cu_assert(env1 == "");

    // try a valid env var, make sure it's not an empty string
    string env2 = SseUtil::getEnvString("HOME");
    cout << "HOME is: " << env2 << endl;
    cu_assert(env2 != "");


    cu_assert(SseUtil::fileIsReadable("/foo/bar/fred") == false);
    cu_assert(SseUtil::fileIsReadable("/dev/null") == true);


    // test strMaxCpy (replacement for strncpy that guarantees 
    // a trailing null)

    int DEST_SIZE = 5;
    char dest[DEST_SIZE];
    
    // try a source string that's short enough to
    // fit in the destination buffer
    const char *src = "abc";
    SseUtil::strMaxCpy(dest, src, DEST_SIZE);
    cu_assert(string(dest) == string(src));

    // try a source string that's exactly long enough
    // to fit, including the trailing null
    src = "abcd";
    SseUtil::strMaxCpy(dest, src, DEST_SIZE);
    cu_assert(string(dest) == string(src));

    // try a source that's too long to fit..
    // should get truncated to the proper length to fit.
    src = "abcde";
    SseUtil::strMaxCpy(dest, src, DEST_SIZE);
    cu_assert(string(dest) != string(src));
    cu_assert(string(dest) == string("abcd"));


    // test strCaseEqual
    cu_assert(SseUtil::strCaseEqual("abc", "ABC"));
    cu_assert(SseUtil::strCaseEqual("abc1", "AbC1")); // include a digit

    cu_assert(SseUtil::strCaseEqual("abc", "abc")); // same & lowercase
    cu_assert(! SseUtil::strCaseEqual("abc", "abcd"));  //lengths differ
    cu_assert(SseUtil::strCaseEqual("", ""));

    // use explicit string classes
    string str1("fred");
    string str2("FRED");
    cu_assert(SseUtil::strCaseEqual(str1, str2));


    cu_assert(SseUtil::strToLower("ABC") == "abc");
    cu_assert(SseUtil::strToUpper("def") == "DEF");


    const string goodPath(".");
    cout << "Disk percent full for path '" << goodPath << "' is " 
	     << SseUtil::getDiskPercentUsed(goodPath) << endl;


    try {
	const string badPath("./NoSuchFileHere");
	stringstream strm;

	strm << "Disk percent full is " 
	     << SseUtil::getDiskPercentUsed(badPath) << endl;

	cu_assert(0);  // shouldn't get here
	
    }
    catch (SseException &except)
    {
	cout << "Caught expected SseUtil::getDiskPercentUsed exception: " << endl
	     <<  except << endl;
    }


    int filesize = SseUtil::getFileSize("/dev/null");
    cu_assert(filesize == 0);

    // try a bad filename
    try {
	int filesize;
	filesize = SseUtil::getFileSize("/ABadFilename");
	
	cu_assert(0);  // shoudn't get here

    }
    catch (SseException & except)
    {
	cout << "caught expected getFileSize error: " << endl
	     << except;
    }


    string inputString1("no quotes in string");
    string expectedString1 = inputString1;
    string result1 = SseUtil::insertSlashBeforeSubString(
	inputString1, "'");
    cu_assert(result1 == expectedString1);

    string inputString2("' a string with ' some ' apostrophes'");
    string expectedString2("\\' a string with \\' some \\' apostrophes\\'");
    string result2 =  SseUtil::insertSlashBeforeSubString(
	inputString2, "'");
    cu_assert(result2 == expectedString2);
    
    string inputString3("another test string with a longer substring");
    string expectedString3("another test string \\with a longer substring");
    string result3 =  SseUtil::insertSlashBeforeSubString(
	inputString3, "with");
    cu_assert(result3 == expectedString3);
    
    cout << result3 << endl;


    // Test ARRAY_LENGTH macro
    const int fooArraySize = 3;
    int fooArray[fooArraySize];

    cu_assert(ARRAY_LENGTH(fooArray) == fooArraySize);

    for (int i = 0; i < ARRAY_LENGTH(fooArray); ++i)
    {
	fooArray[i] = i;
    }
    
}

void Testlib::testRedirectStreamToFile()
{
    cout << "testRedirectStreamToFile" << endl;

    cout << "This line1 is going to cout on the console" << endl;

    // redirect the stream to the file
    // note the extra braces -- the redirect stays in effect as
    // long as the object exists

    string filetext("This line2 is going to the test file\n");

    string testFile("/tmp/test-redirect-strm.txt");
    {
	RedirectStreamToFile redirect(cout,testFile);

	cout << filetext;
    }

    // when above object is destroyed, stream should be restored
    cout << "This line3 is going to cout on the console" << endl;

    // read back the test file contents 
    string inbuff = SseUtil::readFileIntoString(testFile);
    cu_assert(filetext == inbuff);

    remove(testFile.c_str());
}

class TestSubscriber : public Subscriber
{
public:
    TestSubscriber(const string &name);
    ~TestSubscriber();
    void update(Publisher *changedPublisher); 
private:
    string name_;
};

TestSubscriber::TestSubscriber(const string &name)
    :name_(name)
{}

TestSubscriber::~TestSubscriber()
{}

void TestSubscriber::update(Publisher *changedPublisher)
{
    cout << name_ << " update called from publisher " << changedPublisher << endl;
}


void Testlib::testPublisherSubscriber()
{
    cout << "testPublisherSubscriber" << endl;
    
    // create 2 subscribers and attach them to a publisher
    TestSubscriber testSubscriberA("subscriberA");
    TestSubscriber testSubscriberB("subscriberB");

    Publisher testPublisher1;
    cout << "publisher1 is " << &testPublisher1 << endl;

    cout << "adding A" << endl;
    testPublisher1.attach(&testSubscriberA);
    testPublisher1.notify();

    cout << "adding B" << endl;
    testPublisher1.attach(&testSubscriberB);
    testPublisher1.notify();

    cout << "detaching A" << endl;
    testPublisher1.detach(&testSubscriberA);
    testPublisher1.notify();

    cout << "detaching B" << endl;
    testPublisher1.detach(&testSubscriberB);
    testPublisher1.notify();

    // try to detach a nonattached subscriber
    // this will cause an assert failure
    //
    // testPublisher1.detach(&testSubscriberB);
    

}

void Testlib::testTokenize()
{
    cout << "testTokenize" << endl;
    
    string source("happy sneezy    doc");
    vector<string> tokens = SseUtil::tokenize(source, " ");

    cu_assert(tokens.size() == 3);
    cu_assert(tokens[0] == "happy");
    cu_assert(tokens[1] == "sneezy");
    cu_assert(tokens[2] == "doc");

    // try empty list, shouldn't get any tokens
    source = "";
    tokens = SseUtil::tokenize(source, " ");
    cu_assert(tokens.size() == 0);

}

void Testlib::testSplitByDelimiter()
{
    cout << "testSplitByDelimiter" << endl;

    string source("happy\n sneezy\n\n\ndoc");
    vector<string> tokens = SseUtil::splitByDelimiter(source, '\n');
    
    cu_assert(tokens.size() == 5);
    cu_assert(tokens[0] == "happy");
    cu_assert(tokens[1] == " sneezy");
    cu_assert(tokens[2] == "");
    cu_assert(tokens[3] == "");
    cu_assert(tokens[4] == "doc");
    

    // empty list, should get one empty token
    source = "";
    tokens = SseUtil::splitByDelimiter(source, '\n');
    cu_assert(tokens.size() == 1);
    cu_assert(tokens[0] == "");

    // single element, no delimiter 
    source = "abc";
    tokens = SseUtil::splitByDelimiter(source, '\n');
    cu_assert(tokens.size() == 1);
    cu_assert(tokens[0] == "abc");

    // two elements
    source = "dog,cat";
    tokens = SseUtil::splitByDelimiter(source, ',');
    cu_assert(tokens.size() == 2);
    cu_assert(tokens[0] == "dog");
    cu_assert(tokens[1] == "cat");

    // empty element
    source = "red,,blue";
    tokens = SseUtil::splitByDelimiter(source, ',');
    cu_assert(tokens.size() == 3);
    cu_assert(tokens[0] == "red");
    cu_assert(tokens[1] == "");
    cu_assert(tokens[2] == "blue");

    // single character
    source = "a";
    tokens = SseUtil::splitByDelimiter(source, ',');
    cu_assert(tokens.size() == 1);
    cu_assert(tokens[0] == "a");

    // trailing delimiter
    source = "green,";
    tokens = SseUtil::splitByDelimiter(source, ',');
    cu_assert(tokens.size() == 2);
    cu_assert(tokens[0] == "green");
    cu_assert(tokens[1] == "");

    // single delimiter alone
    source = ",";
    tokens = SseUtil::splitByDelimiter(source, ',');
    cu_assert(tokens.size() == 2);
    cu_assert(tokens[0] == "");
    cu_assert(tokens[1] == "");

}



void Testlib::testParams()
{
    cout << "testParams" << endl;

    const int def = 10;
    const int min = -5;
    const int max = 20;
    const int current = def;
    RangeParameter<int> length("length", "feet", "board length",
			       def, min, max);    

    cu_assert(length.getDefault() == def);
    cu_assert(length.getCurrent() == current);
    cu_assert(length.getMin() == min);
    cu_assert(length.getMax() == max);

    cu_assert(length.setCurrent(6));
    cu_assert(length.getCurrent() == 6);

    // Try to set some values around the edges of the range
    cu_assert(length.setDefault(max-1));
    cu_assert(length.setDefault(max));
    cu_assert(! length.setDefault(max+1));

    cu_assert(length.setDefault(min+1));
    cu_assert(length.setDefault(min));
    cu_assert(! length.setDefault(min-1));

    cu_assert(length.isValid());
    cu_assert(length.isValid(max));
    cu_assert(! length.isValid(max+1));

    string errorText;
    cu_assert(length.setValue(4, errorText, Parameter::CURRENT));
    cu_assert(errorText == "");
    cu_assert(length.getValue(Parameter::CURRENT) == 4);

    // test conversions
    cu_assert(length.convertFromString("18", Parameter::CURRENT, errorText));
    cu_assert(length.convertToString(Parameter::CURRENT) == "18");
    cu_assert(length.getCurrent() == 18);

    // try an invalid input string conversion
    cu_assert(! length.convertFromString("badNumberString", Parameter::CURRENT, errorText));
    cu_assert(length.getCurrent() == 18);  // should still be the same
    cout << "error text is: " << errorText << endl;

    // try a truncation of a decimal value
    cu_assert(length.convertFromString("17.39", Parameter::CURRENT, errorText));
    cu_assert(length.getCurrent() == 17); 

    cout << length << endl;
    length.writeValues("", cout);

    cu_assert(length.setMax(100));
    cu_assert(length.setMin(-20));

    // try to set the max less than min, and min greater than max
    cu_assert(! length.setMax(-100));
    cu_assert(! length.setMin(5000));

    cout << length << endl;


    const int weightDefault = 11;
    const int weightCurrent = 9;
    ChoiceParameter<int> weight("weight", "pounds", "weight of stuff",
				  weightDefault);
    weight.addChoice(22);
    weight.addChoice(17);
    weight.addChoice(weightCurrent);
    weight.setCurrent(weightCurrent);
    cu_assert(weight.getDefault() == weightDefault);
    cu_assert(weight.getCurrent() == weightCurrent);

    cout << "weight n choices = " << weight.getNumberOfChoices() << endl;
    cu_assert(weight.getNumberOfChoices() == 4);

    cout << "weight help: " << weight.getHelp() << endl;

    cout << "weight << : " << endl;
    cout << weight << endl;

    cout << " weight after sort " << endl;
    weight.sort();
    cout << weight << endl;

    string colorDefault("red");
    ChoiceParameter<string> color("color", "angstroms", "color help",
				  colorDefault);
    // add some more choices
    // the default should already be stored
    color.addChoice("green");  
    color.addChoice("purple");

    string colorCurrent("green");
    color.setCurrent(colorCurrent);
    cu_assert(color.getDefault() == colorDefault);
    cu_assert(color.getCurrent() == colorCurrent);

    color.setDefault("purple");
    cu_assert(color.getDefault() == "purple");

    // try to set an invalid value
    color.setDefault("yellow");
    cu_assert(color.getDefault() != "yellow");


    cu_assert(color.isValid());    // test current value
    cu_assert(color.isValid("green"));  // check valid choice
    cu_assert(! color.isValid("very very red"));  // check invalid choice

    // test conversion routines
    cu_assert(color.getCurrent() == "green");
    cu_assert(color.convertFromString("red", Parameter::CURRENT, errorText));
    cu_assert(color.convertToString(Parameter::CURRENT) == "red");
    cu_assert(color.getCurrent() == "red");

    cout << "color is: " << color << endl;
    color.writeValues("", cout);

    cout << "color.print: " << endl;
    color.print(cout);

    const string nameDefault = "fred";
    const string nameCurrent = "ethel";
    AnyValueParameter<string> name("name", "", "name of thing",
				   nameDefault);
    name.setCurrent(nameCurrent);
    cu_assert(name.getDefault() == nameDefault);
    cu_assert(name.getCurrent() == nameCurrent);

    cout << "name help: " << name.getHelp() << endl;

    cout << "name << : " << endl;

    cout << name << endl;

}

void Testlib::testMultiChoiceParams()
{
   string errorText;
   string colorDefault("red");

   MultiChoiceParameter<string> color("color", "angstroms", "color help",
                                      colorDefault);
   // add some more choices
   // the default should already be stored
   color.addChoice("green");  
   color.addChoice("purple");

   string colorCurrent("green");
   color.setCurrent(colorCurrent);
   cu_assert(color.getDefault() == colorDefault);
   cu_assert(color.getCurrent() == colorCurrent);

   color.setDefault("purple");
   cu_assert(color.getDefault() == "purple");

   // try to set an invalid value
   color.setDefault("yellow");
   cu_assert(color.getDefault() != "yellow");

   cu_assert(color.isValid());    // test current value
   cu_assert(color.isValid("green"));  // check valid choice
   cu_assert(! color.isValid("very very red"));  // check invalid choice

   // test conversion routines
   cu_assert(color.getCurrent() == "green");
   cu_assert(color.convertFromString("red", Parameter::CURRENT, errorText));
   cu_assert(color.convertToString(Parameter::CURRENT) == "red");
   cu_assert(color.getCurrent() == "red");

   cout << "color is: " << color << endl;
   color.writeValues("", cout);

   cout << "color.print: " << endl;
   color.print(cout);

   // set multiple good values
   string goodMultivalue("green,purple");
   color.setCurrent(goodMultivalue);
   cu_assert(color.isValid());    // test current value
   cu_assert(color.getCurrent() == goodMultivalue);

   // set multiple values, with one bad one
   color.setCurrent("green,badcolor,purple");
   cu_assert(color.getCurrent() == goodMultivalue);
}

class TestParms : public ParameterGroup {
public:
    RangeParameter<int> size;
    ChoiceParameter<string> iceCream;
    AnyValueParameter<string> countries;

    TestParms(string command) :
	ParameterGroup(command),
	size("size", "meters", "size-help", 10, 1, 20),
	iceCream("icecream", "flavor", "ice-cream-help", "chocolate"),
	countries("countries", "", "countries-help", "usa")
    {
	addParam(size);

	iceCream.addChoice("vanilla");
	iceCream.addChoice("rockyroad");
	iceCream.setCurrent("vanilla");
	addParam(iceCream);

	iceCream.sort();

	countries.setCurrent("canada");
	addParam(countries);

	sort();

	// test adding duplicate param
	//addParam(size);
    }
};

void Testlib::testParameterGroup()
{
    cout << "testParameterGroup" << endl;
    TestParms parms("parms");

    cu_assert(parms.isValid());

    cout << "parms.show(size)" << endl;
    cout << parms.show("size", "") << endl;
    string testVal("15");
    cout << "set size to " << testVal << endl;
    cu_assert(parms.set("size", testVal) == testVal);

    cout << "parms.show(all)" << endl;
    cout << parms.show("all") << endl;

    cout << "parms output operator:" << endl;
    cout << parms << endl;

    cout << "parms help: " << endl;
    parms.help(cout);
}


void Testlib::testRound()
{

   // positive, round up
   double value(5.67);
   int numPlaces(1);
   double expectedValue(5.7);
   double tol(0.00001);

   double result(SseUtil::round(value, numPlaces));
   assertDoublesEqual(expectedValue, result, tol);

   // negative number
   value = -5.67;
   expectedValue = -5.7;
   result = SseUtil::round(value, numPlaces);
   assertDoublesEqual(expectedValue, result, tol);

   // round down
   numPlaces = 3;
   value = 12.12345;
   expectedValue = 12.123;
   result = SseUtil::round(value, numPlaces);
   assertDoublesEqual(expectedValue, result, tol);

   // test numPlaces = 0
   numPlaces = 0;
   value = 12.6;
   expectedValue = 13.0;
   result = SseUtil::round(value, numPlaces);
   assertDoublesEqual(expectedValue, result, tol);

}


void Testlib::testInterpLinear()
{
   cout << "testInterpLinear" << endl;

   InterpolateLinear testValues;

   // test empty table
   double askX = 999.0;
   double getY;
   bool status = testValues.inter(askX, getY);
   cu_assert(!status);

   // put in some values
   testValues.addValues(10.0, 100.0);
   testValues.addValues(5.0, 50.0);  // test out of order add
   testValues.addValues(20.0, 200.0);

   double tol(0.01);

   // check max & min y
   double expectedMaxY=200;
   assertDoublesEqual(expectedMaxY, testValues.maxY(), tol);

   double expectedMinY=50;
   assertDoublesEqual(expectedMinY, testValues.minY(), tol);

   // ask for x value before table start
   askX = 2.0;
   status = testValues.inter(askX, getY);
   cu_assert(!status);

   // ask for first value
   askX = 5.0;
   status = testValues.inter(askX, getY);
   cu_assert(status);
   double expectedY = 50.0;
   assertDoublesEqual(expectedY, getY, tol);

   // ask for intermediate value
   askX = 14.0;
   status = testValues.inter(askX, getY);
   cu_assert(status);
   expectedY = 140.0;
   assertDoublesEqual(expectedY, getY, tol);

   // ask for value near high end
   askX = 19.99;
   status = testValues.inter(askX, getY);
   cu_assert(status);
   expectedY = 199.9;
   assertDoublesEqual(expectedY, getY, tol);

   // ask for value off the high end
   askX = 21.0;
   status = testValues.inter(askX, getY);
   cu_assert(!status);

   cout << "interp table:\n"
        << testValues << endl;

}

void Testlib::testDbConvert()
{
   double tol(1e-6);

   double db=10;
   double expectedLinear=10;
   assertDoublesEqual(expectedLinear, SseUtil::dbToLinearRatio(db), tol);

   db=20;
   expectedLinear=100;
   assertDoublesEqual(expectedLinear, SseUtil::dbToLinearRatio(db), tol);

   db=-20;
   expectedLinear=0.01;
   assertDoublesEqual(expectedLinear, SseUtil::dbToLinearRatio(db), tol);

   double linear=10;
   double expectedDb=10;
   assertDoublesEqual(expectedDb, SseUtil::linearRatioToDb(linear), tol);

   linear=0.01;
   expectedDb=-20;
   assertDoublesEqual(expectedDb, SseUtil::linearRatioToDb(linear), tol);
}

Test *Testlib::suite ()
{
	TestSuite *testSuite = new TestSuite("Testlib");

	testSuite->addTest (new TestCaller<Testlib>("testFoo", &Testlib::testFoo));
	testSuite->addTest (new TestCaller<Testlib>("testUtils", &Testlib::testUtils));
	testSuite->addTest (new TestCaller<Testlib>("testRedirectStreamToFile", &Testlib::testRedirectStreamToFile));
	testSuite->addTest (new TestCaller<Testlib>("testPublisherSubcriber", &Testlib::testPublisherSubscriber));
	testSuite->addTest (new TestCaller<Testlib>("testTokenize", &Testlib::testTokenize));
	testSuite->addTest (new TestCaller<Testlib>("testSplitByDelimiter", &Testlib::testSplitByDelimiter));
	testSuite->addTest (new TestCaller<Testlib>("testLoadFileIntoStringVector", &Testlib::testLoadFileIntoStringVector));
	testSuite->addTest (new TestCaller<Testlib>("testParams", &Testlib::testParams));
	testSuite->addTest (new TestCaller<Testlib>("testMultiChoiceParams", &Testlib::testMultiChoiceParams));
	testSuite->addTest (new TestCaller<Testlib>("testParameterGroup", &Testlib::testParameterGroup));
	testSuite->addTest (new TestCaller<Testlib>("testRound", &Testlib::testRound));
        testSuite->addTest (new TestCaller<Testlib> ("testInterpLinear", &Testlib::testInterpLinear));
        testSuite->addTest (new TestCaller<Testlib> ("testDbConvert", &Testlib::testDbConvert));

	return testSuite;
}