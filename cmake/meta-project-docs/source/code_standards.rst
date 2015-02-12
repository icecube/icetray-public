IceCube C++ Coding Standards
============================

.. image:: brazil.jpg

This is the third draft of the IceCube C++ Coding Standards. The first
draft was provided by Thomas Burgess, the second by Erik Blaufuss.  The
third is being expanded by the community.

Introduction
------------

These are coding standards and recommendations for developers of
IceCube software in C++ and Python.  There is a companion document for
:ref:`library_standards`.

This document refers primarily to offline software (including IceTray,
Dataclasses, Simulation and the event viewer).  Users are encouraged
to write their own private software to these standards, to make future
code maintence and the adoption of private code for production use
easier.

Python Coding Standards
-----------------------

Python coding standards follow the recommendations set forth in `PEP
0008 <http://www.python.org/dev/peps/pep-0008/>`_. Where they cause
conflict, the C++ coding standards outlined below, take precedence.

C Coding Standards
------------------

The use of pure C is strongly discouraged.  This is not a statement about
the language, it's simply a choice we have to make based on the limited
resources available.  We have one compiled language (C++) and one
interpreted (Python) that covers everything we need.  We strive to ensure
that all production code is accessible to every member. C++ is no longer
"C with classes" but arguably a separate language.  At the very least,
coding styles differ greatly between the two communities and we have our
hands full with the two official languages we already have.

Should you find it necessary to write something in pure C, we ask that
you follow the Linux kernel coding style.

`Linux Kernel Coding Style <https://www.kernel.org/doc/Documentation/CodingStyle>`_


C++ Coding Standards
--------------------

Good coding standards offer many interrelated advantages:

* *Improved code quality:* Encouranging developers to do the
  right things in a consistent way directly works to improve software
  quality and maintainability.
* *Faster development:* Developers dont need to always make
  decisions starting from first principles.
* *Better Teamwork:* They help reduce needless debates on
  inconsequential issues and make it easier for teammates to read and
  maintain each other's code.
* *Uniformity in the right dimension:* This frees developers
  to be creative in directions that matter.

