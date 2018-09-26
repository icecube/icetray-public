==========
I3Particle
==========

Purpose and properties
======================

The :class:`I3Particle` class is a very generic class that we use to describe
both simulated particles (primaries, secondaries) and reconstructed muon tracks
(through-going, starting, stopping, contained) and cascades.  The class has
nine floating point data members that describe the kinematical properties of
a given particle:

* position (3 degrees of freedom)
* time
* direction (2 d.o.f.)
* energy
* length
* speed

The actual meaning of those properties depends on the type, shape, location and
fit status of the particle, which are specified with integer data members
(``enum`` types, actually) and described in more detail below.

Simulated particles are usually collected in an :class:`I3MCTree` object which
is stored in the ``Q`` frame with the name ``"I3MCTree"``. This tree can
contain any number of particles, from just a few to millions.

Reconstructed particles are usually stored directly in the :class:`I3Frame`.
Some reconstructions produce some extra (algorithm specific) information that
is stored in one or more separate :class:`I3FrameObject` objects.  Usually this
extra information specifies diagnostics of the reconstruction process and/or
reconstructed properties of the particle.  Often the name (frame key) of the
additional object is the name of the particle object with "Params" or
"FitParams" appended.

Data members
============

Floating point data members
---------------------------

Position and time
~~~~~~~~~~~~~~~~~

The time data member of an :class:`I3Particle` object is given with respect to
the event time window.  The position data member (stored as an
:class:`I3Position` object) then gives the location where the particle was at
that time.  What the particle was doing at that position
and what happened before/after that time depends on the type, shape and location information.

Note that the time is not necessarily *within* the event window and the
position can also be far out of the detector array.

The coordinate system for the position should always be the local coordinate
system of the detector you are working with,
e.g. `IceCube <https://wiki.icecube.wisc.edu/index.php/Coordinate_system>`_
or KM3NET (TODO: Claudio, Alexander?).

Direction
~~~~~~~~~

The direction of an :class:`I3Particle` object is stored in an object of type
:class:`I3Direction`.  The direction can be represented in several ways, most
commonly we use astronomy-oriented zenith and azimuth angles, which specify
where the particle is **coming from**. The zenith angle is a polar angle, equal
to zero for particles coming in vertically from above. The
reconstruction-oriented theta and phi angle point in the opposite direction, so
they indicate where the particle is **going to**. This propagation direction can
also be represented as a normalized Cartesian 3-vector, using the IceCube
coordinate system.


Length
~~~~~~

If the length has a positive value then this usually means what you think it means:
the (simulated or reconstructed) particle
traveled over the specified length
into the specified direction (see below)
from a location specified by the position data member.

A zero length usually indicates a shower that short enough to be considered
pointlike.

Negative length can be used for track-like stopping particles (simulated or reconstructed).

For reconstructed through-going tracks (shape ``InfiniteTracks``) the length is set to NAN.

Energy
~~~~~~

For *simulated* particles the energy represents the energy that the particle had at the 
given position (primary at interaction, or secondary at creation).

For *reconstructed* the meaning of the energy value depends on the algorithm. It may be 
an energy proxy, e.g. a number that correlates with the mean energy loss (averaged dE/dX, 
number of emitted photons per meter) along the part of the track that intersects the array. 
I may also be an estimate of the muon energy at some point (the point where it entered the 
array, or where it was closest to the center of the detector or the COG of the pulses).  It 
might even be an estimate of the neutrino energy, assuming the event was caused by a neutrino 
energy. For reconstructed showers the energy usually represents the electron equivalent energy, 
i.e. the energy that an electron should have to cause (a shower that causes) the observed amount
of light in the array.

In order to understand the issues surrounding the reconstructed energy it is highly 
recommended to read the `energy reconstruction paper <http://arxiv.org/abs/1311.4767>`_.

Speed
~~~~~

For most use cases, the speed should be zero or ``c=299792458 m/s``.

However, some reconstruction methods actually try to reconstruct the
speed of the particle, for instance the "line fit" provides a speed.
For a vanilla muon track this should not be too different from ``c``, for shower-like
events it is probably more compatible with zero. There are also more
elaborate reconstruction methods that are used in
for analyses searching for events triggered by magnetic monopoles, which
may (or may not) travel at speeds less than ``c``.

