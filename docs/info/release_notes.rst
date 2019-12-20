Combo Release V00-00-00
-----------------------

Stats
~~~~~
* Tickets: Resolved 40 tickets. Pushed 5 to the next milestone.
* Collaborator Contributions: 49 Collaborators made commits to projects.
* Code Sprint Attendance: 29 Collaborators made commits in December.
* Code Coverage: Line 53.3% and Function 49.7%

Project Highlights
------------------
Highlights of features and fixes for each project are below.  For more details
about minor fixes and cleanups see each project's individual release notes
(in <project>/RELEASE_NOTES).

clsim
~~~~~

* Merge multiprocess-server branch. This is a major restructing of clsim.
  - I3CLSimServer accepts steps over a ZMQ socket and distributes them to
    instances of I3CLSimStepToPhotonConverter. Multiple clients can share a
    server, allowing M:N CPU:GPU ratios rather than only 1:N.
  - I3CLSimModule is replaced by I3CLSimClientModule. I3CLSimClientModule only
    emits I3CompressedPhotons, so no pretty photon histories.
  - MaxParallelEvents and MaxEnergyToProcess options are gone; internal queues
    are filled dynamically to create complete propagator kernel invocations.
  - I3CLSimLightSourceToStepConverterGeant4 is factored into
    I3CLSimLightSourceToStepConverterAsync and
    I3CLSimLightSourcePropagatorGeant4. The latter is truly optional, making it
    possible to use clsim without initializing Geant4.
  - I3CLSimLightSourcePropagatorFromI3PropagatorService allows e.g. PROPOSAL
    to be run in a thread in I3CLSimClientModule. This can be used to save
    memory by discarding stochastic losses as soon as their light yield has been
    simulated.
  - Both I3CLSimLightSourcePropagatorGeant4 and 
    I3CLSimLightSourcePropagatorFromI3PropagatorService can optionally record
    the secondaries they produce in an MCTree.
  - I3CompressedPhoton positions are relative to the module they hit, and have
    oversizing removed.
  - Photons are converted directly to (coalesced) I3MCPE in the collector
    thread of I3CLSimClientModule. This can save significant memory for
    high-energy events.
* Merge spe-template branch.
* I3CLSimLightSourcePropagatorGeant4 can be reinitialized, making it possible
  to change the wavelength bias without starting a new process. This is useful
  for implementing a hobo SnowStorm simulaton where systematic parameters change
  every N frames.
* Removed MMCTrackListName option from I3CLSimMakePhotons. PROPOSAL now emits 
  muon track segments directly, and they do not need to be reverse-engineered 
  by I3MuonSlicer.
* Direct, in-kernel cable shadow simulation can be enabled by setting the
  CableOrientation parameter in I3CLSimMakePhotons. This reduces isotropic
  efficiency by ~10%, but in a directionally specific way.
  
PROPOSAL
~~~~~~~~

Uses the latest highly restructed trunk.  Essentially the same as you would
pull from Dortmund's GitHub repo.

trigger-sim
~~~~~~~~~~~
* (r163657) Bug fix: The end of the readout window is calculated w.r.t.
  the end of the trigger and not the start of the trigger.  Note this is
  only a bug when the subdetector is different from the trigger source, so
  only affects InIce/IceTop coincident events.

vuvuzela
~~~~~~~~

* Add in a pythonic module to handle multi-pmt simulation using pre-generated
  GEANT4 noise. It's slow enough that it can only be a temporary solution, but
  it should work in pinch.

photonics-service
~~~~~~~~~~~~~~~~~
* Copy over the I3PhotonicsServiceCollection from a branch so that we can
use the trunk of photonics-service with the upgrade and Gen2 recos.
  
millipede
~~~~~~~~~
* (r170477): Use all OMTypes except IceTop and scintillators (Upgrade support)

gcdserver
~~~~~~~~~
Disabled.  It doesn't work in Pyton2 or 3.

dataclasses
~~~~~~~~~~~
* fix bugs in `I3Time` get and set for unix time
* add `__bool__` operateor to `I3Double`, `I3UInt32`, `I3MCTree` and
  `I3FilterResult` in addition to `__nonzero__`. python2 uses `__nonzero__` but
  python3 uses `__bool__`.
* Logic on bool was changed for `I3MCTree` to call false on an empty tree and true
  on a non-empty tree. It had mistakenly been set to the opposite. 
* Add calibration, R&D devices to OMGeo/ModuleGeo

sim-services
~~~~~~~~~~~~
* (r176200) Allow I3PropagatorService implementations to consume their own secondaries.
* (r172144) Add the option to I3PropagatorModule to skip NaN length nodes.
* Add `I3CosmicEventGenerator`, an interface for using CORSIKA as a service in mulitprocess-server clsim

