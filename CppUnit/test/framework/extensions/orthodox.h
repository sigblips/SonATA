
#ifndef CPP_UNIT_ORTHODOX_H
#define CPP_UNIT_ORTHODOX_H

#include "CppUnit.h"

/*
 * Orthodox performs a simple set of tests on an arbitary
 * class to make sure that it supports at least the
 * following operations:
 *
 *      default construction    - constructor
 *      equality/inequality     - operator== && operator!=
 *      assignment              - operator=
 *      negation                - operator!
 *      safe passage            - copy construction
 *
 * If operations for each of these are not declared
 * the template will not instantiate.  If it does 
 * instantiate, tests are performed to make sure
 * that the operations have correct semantics.
 *      
 * Adding an orthodox test to a suite is very 
 * easy: 
 * 
 * public: Test *suite ()  {
 *     TestSuite *suiteOfTests = new TestSuite;
 *     suiteOfTests->addTest (new ComplexNumberTest ("testAdd");
 *     suiteOfTests->addTest (new orthodox<Complex> ());
 *     return suiteOfTests;
 *  }
 *
 * Templated test cases be very useful when you are want to
 * make sure that a group of classes have the same form.
 *
 * see TestSuite
 */


template <class T> class orthodox : public TestCase
{
protected:
    T               call (T t);
    void            runTest ();

public:
                    orthodox () : TestCase ("orthodox") {}

};


template <class T> void orthodox<T>::runTest ()
{
    // make sure we have a default constructor
    T   a, b, c;

    // make sure we have an equality operator
    assert (a == b);

    // check the inverse
    b.operator= (a.operator! ());
    assert (a != b);

    // double inversion
    b = !!a;
    assert (a == b);

    // invert again
    b = !a;

    // check calls
    c = a;
    assert (c == call (a));

    c = b;
    assert (c == call (b));

}


template <class T> T orthodox<T>::call (T t)
{
    return t;
}



#endif