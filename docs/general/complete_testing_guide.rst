Complete Testing Guide for IceCube
==================================

The purpose of these guidelines is to give some guidance as to what should be...

Test Overview
-------------
In order to run the tests you must have :envvar:`I3_TESTDATA` set.  To get the test data, in
your build directory, simply type::

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
out the docs on :ref:`I3Test <cmake-i3test>`.

Running the Tests
-----------------
To run the tests as they would be run on the build bots use :command:`ctest`.

* Only run the tests for your project. :command:`ctest -R <project_name>`
* Note that this needs to be run from your build directory.
* Note that you need to be in the env shell (run :command:`env-shell.sh`) for both C++ and Python tests

Make sure to check the `workflows <https://github.com/icecube/icetray/actions>`_ and verify you didn't break anything.

Test Coverage
-------------
Check out the last coverage report to see what kind of coverage your project has.

.. warning::

   Periodic coverage builds are not currently implemented. You can
   still generate coverage reports locally though.

.. comment
   `Latest Coverage Report <http://software.icecube.wisc.edu/coverage/00_LATEST/>`_

Coverage builds are run weekly, so if you want to see the effect your test had 
on your project's coverage, you'll need to be patient. If you don't
want to be patient, and would rather generate coverage reports
locally, read on.

.. toctree::
   :titlesonly:

   test_guides/testing_coverage
   test_guides/testing_python_coverage

Adding Test Data
----------------

The test data is downloaded from the svn repository https://code.icecube.wisc.edu/test-data/trunk/.
When you write tests which need need additional test data please commit it to this repository.

Please see the `README <https://code.icecube.wisc.edu/test-data/trunk/README>`_ for instructions on how to
commit new test data to the repository.
