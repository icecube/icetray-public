Services in python
==================

Icetray v3 supports parameters to :class:`I3Module` instances of
arbitrary type, as shown in :ref:`paramtypes`.  This means that in
various cases, given the necessary python wrappers, it is possible to
configure services for a module by passing them directly through the
module's parameters.

The only option in V2 - pass service to module via context/factory
------------------------------------------------------------------

Consider the following C++ module which uses an :class:`I3RandomService`
to get random numbers and put them into the frame inside an :class:`I3Double`:

.. code-block:: cpp

  class UseRandomV2 : public I3Module
  {

    I3RandomServicePtr rs_;   // will hold service fetched manually from context
    std::string rs_key_;
    std::string dest_key_;    // will hold name of service's location

   public:

    UseRandomV2(const I3Context& ctx)
      : I3Module(ctx)
    {
      AddParameter("I3RandomServiceKey",
		   "my random service location",
		   rs_key_);

      AddParameter("PutWhere",
		   "where the doubles go",
		   dest_key_);
    }

    void Configure()
    {
      GetParameter("I3RandomServiceKey", rs_key_);
      GetParameter("PutWhere", dest_key_);

      rs_ = context_.Get<I3RandomServicePtr>(rs_key_);  // manual 'fetch service' step
    }

    void Physics(I3FramePtr frame)
    {
      double d = rs_->Gaus(0, 1);
      I3DoublePtr dp(new I3Double(d));
      frame->Put(dest_key_, dp);
      PushFrame(frame);
    }

  };

  I3_MODULE(UseRandomV2);


Of note are that the module takes a parameter of type 'string' that it
uses to locate an :class:`I3RandomService` instance in its
:class:`I3Context`.

Configuration of this module and its random service would look like this:

.. code-block::python

   tray.AddService("I3GSLRandomServiceFactory", "servfactory")(
       ("InstallServiceAs", "gslrandomserv"),
       ("Seed", 31337)
       )

   tray.AddModule("UseRandom", "userand")(
       ("I3RandomServiceKey", "gslrandomserv"),
       ("PutWhere", "randomdouble")
       )

Of note here:

* We can't see how the ``I3GSLRandomServiceFactory`` knows which
  context to install I3GSLRandomServices in.
* The configuration of this random-using module ``UseRandom`` is
  smeared across the steering file.
* The fact that a 'factory' is involved is distracting.  Sometimes
  called a 'leaky abstraction', this is a pattern that is intended to
  simplify things for the end user but doesn't adequately hide its
  implementation details.  It simply exchanges one kind of complexity
  for a different kind.
* It is difficult to test the I3GSLRandomService... configuration
  and construction is tied to this factory pattern.

New option in V3 - just pass as parameter
-----------------------------------------

The V2 way is still available, but there is now a simpler way to get
the job done.  The goal is to simplify configuration and testing of
icetray components and if possible provide ways to do rapid
prototyping.

The first requirement is that the random service in question have
python wrappers.  A wrapped I3GSLRandomService is usable from python
like this:

.. code-block:: pycon

   >>> from icecube import icetray, phys_services
   >>> rng = phys_services.I3GSLRandomService(seed = 31337)
   >>> rng.Gaus(0,1)
   -0.046058528394790486
   >>> rng.Gaus(0,1)
   -1.0140449021555507

Here we construct an instance of I3GSLRandomService, passing in the
seed value, and call the :meth:`Gaus` a couple of times.  We can modify
the :class:`UseRandom` module above to take this service via parameter:

.. code-block:: cpp

   class UseRandom : public I3Module
   {
     I3RandomServicePtr rs;
     std::string key;

    public:

     UseRandom(const I3Context& ctx) : I3Module(ctx)
     {
       AddParameter("I3RandomService",           //  not 'key' anymore, not a string
		    "my random service",
		    rs);

       AddParameter("PutWhere",
		    "where the doubles go",
		    key);
     }

     void Configure()
     {
       GetParameter("I3RandomService", rs);       // Get a randomservice right from the tray
       log_debug("rndserv is at %p", rs.get());
       GetParameter("PutWhere", key);
     }

     void Physics(I3FramePtr frame)
     {
       log_debug("rndserv is at %p", rs.get());
       double d = rs->Gaus(0, 1);
       I3DoublePtr dp(new I3Double(d));
       frame->Put(key, dp);
       PushFrame(frame);
     }
   };

So the parameter "I3RandomServiceKey", a lookup string, has been
replaced with a parameter "I3RandomService".  The module calls
``GetParameter`` passing the I3RandomServicePtr named ``rs``, which
the steering file connects to whatever is passed in by the user:

.. code-block:: python

   rndserv = phys_services.I3GSLRandomService(31334)

   tray.AddModule("UseRandom", "ur",
		  I3RandomService = rndserv,  # this parameter is a python object
		  PutWhere = "here")

here,

* It is clear what random service is connected to what module.
* You can test the I3GSLRandomService with a python script, or use it
  in non-icetray contexts.
