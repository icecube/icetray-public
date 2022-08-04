I3 Prefix Convention
--------------------

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

A specific line length is also not specified, but do keep it readable.


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
https://root.cern.ch/root/Conventions.html, last updated 1999).

Classes that inherit from IceTray base classes take the prefix ``I3``.  
Other classes should not have the ``I3`` prefix.  (NB : There are several 
classes out there that do not follow this convention.)  The name should 
tell the reader something about the semantics of the variable, function 
or class. The name should be concise and in plain English.  Try to avoid 
things like abbreviations, acronyms, and jargon (like the word "semantics").  
It should say something about what the thing does or means.  Think about 
being choosy with the verbs you use in your function names: 
``handleCalculation()`` doesn't say much about what that routine does.  
Acronyms and abbreviations which are inherent to IceCube research, like 
OM, PMT, or BFD are OK.  Examples: I3EventViewerModule, I3ParticleDataService.  
Maps (key/value containers) end with the Dict suffix (*Dict* as in *Dictionary*,
an association of word and definition).  Examples: ``I3RecoResultDict``, 
``I3MCParticleDict``.  Vectors end with the Vect
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
**Do explain approach and rationale.**  A constructor that takes no
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
  * $Id: code_standards.rst 1953 2014-10-14 22:13:56Z nega $
  *
  * @file Dummy.h
  * @date $Date: 2014-10-14 16:13:56 -0600 (Tue, 14 Oct 2014) $
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

Please see the doxygen documentation at https://www.doxygen.nl/manual/
for an overview.  Specifically, see `Special Commands <https://www.doxygen.nl/manual/commands.html>`_ for a list of things like
``@param, @author, @todo`` that you can use, and look through the
dataclasses for some examples.


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
module, without recompiling.  The logging statements make great 
documentation, leave clues as to what the author intended for the code 
to do, and assist in future debugging.


As a guideline, the following guidlines are provided for choosing a 
logging level:

* log_fatal - Only called for fatal errors, will throw.
* log_error - Non-fatal (recoverable) exception. No exception thrown.
* log_warn  - Possible error conditions approaching....
* log_info  - Information to tell operator what's going on.
* log_debug - Information for system expert.
* log_trace - Chronic logorrhea. For step by step debugging.

For non-CS experts, the guidline is:  Standard logging level is log_warn. Under 
absolutely normal conditions, no output at log_warn or higher.
The following list are examples of some messages you might see at each logging l
evel:

* log_fatal - only when you want to exit.
* log_error - won't exit, but your module has a serious problem that 
  needs attention, single bad event, divide by zero detected in a module.
* log_warn  - OM key out of range, minor config errors, <1TB disk space 
  remains, no AMANDA data in >5 minutes....
* log_info  - (<< 1/event) Major transitions, new files open, 
  "processed 1000 events" type statements,
* log_debug - One or two lines output for each process call (per 
  event, config, etc) per module
* log_trace - line by line debugging is possible.

Documentation
^^^^^^^^^^^^^

Check out the existing software for what your module's documentation should
finally look like. Don't check in html, openoffice or word documents.  Write 
your documentation in ``doxygen`` or ``rst`` markup.  This allows everybody 
to use the same markup scheme for documenting their code both in their source
modules and in standalone documents.  It also allows others to fix bugs in 
documentation with their favorite editor.
