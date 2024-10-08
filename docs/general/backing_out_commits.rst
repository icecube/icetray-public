.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: text

Backing Out a Commit
====================

.. warning::

   This documentation is deprecated, and only serves as a historical reference.
   Since 2018 IceTry has used ``git`` for source control. See the documentation
   for `git revert <https://git-scm.com/docs/git-revert>`_ for the equivalent
   commands.

OK, so you just made a commit to the repository that broke the build bots and
now you want to just back the whole thing out, get the bots green again, and
give it a think before you commit again.  The following is the easiest way to
do this:

.. code-block:: console

  $ cd $I3_SRC/<project>
  $ svn merge --change -<revision> .
  $ svn commit -m "undoing the commit r<revision>.  going to give it a think."

From the svn docs, here's more information on 'svn merge --change' ::

  --change (-c) ARG
    Perform the requested operation using a specific change. Generally speaking,
    this option is syntactic sugar for -r ARG-1:ARG. Some subcommands permit a
    comma-separated list of revision number arguments (e.g., -c ARG1,ARG2,ARG3).
    Alternatively, you can provide two arguments separated by a dash
    (as in -c ARG1-ARG2) to identify the range of revisions between ARG1 and
    ARG2, inclusive. Finally, if the revision argument is negated, the implied
    revision range is reversed: -c -45 is equivalent to -r 45:44.