Code reviews will make constant reference to *C++ Coding Standards* [#]_, by Herb Sutter and Andrei Alexandrescu.  If you submit code that
gets reviewed, you should have access to a copy: reviewers will refer
to the book, and everybody will save time if you can read the full
details/explanations yourself.

This document borrows pieces from the book's structure, and you will find direct
quotes from the book throughout (like the opening paragraph of this
section).  Each of the headings below corresponds to a several-page
long chapter in the book, where one can find Sutter's treatment, so
look there for more information.  Icecube-specific elaborations,
modifications, and excerpts from code reviews are found here.  You are
*not* expected to memorize them or to submit code in full compliance
with every single rule, as these are goals to strive for, and many are
achievably only in degree.  Do your best, and we will refer to this
document in reviews.

Organizational and Policy Issues
--------------------------------
.. toctree::
   :titlesonly:

   cpp_standards/dont_sweat_the_small_stuff
   cpp_standards/compile_cleanly_at_high_warning_levels
   cpp_standards/use_a_version_control_system
   cpp_standards/invest_in_code_reviews



Coding Style
------------

Declare variables as locally as possible.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Avoid leaving variables laying around whose scope is larger than their
actual usefulness.  You shouldn't declare a variable until you have
enough information to initialize it and are ready to use it.  Don't
make variables members of your class if they can be local to one of
the classes' member functions.  Globals are the worst, only use them
when necessary.

Define each variable as locally as you can, which is usually exactly
at the point where you also have enough data to initalize it, and
immediately before its first use.   

Make header files self-sufficient.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Be sure that the path to your public header files, as seen from other
projects, makes sense.  Don't put #includes of tools in your header
files, otherwise I will have to add those tools to my ``USES_TOOLS``
when I use your project.

Always write internal #include guards. Never write external #include guards.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Header files should start with::

 #ifndef DATACLASSES_I3MYHEADER_H_INCLUDED
 #define DATACLASSES_I3MYHEADER_H_INCLUDED

and end with::

 #endif // DATACLASSES_I3MYHEADER_H_INCLUDED


Where ``DATACLASSES`` represents the name of your project, and
``I3MYHEADER_H`` is the name of your header file.  This is different than
what you'll see in the code in a lot of places, we are missing the
DATACLASSES part.  One has to be careful that this
PROJECTNAME_CLASSNAME_H_INCLUDED does not clash with other such
include guard ``#defines`` elsewhere.  For instance, if ``#define
TRACK_H`` appeared in one of your files and somebody else's, could
silently prevent inclusion of that foreign file, and it could take
forever to figure out why, since you definitely wouldn't be expecting
the include guards to be the problem.

Functions and Operators
-----------------------

Be clear what kind of class you're writing.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

 > My C++ books are at home, and I'm at work, but I'm pretty sure at least one
 > of them says destructors should ALWAYS be virtual.  There's always a
 > possibility that someone will want to subclass your class, and if the
 > superclass destructor isn't called, it could be disasterous.  I've also
 > been told that ALL methods should be declared virtual, because you never
 > know when someone might want to subclass your class and override a method.
 > I never saw that in writing, and it never made much sense to me, but I
 > thought I saw some I3 classes with everything virtual, so I thought maybe
 > someone else heard the same thing I did.  

Sutter writes::


   For a base class Base, calling code might try to delete derived
   objects through pointers to Base.  If Base's destructor is public and
   nonvirtual (the default) it can be accidentally called on a pointer
   that actually points to a derived object, in which case the behavior
   of the attempted deletion is undefined.  This state of affairs has led
   older coding standards to impose a blanket requirement that all base
   class destructors must be virtual.  This is overkill (even if it is
   the common case); instead, the rule should be to make base class
   destructors virtual if and only if they are public.


So even in base classes the virtual destructor requirement is
overkill; for classes generally, it's just nonsense.

The advice on "there's always a possibility that someone will want to
subclass your class" is really bad.  The first thing everybody learns
about object-oriented programming is inheritance, and it is the most
overused feature of the language.  

Base classes define an interface to a set of abstractions and should
be carefully designed as such.  One does very different and often
contrary things when designing standalone classes.

All classes Derived that inherit from Base *are* also base, as
inheritance models "IS-A".  Classes chosen at random are almost never
good candidates for such abstractions.  Base classes and their
subclasses are very tightly coupled (read: they quickly become hard to
change later).  For instance, the cost of changing Base is very high:
you have to change all the subclasses and everybody who interacts with
the base class or any of the subclasses.  I could go on.

The guideline in the quote at the start of this item surely came from
someone who saw that if you made everything protected/virtual you
could do unlimited subclassing, but who didn't understand that the
problems of unlimited effortless subclassing are much greater than the
extra typing required to add some virtuals and change "private" to
"protected" if you find that you do want to subclass something.

::

 > because you never know when someone might want to subclass your
 > class and override a method.


One does not inherit in order to reuse code in the Base class, this is
looking at it from the wrong perspective.  One inherits from Base in
order to *be reused* by classes who talk to pointers to objects
of type Base who dont know and don't care whether that is actually a
Derived on the other end of that pointer.  (This a
simple restatement of the "Liskov Substitution Principle").  When it
comes to reusing code there are plenty of other, more straightforward
ways to do it that don't create the couplings that inheritance does.
You can have one as a member, you can use templates, you can use
private inheritance, you can just use functions.

STL: Containers
---------------

Use vector and string instead of arrays.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Don't use C-style const char \*'s, arrays, ``strcmp``, ``strcpy``, or
``strncmp``.  C-style arrays (``float[3]``) can be used if their size
is known at compile time.  Use STL strings instead::

 string s1, s2;

 s1 == s2; // equality comparison
 if (s1 < s2) { } // dictionary comparison

 s1 = s2; // assignment

 s1 = "heeheehee";

 s1 += s2; // concatenation

 cout << "length of s1 is " << s1.length();


There's a decent reference on strings, vectors, and other STL stuff at
http://www.cppreference.com/cppstring/.

Use vector (and string::c_str) to exchange data with non-C++ APIs.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you need to get a ``const char*`` out of a  ``string`` so that
you can pass it to a C function, use the function ``c_str``::

 void takes_a_c_style_string(const char*);
 void takes_an_stl_string(const string&);
 string cpp_string;
 cpp_string = "something";

 takes_an_stl_string(cpp_string); // OK
 takes_a_c_style_string(cpp_string); // error, string doesnt convert to const char*
 takes_a_c_style_string(cpp_string.c_str()); // ok, explictly convert it
 takes_an_stl_string("some string literal"); // ok, const char* does convert to string


Useful links
------------

Bjarne Stroustrup's C++ pages <http://www.research.att.com/%7Ebs/C++.html>
    The pages of the creator of C++ 

Effective C++ <http://www.aristeia.com/books_frames.html>
    Effective C++ and More Effective C++ contains many very useful items
    for C++ programmers. 

C/C++ reference <http://www.cppreference.com/>
    General C/C++, standard C libraries, C++ I/0, strings and STL
    reference. 

C++ FAQ lite <http://www.parashift.com/c++-faq-lite/>
    Many useful answers on most C++ topics 

C++ Notes <http://www.fredosaurus.com/notes-cpp/index.html>
    Small useful examples illustrating basic C++ usage. 

Doxygen <http://www.doxygen.org/>
    Generates code documentation from source code comments 

Also handy for C++ programming GCC <http://www.gnu.org/software/gcc/>
    GNU Compiler Collection - many compilers, including the g++ C++
    compiler! 



.. rubric:: Footnotes

.. [#] The *C++ Coding Standards* homepage can be found at http://www.gotw.ca/publications/c++cs.htm
