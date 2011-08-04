=========================
 Python bindings changed
=========================

The interface to many of the dataclasses python bindings have undergone an
extensive update.  The idea behind this update was to make the python
bindings to dataclasses more complete, more uniform, and more "pythonic".


Generally, we are following the Python PEP-8 code standards:
http://www.python.org/dev/peps/pep-0008/

With the following guidelines

#. Prefer the use of python properties rather than exposing the Get/Set methods
#. use snake_case for property/function names rather than CamelCase.
#. provide __str__ pretty print messages (ideally also in c++)
#. time-like objects use standard names "time" for relative times and
    items using the I3Time class use names like start_time/end_time.
#. make sure enums are bound, and included at the proper scope with the class.

As an example, this generally changes the interfaces such as:
::
  dataclasses.I3EventHeader.EventID -> dataclasses.I3EventHeader.event_id
  dataclasses.I3DOMLaunch.GetLCBit -> dataclasses.I3DOMLaunch.lc_bit
  dataclasses.I3Particle.GetAzimuth -> dataclasses.I3Particle.azimuth   

A good example of the dataclasses examples can be found in the test script
::
  $I3_SRC/dataclasses/resources/test/workout_pybindings.py