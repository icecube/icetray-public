.. 
.. copyright  (C) 2011
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$


Existing converters
===================

.. highlight:: python

Converters for classes provided by the dataclasses library come with tableio while converters reconstruction specific classes come with the respective projects.
Additionally there are some convenience converters in phys-services and coordinate-service.
This page lists the dataclasses and phys-services converters and their options.
Options can be passed to the converters by specifying them as a custom converter, e.g.::

    keys = [ dict(key='IceTopVEMPulses', converter=dataclasses.converters.I3RecoPulseSeriesMapConverter(bookGeometry=True)) ]


dataclasses
___________

.. highlight:: c++

:I3EventHeaderConverter: Dumps an ``I3EventHeader`` object.

:I3BoolConverter, I3IntConverter, I3DoubleConverter: Dump ``I3Bool``, ``I3Int``, and ``I3Double`` objects.

:I3DirectionConverter: Dumps an ``I3Direction`` object.

  *Options:*

  ``BookRefFrame`` (enum `icecube.dataclasses.converters.I3DirectionConverter.BookRefFrame`)

      Books direction data only for the specified reference frame. The following
      enum values exist:

      - ``Car``: Cartesian reference frame
          *Fields*: ``x``, ``y``, ``z``

      - ``Sph``: Spherical reference frame
          *Fields*: ``azimuth``, ``zenith``

      - ``All``: Books position data for all reference frames
          *Fields*: ``x``, ``y``, ``z``, ``azimuth``, ``zenith``

      Default value: ``Car``

  ``TableColumnNamePrefix`` (str)

      The prefix string that should be prefixed to all table column names.
      This is useful when the converter is used internally by an other
      converter to book an I3Direction inside its own table.

      Default value: ``""``

:I3ParticleConverter: Dumps an ``I3Particle`` object.

:I3PositionConverter: Dumps an ``I3Position`` object.

  *Options:*

  ``BookRefFrame`` (enum `icecube.dataclasses.converters.I3PositionConverter.BookRefFrame`)

      Books position data only for the specified reference frame. The following
      enum values exist:

      - ``Car``: Cartesian reference frame
          *Fields*: ``x``, ``y``, ``z``

      - ``Cyl``: Cylindrical reference frame    
          *Fields*: ``rho``, ``z``, ``phi``

      - ``Sph``: Spherical reference frame
          *Fields*: ``r``, ``phi``, ``theta``

      - ``All``: Books position data for all reference frames
          *Fields*: ``x``, ``y``, ``z``, ``r``, ``rho``, ``phi``, ``theta``

      Default value: ``Car``

  ``TableColumnNamePrefix`` (str)

      The prefix string that should be prefixed to all table column names.
      This is useful when the converter is used internally by an other
      converter to book an I3Position inside its own table.

      Default value: ``""``

:I3Vector*Converter: Dump ``I3Vector*`` objects.
  
  *Variants:* Bool, Short, UShort, Int, UInt, Int64, UInt64, Float, Double, DoubleDouble (pair of doubles).

:I3*SeriesMapConverter: Dump ``I3*SeriesMap`` objects.

  *Variants:* DOMLaunch, RecoPulse, RecoHit.

  *Options:*

  ``bookGeometry`` (bool) books OM geometry for the hit DOMs.

  ``bookToParticle`` (string) books perpendicular distance to track, longitudinal distance along track, and distance to track vertex for the hit DOMs.  The string is the name of the track.

:I3RecoPulseSeriesMapMaskConverter: Applies the mask, then dumps the resulting RecoPulses.
  
  *Options:*

  ``bookGeometry`` (bool) books OM geometry for the hit DOMs.

  ``bookToParticle`` (string) books perpendicular distance to track, longitudinal distance along track, and distance to track vertex for the hit DOMs.  The string is the name of the track.

:I3WaveformSeriesMapConverter: Dumps a single I3WaveformSeriesMap.
  
  *Options:*
  
  ``bookGeometry`` (bool) books OM geometry for the hit DOMs.
  
  ``calibrate`` (bool) calibrate in pe/bin.

:I3MapKeyVectorDoubleConverter, I3MapKeyVectorIntConverter: Dump ``I3Map<OMKey, vector<double> >`` and ``I3Map<OMKey, vector<int> >`` objects.

  *Options:*

  ``bookGeometry`` (bool) books OM geometry for the hit DOMs.

  ``bookToParticle`` (string) books perpendicular distance to track, longitudinal distance along track, and distance to track vertex for the hit DOMs.  The string is the name of the track.

:I3MapStringDoubleConverter: Dumps a ``std::map<string,double>`` verbatim.

.. highlight:: python

:I3WaveformConverter: Dumps ATWD and FADC waveforms together.
  Does not have a default constructor, but needs to be added explicitely with FADC and ATWD waveform names.
  If you only want to book one ``I3WaveformSeriesMap`` use the ``I3WaveformSeriesMapConverter``.
  
  *Constructor arguments:* ATWD name, FADC name, calibrate to pe/bin
  
  *Example:* ``keys = [ dict(key='MyATWDPulses', converter=dataclasses.converters.I3WaveformConverter('MyATWDPulses', 'MyFADCPulses', True)) ]``

:I3MCTreeConverter: Dumps all particles in the MC Tree.

:I3TriggerHierarchyConverter: Dumps all triggers in the I3TriggerHierarchy tree.

:I3FlasherInfoVectConverter: Dumps an I3FlasherInfoVect.


phys-services
_____________

.. highlight:: python

:I3EventInfoConverterFrom*: Dumps event information, special converter for various ``*SeriesMap`` objects.
  
  *Variants:* RecoPulse, RecoHit, MCHit, DOMLaunch, Waveform.
  
  *Fields:* NChannel, NChannel (1 hit), NStrings, NHit, total charge, time of earliest hit, event length.

:I3CutValuesConverter: Dumps an ``I3CutValues`` object verbatim.

:I3RecoInfoConverter: Calculates number of direct hits and icecube scale cut variables. Special converter for ``I3Particle``.

  *Constructor argument:* Name of an ``I3RecoPulseSeriesMap`` object.
  
  *Fields:* NDir[A-E], LDir[A-E], number of early and late hits.
  
  *Example:* ``keys = [ dict(key='MyI3Particle', converter=phys_services.converters.I3RecoInfoConverter('MyRecoPulses')) ]``


coordinate-service
__________________

:I3SkyCoordinateConverter: Books sky coordinates. Special converter for ``I3Particle``.
  
  *Optional constructor argument:* Name of an ``I3EventHeader``. Defaults to ``I3EventHeader``'s default name. Normally, you don't need to specify this.