toprec
~~~~~~

* Modified Laputop so that it uses the X/Y/Z coordinates of the Tank that was hit 
  using StationGeo information, instead of the coordinates of the DOM inside the tank,
  like 61 or 63, which is off to one side.  Previous functionality can be restored
  using the "OldXYZ" option, which is default False.  "LaputopStandard" traysegment,
  used by L3 processing, has this set to True, so as to reproduce previous L3 results.

neutrino-generator
~~~~~~~~~~~~~~~~~~
* I3NuGInjector now insert `I3NuGenInfo` objects into S-frames
* Only convert string to neutrino type once per tray rather than every event

simclasses
~~~~~~~~~~
* Add I3NuGenInfo and I3CorsikaInfo for use in S-Frame weighting
* Adding three remaining Gaisser-Hillas fit parameters (a, b, and c) to I3CorsikaShowerInfo.
* Added CROSSTALK_PULSE to I3MCPulse
* New classes for cable shadow:
  - I3ExtraGeometry
  - I3ExtraGeometryItem
  - I3ExtraGeometryItemMove
  - I3ExtraGeometryUnion
  - I3ExtraGeometryCylinder
  - I3CylinderMap

paraboloid
~~~~~~~~~~
* Allow Paraboloid to take services by pointer as well as by string

icetop_Level3_scripts
~~~~~~~~~~~~~~~~~~~~~
* Updated in-situ snow-measurement file, and associated elements in scripts
* Modified scripts to use STTools instead of SeededRT
* Added two-station filter functionality for 2016 and beyond

corsika-reader
~~~~~~~~~~~~~~
* I3CORSIKAReader will now emit an S-Frame with a I3CorsikaInfo object 
  to be used for weighting

wimpsim-reader
~~~~~~~~~~~~~~
* Cleaned up some of the file handling
* Added the ability to read gzipped WimpSim files
* Allow the tray segment to accept multiple input files

wavedeform
~~~~~~~~~~
* (r173060): Added Spencer Axani's changes for SPE correction

weighting
~~~~~~~~~
* r171768, r177779 Sometimes steering parameters are stored as strings in the
  iceprod1 database. Explicitly cast to desired type.
* r171604, r175937 Detect primary in more kinds of CORSIKA simulation

MuonGun
~~~~~~~
* MuonGun.Track handles sliced MCTrees
* MuonGun.MuonBundleConverter ignores secondary muons in MCTree
* EnergyDistribution and friends integrate over energy in log space for better
  numerical stability
* Update MuonPropagator to use restructured PROPOSAL API

filterscritps
~~~~~~~~~~~~~
* several python3 compatiblity changes (print(print))
* add first version of pass3 processing scripts.
* rewritten monopole filter and L2 processing scripts (cleanups)
* #2347: Shadow filter in simulations does not vary moon/sun position

polyplopia
~~~~~~~~~~
* Changed calculation of TimeAtDetecor to use I3Calculator::ClosestApproachCalc
  for an infinite track for better estimation of arrival time.

dataio
~~~~~~
* Tweak the behavior of I3Writer when using DropOrphanStreams. (bug fix for edge case)
* I3RecoPulse printing improved in the shovel.
* DeleteUnregistered moved here.
* Async i/o is removed.  Tests deadlock.

phys-services
~~~~~~~~~~~~~
* I3GeometryDecomposer is now a conditional module.
* Added Cup geometry.
* fix and simplify `I3Surfaces::Cylinder::acceptance` and add pybindings and unit tests for it
* add `Print()` and `operator<<` for `I3Surfaces::Cylinder`
* add `I3OMGeo::mDOM` case to switch statement in I3GeometryDecomposer

payload-parsing
~~~~~~~~~~~~~~~
* using new "target_link_libraries" for BSD in case GotoBLAS2 is used

steamshovel
~~~~~~~~~~~
* Removed Qt4 support.

icetray
~~~~~~~
* (r177030) Made zstd optional.
* add `__bool__` operator to `I3Bool` and `I3Int` in addition to `__nonzero__`
  in addition to `__nonzero__`. python2 uses `__nonzero__` but python3 uses `__bool__`.
* (r3063) Removed I3OpNewServiceFactory.
* Moved DeleteUnregistered module to dataio.

truncated_energy
~~~~~~~~~~~~~~~~
* Added dEdxVector to the Bins method that stores muon energy losses as a vector

cmake
~~~~~
* Clean up documentation, fixing urls, stripping ancient history
* Update geant4
* Update cppzmq
* Update gsl detection for tarballs
* Update Minuit2 detection
* Update parasite project variables
* Update "meta info" collection and variables
* Declare "highlander" to be "combo/trunk"


  
