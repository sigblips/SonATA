/*******************************************************************************

 File:    testCondition.cpp
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

// Test unit for sse package

#include "ace/Task.h"
#include "TestCase.h"
#include "TestSuite.h"
#include "TextTestResult.h"
#include "TestRunner.h"
#include "Condition.h"

/* In order to test our Condition we'll derive from ACE_Task<> so that
   we can have several threads accessing the condition variable
   together.
 */
template<class TypeClass>
class TestTask : public ACE_Task<ACE_NULL_SYNCH>
{
public:
    // TBD this is a repeat of the typedef in the NumericCondition
    // class to appease the gcc3.3 compiler
    typedef typename NumericCondition<TypeClass>::value_t Numeric_value_t;

     // Construct the condition variable with an initial value.
    TestTask( int max_threads, Numeric_value_t value )
        : max_threads_(max_threads), condition_(value)
    {}

    ~TestTask(void)
    {}

    // Seed the random number generator and start the threads.
    int open(void * args = 0)
    {
      seed_ = ACE_OS::gettimeofday().usec();
      
      ACE_OS::srand( seed_ );
     
      // This is not a place where we want to use THR_DETACHED.
      // We're going to be waiting for our threads and if we detach
      // them, we'll loose track and horrible things will happen.
      return this->activate(THR_NEW_LWP, max_threads_);
    } 
 
 

    TypeClass value() const
      {
	return condition_.value();
      }
    
protected:
     // Each thread will do work on the NumericCondition.
    int svc(void);

     // Override this method to modify the NumericCondition in some way.
    virtual void modify(void) = 0;

     // Override this to test the NumericCondition in some way.
    virtual void test(void) = 0;

     // How many threads to use in the test.  This is also used in the
     // modify() and test() methods of the derivatives.
    int max_threads_;

     // We want to sleep for a random amount of time to simulate
     // work.  The seed is necessary for proper random number generation.
    ACE_RANDR_TYPE seed_;

     // This is the actual condition variable set.
    NumericCondition<TypeClass> condition_;
};


/* Each thread will modify the condition variable in some way and then
   wait for the condition to be satisfied.  The derived classes
   overload modify() and test() to implement a specific test of the
   NumericCondition class.
 */
template<class TypeClass>
int TestTask<TypeClass>::svc(void)
{
        // Take a moment before we modify the condition.  This will
        // cause test() in other threads to delay a bit.
    //int stime = ACE_OS::rand_r( seed_ ) % 5;
    //    ACE_OS::sleep(abs(stime)+2);

    // ACE_DEBUG ((LM_INFO, "(%P|%t|%T)\tTest::svc() before modify, condition_ is:  %d\n", (int)condition_ ));

     // Change the condition variable's value
    modify();

    // ACE_DEBUG ((LM_INFO, "(%P|%t|%T)\tTest::svc() after modify, condition_ is:  %d\n", (int)condition_ ));

     // Test for the condition we want
    test();

    // ACE_DEBUG ((LM_INFO, "(%P|%t|%T)\tTest::svc() leaving.\n" ));

    return(0);
}

/* Test NumericCondition::operator!=()
   The task's svc() method will increment the condition variable and
   then wait until the variable's value reaches max_threads_.
 */
template<class TypeClass>
class TestNe : public TestTask<TypeClass>
{
 public:
  // Initialize the condition variable to zero since we're counting up.
  TestNe( int max_threads )
    : TestTask<TypeClass>(max_threads,0)
    {
      // ACE_DEBUG ((LM_INFO, "\n(%P|%t|%T)\tTesting condition_ != %d\n", max_threads_));
    }

  // modify the variable
  virtual void modify(void)
    {
      ++(this->condition_);
    }

  // Wait until it equals max_threads_
  virtual void test(void)
    {
      this->condition_.waitWhileNe(this->max_threads_);
    }
};

template<class TypeClass>
class TestEq : public TestTask<TypeClass>
{
 public:
  // Initialize the condition variable to zero since we're counting up.
  TestEq( int max_threads )
    : TestTask<TypeClass>(max_threads,max_threads)
    {
      // ACE_DEBUG ((LM_INFO, "\n(%P|%t|%T)\tTesting condition_ == %d\n", max_threads_));
    }

  // modify the variable
  virtual void modify(void)
    {
      --(this->condition_);
    }

  // Wait until it equals max_threads_
  virtual void test(void)
    {
      this->condition_.waitWhileEq(this->max_threads_);
    }
};