* Configuration is shorter
* There is no 'servicefactory' involved.

New in icetray version 11-01-01 to ease with this transition: many modules will still need
to maintain the functionality to get some services from the context and also as a parameter.
You might expect, if you don't explicitly pass a pointer to a service, after the call to GetParameter ``rs``
(in the example above) should remain uninitialized as a NULL pointer (i.e. the same value it was
when it was "Add"ed).  This was, in fact, not the case and would throw an error.  Python didn't 
know how to convert the ``NoneType`` object.  In general it's not clear, but when you have ``None`` 
on the python side and are expecting a shared pointer it's perfectly reasonable to convert that 
to NULL pointer.  So now you can decide how to handle that in the code.  Here's an example:

.. code-block:: cpp

   class UseRandom : public I3Module
   {
     I3RandomServicePtr rs;
     std::string key;

    public:

     UseRandom(const I3Context& ctx) : I3Module(ctx)
     {
       AddParameter("I3RandomService",           //  not 'key' anymore, not a string
		    "my random service",
		    rs);

       AddParameter("PutWhere",
		    "where the doubles go",
		    key);
     }

     void Configure()
     {
       GetParameter("I3RandomService", rs);       // Get a randomservice right from the tray
       if(!rs){
        // This script is still using the old method and has loaded the service
	// with a Factory.  Without the change to I3Configuration
	// the above call to GetParameter would have thrown an error.
	rs = ctx_.Get<I3RandomServicePtr>()
       }
       log_debug("rndserv is at %p", rs.get());
       GetParameter("PutWhere", key);
     }

     void Physics(I3FramePtr frame)
     {
       log_debug("rndserv is at %p", rs.get());
       double d = rs->Gaus(0, 1);
       I3DoublePtr dp(new I3Double(d));
       frame->Put(key, dp);
       PushFrame(frame);
     }
   };

Using services from python modules
----------------------------------

If the class of a service is properly python-wrapped, like the
I3GSLRandomService, it is of course just as usable from python modules
as it is from c++ modules.  Here is the corresponding python
implementation of the UseRandom module, above:

.. code-block:: python

   from icecube import icetray, dataclasses

   class UseRandom(icetray.I3Module):
       def __init__(self, context):
	   icetray.I3Module.__init__(self, context)
	   self.AddParameter("I3RandomService", "the service", None)
	   self.AddParameter("PutWhere", "where the doubles go", None)

       def Configure(self):
	   self.rs = self.GetParameter("I3RandomService")
	   self.where = self.GetParameter("PutWhere")

       def Physics(self, frame):

	   rnd = self.rs.Gaus(0,1)
	   d = dataclasses.I3Double(rnd)
	   frame.Put(self.where, d)
	   self.PushFrame(frame)

Assuming that this class is inside file :file:`MyModules.py`, the
steering file looks nearly identical to that for the c++ version,
except :class:`UseRandom` is no longer quoted, as we pass the python
class object itself to :meth:`I3Tray.AddModule`:

.. code-block:: python

   from icecube import phys_services
   from MyModules import UseRandom
   rndserv = phys_services.I3GSLRandomService(31334)

   tray.AddModule(UseRandom, "ur",
		  I3RandomService = rndserv,  
		  PutWhere = "here")


Implementing services in python
-------------------------------

Given the necessary python wrapper of the C++ base class (in these
examples, :class:`I3RandomService`), one can implement the service in
python and pass this to I3Modules (both C++ modules and python).  

Here is an dummy python implementation, :class:`ConstantService`, of
I3RandomService:

.. code-block:: python

   from icecube import icetray, dataclasses
   from icecube.phys_services import I3RandomService

   class ConstantService(I3RandomService):
       def __init__(self, value):
	   I3RandomService.__init__(self)
	   self.value = value

       def Binomial(self, ntot, prob):
	   return self.value

       def Exp(self, tau):
	   return self.value

       def Integer(self, imax):
	   return self.value

       def Poisson(self, x1):
	   return self.value

       def PoissonD(self, x1, x2):
	   return self.value

       def Gaus(self, mean, stddev):
	   return self.value

The python implementation inherits from the abstract base class which
forms the interface: exactly the same as in C++.  
    
Putting this class into a file MyServices.py, you can instantiate 
and test this class from the python command line:

.. code-block:: pycon

   >>> from MyServices import ConstantService
   >>> cs = ConstantService(value = 333)
   >>> cs.Gaus(0,1)
   333
   >>> cs.Gaus(0,1)
   333
   >>> cs.Poisson(3)
   333

and pass it to the UseRandom module like any other I3RandomService:

.. code-block:: python

   tray.AddModule("UseRandom", "ur",
		  I3RandomService = cs,  
		  PutWhere = "here")

Note here that we have passed *'UseRandom'* in quotes: we mean the C++
module.  This module receives an I3RandomServicePtr in its arguments,
and in this example, that randomservice will be implemented in python.
The C++ module doesn't know this, and doesn't need to know it: it
cares only that it has an object that it request random numbers from.










