.. -*- mode:rst; mode:flyspell; mode:fci; mode:auto-fill -*-

Running Python Coverage Locally
===============================

.. highlight:: console

Coverage reports can also be generated for our Python code. The only
requirement is that Python's coverage module is installed. This can be
accomplished via your package manager::

  $ sudo apt-get install python-coverage

or via `pip`::

  $ pip install coverage

Once that's completed, proceed to build your workspace as normal.

::

   $ cd $I3_BUILD
   $ cmake -DCMAKE_BUILD_TYPE=Debug ../src
   $ make
   $ make pybindings
   $ make test-bins

At this point we can generate our report.

:: $ make pycoverage

When that completes you can find the report in
`$I3_BUILD/pycoverage/00_LATEST/`.
