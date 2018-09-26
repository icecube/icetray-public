Using the Various Random Number Services
----------------------------------------

Description
&&&&&&&&&&&

:cpp:class:`I3RandomService` is patterned off of ROOT's
`TRandom <https://root.cern.ch/doc/master/classTRandom.html>`_, and provides access 
to random numbers drawn from several possible distributions. The 
``I3RandomService`` is an abstract interface, which can be serviced by any of
several implementations. 

The advantage of having an ``I3RandomService`` interface between us and the 
random number generator.  We can dynamically switch between any implementations 
of ``I3RandomSerivce``.

Currently there are three implementations of this interface: 

* :cpp:class:`I3GSLRandomService` - uses the gnu scientific library
  to generate random numbers.  It is added to the framework with the
  :js:data:`I3GSLRandomServiceFactory`.  It takes a single integer parameter ``Seed`` which is 
  given to gsl as the seed for the random number sequence. 

* :cpp:class:`I3SPRNGRandomService` - uses the SPRNG library to
  produce independent streams of pseudo-random numbers for distributed
  computation.

* :cpp:class:`I3MT19937` - Uses the c++11 random number interface for the widely used
  MT19937 algorithm. Can take a single int as a seed or a sequence of ints of
  arbitrary length for distributed computing. It can be added to the context
  with :js:data:`I3MT19937Factory` or the python class :py:class:`icecube.phys_services.I3MT19937`.
  
The original ``I3TRandomService`` implementation was removed due to the fact that it was unused.
