/*******************************************************************************

 File:    testAngle.cpp
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


#include "Angle.h"
#include "TestCase.h"
#include "TestSuite.h"
#include "TextTestResult.h"
#include <sstream>

using namespace std;

template<class testClass>
class TestAngleClass  : public TestCase 
{
public:
   TestAngleClass(string name = "TestAngleClass") : TestCase(name)
   {
   }
   void runTest();
};

template<class testClass>
void TestAngleClass<testClass>::runTest()
{
   double const testValue = 1.2;
   double const delta = 0.001;
   testClass testClassTest(testValue);
   assertDoublesEqual(Radian(testValue * testClass::UnitToRadian).getRadian(),
                      Radian(testClassTest).getRadian(), delta);
}

class TestAngle : public TestCase  {
public:
   TestAngle(string name = "TestAngle") : TestCase(name) 
   {
   }
   void runTest();
   void testRadian();
   void testDegree();
   void testHour();
   void testDms();
   void testDm();
   void testHms();
   void testHm();
   void testRaDec();
   static Test *suite ();
};

void TestAngle::testRadian()
{
   double const testValue = 1.2;
   double const delta = 0.001;
   Radian radianTest(testValue);
   assertDoublesEqual(testValue, radianTest.getRadian(), delta);
   assertDoublesEqual(Degree(Radian(M_PI/2.0)).getDegree(), 90.0, delta);
}

void TestAngle::testDegree()
{
   double const testValue = 1.2;
   double const delta = 0.001;
   Degree degreeTest(testValue);
   assertDoublesEqual(testValue, degreeTest.getDegree(), delta);
   assertDoublesEqual(Radian(Degree(90.0)).getRadian(), M_PI/2.0, delta);
   assertDoublesEqual(Hour(Degree(90.0)).getHour(), 6.0, delta);
}

void TestAngle::testHour()
{
   double const testValue = 1.2;
   double const delta = 0.001;
   Hour hourTest(testValue);
   assertDoublesEqual(testValue, hourTest.getHour(), delta);
   assertDoublesEqual(Radian(Hour(6.0)).getRadian(), M_PI/2.0, delta);
   assertDoublesEqual(Degree(Hour(6.0)).getDegree(), 90.0, delta);
}

void TestAngle::testDms()
{
   stringstream angleString;
   angleString << Dms(Degree(32.5));
   cu_assert(angleString.str() == "  32:30:00");

   stringstream angleString2;
   angleString2 << Dms(Degree(-32.5));
   cu_assert(angleString2.str() == " -32:30:00");

   stringstream angleString3;
   angleString3 << Dms(Degree(-0.5));
   cu_assert(angleString3.str() == "  -0:30:00");
}

void TestAngle::testDm()
{
   stringstream angleString;
   angleString << Dm(Degree(32.5));
   cu_assert(angleString.str() == "  32:30");

   stringstream angleString2;
   angleString2 << Dm(Degree(-32.5));
   cu_assert(angleString2.str() == " -32:30");

   stringstream angleString3;
   angleString3 << Dm(Degree(32.51));
   cu_assert(angleString3.str() == "  32:31");

   stringstream angleString4;
   angleString4 << Dm(Degree(-32.51));
   cu_assert(angleString4.str() == " -32:31");
}

void TestAngle::testHms()
{
   stringstream angleString;
   angleString << Hms(Hour(11.5));
   cu_assert(angleString.str() == " 11:30:00");

   stringstream angleString2;
   angleString2 << Hms(Radian(Hour(11.5)));
   cu_assert(angleString2.str() == " 11:30:00");

   stringstream angleString6;
   Radian ra(0.171755);
   angleString6 << Hms(ra);
   cu_assert(angleString6.str() == " 00:39:22");

   stringstream angleString7;
   Radian ra1(4.028242);
   angleString7 << Hms(ra1);
   cu_assert(angleString7.str() == " 15:23:12");
}

void TestAngle::testHm()
{
   stringstream angleString;
   angleString << Hm(Hour(11.5));
   cu_assert(angleString.str() == " 11:30");

   stringstream angleString2;
   angleString2 << Hm(Hour(-11.5));
   cu_assert(angleString2.str() == "-11:30");

   stringstream angleString3;
   angleString3 << Hm(Hour(11.51));
   cu_assert(angleString3.str() == " 11:31");

   stringstream angleString4;
   angleString4 << Hm(Hour(-11.51));
   cu_assert(angleString4.str() == "-11:31");

   stringstream angleString5;
   angleString5 << Hm(Hour(1.37));
   cu_assert(angleString5.str() == "  1:22");
}

void TestAngle::testRaDec()
{
   stringstream angleString;
   angleString << RaDec(Hour(11.5), Degree(32.5));
   cu_assert(angleString.str() == " 11:30:00  32:30:00");

   stringstream angleString2;
   angleString2 << RaDec(Hour(-11.5), Degree(-32.5));
   cu_assert(angleString2.str() == " 12:30:00 -32:30:00");

   stringstream angleString3;
   angleString3 << RaDec(Hour(-0.5), Degree(-0.5));
   cu_assert(angleString3.str() == " 23:30:00  -0:30:00");
}

void TestAngle::runTest()
{
   testRadian();
   testDegree();
   testHour();
   testDms();
   testDm();
   testHms();
   testHm();
   testRaDec();
}

Test *TestAngle::suite()
{
   TestSuite *testsuite = new TestSuite;
   testsuite->addTest(new TestAngle ());
   testsuite->addTest(new TestAngleClass<Radian> ());
   testsuite->addTest(new TestAngleClass<Degree> ());
   testsuite->addTest(new TestAngleClass<Hour> ());
   return testsuite;
}

#include "TestRunner.h"
int main(int argc, char **argv)
{
   TestRunner runner;
   runner.addTest("TestAngle", TestAngle::suite());
   return runner.run(argc, argv);
}