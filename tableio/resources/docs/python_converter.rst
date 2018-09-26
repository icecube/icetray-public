.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

Writing an I3Converter in Python
==================================

.. highlight:: python

The converter implementation
******************************

It is pretty simple to implement a converter in Python. All you need to do is
define a class that inherits from I3Converter and implements two methods,
CreateDescription() and FillRows(), and defines a class-level attribute
'booked' holding the type the converter expects. Here, for example, is a complete implementation of a converter for I3LineFitParams::

    from icecube.linefit import I3LineFitParams
    from icecube import tableio

    class I3LineFitParamsConverter(tableio.I3Converter):
        booked = I3LineFitParams
        def CreateDescription(self,lfparams):
            desc = tableio.I3TableRowDescription()
            desc.add_field("lf_vel",tableio.types.Float64,"Gm/s","velocity of the fitted track (from LineFit)")
            desc.add_field("lf_vel_x",tableio.types.Float64,"Gm/s","x-component of the velocity of the fitted track")
            desc.add_field("lf_vel_y",tableio.types.Float64,"Gm/s","y-component of the velocity of the fitted track")
            desc.add_field("lf_vel_z",tableio.types.Float64,"Gm/s","z-component of the velocity of the fitted track")
            desc.add_field("n_hits",tableio.types.UInt32,"number","number of reconstructed hits (or reconstructed pulses, depending on how the fit was done) used to calculate the fit")
            return desc
        def FillRows(self,lfparams,rows):
            rows["lf_vel"]   = lfparams.LFVel
            rows["lf_vel_x"] = lfparams.LFVelX
            rows["lf_vel_y"] = lfparams.LFVelY
            rows["lf_vel_z"] = lfparams.LFVelZ
            rows["n_hits"]   = lfparams.NHits
            return 1

    tableio.I3ConverterRegistry.register(I3LineFitParamsConverter)

The converter expects a FrameObject of type linefit.I3LineFitParams and produces an I3TableRowDescription with 5 fields. The last line registers the converter so that it can be used automatically when objects of type linefit.I3LineFitParams are encountered in the frame.

Including Python converters in your project
*********************************************

CMakeLists.txt
________________

First, you should add a python directory to your project if it doesn't already have one::

    i3_project(linefit PYTHON_DIR python)

`python/__init__.py`
_____________________

You should also create a directory, e.g. linefit/python, containing a file `__init__.py` with the following contents::

    from icecube.load_pybindings import load_pybindings
    load_pybindings(__name__,__path__)
    
    try:
        import icecube.tableio
        import converters
    except ImportError:
        pass

The call to :func:`load_pybindings` merges the python bindings for the C++ bits of your project into the script. The try/except block will attempt to import `tableio`, then the file `converters.py`, failing gracefully if `tableio` can not be found. This will make converters available whenever the bindings for LineFit are imported into Python.

`python/converters.py`
________________________

Any converters should be defined in `converters.py`, including the call to :func:`tableio.I3ConverterRegistry.register`.

FAQs
*********************************************

How do I create a description for an enumerated type?
______________________________________________________

You can construct an I3Datatype from a boost-python-wrapped enum like this::

    desc = tableio.I3TableRowDescription()
    desc.add_field('type', tableio.I3Datatype(dataclasses.I3Particle.ParticleType), '', '')




