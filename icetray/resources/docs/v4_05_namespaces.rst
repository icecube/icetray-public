==========================
 "using namespace" removed
==========================

What is "using namespace"?
==========================

Namespaces
----------

Namespases allow a programmer to group entities (classes, objects,
functions, etc) under an identifier. This way the "global scope" can
have "sub-scopes" to logically group entities, and prevent the names
of entities from colliding. For instance:
::

  namespace foo {
    int yee;
    int haw;
  }

defines a namespace ``foo`` with to integers belonging to it, ``yee``
and ``haw``. From a parent scope (including the global scope), these
entities are referenced by:
::

  foo::yee;
  foo::haw;

The ``using`` keyword
---------------------

The **C++** keyword ``using`` brings entities from a namespace into
the current scope (or even the whole namespace!) For instance:
::

  using foo::yee;

will bring ``yee`` into the current scope so that it can be referred
to as:
::

  yee;

Additionally the entire namespace can be brought into the current
scope like this:
::

  using namespace foo;

Why was it "removed"?
=====================

Like many things in **C++** (and especially **C**), the utility of
``using`` is often outweighed by how much rope it gives you to hang
yourself. The most common issues with ``using`` relate to
::

  using namespace ... ;

While it often seems like a good idea (and often times is quite
convenient!) to employ ``using namespace``, you are more likely to use
it incorrectly **and** cause problems for other people. It all depends
on context. Sutter and Alexandrescu summarized it best:

  Namespace **using**\ s are for your convenience, not for you to inflict
  on others: Never write a **using** declaration or a **using** directive
  before and **#include** directive.

  Corollary: In header files, don't write namespace-level **using**
  directives or **using** declarations; instead, explicitly
  namespace-qualify all names [*]_.

What does this mean for me?
===========================

  .. [*] Section 59, Sutter and Alexnadrescu, C++ Coding Standards (101 Rules, Guidelines and Best Practices)
