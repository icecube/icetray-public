Using the Geometry Selector
---------------------------

Description:
&&&&&&&&&&&&

This module now has four parameters

 - StringsToUse - Default is -19:80
 - StringsToExclude - Default is empty (i.e. nothing is excluded)
 - StationsToUse - Default is 1:80
 - StationsToExclude - Default is empty (i.e. nothing is excluded)
 - NewGeometryName - Default is "NewGeometryName"  This is the name of the new Geometry that is put in the frame.  Modules later on down the road will need to use this geometry instead.
 - ShiftX - Distance to shift the whole detector in the x direction.
 - ShiftY - Distance to shift the whole detector in the y direction.
 - ShiftZ - Distance to shift the whole detector in the z direction.

The default values for StringsToUse is the entire 80 string IceCube 
detector and AMANDA (i.e. "-19:80").  No strings are excluded by default.

The default for IceTop is to use all tanks (1:80).

Examples values for the parameters are...

 - Use only the IceCube detector. Either set StringsToUse to "1:80" or set StringsToExclude to "-19:0" (remember the default value for StringsToUse is "-19:80").

 - Use the current geometry (as of mid Jan '06). Set StringsToUse to "21,29,30,38,39,-19:-1"

 - To shift the detector in the x direction to perhaps center a 9 string detector in the middle of corsika showers, set ShiftX to -450.0 * I3Units::m.

There's one simple example script (FileGeometrySelectorExample.py) in
phys-services/resources/scripts.  This illustrates its use but doesn't
really show that it works.  I should probably come up with something more
convincing.  One could always tack on the eventviewer at the end and
"see" that the strings are correctly removed.  There are also various tests
one could look at.
