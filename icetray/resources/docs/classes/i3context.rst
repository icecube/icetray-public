.. index:: 
   pair: I3Context; C++ Class
.. _I3Context:

.. highlight:: pycon

I3Context Reference
======================

The I3Context is a container similar to the I3Frame. The main similarities:
 * Both are implemented with a map of strings to smart pointers.
 * With both, one Puts smart pointers in, but can retrieve either smart pointers or references.

The main differences are:
 * I3Context is not used to pass event data from module to module as I3Frame is. I3Context is how instances of I3Module access services and configuration information shared across all modules.
 * Consequently, only service factories and users of I3Tray can put things into the context but modules cannot -- and then only before the tray starts.
 * I3Context does not dynamic_cast when asked to Get() something. I3Context will only return an object as type T if that object was installed as exactly type T. (In contrast, the I3Frame will return an object as type T if it is a type T or a descendant of T, regardless of how it was Put().)

The icetray passes a const I3Context to I3ServiceFactories and I3Modules in their constructors::

 class MyModule : public I3Module 
 {
 public:
 
   MyModule(const I3Context&);
 
   // etc
 };         
 
 class MyServiceFactory : public I3ServiceFactory
 {
 public:
 
   MyServiceFactory(const I3Context&); 
 
 };

and it is through this context that an I3Module accesses whatever
services have been installed by the framework. The framework installs
all requested services before it creates the first module.

I3Modules store this context in the protected member ``context_``, which
(because it is protected and not private) is visible to derived module
classes::

 class I3Module 
 {
 public:
   I3Module(const I3Context& context); 
 
 protected:
   I3Context context_;  // this is visible to your module. 
 
 };

Interface
---------------- 

I3Context::Has()
^^^^^^^^^^^^^^^^

These following examples all assume an I3Context named
``context_`` is in scope:

Find out if the context contains something named "something" regardless of what type that "something" might be::

 I3Context context_;

 bool contains = context_.Has("something");

Find out if the context contains something named "littleman" of type RatMonster (something may exist at slot "littleman", this function will return true only if it is of type RatMonster::

 bool contains = context_.Has<RatMonster>("littleman");

if this call returns true, it is guaranteed that a subsequent call to Get() will succeed.

I3Context::Get()
^^^^^^^^^^^^^^^^^^^^^

These examples get references to services out of the context with the name and type of the object fully specified. This form either succeeds or throws via log_fatal()::

 SomeService& service = context_.Get<SomeService>("where");

These get references to services where the object has a DefaultName, determinable from its type; again, the call succeeds or calls log_fatal()::

 I3MediumService& medium = context_.Get<I3MediumService>();

 I3RandomService& rng = context_.Get<I3RandomService>();
 
The following form gets smart pointers to the same services. Get will return a null shared_ptr if either the context is empty at the relevant slot, or if the contents of the relevant slot are of the wrong type::

 I3MediumServicePtr medium_p = context_.Get<I3MediumServicePtr>(); 
 // medium_p == false,  if retrieval failed.
 
 I3RandomServicePtr rng_p = context_.Get<I3RandomServicePtr>();
 // rng_p == false,  if retrieval failed.


I3Context::Put()
^^^^^^^^^^^^^^^^^

The big difference between I3Frame's Put and I3Context's Put is that with the I3Frame, one doesn't have to pay attention to the type of the object at the time that one Puts it into the frame. With the I3Context, however, one is normally Putting a newly-created service, and one has to Put that service as its base type. That is, if you have just created a LaggedFibonacciRandomService, you must Put it as an I3RandomService, if client modules will be accessing it as an I3RandomService::

 LaggedFibonacciRandomServicePtr fib_p(new LaggedFibonacciRandomService);
 fib_p->initialize();
 context_.Put<I3RandomService>(fib_p);

The example above assumes that I3RandomService has some default name. 

Example: A random number service
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The pattern I3Context follows helps keep modules ignorant of exactly what types of services they are using. For instance, one might want to run simulations with various different random number generators: you don't want to have to recompile when you switch generators, and if you want to run others' modules with your random number generator, you don't want to have to hunt through other people's code and remove/replace all the generators by hand.

Therefore one requires some single interface that all random number generators provide. This interface is specified in an /abstract base class/. Each particular kind of random number generator implements this base interface (that is, it provides implementations of the base classes' pure virtual functions). Modules that use these random number "services" can remain ignorant of exactly what kind of random number service they are using because,
# They only use the base class' interface.
# The icetray framework takes care of creating these random number services and putting them where modules can get them.

We'll take a family of random number generators, each of which must supply random doubles and be seedable with an int. The base class specifies what any derived class must be able to do::

 class RandomNumberService 
 {
 protected:
 
    RandomNumberService();
    virtual ~RandomNumberService() = 0;
 
 public: 
 
    virtual double Rand()    = 0;
    virtual void   Seed(int) = 0;
 };  

Derived classes implement those pure virtual functions (the ones followed by = 0). Here is one that uses UNIX's built in rand() function::

 class UnixRandService : public RandomNumberService 
 { 
 
 public:
   UnixRandService() { }
   ~UnixRandService() { }
 
   double Rand()      { return std::rand(); }
   void   Seed(int i) { seed(i);            }
 };

This one isn't random at all, but it illustrates how the derived classes "hide" behind the base class interface::

 // this one isn't all that random
 class SequentialNumberService : public RandomNumberService 
 {
   double d;
 
 public:
   SequentialNumberService() { }
   ~SequentialNumberService() { }
 
   double Rand()      { d += 1.1; return d; }
   void   Seed(int i) { d = i * 1.1;       }
 };

And so on. One could implement as many of these random number service derived classes as one likes. The code of the classes that use them don't know and don't care how many random number services exist; they only know (and must only know) that one is available. This is what "object-oriented" is all about.

The steering file instructs the framework to install a UnixRandService object at the name "rand"::

 tray.AddService("UnixRandServiceFactory", "rand")(
      ("InstallAs", "rand"),
      ("Seed", 10101)
      )

And modules looking for this random service access it via its name and base class, not derived type. It looks like this::

 void 
 MyModule::Physics(I3FramePtr frame)
 {
   RandomNumberService &randoms = context_.Get<RandomNumberService>("rand"); 
 
   cout << "Next random number is " << randoms.Rand() << "!\n";
 }

and not::

 void 
 MyModule::Physics(I3FramePtr frame)
 {
   UnixRandService &randoms = context_.Get<UnixRandService>("rand"); 
 
   cout << "Next random number is " << randoms.Rand() << "!\n";
 }

Why? Again, the goal is to keep modules blissfully ignorant of the real derived types of the services they use. This makes it possible to experiment with different random number generators without modifying or recompiling any code.

For instance, one can switch the UnixRandService for the SequentialNumberService in the steering file::

 tray.AddService("SequentialNumberService", "rand")(
      ("InstallAs", "rand"),
      ("Seed", 10101)
      )

And MyModule will run without recompilation, without relinking.
