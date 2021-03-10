.. -*- mode:rst; mode:flyspell; mode:fci; mode:auto-fill -*-

Running C++ Coverage Locally
============================

.. highlight:: console

Coverage reports can be generated locally quite easily. The only 2
pre-requisites are:

  gcov
    this almost always comes for free with GNU's gcc/g++
  lcov
    this can be installed via your package manager

NB : Set the COVERAGE cmake flag. (e.g. "cmake ../src/ -DCOVERAGE=True").

Example::

  $ cd <build_dir>
  $ cmake <src_dir> -DCOVERAGE=True
  $ make
  $ make test-bins
  $ make coverage

Once ``make coverage`` has completed, you can find the coverage report
in ``$I3_BUILD/../output/00_LATEST``. You'll notice that this report
took a long time to build, and covers your entire
workspace/meta-project. If you're only interested in a particular
subset of packages. For this next example let's say we're only
interested in coverage reports for :ref:`icetray` and
:ref:`dataclasses`.

First we'll need to install ``lcov``.

For OS X (with homebrew)::

  $ brew install lcov

For Ubuntu::

  $ apt-get install lcov

For Scientific Linux (or CentOS, or Fedora)::

  $ yum install lcov

Once that's done we can get to work. We'll need to build the bits we
want to get a report on (:ref:`icetray` and :ref:`dataclasses`), make sure our
stats are zeroed out, run the tests, collect our stats, and finally
generate the report.

First, we make the bits we want to check::

  $ cd $I3_BUILD
  $ make icetray dataclasses
  $ make icetray-pybindings dataclasses-pybindings
  $ make icetray-test dataclasses-test

When that's completed successfully, we need to zero out our stats::

  $ cd $I3_BUILD
  $ lcov -d . -z
  $ lcov -d . -c -i -o test_base.info

You'll see some output from ``lcov`` about it scanning ``*.gcno``
files. This is good. Next, we run our tests. That's the whole point,
right? We want to ensure that our tests completely exercise our code.

::

   $ ./env-shell.sh ctest -R 'icetray::' -R 'dataclasses::' -j2

Now, at this point, if running the tests have failed, you need to go
back and figure out why they failed, fix them, go back to the previous
step and re-zero our stats, and then re-run the tests. Continuing with
failed tests is going to give us erroneous results in our coverage
report.

Assuming your tests have completed successfully, let's capture the
results of running the tests.

::

  $ lcov -d . -c -o test_run.info

Let's generate our coverage report. First, we need to combine our
zeroed-stat baseline with the results of running the tests.

::

   $ lcov -d . -a test_base.info -a test_run.info -o test_total.info

Secondly, we'll want to prune some files from the report. We're not
interested in things like standard headers or boost.

::

   $ lcov -r test_total.info '/usr/include/*' -o reports.info

Now, finally, we generate our HTML report.

::

   $ mkdir $I3_BUILD/coverage
   $ genhtml --legend -o $I3_BUILD/coverage reports.info

The resulting report is in standard HTML and can be viewed locally in
your web browser.

On OS X::

  $ open $I3_BUILD/coverage/index.html

On Linux::

  $ xdg-open $I3_BUILD/coverage/index.html

If you have further questions, please ask on `Slack
<https://icecube-spno.slack.com/messages/software/>`_ or `file a
ticket <http://github.com/icecube/icetray/issues>`_
against this document.
