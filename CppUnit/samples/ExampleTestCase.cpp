

#include "ExampleTestCase.h"
#include "TestRunner.h"

void ExampleTestCase::example ()
{
	assertDoublesEqual (1.0, 1.1, 0.05);
	cu_assert (1 == 0);
	cu_assert (1 == 1);
}


void ExampleTestCase::anotherExample ()
{
	cu_assert (1 == 2);
}

void ExampleTestCase::setUp ()
{
	m_value1 = 2.0;
	m_value2 = 3.0;
}

void ExampleTestCase::testAdd ()
{
	double result = m_value1 + m_value2;
	assertDoublesEqual (result, 6.0, 0.05);
}


void ExampleTestCase::testDivideByZero ()
{
	int	zero	= 0;
	int result	= 8 / zero;
}


void ExampleTestCase::testThrowUnexpectedException()
{
  throw 1;
}

void ExampleTestCase::testThrowStandardException()
{
  throw exception();
}

void ExampleTestCase::testEquals ()
{
	auto_ptr<long>	long1 (new long (12));
	auto_ptr<long>	long2 (new long (12));

	assertLongsEqual (12, 12);
	assertLongsEqual (12L, 12L);
	assertLongsEqual (*long1, *long2);

	cu_assert (12L == 12L);
	assertLongsEqual (12, 13);
	assertDoublesEqual (12.0, 11.99, 0.5);



}



Test *ExampleTestCase::suite ()
{
	TestSuite *testSuite = new TestSuite("ExampleTestCase");

	testSuite->addTest (new TestCaller <ExampleTestCase> ("anotherExample", &ExampleTestCase::anotherExample));
	testSuite->addTest (new TestCaller <ExampleTestCase> ("testAdd", &ExampleTestCase::testAdd));
	//	testSuite->addTest (new TestCaller <ExampleTestCase> ("testDivideByZero", &ExampleTestCase::testDivideByZero));
	testSuite->addTest (new TestCaller <ExampleTestCase> ("testThrowUnexpectedException", &ExampleTestCase::testThrowUnexpectedException));
	testSuite->addTest (new TestCaller <ExampleTestCase> ("testThrowStandardException", &ExampleTestCase::testThrowStandardException));
	testSuite->addTest (new TestCaller <ExampleTestCase> ("testEquals", &ExampleTestCase::testEquals));

	return testSuite;
}
