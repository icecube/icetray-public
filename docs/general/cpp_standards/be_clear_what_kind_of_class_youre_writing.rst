
Be clear what kind of class you're writing.
-------------------------------------------
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
