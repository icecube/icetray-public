********************************
Code Review for I3SPEFitInjector
********************************

svn info
........
- URL: http://code.icecube.wisc.edu/svn/projects/phys-services/trunk
- Repository UUID: 16731396-06f5-0310-8873-f7f720988828
- Revision: 131203
- Last Changed Author: olivas
- Last Changed Rev: 131145
- Last Changed Date: 2015-04-07 08:35:19 -0600 (Tue, 07 Apr 2015)

Code 
....
Claudio (via slack...paraphrasing) :

spe_fit_injector.py
```````````````````
Recommended changing copying the frame object and 
immediately deleting it to mimic const-ness on the C++ side.  This protects 
against silently modifying frame objects on-the-fly causing weird problems for 
C++ code that reasonably expects and relies on the const-ness of frame objects.

Remove whitespace before ':' to appease Guido.

"Compound statements are generally discouraged."  Move 'continue' to the next line
with proper indentation.

Configure uses a local variable *fit_dict* and a class variable *self.fit_dict*.
This may be a little confusing so the local variable should be renamed to
something like *json_fit_dict*.  I chose *json_fit_values*.

Olivas :
Make the python 3 crowd happy by using print functions.

Tests
.....
Olivas :
There's one test that reads the frame object in the Calibration stop and verifies
that the correct number of valid entries exists.  That should be pretty complete
line coverage, but there could be some cases that aren't coming to me that I 
could be testing for.

Documentation
.............


