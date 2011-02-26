/*******************************************************************************

 File:    RangeTest.cpp
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


#include "Range.h"
#include "TestCase.h"
#include "TestSuite.h"
#include "TextTestResult.h"
#include "sseDxInterface.h"
#include <sstream>

using namespace std;

class RangeTest : public TestCase  {
public:
   RangeTest(string name = "RangeTest") : TestCase(name) 
   {
   }
   void runTest();
   void testAddInOrder();
   void testAddOutOfOrder();
   void testRangeMinusObs();
   void testObsMinusRange();
   void testObsRangeMinusObsRange();
   void testAboveRange();
   void testTotalRange();
   void testRemoveAllRanges();
   void testRemoveRangesLessThanWidth();
   void testSubtractFreqBands();
   void testMinMaxValue();
   void testIsIncluded();
   void testHasRangeGt();
   void testRangeOutputOp();
   void testMisc();

   static Test *suite ();
};

void RangeTest::runTest()
{
   testAddInOrder();
   testAddOutOfOrder();
   testRangeMinusObs();
   testObsMinusRange();
   testObsRangeMinusObsRange();
   testAboveRange();
   testTotalRange();
   testRemoveAllRanges();
   testRemoveRangesLessThanWidth();
   testSubtractFreqBands();
   testMinMaxValue();
   testIsIncluded();
   testHasRangeGt();
   testRangeOutputOp();
   testMisc();
}


void RangeTest::testAddInOrder()
{
   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.125, 1005.250);
      obsrange.addInOrder(1002.1, 1010.3);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() == "1000.125000-1010.300000");
   }
   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addInOrder(1010.1, 1020.3);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"1000.100000-1005.200000 1010.100000-1020.300000");
   }
}

void RangeTest::testAddOutOfOrder()
{
   {
      // added range falls fully between existing ranges
      ObsRange obsrange;
      obsrange.addInOrder(1020.0, 1030.0);
      obsrange.addInOrder(1050.0, 1060.0);
      obsrange.addOutOfOrder(1040.123456, 1045.0);

      stringstream rangeStream;
      rangeStream << obsrange;

      cu_assert(rangeStream.str() ==
		"1020.000000-1030.000000 1040.123456-1045.000000 1050.000000-1060.000000");
   }


   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addInOrder(1010.1, 1020.3);
      obsrange.addOutOfOrder(990.0, 1001.0);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"990.000000-1005.200000 1010.100000-1020.300000");
   }
   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addInOrder(1010.1, 1020.3);
      obsrange.addOutOfOrder(980.0, 990.0);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"980.000000-990.000000 1000.100000-1005.200000 1010.100000-1020.300000");
   }
   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addInOrder(1010.1, 1020.3);
      obsrange.addOutOfOrder(980.0, 1001.0);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"980.000000-1005.200000 1010.100000-1020.300000");
   }

   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addInOrder(1010.1, 1020.3);
      obsrange.addOutOfOrder(980.0, 1011.0);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"980.000000-1020.300000");
   }


   {
      // add out of order range that covers all old ranges
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addInOrder(1010.1, 1020.3);
      obsrange.addOutOfOrder(520.0, 1030.5);

      stringstream rangeStream;
      rangeStream << obsrange;

      cu_assert(rangeStream.str() ==
		"520.000000-1030.500000");
   }

   {
      ObsRange obsrange;
      obsrange.addInOrder(1000.1, 1005.2);
      obsrange.addOutOfOrder(1010.1, 1011.1);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"1000.100000-1005.200000 1010.100000-1011.100000");
   }

   {
      ObsRange obsrange;
      obsrange.addInOrder(1200.0, 1650.2);
      obsrange.addOutOfOrder(1650.1, 1651.5);
      stringstream rangeStream;
      rangeStream << obsrange;
      cu_assert(rangeStream.str() ==
		"1200.000000-1651.500000");
   }



   // add to empty obsrange
   {
       ObsRange obsrange;
       obsrange.addOutOfOrder(1650.1, 1651.5);
       stringstream rangeStream;
       rangeStream << obsrange;
       cu_assert(rangeStream.str() ==
		 "1650.100000-1651.500000");
   }

}


void RangeTest::testRangeMinusObs()
{
   {
      // empty obs range
      ObsRange right;
      Range left(1000.0, 1200.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1000.000000-1200.000000");
   }

   {
      ObsRange right;
      right.addInOrder(1005.0, 1010.0);
      Range left(1000.0, 1200.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1000.000000-1005.000000 1010.000000-1200.000000");
   }
  
   {
      ObsRange right;
      right.addInOrder(995.0, 1010.0);
      Range left(1000.0, 1200.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1010.000000-1200.000000");
   }
  
   {
      ObsRange right;
      right.addInOrder(1190.0, 1210.0);
      Range left(1000.0, 1200.0);

      ObsRange result = left - right;
      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1000.000000-1190.000000");
   }

   {
      ObsRange right;
      right.addInOrder(1000.0, 1200.0);
      right.addInOrder(1400.0, 1600.0);
      right.addInOrder(2500.0, 3000.0);
      Range left(1500.0, 1800.0);

      ObsRange result = left - right;
      stringstream rangeStream;
      rangeStream << result;

      cout << "rangeminusobs: " << rangeStream.str() << endl;

      cu_assert(rangeStream.str() == "1600.000000-1800.000000");
   }



}


void RangeTest::testObsMinusRange()
{
   {
      ObsRange left;
      left.addInOrder(1005.0, 1010.0);
      left.addInOrder(1015.0, 1020.0);
      Range right(900.0, 1000.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1005.000000-1010.000000 1015.000000-1020.000000");
   }
   {
      ObsRange left;
      left.addInOrder(1005.0, 1010.0);
      left.addInOrder(1015.0, 1020.0);
      Range right(900.0, 1007.0);

      ObsRange result = left - right;
      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1007.000000-1010.000000 1015.000000-1020.000000");
   }
   {
      ObsRange left;
      left.addInOrder(1005.0, 1010.0);
      left.addInOrder(1015.0, 1020.0);
      Range right(1017.0, 1030.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1005.000000-1010.000000 1015.000000-1017.000000");
   }
   {
      ObsRange left;
      left.addInOrder(1005.0, 1010.0);
      left.addInOrder(1015.0, 1020.0);
      Range right(1007.0, 1017.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1005.000000-1007.000000 1017.000000-1020.000000");
   }
   {
      ObsRange left;
      left.addInOrder(1258.5, 1264.9);
      Range right(1255.0, 1265.0);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "");
   }
   {
      ObsRange left;
      left.addInOrder(1258.5, 1264.9);
      Range right(1255.0, 1264.9);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "");
   }
   {
      ObsRange left;
      left.addInOrder(1258.5, 1264.9);
      Range right(1258.5, 1264.9);

      ObsRange result = left - right;
      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "");
   }
   {
      ObsRange left;
      Range right(1258.5, 1264.9);
      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "");
   }
}

void RangeTest::testObsRangeMinusObsRange()
{
   {
      ObsRange left;
      ObsRange right;

      left.addInOrder(1250, 1260);
      right.addInOrder(1255, 1260);

      ObsRange result = left - right;

      stringstream rangeStream;
      rangeStream << result;
      cu_assert(rangeStream.str() == "1250.000000-1255.000000");
   }

}


void RangeTest::testAboveRange()
{
   ObsRange obsRange;
   obsRange.addInOrder(1005.0, 1010.0);
   obsRange.addInOrder(1015.0, 1020.0);

   list<Range>::const_iterator index = obsRange.aboveRange(1000.0);
   cu_assert((*index).low_ == 1005.0 && (*index).high_ == 1010.0);

   index = obsRange.aboveRange(1012.0);
   cu_assert((*index).low_ == 1015.0 && (*index).high_ == 1020.0);

   index = obsRange.aboveRange(1006.0);
   cu_assert((*index).low_ == 1005.0 && (*index).high_ == 1010.0);

   index = obsRange.aboveRange(1025.0);
   cu_assert(index == obsRange.rangeEnd());

   // test const iterator version
   const ObsRange constObsRange = obsRange;
   list<Range>::const_iterator constIndex = constObsRange.aboveRange(1025.0);
   cu_assert(constIndex == constObsRange.rangeEnd());
}


void RangeTest::testTotalRange()
{
   cout << "RangeTest::testTotalRange()" << endl;

   ObsRange obsRange;
   obsRange.addInOrder(1005.0, 1010.0);  // 5
   obsRange.addInOrder(1015.0, 1020.0);  // 5
   obsRange.addInOrder(1060.0, 1080.0);  // 20
   obsRange.addInOrder(1100.0, 1130.0);  // 30


   double totalRange = obsRange.totalRange();
   double expectedTotal = 60;
   double tol = 0.1;
   assertDoublesEqual(expectedTotal, totalRange, tol);

   // find sum of all elements that are above a given size

   cout << "RangeTest::testTotalRange() totalRangeGt" << endl;

   double minSize = 15.0;
   expectedTotal = 50;
   double totalRangeGt = obsRange.totalRangeGt(minSize);
   assertDoublesEqual(expectedTotal, totalRangeGt, tol);


}

void RangeTest::testRemoveRangesLessThanWidth()
{
   cout << "RangeTest::testRemoveRangesLessThanWidth()" << endl;

   ObsRange obsRange;

   obsRange.addInOrder(1005.0, 1010.0);  // 5
   obsRange.addInOrder(1015.0, 1020.0);  // 5
   obsRange.addInOrder(1060.0, 1080.0);  // 20
   obsRange.addInOrder(1100.0, 1130.0);  // 30
   obsRange.addInOrder(1135.0, 1137.0);  // 2
   obsRange.addInOrder(1200.0, 1230.0);  // 30

   obsRange.removeRangesLtWidth(5.1);

   stringstream rangeStream;
   rangeStream << obsRange;

//  cout << "testRemoveRangesLessThanWidth(): output is: " 
//       << obsRange << endl;
  
   cu_assert(rangeStream.str() ==
	     "1060.000000-1080.000000 1100.000000-1130.000000 1200.000000-1230.000000");

}


void RangeTest::testRemoveAllRanges()
{
   cout << "RangeTest::testRemoveAllRanges()" << endl;

   ObsRange obsRange;

   obsRange.addInOrder(1005.0, 1010.0); 
   obsRange.addInOrder(1015.0, 1020.0); 

   stringstream rangeStream;
   rangeStream << obsRange;
   cu_assert(rangeStream.str() == "1005.000000-1010.000000 1015.000000-1020.000000");

   obsRange.removeAllRanges();

   stringstream emptyRangeStream;
   emptyRangeStream << obsRange;
   cu_assert(emptyRangeStream.str() == "");

}


void RangeTest::testSubtractFreqBands()
{
   cout << "RangeTest::testSubtractFreqBands()" << endl;

   ObsRange obsRange;

   obsRange.addInOrder(1000.0, 2000.0); 
   obsRange.addInOrder(3000.0, 5000.0); 
   obsRange.addInOrder(7000.0, 9000.0);

   stringstream rangeStream;
   rangeStream << obsRange;

   string expected("1000.000000-2000.000000 3000.000000-5000.000000 7000.000000-9000.000000");
   cu_assert(rangeStream.str() == expected);

   vector <FrequencyBand> maskList;
   FrequencyBand band1;
   band1.centerFreq = 1500;
   band1.bandwidth = 200;
   maskList.push_back(band1);

   FrequencyBand band2;
   band2.centerFreq = 2500;
   band2.bandwidth = 1200;
   maskList.push_back(band2);

   obsRange = obsRange - maskList;
   stringstream subtractedStream;
   subtractedStream << obsRange;

   string expectedSubtracted("1000.000000-1400.000000 1600.000000-1900.000000 3100.000000-5000.000000 7000.000000-9000.000000");
   cu_assert(subtractedStream.str() == expectedSubtracted);

}

void RangeTest::testMinMaxValue()
{
   ObsRange obsrange;
   obsrange.addInOrder(1000.1, 1005.2);
   obsrange.addInOrder(1002.1, 1010.3);

   double tol=0.1;
   assertDoublesEqual(1000.1, obsrange.minValue(), tol);
   assertDoublesEqual(1010.3, obsrange.maxValue(), tol);

}

void RangeTest::testMisc()
{
   ObsRange obsRange;

   cu_assert(obsRange.isEmpty());

}

void RangeTest::testIsIncluded()
{
   ObsRange obsrange;
   obsrange.addInOrder(1000.1, 1005.2);
   obsrange.addInOrder(1010.1, 1020.3);

   // single value included   
   cu_assert(obsrange.isIncluded(1012.5));
   cu_assert(! obsrange.isIncluded(1007.0));
   
   // range included
   cu_assert(obsrange.isIncluded(Range(1001,1002)));
   cu_assert(! obsrange.isIncluded(Range(1019,1022)));
   cu_assert(! obsrange.isIncluded(Range(2000,3000)));
}

void RangeTest::testHasRangeGt()
{
   ObsRange obsrange;
   obsrange.addInOrder(1000.1, 1005.2);
   obsrange.addInOrder(1010.1, 1020.3);

   cu_assert(obsrange.hasRangeGt(2));
   cu_assert(! obsrange.hasRangeGt(100));
}

void RangeTest::testRangeOutputOp()
{
   Range range(1005,1020);

   stringstream strm;
   strm << range;

   string expected("1005.000000-1020.000000");
   cu_assert(strm.str() == expected);

}


Test *RangeTest::suite()
{
  TestSuite *testsuite = new TestSuite;
  testsuite->addTest(new RangeTest());
  return testsuite;
}


#include "TestRunner.h"
int main(int argc, char **argv)
{
   TestRunner runner;
   runner.addTest("RangeTest", RangeTest::suite());
   return runner.run(argc, argv);
}

