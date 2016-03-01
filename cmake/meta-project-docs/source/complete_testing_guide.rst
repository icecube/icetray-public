Complete Testing Guide for IceCube
==================================

The purpose of these guidelines is to give some guidance as to what should be...

Test Overview
-------------
In order to run the tests you must have I3_TESTDATA set.  To get the test data, in 
your build directory, simply type :
::
   $ make rsync

That's pretty much it.


Python Tests
............
Below is a document that explains how to write tests for python classes or C++ 
classes with python bindings.

.. toctree::
   :titlesonly:

   test_guides/python_tests

Testing Scripts
...............
Below are instructions for how to write tests for your example scripts.  This
just tests that the example scripts runs without failing, which is useful, but
should **NOT** be a replacement for proper unit tests as a method to increase
test coverage.

.. toctree::
   :titlesonly:

   test_guides/testing_scripts

Testing I3Modules
.................
Below are instructions for how to write tests for your I3Modules.  You're
essentially going to want to test stuff that your module put in the frame
when running in an I3Tray instance.  The I3TestModuleFactory allows you
to write a python unittest and run it as a module in a chain.

.. toctree::
   :titlesonly:

   test_guides/testing_i3modules

C++ Tests
..........
If you have a C++ class that either has no python bindings or you want to test some 
class method that's not bound in C++, then you have to write tests in C++.  Check
out the docs on `I3Test <projects/cmake/i3test.html>`_.

Running the Tests
-----------------
To run the tests as they would be run on the build bots use **ctest**.
* Only run the tests for your project. 'ctest -R <project_name>'
* Note that this needs to be run from your build directory.

Make sure to check the `buildbots <http://builds.icecube.wisc.edu/>`_ and verify you didn't break anything on another platform.


Test Coverage
-------------
Check out the last coverage report to see what kind of coverage your project has.

`Latest Coverage Report <http://software.icecube.wisc.edu/coverage/00_LATEST/>`_

Coverage builds are run weekly, so if you want to see the effect your test had 
on your project's coverage, you'll need to be patient.
