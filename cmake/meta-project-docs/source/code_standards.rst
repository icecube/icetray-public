IceCube C++ Coding Standards
============================

.. image:: brazil.jpg

This is the third draft of the IceCube C++ Coding Standards. The first
draft was provided by Thomas Burgess, the second by Erik Blaufuss.

Introduction
------------

These are coding standards and recommendations for developers of
IceCube software in C++ and Python.  This is primarily offline software
(including IceTray, Dataclasses, Simulation and the event viewer).
Users are encouraged to write their own private software to these
standards, to make future code maintence and the adoption of private
code for production use easier.

Python Coding Standards
-----------------------

Python coding standards follow the recommendations set forth in `PEP
0008 <http://www.python.org/dev/peps/pep-0008/>`_. Where they cause
conflict, the C++ coding standards outlined below, take precedence.

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

Code reviews will make constant reference to C++ Coding Standards 
(http://www.gotw.ca/publications/c++cs.htm),
by Herb Sutter and Andrei Alexandrescu.  If you submit code that
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

Don't Sweat The Small Stuff
^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are things that appear in many coding standards that are
unenforceable and a waste of time.  Any programmer can read and write
code that is indented slightly differently than he/she is used to.
For instance::

 if (x > y) {
   putsLotionIn(basket);
 } else {
   getsHoseAgain();
 }

is not much different in readability than::

 if (x > y)
 {
   puts_lotion_in (basket);
 } 
 else
 {
   gets_hose_again ();
 }

So exactly how much to indent isn't specified.  If you are an emacs
user, "gnu" indentation style (the default) will produce good,
standard indentation, but in any case *do* indent to show structure
and be consistent within at least each file, because it is jarring to
jump around among several styles in the same piece of code.

A specific line length is also not specified.  Keep it readable.  

Organizational and Policy Issues
--------------------------------

Naming
^^^^^^

Name classes, functions and enums ``LikeThis``, name variables 
``likeThis``, name private member variables ``likeThis_``, and name macros
and constant fundamental types ``LIKE_THIS``.

Never use names that begin with an underscore or that contain a double
underscore (this is because of how linking works, it's not a matter of
taste.)  Internal single underscores and trailing underscores are OK.
Always use ``ONLY_UPPERCASE_NAMES`` for macros and never think about
writing a macro that is a common word or abbreviation (including
common template parameters such as T and U; writing ``#define T``
*anything* is extremely disruptive.)

Do *not* use complicated Hungarian or the ROOT/Taligent-style
quasi-hungarian notation.  One of the main advantages of not using
them, as Thomas Burgess observes, is that you can talk about the code
in English as well as Hungarian.  ``"fThey EJust fgMake MThe fCode
EHarder kTO gRead."`` The Hungarian styled conventions are over
forty-five years old. When the ROOT people appear to have chosen that
style, Taligent had "written the largest body of C++ and the rules
seem well thought out.  No need to invent something else."  (From
http://root.cern.ch/root/Conventions.html, last updated 1999).

Icecube-specific classes take the prefix ``I3``.  The name should tell
the reader something about the semantics of the variable, function or
class. The name should be concise and in plain English.  Try to avoid
things like abbreviations, acronyms, and jargon (like the word
"semantics").  It should say something about what the thing does or
means.  Think about being choosy with the verbs you use in your
function names: ``handleCalculation()`` doesn't say much about
what that routine does.  Acronyms and abbreviations which are inherent
to IceCube research, like OM, PMT, or BFD are OK.  Examples:
I3EventViewerModule, I3ParticleDataService.  Maps (key/value
containers) end with the Dict suffix (*Dict* as in *Dictionary*,
an association of word and definition).  Examples: 
``I3RecoResultDict``, ``I3MCParticleDict``.  Vectors end with the Vect
suffix, e.g. ``I3MCParticleVect``.

Publicly accessible files (the interface to your class) go under 
``public/``.  Private files go under ``private/``.  The difference is not
just header files vs. implementation files.  Some headers should not
be visible to other projects.  Keep #includes of other header files
out of your header files.

Do *not* use the verbose root-style typedefs for plain old types,
unless you find that you need them for some specific reason, which you
won't.  These typedefs address problems that we don't have.  They just make
the code more verbose and most importantly bind every line of our code
to the ROOT headers.  Just write the names of the types, it's easier
and it's completely safe.  Write ``double`` and not ``Double_t``, 
``char`` and not ``Char_t``, ``int`` instead of ``Int_t``.

A class is declared in a header file with the same name as the class
and with suffix ``.h``.  For example, the **I3Position** class
declaration is located in ``I3Position.h``, and defined in a
source file with the same name as the class and with suffix ``.cxx``
For example, **class I3Position** source is located in
``I3Position.cxx``

Files containing root scripts should have suffix ``.C``.

Comments
^^^^^^^^

Comments need to be intelligible to **doxygen**, our documentation
generator, and doxygen markup is our standard documentation format.

Don't write comments that just duplicate the code: they get out of
sync, add no additional information and just make the files longer.
Do explain approach and rationale.  A constructor that takes no
arguments is the default constructor, it couldn't be anything else and
the comment "default constructor" does not help.  Doxygen will extract
the function anyway.

Compare::

   /**
    *  The weight
    */
    float weight_;

to:: 

   float weight_;


They are equally descriptive.

Avoid writing jargon, Latin, acronyms, abbreviations or other non
English words. The idea is to make things easier to understand!
Acronyms and abbreviations which are inherent to icecube research,
like OM or PMT allowed, but should be used judiciously.  

File description block
^^^^^^^^^^^^^^^^^^^^^^

The file description block is a special documentation comment that
should be in the head of every file. It contains a few lines with very
basic information about the file. The lines should be, example is below::

  A very short file description (preferably one line), followed by a
      blank line
  (c) 2004 (year as appropriate)
  the IceCube Collaboration
  Revision number tag (Id) , followed by a blank line
  @file - Doxygen command to indecate file description
  @date - Followed by the Date  tag
  @author name to identify author

Example::

 /**
  *
  * Definition of Dummy class
  *
  * (c) 2004
  * the IceCube Collaboration
  * $Id: standards.dox 14697 2006-01-24 23:06:03Z blaufuss $
  *
  * @file Dummy.h
  * @date $Date: 2006-01-24 18:06:03 -0500 (Tue, 24 Jan 2006) $
  * @author burgess
  * @author blaufuss
  *
  */


Doxygen auto generates documentation from the code with the help
of specially formated comments::

  @brief -  Used for brief descriptions and is written in  C style comment block

  /** - Used for detailed descriptions and is writen like a C style comment block starting with an extra star. 

  /// - Used for member variable descriptions.

  @ - Used to start special documentation commands. ( @todo, @author)

Doxygen Links
^^^^^^^^^^^^^

Please see the doxygen documentation at 
http://www.stack.nl/~dimitri/doxygen/manual.html 
for an overview.  Specifically, see the 
http://www.stack.nl/~dimitri/doxygen/commands.html
for a list of things like  ``@param, @author, @todo`` that you
can use, and look through the dataclasses for some examples.


Logging
^^^^^^^

Don't use any of the forms of printf, nor C++ iostreams ( ``cout`` and
friends) for reporting your code's progress or debugging output.
There should  *not* be ``printf`` or ``cout`` statements in your code,
unless your code has a well-defined command line interface, (I3
Modules don't), which include ``printf`` or ``cout`` statements that are
commented out or bracketed in ``#if 0``.  

Instead, use icetray's logging facilities, which work just like
printf, with the added feature that you can turn them on and off, by
module, from a config file, without recompiling.  The logging
statements make great documentation, leave clues as to what the author
intended for the code to do, and assist in future debugging.


As a guideline, the following guidlines are provided for choosing a 
logging level::

* log_fatal - Only called for fatal errors, will throw.
* log_error - Non-fatal (recoverable) exception. No exception thrown
* log_warn - Possible error conditions approaching....
* log_info - Information to tell operator what's going on.
* log_debug - Information for system expert.
* log_trace - Chronic logorrhea. For step by step debugging.

For non-CS experts, the guidline is:  Standard logging level is log_warn. Under 
absolutely normal conditions, no output at log_warn or higher.
The following list are examples of some messages you might see at each logging l
evel:

* log_fatal - only when you want to exit.
* log_error - won't exit, but your module has a serious problem that 
  needs attention, single bad event, divide by zero detected in a module.
* log_warn : OM key out of range, minor config errors, <1TB disk space 
   remains, no AMANDA data in >5 minutes....
* log_info : (<< 1/event) Major transitions, new files open, 
  "processed 1000 events" type statements,
* log_debug: One or two lines output for each process call (per 
  event, config, etc) per module
* log_trace: line by line debugging is possible.

Documentation
^^^^^^^^^^^^^

Check out the existing software for what your module's documentation should
finally look like.
Don't check in html, openoffice or word documents.  Write your
documentation in ``doxygen`` or ``rst`` markup.  This allows everybody to use the
same markup scheme for documenting their code both in their source
modules and in standalone documents.  It also allows others to fix
bugs in documentation with their favorite editor.

Compile cleanly at high warning levels
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The build system will specify ``-Wall``, the maximum warning level, by
default.  If you don't see any complaints from the compiler, you're
golden.  If you do, pay attention to them.  The compiler is your
friend.  The normal state of affairs must be that the code compiles
without warning, otherwise people will start to ignore the warnings
and we will end up spending lots of time chasing bugs that were
actually very easy to find: the compiler was telling us about them all
the time.

This also makes our coding standard much much shorter.  For instance
"Dont return a pointer or reference to a temporary", is covered under
this item, because the compiler will flag this for you with all
warnings.

Use a version control system
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Keep your stuff in the code repository, and don't be afraid 
to check in every time
your software builds, and don't be afraid to tag every time it works.
It often happens, when big changes are necessary, that you can get
ahead of yourself and then forget how to back up to something that
worked.  Code repositories (SVN) can be a huge help.

Invest in code reviews
^^^^^^^^^^^^^^^^^^^^^^
::

 > The constructive
 > criticism that I just got from my code review was great.  

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



