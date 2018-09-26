Using I3SPRNGRandomService in distributed systems
-------------------------------------------------

Description:
&&&&&&&&&&&&

I3SPRNGRandomService inherits from I3RandomService. I3SPRNGRandomService is 
intended for use in distributed systems but can also be used in a single 
processor environment. 

About I3SPRNGRandomService
&&&&&&&&&&&&&&&&&&&&&&&&&&

 I3SPRNGRandomService is and implementation of the I3RandomService interface
 which uses the Scalable Pseudo-Random Number Generator (SPRNG) library in
 combination with GSL to generate *statistically independent* streams of 
 pseudo-random number distributions.
 `SPRNG <http://sprng.cs.fsu.edu>`_  was developed by Michael Mascagni, 
 *et al* at Florida State University
 
 The purpose of this implementation is to provide an I3RandomService that
 can be used in a distributed computing environment and minimizing
 correlation between parallel streams of numbers. 

 SPRNG uses a parameterized approach to generate independent streams rather
 than splitting or "leap-frog" approaches used in some parallel algorithms.

 The basic approach for adapting sprng for use with gsl via a gsl_rng_type 
 is based on code from `Darren Wilkinson <http://www.staff.ncl.ac.uk/d.j.wilkinson>`_
 who wrote an MPI simple interface that doesn't does not require configuration 
 of streams.


Configuring the I3SPRNGRandomService
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

 For reproducibility SPRNG requires that for each generator to be given in
 addition to the **seed** two other parameters which are used for
 determining the stream of random numbers and their state. It's important that
 you use the *same* seed for different jobs of the same type.  Each job sets a 
 different stream number.  The following are the parameters used to configure 
 I3SPRNGRandomService:

 * Seed: this determines the initial state of the rng.
 * NStreams: the total number of streams used throughout the the entire computing environment. 
   A single stream per process or instance of IceTray is assumed.
 * StreamNum: the stream number [0,NStreams-1]. In most applications this
   would correspond to the proccess ID for an instance of IceTray.

 Within IceTray the service is then added and configured using the syntax: ::

	tray.AddService("I3SPRNGRandomServiceFactory",
	                 Seed = 0,
	                 NStreams = 2,
	                 StreamNum = 1)

