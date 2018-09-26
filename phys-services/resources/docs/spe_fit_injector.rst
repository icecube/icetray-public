
I3SPEFitInjector
^^^^^^^^^^^^^^^^
This class simply loads SPE fit values from a JSON file (provided by the 
Calibration Group) into the I3DOMCalibration class.  I3DOMCalibration has 
both a struct with combined fit values (SPEChargeDistribution) and mean
values for the FADC and ATWD channels and accessed via C++ as follows :

* I3DOMCalibration::GetMeanATWDCharge()
* I3DOMCalibration::GetMeanFADCCharge()
* I3DOMCalibration::GetCombinedSPEChargeDistribution()

Caveats
!!!!!!! 

* Missing entries in the I3Calibration::domCal map - This can happen if the combined fit, mean ATWD, **and** mean FADC charges are all marked invalid in the JSON file.
* Downstream code should check that the mean ATWD and FADC charges are valid using the following convenience methods:

 - I3DOMCalibration::IsMeanATWDChargeValid
 - I3DOMCalibration::IsMeanFADCChargeValid

For more information :
https://wiki.icecube.wisc.edu/index.php/SPE_recalibration

Code Review
^^^^^^^^^^^
.. toctree::
   :titlesonly:

   spe_fit_injector_code_review
   
