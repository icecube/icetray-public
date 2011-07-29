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
``using`` is can be outweighed by how much rope it gives you to hang
yourself. The most common issues with ``using`` relate to
::

  using namespace ... ;

While it often seems like a good idea (and often times is quite
convenient!) to employ ``using namespace``, you are more likely to use
it incorrectly **and** cause problems for other people. It all depends
on context. Sutter and Alexandrescu summarized it best:

  Namespace **using**\ s are for your convenience, not for you to inflict
  on others: Never write a **using** declaration or a **using** directive
  before an **#include** directive.

  Corollary: In header files, don't write namespace-level **using**
  directives or **using** declarations; instead, explicitly
  namespace-qualify all names [*]_.

At best, ``using namespace`` in header files encourages sloppy and
lazy coding practices. At worst, it introduces bugs caused by namespace
clashes or undefined behavior. In fact, in IceCube software, some
subtle bugs were found and subsequently squashed during the ``using
namespace`` removal.

What does this mean for me?
===========================

In software that you use or may have written, you might run into an
error similar to the following when compiling::

  In file included from /Users/nega/i3/offline-software/src/icetray/private/icetray/I3Frame.cxx:40:
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h:95: error: ‘string’ does not name a type
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h: In constructor ‘I3Tray::param_setter::param_setter(I3Tray&, const std::string&)’:
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h:99: error: class ‘I3Tray::param_setter’ does not have any field named ‘module_name_’
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h: In copy constructor ‘I3Tray::param_setter::param_setter(const I3Tray::param_setter&)’:
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h:101: error: class ‘I3Tray::param_setter’ does not have any field named ‘module_name_’
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h:101: error: ‘const class I3Tray::param_setter’ has no member named ‘module_name_’
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h: In member function ‘I3Tray::param_setter& I3Tray::param_setter::operator()(const std::string&, T)’:
  /Users/nega/i3/offline-software/src/icetray/public/icetray/I3Tray.h:107: error: ‘module_name_’ was not declared in this scope
  make[3]: *** [icetray/CMakeFiles/icetray.dir/private/icetray/I3Frame.cxx.o] Error 1
  make[2]: *** [icetray/CMakeFiles/icetray.dir/all] Error 2
  make[1]: *** [icetray/CMakeFiles/icetray.dir/rule] Error 2
  make: *** [icetray] Error 2

The key here is the statement ``error: ‘string’ does not name a
type``. On line 95 of the file ``icetray/public/icetray/I3Tray.h``,
the type ``string`` was used instead of the correct type
``std::string``.

In software that you write for private use, you are free to do what
you will (nobody is looking!), but for software you intend to share,
please adhere to this guideline and avoid ``using namespace`` in your
header files.

  .. [*] Sutter, Herb & Alexnadrescu, Andrei. "Don't write namespace usings in a header file or before an #include." :t:`C++ Coding Standards`. Boston: Addison-Wesley, 2005. 108-110.
