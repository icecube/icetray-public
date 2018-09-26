.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

Writing and extending converters
================================

tableio makes it particularly easy to write converters for FrameObjects defined in your own project. These can be written either in Python or C++.

A converter must complete two tasks:

1. Create a table description based on a frame object. This includes the data
type, name, unit, and docstring of each field, as well as the number of
elements in the field if it should store an array.

2. Fill a table structure with data extracted from the frame object. This will
be passed to the table writer service, which interprets it according to the
description and writes it out to disk. The data may come directly from the frame object or be the result of a calculation (e.g. the number of entries in a DOMLaunchSeriesMap, which can be useful for identifying high-energy events).

Best practices
**************

First, there are a few conventions you should follow to make the output of your converter useful.

Field names
___________

Your field names should be all lower-case, separated by underscores, like 'raw_atwd_0', 'fit_status', or 'zenith'. 

Units
_____

*Always* include unambiguous units in your table descriptions. Even if you're using the standard I3Units (e.g. gigameters per second for velocity), you should document it.

Docstrings
__________

Include a useful documentation string whenever applicable. This is especially important if there is *any* possible ambiguity about how the field's value was calculated or should be interpreted. 

Data types
__________

In Python, you use the predefined types (e.g. tableio.types.Float32 or
tableio.types.UInt8) to specify the type of the field. For enumerated types,
you can create an I3Datatype from a boost-python-wrapped enum as shown in the
example below.

.. highlight:: c++

In C++, you have slightly more freedom and thus more responsibility. In order
to prevent the output of your converter from depending on the architecture and
operating system on which it is compiled, you should use the C99 fixed-length
integer types rather than the C89 integers, whose length is
implementation-defined. If you require a 64-bit unsigned integer, write::

    descPtr->AddField<uint64_t>("far","m","Distance to Markarian 501")

rather than the ambiguous::

    descPtr->AddField<unsigned long>("far","m","Distance to Markarian 501")

which will create a 32-bit field on 32-bit systems and a 64-bit field on
64-bit systems.

.. note:: Fields are arranged in the table in the order they were added.
	It is best to arrange the types from largest to smallest
	(e.g. doubles, then 32-bit integers, then bytes) in order to achieve
	the most efficient memory layout.

Writing new converters
**********************

You can write converters the easy way, :doc:`python_converter`, or the hard way, :doc:`cpp_converter`.

Extending an existing converter
*******************************

It is particularly simple to add additional fields to an existing
converter. Suppose that for a particular analysis, we wish to extend
the I3Particle converter to book celestial coordinates in addition to
detector coordinates. We would define a new converter for I3Particle
that looks something like this:

.. code-block:: python

    class SkyBooker(tableio.I3Converter):
        """Demo of a booker extension, e.g. to book celestial coordinates"""
        def CreateDescription(self,part):
            desc = tableio.I3TableRowDescription()
            desc.add_field('RA', tableio.types.Float64,'radian','right ascension')
            desc.add_field('Dec',tableio.types.Float64,'radian','declination')
            desc.add_field('galactic_latitude (epoch 2000)', tableio.types.Float64,'radian','Galactic latitude (epoch 2000)')
            desc.add_field('galactic_longitude',tableio.types.Float64,'radian','Galactic longitude')
            return desc
        def Convert(self,particle,row,frame):
            """Here, we would have to get the event time from the header
            In order to transform from detector to celestial coords."""
            # coordinate-service magic
            coord = coordinate_service.Coordinate(frame['I3EventHeader'],particle)
            row['RA'] = coord.RA
            row['Dec'] = coord.Dec
            row['galactic_latitude'] = coord.GalacticLatitude
            row['galactic_longitude'] = coord.GalacticLongitude
        
            return 1

We can then pass a *list* of converters to I3TableWriter instead of a single converter instance::

        types = {dataclasses.I3Particle: [I3ParticleConverter(), SkyBooker()]}

Each converter will get a chance to fill the table structure with the appropriate data.
