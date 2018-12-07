.. _cmake-i3test:

I3Test Reference
================

.. highlight:: cpp

Overview
--------

The best way to verify the correctness of an implementation is to
fully exercise all functionality with an independent
implementation. "Thorough" means: a run of the test suite to
completion without error should be enough to convince an aggressive
skeptic that the implementation is correct. The independent
implementation itself should be so simple that it is plausibly
correct.  (This specification from the Library of Practical
Abstractions manual, see References at the bottom of this document.)

One should provide test suites for all code. Test suites should
exercise the interface of a class in normal and borderline cases. They
should demonstrate what it is possible to do with a class, and that a
class behaves reasonably (correctly signals error conditions) when
misused.

A test suite should run to completion in a reasonable amount of
time. The complexity of a test suite will of necessity be proportional
to the complexity of the behavior of what it is testing. One of the
main guidelines of object-oriented design is "one class, one
responsibility". A class' tests verify its' ability to assume this
responsibility: if it is hard to write a set of tests for a class
because it does so many things, this indicates that the class should
probably be broken up into smaller pieces, each with a clearly defined
purpose and interface.

**I3Test** allows anyone capable of running the command "make" to
identify at a glance exactly what failed, including file, line number,
the expression that failed, and optional user-defined
messages. Intimate knowledge of the software under testing is
unnecessary.  It provides a uniform interface to testing. This allows
developers to verify that they have not broken one anothers' code when
modify their own, and allows you to do the same.  It also reduces the
amount of "boilerplate" required to write tests.

Test suites as documentation, and insurance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Test suites serve several additonal purposes. They allow people other
than the original author to fix bugs in modules with confidence that
they have not introduced additional bugs. They also serve as excellent
documentation for how to use modules: if a testsuite is thorough, it
demonstrates normal uses, borderline cases and errors, and can be an
excellent reference if you are trying to figure out how to use the
class yourself.

They have the additional advantage over documenation that they will be
verified to work before each release. Unlike documentation, they
cannot get out of sync with the actual code.

Time invested in writing test suites is well invested.

Regression tests: submit bugfixes with a test for the bug.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This isn't regression as in 'least squares curve fitting', this is
regression as the opposite of progression.  Bugs reappear. Regression
tests provide a way to automatically know what bugs have done
so. Often the easiest way to track down a bug is to write a little
program that produces it, and then hack at the code exercised by the
little program, until the error condition is fixed.

This little program is precisely a regression test. If you stop using
it when you have fixed the bug, you have wasted your time and created
conditions where you will likely have to track down the bug again in
the future. Formulate this little program as a test, however, and it
will continue to work for you for the rest of the life of your
code. As you encounter and fix bugs in your code, your base of
testsuites will grow, and the amount of time you spend debugging will
shrink.

Testing Interface
-----------------

Example
^^^^^^^

Here is a file containing test group named example_group containing
three unit tests named *it_works*, *pinkness_is_almost_pi*, and
*this_one_fails* ::

  #include <I3Test.h>
  
  TEST_GROUP(example_group);
  
  TEST(it_works)
  {
    bool it_works = true;
    ENSURE(it_works); 
  }
  
  TEST(pinkness_is_4)
  {
    double pinkness = 4.0;
    double redness = 4.0;
    ENSURE_EQUAL(pinkness, redness, "pinkness and redness not equal");
  }
  
  TEST(pinkness_is_almost_pi)
  {
    double pinkness = 4;
    ENSURE_DISTANCE(pinkness, M_PI, 1.0, "pinkness not within 1 of pi");
  }
  
  TEST(this_one_fails)
  {
    FAIL("this will fail unconditionally");
  }


The unit tests signal success or failure using the statement
``ENSURE()``, ``ENSURE_DISTANCE()``, or ``FAIL()``.

Build configuration
^^^^^^^^^^^^^^^^^^^

