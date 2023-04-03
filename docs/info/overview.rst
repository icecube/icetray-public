Overview
========

What is IceTray?
----------------

IceCube core software is the suite of software used to provide a uniform
analysis framework for simulation, reconstruction and analysis modules
to work within. For IceCube, this software includes the IceTray
framework, the Dataclasses data storage classes, the event viewer,
Input/Output file utilities and tools providing access to external
services such as databases.

Nominally, there are several places this suite of software will be used:

#.  On a worker node in the CPU farm attached to the online filtering
    system running at South Pole.

#.  In the "production" processing of events as they are checked into
    the data warehouse in the Northern hemisphere.

#.  In the generation of production simulation data.

#.  On the desk of the physicist doing analysis of data and/or
    developing reconstruction/analysis routines.

This software framework has been designed to be modular. A user of
this system does not write a main() routine. A user will write a
module which will perform the analysis or reconstruction task. The
users module can then be combined with other modules in a steering
file to direct the operation of the framework. Some modules, such as
I/O, database access tools, and other such services are made available
to all users in a standard toolbox of modules. Using this system, a
user, for example, can write a new hit cleaning algorithm, plug this
new module into the steering file in place of the standard one, and
test their code with a standard reconstruction library. Additionally,
a simple reconstruction or analysis program can be put together from
existing modules in the standard toolbox.

In principle, a user's reconstruction module developed in this way
should be able to plug into any one of these places without needed
changes to the module. The only changes that would be needed would be
in the choice of toolbox modules to read data. For example, the
filtering worker needs an input module that reads data from a socket
connection, while the desktop analysis needs an input module that
retrieves that data from the data warehouse or local ROOT file. This
approach should make it easy for modules developed offline to be moved
into production (data warehouse and/or filter farm) locations.

Component overview
------------------

There are several major components:

#. IceTray framework - The software framework is the glue that holds
   the other pieces (modules) together and provides control of
   flow. It does not make any changes to data.

#. Dataclasses storage classes - An offline, in-memory representation
   of the data that is produced by the IceCube detector data
   acquisition system or by IceCube simulations. Data from
   post-acquisition reconstruction, processing and analysis is added
   to the dataclassses structures as the event data is passed from
   module to module. This is the primary means of recording
   reconstruction results, analysis results, and communicating between
   modules.

#. Services - Common tools used by many modules running in the
   framework. Examples include database access and random
   number generation.

#. User modules - User written modules that perform a reconstruction,
   an analysis, etc., and add new information to the dataclasses
   structures.

#. I/O Modules - Special modules for reading and writing data files
   (so-called i3 files).

These pieces fit together to create an environment to read,
reconstruct, analyze, view and write data.  Additionally, strong
bindings to python make it possible to do much of this work directly
from python, including:

#. Loading and using most icetray modules directly in python interactively.

#. Writing IceCube modules (from very simple event selections to
   complicated reconstructions) directly in python.

#. Easily accessing advanced analysis tools (like Matplotlib or ROOT
   (via PyROOT))
