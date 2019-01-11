Branches, Releases and The Trunk
================================

To aid in development of both projects and meta-projects, each have
three areas dedicated in the SVN repository, for example, in a project
called "superfit", you might find::

  $SVN/projects/superfit/trunk
  $SVN/projects/superfit/branches/pre-likelihood-conversion
  $SVN/projects/superfit/releases/V01-00-01

The "trunk" is a single development frontier, where releases are
usually made from, and the majority of development work is done,
especially for a smaller project.

The "branches" area is where special development work can take place.
For example, you might make a branch to try experimental work, allows
others to do some work in parallel to yours, or to make copies before
major changes are made to the trunk.

The "releases" area is where tagged, frozen copies of the trunk are
made for inclusion in software releases.  Releases should not be
changed once made, and most users do not have commit privledges in
this area.  Release are typically tagged with a number: VXX-YY-ZZ.

* XX - Major release number, should be incremented if the interface
  changes.

* YY - Minor release number, should be incremented if the interface is
  extended.

* ZZ - Bug fix version, incremented if a minor change is made fixing
  issues without changing the interface.

.. note:: 

   In fact, these conventions are almost universally ignored as they
   are difficult to test and enforce.  Generally one increments the X,
   Y, or Z based on some vague estimate of the size of the change.



