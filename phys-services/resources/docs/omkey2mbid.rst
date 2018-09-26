The OMKey2MBID Service
----------------------

Introduction
&&&&&&&&&&&&

The IceCube DAQ prefers to use a DOM's main board id (MBID) to refer to 
individual DOMs.  In the IceCube offline environment, we prefer to 
refer to a DOM by it's position in the array, it's OMKey.

When interfacing to data provided by the DAQ it is often necessary to 
easily convert MBID to OMKey and back again.  That is what the I3OMKey2MBID 
service is for.

Provided in phys-services is the I3OMKey2MBID abstract class interface and
one implementation that reads the conversions from a file.  This 
implementation, the I3FileOMKey2MBID class takes a string as a constructor.
Check the file @ref omkey2mbidformat to see the expected format.  The 
installer for this service the I3FileOMKey2MBIDFactory takes one parameter
'Infile' which is the path to the required input file.