A test group is contained in one file, which contains any number of
uniquely named unit tests. A directory containing one or more test
groups, plus a ``main()`` routine, is called a test suite. One informs
the build system which directories contain test groups via
:ref:`i3_test_executable() <i3_test_executable()>` (in the project's CMakeCache.txt).

.. index:: I3TestMain.ixx 

One of these files must contain the main routine for the test
driver. To do so, place one file (main.cxx is a reasonable name) in
the test suite directory containing only the line::

  #include <I3TestMain.ixx>

This :file:`I3TestMain.ixx` contains what is required.

Each test group (file) must include the statement::

  #include <I3Test.h>

which pulls in definitions for ``TEST_GROUP``, ``ENSURE``, etc. (see below). 

.. c:macro:: TEST_GROUP(groupname)

Valid context: toplevel scope of implementation files in test suite
directory.

This signals to the build system that this file contains a TEST_GROUP
with the name groupname. groupname must be a valid C++ identifier name
(alphanumeric plus underscore, starting with letter or underscore.)

The individual unit tests found in this file will be organized under
this test group.
	
.. c:macro:: TEST(testname)

Valid context: toplevel scope of implementation files in test suite
directory.

``TEST(testname)`` defines a unit test named testname. As with
``TEST_GROUP()``, testname must be a valid C++ identifier name.

``TEST(t)`` looks much like a function definition (which it becomes after
``TEST()`` is expanded by the preprocessor). It is immediately followed by
a scope (open curly-brace, statements, close curly-brace), containing
testing statements.
	
.. c:macro:: ENSURE(predicate [, comment] )

``ENSURE`` is analogous to ``assert()``. It takes one argument, a predicate,
and an optional comment.  ``ENSURE`` checks whether the predicate is true
or false. If it is false, it will return a test failure. If the
optional comment is included as a string, it will return it along with
the failure.  

.. note:: 

   A predicate is simply an expression that evaluates to true or
   false. Comparisons of two numbers, e.g. numhits>50 or pinkness=4, a
   call of a function that returns bool or something convertable to
   bool, e.g. ``hits_are_ok();``, and evaluation of pointers for
   nullness are all predicates.

.. c:macro:: ENSURE_EQUAL(left-value, right-value, [, comment])

ENSURE_EQUAL ensures that left-value and right-value are, well,
equal. If they aren't, it will throw a test failure. If the optional
comment is specified, the failure will come with that message.

Why not just::

  ENSURE(something == something_else);

you ask. If this ENSURE fails, you will get only the error message::

  File:      private/test/sample_error.cxx
  Line:      17
  Predicate: something == something_else

Whereas ENSURE_EQUAL will show::

  File:      private/test/SampleClassTest.cxx
  Line:      24
  Predicate: ensure_distance: expected [0.16665;0.16667] actual 0.3333333333

Which gives one a better idea what the problem is. 

.. c:macro:: ENSURE_DISTANCE(left-value, right-value, distance [, comment])

ENSURE_DISTANCE verifies that left-value is within distance of
right-value. If it is not, it throws a test failure. If the optional
comment is specified, the failure will come with that messages.

.. c:macro:: FAIL(comment)

This fails unconditionally. Useful when verifying that an operation
should throw, or that a certain point in the code is never reached::

  try {
    operation_that_throws();
    FAIL("that should have thrown");
  } catch (const std::exception& e) {
    // NB: DO NOT catch-all, meaning NEVER, EVER, EVER 'catch(...)'
    //     In that case your test will alway pass and test nothing.
    // OK.  That operation threw and therefore we didn't hit FAIL()
  }

Even better::
  EXPECT_THROW(operation_that_throws);    
  

Building and running I3Tests
----------------------------

See :ref:`targets` for how to build and run.  Once built the test driver binaries
will be in the ``bin/`` directory of the build.

The test driver command line interface
--------------------------------------

.. highlight:: none

One may also run tests manually. The test driver programs have
command-line help and a robust set of arguments. With no arguments, a
test driver will show the following help::

  % icetray-test --help
  I3 Test Suite Options Summary:
    -h [ --help ]          : this message
    -f [ --fork ]          : fork() before each test (all tests will run 
                             even if one dumps core)
    -a [ --all ]           : run all tests
    -x [ --xml ] arg       : run all tests, output xml to FILE (implies --fork 
                             and --timeout 600)
    -s [ --show-spew ]     : pass along any thing the test spews 
                             to cout/cerr (you want to see it)
    -l [ --list ]          : list tests and groups in this suite
    --run-tests arg        : list of tests to run
    -t [ --timeout ] arg   : timeout unit tests after this many seconds

One may specify short or long arguments in typical UNIX command-line utility style.
Test driver arguments in detail (default: off)

References
----------

::

  Eric Sven Ristad and Peter N. Yianilos
  Library of Practical Abstractions
  http://www.pnylab.com/pny/software/libpa/main.html
  February 1998