IsCascade
~~~~~~~~~
A Cascade interaction in the detector would look like a blob instead of a track. An example of an interaction 
that produces a blob is a neutrino neutral current Deep Inelastic Scattering in the ice. This will produce a shower of hadrons 
(many of those are Cascades) in the final state of the interaction.  

An :class:`I3Particle` can be set as a cascade via :class:`ParticleShape` or :class:`ParticleType` 
in the constructor of :class:`I3Particle` class. A description of :class:`ParticleShape` and :class:`ParticleType`
is given below of this documentation.
  
* The :class:`ParticleShape` options to set an :class:`I3Particle` as a Cascade are: ``Cascade`` or ``CascadeSegment``.
* Regarding to the type: ``EPlus``, ``EMinus``, ``Brems``, ``DeltaE``, ``PairProd``, 
  ``NuclInt``, ``Hadrons``, ``PiPlus`` or ``PiMinus``. All these type of particles are cascades.
* Furthermore, a :class:`ParticleShape` set as ``Primary`` and with a type of :class:`ParticleType`
  as ``PPlus``, ``PMinus``, ``IsNucleus`` or ``Gamma`` will be a Cascade.  

Below an example of how to set the type and other properties of an :class:`I3Particle` in python::

       from icecube import dataclasses
       
       particle= dataclasses.I3Particle()
       particle.pos= dataclasses.I3Position(2.5e6,4.8e6,-1.7e6)
       particle.dir= dataclasses.I3Direction(0.8,1.1)
       particle.type= dataclasses.I3Particle.PiPlus
       particle.energy= 8.6e2

If the user makes: ``print particle`` all the information of the particle will be displayed. Furthermore, 
the user will obtain a ``True`` value by executing ``particle.is_cascade``. This value is ``True`` since 
a ``PiPlus`` is defined as a Cascade.  

The user can also set the shape of an :class:`I3Particle` through ``particle.shape= dataclasses.I3Particle.Cascade``.


Integer data members
--------------------

Type
~~~~

In simulation we know exactly which *type* a particle has: a neutrino, a muon,
a calcium nucleus, etcetera. The Particle Data Group made a list of all known
particles and gave them a number. For particles in that list we use their PDG
number (note: in the past we used a different convention, but if you always use
an ``enum`` to specify a type and not a literal integer then you should be able
to stay happily oblivious of that detail). There are some "particle" types that
we use in our simulation that do *not* exist in the PDG list. Frequently used
types include ``PPlus`` (proton), ``Fe56Nucleus`` (iron), ``MuPlus`` and
``MuMinus`` (muons), and ``NuE`` and ``NuEBar`` (electron neutrinos).  Some of
these are particle types that have not yet been observed in experiments, such
as monopoles and staus.  Others have to do with shortcuts in our simulation,
such as ``Hadrons`` for a hadronic showers and  ``Pair``, ``Brems`` and
``NuclInt`` for the various kinds of stochastic interactions.  And finally
there are particle type values that do not describe an actual particle but a
calibration device, such as a laser or a flasher in calibration-related simulations.

Our current reconstruction algorithms do not identify any type, so the type
data member is usually left "UnSet". Even for infinite tracks the type is left
as 'unknown'. If the fit is good then it might be a muon (MuMinus), but it
could also be an anti-muon (MuPlus) or a bundle of several muons (we don't have
a type for that).


Shape
~~~~~

For reconstructions it is more meaningful to assign a "shape" than a "type".
The shape data member is also used for simulated particles, but not very
consistently.  The shape can take the following values:

