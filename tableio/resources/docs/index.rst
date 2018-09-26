.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

.. highlight:: python

.. _tableio-main:

tableio
=================

A generic framework for producing tabular data for analysis from IceCube data
files. This project is maintained by Jakob van Santen <jvansanten@gmail.com>.

.. toctree::
   :maxdepth: 1
   
   release_notes

Rationale
^^^^^^^^^

The I3 file format is excellent for passing data from module to module during
data processing, but is a terrible format for analysis and plotting. For these
tasks, it is often more convenient to dump a subset of the physics data
contained in an I3 file to a format that can be easily read in to one's
analysis tool of choice, such as HDF5 or ROOT files, or even plain text. A
number of projects, including analysis-tree, flat-ntuple, hdf5booker, and
hdf-writer, were created over the years to extract physics data from I3 files.
This kind of diversity is actually a problem, as it inhibits communication
between analyzers using different booking tools. In addition, many previous
attempts at booking were difficult to modify and extend without replacing
large swaths of their internal code. 

This project was designed to create a single, generic booking framework that
is modular in both input and output. In addition, it allows coders to document
any calculations they make by assigning units and a documentation string to
every field.

Design
^^^^^^

Writing data to a table involves three components:

1. A *Converter* that takes a specific FrameObject, defines the types and names of the appropriate table columns, and fills a table structure with the data in that object

2. A *Writer Service* that writes the filled table structures out to a specific file format (e.g. an HDF5 table, a ROOT ntuple, or a CSV text file ), 

3. The *Table Writer*, an I3Module that coordinates activities between the two.

The modularity of the design comes in three parts:

1. Converters can be spread across different projects (e.g. a converter for I3FilterResultMap would live in the jebclasses project), and new converters can be written quickly in Python.

2. Data can be written to a different file format simply by swapping out the writer service.

3. Which frame objects are booked and *how* they are booked is configured in Python (i.e. you don't have to create an entirely new branch of a project to change small things).

Table of Contents:

.. toctree::
   :maxdepth: 3

   howto
   existing_converters
   converters
   python_converter
   cpp_converter
   container_converter
   make_a_writer_service
   transcriber
   faq


