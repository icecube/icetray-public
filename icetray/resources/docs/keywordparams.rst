.. _i3tray_keyword_parameters:

More natural parameter-passing syntax to I3Tray
===============================================

The first new feature is small but significant: the addition of a more
pythonic keyword-parameter syntax for passing parameters to modules
and services.  The standard way to configure modules has been this:

.. code-block:: python

   #
   #    Deprecated, old syntax
   #
   tray.AddModule("I3DOMcalibrator", "calibrate-inice")(   
       ("CalibrationMode", 0),
       ("OutputToFile", False),
       ("InputRawDataName", "InIceRawData"),
       ("OutputATWDDataName", "InIceCalibratedATWD"),
       ("OutputFADCDataName", "InIceCalibratedFADC")
       )

Where the call to :func:`AddModule` is followed by a tuple of pairs,
the first containing the name of the parameter as a string, the second
containing the corresponding parameter value.  This could provoke 
confusing errors if you forget a comma:

::

  File "./DOMcalibrator/resources/scripts/DOMcalibratorTest.py", line 39, in <module>
    ("OutputATWDDataName", "IceTopCalibratedATWD"),
  TypeError: 'tuple' object is not callable

The new syntax is typical of keyword parameters in python:

.. code-block:: python

   tray.AddModule("I3DOMcalibrator", "calibrate-inice",
                  CalibrationMode = 0,
		  OutputToFile    = False,
		  InputRawDataName = "InIceRawData",
                  OutputATWDDataName = "InIceCalibratedATWD",
                  OutputFADCDataName = "InIceCalibratedFADC")

Which is less visually noisy, property indented by most editors, and 
provokes a much more sensible error message should you forget a comma:

::

  % ./DOMcalibrator/resources/scripts/DOMcalibratorTest.py
    File "./DOMcalibrator/resources/scripts/DOMcalibratorTest.py", line 38
      InputRawDataName = 'IceTopRawData',
		     ^
  SyntaxError: invalid syntax

.. note::

   *Currently, both syntaxes work*, and may be mixed together in the same
   file, but the keyword-parameters version is preferred.  In some
   future version of icetray the tuple-of-tuples version will
   disappear.