* Null: Not set, not defined, not relevant.
* Primary: Could be used for MC primaries, but e.g. in CORSIKA this needs to be fixed.
* TopShower: Cosmic ray air shower reconstructed from IceTop data.
* Cascade: A shower in the ice, reconstructed with in-ice data
* InfiniteTrack: A through-going track (usually a muon).
* StartingTrack: A starting track (supposedly due to a CC numu/nutau interaction).
* StoppingTrack:  A stopping track (supposedly due to ranged out muon or tau).
* ContainedTrack: A track starting and stopping within the in-ice array.
* MCTrack: simulated tracks [#mctrack]_
* Dark: tracks that do not emit any light [#dark]_

.. [#mctrack] Strangely, the ``MCTrack`` shape is *not* used for simulated muon tracks. It was never used in simulation and will likely go away in the future, since it's not needed.

.. [#dark] The ``Dark`` shape was invented for the Tau Working Group because they didn't like what cmc (Cascade Monte Carlo) did to the original cascades, where it would split the cascade into several smaller cascades for a more accurate simulation.  This made it difficult for tau reconstructions (e.g. double-bang), which attempt to reconstruct the interaction vertex, for example, as one single cascade to Monte Carlo truth.  Thus, the original unadultered (by cmc) cascade remained in the tree and labeled as ``Dark`` where the daughters of this cascade are responsible for generating light.


Location
~~~~~~~~

For fast selection of particles in an :class:`I3MCTree` it is useful to have a
rough indicator of "where the particle is", because the position alone leaves
that sometimes ambiguous.  Values are ``Anywhere``, ``IceTop``, ``InIce`` and
``InActiveVolume``.  If KM3NET gets built on multiple sites, we could consider
to add ``enum`` values to specify those sites.

Also this data member is not used very consistently. For instance, simulated neutrinos may 
have their interaction vertex somewhere in the atmosphere above Arizona and still be 
classified as ``InIce``.

NB: This will be deprecated in future releases and no longer used.

Fit status
~~~~~~~~~~

The fit status is an integer type data member. The possible values are defined by the
``FitStatus`` enum type:

* NotSet: particle object is not the result of a fit (e.g. if it is a MC "true" particle)
* OK: according to the fit algorithm the fit was successful
* InsufficientHits: there was not enough data to do a fit
* FailedToConverge: indicates that a likelihood maximization fit failed to find a maximum
* MissingSeed: the fit algorithm requires a seed which was not available for the current event
* InsufficientQuality: the fit succeeded but according to the algorithm it is not good enough to use
* GeneralFailure: something else went wrong in the fit

You can in principle often use the integer values for these status values, but that
makes your code harder to read. If you use the actual ``enum`` constants (e.g.
``p.fit_status == dataclasses.I3Particle.OK`` in python) then you and your
colleages do not need to memorize the meanings of those numbers.

Major and Minor ID
~~~~~~~~~~~~~~~~~~

In icetray, the "ID" of a particle object is a pair of (integer) numbers that
serve as its unique identifier.  No two particles, even two particles in
different data sets, can have the same identifier.  This is important because
simulation is not done in one single process, but instead in stages, where
particle generation and propagation can happen on different machines.  So if
we simply assigned '0' to the first particle created in a generator process,
its ID would clash with the first particle created by the propagator, both of
which will be added to the same I3MCTree.

* Major ID - This is a hash of the time (of I3Particle object creation), process ID, and hostname.  So two particles created at the same time on the same machine, but in different processes will have different major IDs.
* Minor ID - This is meant to provide the bone-headed (and commonly used) method of particle ID creation.  The first particle created is '0' and the next '1', and so on.

This numerical particle ID should *not* be confused with term "PID" that is
commonly used in other HEP experiments to denote the (process of determining
the) type of a detected particle.

For instance, in simulation data this ID can be used to store maps that give
for each simulated particle a list of PMT pulses (:class:`I3MCPulse`) that were
caused by photoelectrons due to Cherenkov photons emitted that particle.  These
maps only store the particle ID numbers, which can then be used (by an
inquisitive student) to find the corresponding particle back in the
:class:`I3MCTree`, if desired.

In simulated data, a primary and its associated secondaries can have the same
major ID but different minor ID.  This is used mainly for storage in the I3MCTree.

This is likely not widely (if at all) used in experimental data.

*NB : Copying particles* - When I3Particles are copied they *preserve* their
particle IDs.  This is intentional.  I3Particles are copied when added to STL
containers.  It would defeat the purpose of having a particle ID if it changed
each time the particle was added to a container.  If you want to create a new
particle with all the same properties of another particle, the method you're
looking for is I3Particle::Clone.


Use case overview
=================

(Note: this table does not contain *all* use cases.)

================================ ================= ======================= ======= ==============================================================
Use case                         Shape             Location                Length  Meaning of position
================================ ================= ======================= ======= ==============================================================
CR primary                       ``Primary``       ``Anywhere``            NAN     interaction vertex (cosmic ray gets destroyed) [#CRprimaries]_
Neutrino primary                 ``Null``          ``Anywhere``?           >0      interaction vertex (neutrino gets destroyed) [#NUprimaries]_
Secondary muon                   ``Null``          ``InIce``               >0      interaction vertex (muon gets created)
Secondary (other)                ``Null``          ``InIce`` or ``IceTop`` 0       shower location (secondary gets created)
Reconstructed cascade            ``Cascade``       ``InIce``               0       shower location (possibly: neutrino interaction vertex)
Reconstructed stopping track     ``Stopping``      ``InIce``               NAN     end point (possibly: muon/tau decay vertex)
Reconstructed starting track     ``Starting``      ``InIce``               NAN     starting point (possibly: neutrino interaction vertex)
Reconstructed contained track    ``Contained``     ``InIce``               >0      starting point (possibly: neutrino interaction vertex)
Reconstructed through-going muon ``InfiniteTrack`` ``InIce``               NAN     random point on the track [#InfTracks]_
================================ ================= ======================= ======= ==============================================================

.. [#CRprimaries] TODO: why always z=1950m (approx) for CR primaries?

.. [#NUprimaries] TODO: say something about length of neutrino primaries.

.. [#InfTracks] For ``InfiniteTrack`` the position is in principle degenerate with the particle time. One can use any other position on the track, with a correspondingly adjusted time. For numerical reasons it can be good to choose a position (and corresponding time) close to the COG of the pulses of the event.


Conventions and recommendations
===============================

* Like most things in icetray, if you want to specify the :class:`I3Particle` data members
  in specific units, you should use :class:`I3Units`, e.g. (in python):

  p=dataclasses.I3Particle()
  p.dir.zenith = 42.0 * I3Units.degree
  print("The zenith angle is %.1f degrees" % (p.dir.zenith/I3Units.zenith))

* Most data members (all, except the major and minor ID) have a default initialization value that 
  indicates that it is not yet set. For floating point data members this is NAN
  (Not a Number, TODO: add link), for the ``enum`` data members it is
  ``unknown`` (type), ``Null`` (shape), ``NotSet`` (fit status) and ``Anywhere`` (location).

* Never use the explicit numerical values of the ``enum`` types. First, the ``enum`` constants
  have descriptive names, so they are much more informative. Secondly, it may
  occasionally happen that the definition of the ``enum`` changes (e.g. the
  type ``enum`` changed from the AMANDA/rdmc convention to the PDG convention).
  The icetray versioning system guarantees that old data is converted correctly
  when you read it in with new software but it will not update the literal
  integer constants in your code.

History
=======

The current documentation attempts to provide a correct and more or less
complete description of the **current** state (winter 2015) of
:class:`I3Particle`.  In older data (many years older) you may discover that
e.g. some conventions were different or some data members or ``enum`` values
were not yet in use, but the basics have been relatively stable.

The first version of the :class:`I3Particle` class was added to icetray in
2005.  It was at least partly inspired by the :class:`mtrack` struct in the
``rdmc`` library that was the foundation of the early AMANDA data processing
and analysis software. In the early stages of icetray development (2004) we
first tried to give literally every kind of simulated particle and
reconstructed phenomenon (track, shower, double bang, monopole, elephantino)
its own special dedicated class, but things that were common (like position or
direction) would always have to represented in the same way. The result was
very template-heavy and impressive but hard to work with. That's why we went
back to the single simplistic 8-dimensional (x, y, z, zenith, azimuth, length,
energy) AMANDA solution (well, adding a 9th: speed),  with the type and shape
given by ``enum`` data members. 

Some elements of :class:`I3Particle` were added only a few years ago. The versioning
system of icetray classes helps to make this backwards compatible, in the sense
that if you read older data with newer software, the software deals with
the version mismatches and you should not even notice that there was a version mismatch. 
Still, if you ever need to process older data and you run into strange problems, 
it may be useful to know which items are new and why they
were added/changed:

* Particle ID: this used to be a single number, but in 20XY this was replaced
  by a "major" and "minor" ID, to guarantee unique IDs between all particles
  created in mass production.
* Particle types: we have been using a set of ``enum`` values that were invented in the AMANDA
  days. This reinvented wheel was eliminated by changing to an ``enum`` system mostly based
  on the PDG (Particle Data Group) list of elementary particles and nuclei.
* The ``LocationType`` is relatively new and will be going away soon.  This
  was meant to make it easier for analysts to pull *the* muon, for example, 
  out of the I3MCTree.  It's proved to cause more problems than it solves and
  a more sophisticated method is in the works.