template<class TypeClass>
class TestGe : public TestTask<TypeClass>
{
 public:
  // Initialize the condition variable to zero since we're counting up.
  TestGe( int max_threads )
    : TestTask<TypeClass>(max_threads, 2 * max_threads - 1)
    {
      // ACE_DEBUG ((LM_INFO, "\n(%P|%t|%T)\tTesting condition_ >= %d\n", max_threads_));
    }

  // modify the variable
  virtual void modify(void)
    {
      --(this->condition_);
    }

  // Wait until it equals max_threads_
  virtual void test(void)
    {
      this->condition_.waitWhileGe(this->max_threads_);
    }
};

template<class TypeClass>
class TestGt : public TestTask<TypeClass>
{
 public:
  // Initialize the condition variable to zero since we're counting up.
  TestGt( int max_threads )
    : TestTask<TypeClass>(max_threads, 2 * max_threads)
    {
      // ACE_DEBUG ((LM_INFO, "\n(%P|%t|%T)\tTesting condition_ > %d\n", max_threads_));
    }

  // Modify the variable
  virtual void modify(void)
    {
      --(this->condition_);
    }

  // Wait until it equals max_threads_
  virtual void test(void)
    {
      this->condition_.waitWhileGt(this->max_threads_);
    }
};

template<class TypeClass>
class TestLe : public TestTask<TypeClass>
{
 public:
  // Initialize the condition variable to zero since we're counting up.
  TestLe( int max_threads )
    : TestTask<TypeClass>(max_threads, 1)
    {
      // ACE_DEBUG ((LM_INFO, "\n(%P|%t|%T)\tTesting condition_ <= %d\n", max_threads_));
    }

  // modify the variable
  virtual void modify(void)
    {
      ++(this->condition_);
    }

  // Wait until it equals max_threads_
  virtual void test(void)
    {
      (this->condition_).waitWhileLe(this->max_threads_);
    }
};

template<class TypeClass>
class TestLt : public TestTask<TypeClass>
{
 public:
  // Initialize the condition variable to zero since we're counting up.
  TestLt( int max_threads )
    : TestTask<TypeClass>(max_threads, 0)
    {
      // ACE_DEBUG ((LM_INFO, "\n(%P|%t|%T)\tTesting condition_ <= %d\n", max_threads_));
    }

  // modify the variable
  virtual void modify(void)
    {
      ++(this->condition_);
    }

  // Wait until it equals max_threads_
  virtual void test(void)
    {
      this->condition_.waitWhileLt(this->max_threads_);
    }
};


class ConditionTest  : public TestCase
{
public:
  ConditionTest(string name = "ConditionTest") : TestCase(name)
    {
    }
  
  void testNe();
  void testEq();
  void testLe();
  void testLt();
  void testGe();
  void testGt();
  void runTest();
  static Test *suite();
};


void ConditionTest::testNe()
{
  TestNe<int> testNe(5);
  testNe.open();
  testNe.wait();
  assertLongsEqual(testNe.value(), 5);
  
}

void ConditionTest::testEq()
{
  TestEq<int> testEq(5);
  testEq.open();
  testEq.wait();
  assertLongsEqual(testEq.value(), 0);
  
}

void ConditionTest::testGe()
{
  TestGe<int> testGe(5);
  testGe.open();
  testGe.wait();
  assertLongsEqual(testGe.value(), 4);
  
}

void ConditionTest::testGt()
{
  TestGt<int> testGt(5);
  testGt.open();
  testGt.wait();
  assertLongsEqual(testGt.value(), 5);
  
}

void ConditionTest::testLe()
{
  TestLe<int> testLe(5);
  testLe.open();
  testLe.wait();
  assertLongsEqual(testLe.value(), 6);
  
}

void ConditionTest::testLt()
{
  TestLt<int> testLt(5);
  testLt.open();
  testLt.wait();
  assertLongsEqual(testLt.value(), 5);
  
}

void ConditionTest::runTest()
{
  testNe();
  testEq();
  testGe();
  testGt();
  testLe();
  testLt();
}


Test *ConditionTest::suite()
{
  TestSuite *testsuite = new TestSuite;
  testsuite->addTest(new ConditionTest ());
  return testsuite;
}


/* 
 * Driver 
 */

int main (int argc, char **argv)
{
    TestRunner runner;
    runner.addTest ("ConditionTest ", ConditionTest::suite ());
    return runner.run (argc, argv);
}