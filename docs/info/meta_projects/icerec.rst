######
Icerec
######

The icerec meta-project contians all the processing and reconstruction
projects nescessary for the online filter as well as the offline processing.
In order for a reconstruction to be included in processing if must first
be added to icerec. Major parts of **icerec**
are hit/pulse extraction and cleaning, fast (first guess)
reconstruction methods that will operate on Pole and elaborate
(Likelihood) algorithms to work offline.
**icerec** also inherits projects from the :doc:`offline software <offline_software>`
meta-project.

In order for a new project included in **icerec**, It first must be thoroughly
:doc:`documented </projects/cmake/documentation>`,
:doc:`code reviewed </general/code_reviews>`, and have appropiate
:doc:`unit tests </projects/cmake/i3test>`.
Further tests will encompass physics comparisons with simulated and real data.

Calibration and Feature extraction
----------------------------------

Projects involved in translating InIce DOM records into pulses

.. list-table::
   :widths: 20 20 60   
   :header-rows: 1
   :stub-columns: 1
		 
   * - Project
     - Maintainer
     - Description 
   * - :ref:`wavedeform-main`
     - Jim Braun
     - Deconvolvs DOM responses into photons using linear algebra.
   * - :ref:`wavereform-main`
     - Jim Braun
     - Convolves pulses back into waveforms to check the accuracy of wavedeform.
   * - :ref:`portia`
     - Aya Ishihara
     - Simple algorithm to convert waveforms into pulses, used for high-energy reconstructions.


Pulse Cleaning
--------------------------

Hit cleaning and veto algorithms 

.. list-table::
   :widths: 20 20 60
   :header-rows: 1
   :stub-columns: 1

   * - Project
     - Maintainer
     - Description
   * - :ref:`DomTools`
     - Alex Olivas
     - Grab bag of stuff including some pulse cleaning 
   * - :ref:`STTools`
     - Markus Vehring
     - Cleans pulses based on Space and Time
   * - :ref:`static-twc`
     - **NONE**
     - Cleans pulses based on time windows.
   * - :ref:`NoiseEngine-main` 
     - Michael Larson
     - Identify triggers caused only by noise
   * -  :ref:`StartingTrackVeto`
     -  Kyle Jero
     -  find starting tracks based on location of early hits
   * -  :ref:`VHESelfVeto`
     - Claudio Kopper
     - implements simple detector edge veto to identify in-detector starting events at high energy
   * - :ref:`CommonVariables`
     - Anna Obertacke Pollmann
     - Common routines for calculating cut variables
       

Filtering and Triggering
------------------------

Trigger, Filter implementations and DST.

.. list-table::
   :widths: 20 20 60      
   :header-rows: 1
   :stub-columns: 1

   * - Project
     - Documentation
     - Description
   * - :ref:`DeepCore_Filter`
     - Mike Larson
     - A filter designed to reject obvious atmospheric muons for the low-energy analyses using DeepCore.
   * - :ref:`trigger-sim`
     - Alex Olivas
     - simulates the online trigger


Splitters
------------------------

.. list-table::
   :widths: 20 20 60      
   :header-rows: 1
   :stub-columns: 1

   * - Project
     - Maintainer
     - Discription		  
   * - :ref:`trigger-splitter`
     - Alex Olivas
     - Splits Q-frames into P-frames based on trigger information.
   * - :ref:`TopologicalSplitter`
     - Chris Weaver
     - Split events into caussialy disconnected groups
   * - :ref:`HiveSplitter`
     - **NONE**
     - Split events into caussialy disconnected groups
   * - :ref:`IceHive`
     - Kai Krings 
     - Split events into caussialy disconnected groups but even more general than HiveSplitter
   * - :ref:`CoincSuite`
     - Kai Krings 
     - Identifies false split



Libraries
----------

Libraries and container classes

.. list-table::
   :widths: 20 20 60         
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Maintainer
     - Description		 
   * - :ref:`simclasses`
     - Alex Olivas
     - Container classes for used by simulation
   * -  :ref:`recclasses`
     - **NONE**     
     - Container classes for storing the results of various reconstruction 
   * - :ref:`pybdt` 
     - Mike Richman
     - Machine learning algorithm for signal/background discrimination using BDT classifiers 
   * - :ref:`ipdf`
     - Kevin Meagher
     - Analytic probibility distributions for photon tracking
   * - :ref:`photonics-service`
     - Andrii Terliuk
     - Table based probibility distributions for photon tracking
   * - :ref:`photospline`
     - Jakob van Santen
     - Spine based probibility distributions for photon tracking
   * - :ref:`gulliver`
     - Kai Krings
     - generic framework for Log-Likelihood-based Reconstructions plugable interface for likelihood functions, minimizers, parametrization and seed preparation.
   * - :ref:`lilliput`
     - Kai Krings
     - implementations of various gulliver plugins
       
Muon Directional 
----------------

Muon hypothesis based reconstructions.

.. list-table::
   :widths: 20 20 60            
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Documentation
     - Description
   * - :ref:`linefit`
     - Giuliano Maggi
     - Robust, fast first-guess muon reconstruction.
   * - :ref:`dipolefit`
     - Alex Olivas
     - First guess algorithm which uses dipole moments of pairs of hits
   * - :ref:`gulliver-modules`
     - Kai Krings
     - Modules which implement gulliver log-liklihood reconstructions     
   * - :ref:`bayesian-priors`
     - Kevin Meagher
     - Zenith biased gulliver reconstruction for generating quailty cuts
   * - :ref:`finiteReco`
     - **None**
     - Identifies and reconstructs finite (starting, stopping or fully contained) muon tracks based on the hit/no-hit probability
   * - :ref:`ophelia`
     - Aya Ishihara
     - Quick reconstruction for EHE events
   * - :ref:`double-muon`
     - Kevin Meagher
     - Evaluates multiple muon hypoteses for log-liklihood reconstructions
   * - :ref:`spline-reco-main`
     - **None**
     - log-liklihood with spline base photon probabilities
   * - :ref:`shield`
     - Chris Weaver
     - Veto for cosmic-ray muons which should have hit icetop

Muon Directional Uncertianty
----------------------------

.. list-table::
   :widths: 20 20 60               
   :header-rows: 1
   :stub-columns: 1

   * - Project
     - Documentation
     - Description		  
   * - :ref:`cramer-rao`
     - Jan Lüneman
     - Analytic muon angular uncertianty calculation
   * - :ref:`paraboloid`
     - Kevin Meagher
     - Estimates angular uncertainty by exploring the log-liklihood space around the minimum
   * -  :ref:`gulliver-bootstrap`
     - Chris Weaver
     - estimation of the angular uncertainty of track reconstructions using a resapleing technique
       
Muon Energy
-----------

.. list-table::
   :widths: 20 20 60                  
   :header-rows: 1
   :stub-columns: 1

   * - Project
     - Documentation
     - Description		  
   * - :ref:`mue`
     - Dima Chirkin
     - Estimate energy losses along muon track using an icemodel 
   * - :ref:`millipede-main`
     - Jim Braun
     - Unfolds multiple energy loss segments along a muon track
   * - :ref:`truncated_energy`
     - Tomasz Palczewski
     - Calculates the muon energy by removing the highest energy losses and averaging the remaining losses
   * - :ref:`ddddr`
     - Javier Gonzalez
     - Calculate energy loss assuing exponential light attenuation in bulk ice
   * - :ref:`stochastics`
     - Javier Gonzalez
     - Converts enegy losses calculated by millipede into a single energy parameter assuming the Elbert formula. 

Cascade reconstruction
----------------------

Cascade hypothesis based reconstructions.

.. list-table::
   :widths: 20 20 60                     
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Maintainer
     - Description
   * - :ref:`core-removal`
     - **NONE**
     - Splits pulses into core region and outer region for cascades
   * - :ref:`fill-ratio`
     - Mike Larson
     - Calculate how much of an event fills out a specified volume
   * - :ref:`tensor-of-inertia`
     - Alex Olivas
     - Calculates the spatial eigenvalues of a the pulses in an event
   * - :ref:`clast`
     - Mark Aartsen
     - First guess algorithm based on center-of-gravity and tensor of inertia
   * - :ref:`cscd-llh`
     - Tomasz Palczewski
     - Log-likelihood reconstruction for cascades
   * - :ref:`credo`
     - **NONE**
     - Log-likelihood reconstruction for cascades uses photonics for icemodel and full timing information
   * - :ref:`CascadeVariables`
     - Mark Aartsen
     - Calculate Cascade specific cut variables

IceTop
----------

IceTop related projects: hit extraction, calibration, cleaning, and reconstruction.

.. list-table::
   :widths: 20 20 60                        
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Maintainer
     - Description
   * - :ref:`coinc-twc`
     - Sam de Ridder
     - Looks for InIce triggers which are causally connected with IceTop triggers
   * - :ref:`smallshower-filter`
     - Timo Karg
     - Selects small, fully contained cosmic ray shower with IceTop.       
   * - :ref:`toprec`
     - Kath Rawlins
     - Contains several modules for IceTop reconstructions
   * - :ref:`topeventcleaning-main`
     - Timo Karg
     - IceTray modules for IceTop event cleaning and splitting.
   * - :ref:`tpx-main`
     - Timo Karg
     - Modules for IceTop pulse extraction and calibration.
   * -  :ref:`vemcal`
     - **NONE**
     -  Vertical Equivalent Muon Calibration for IceTop


Monopoles
---------

Monopole

.. list-table::
   :widths: 20 20 60                           
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Maintainer
     - Discription
   * - :ref:`KalmanFilter-main`
     - Emanuel Jacobi
     - Tracks subrelativistic magnetic monopoles
   * - :ref:`SLOPtools-main` 
     - Emanuel Jacobi
     - Collection of scripts used to process the SLOP triggered data. 
       
Utilities
----------

Various utilities.

.. list-table::
   :widths: 20 20 60                              
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Maintainer
     - Description
   * - :ref:`BadDomList`
     - Jan Oertlin
     -
   * -  :ref:`full_event_followup-main`
     -  Claudio Kopper
     -  Reader and writer modules to convert single frames for fast followup transmission via iridium 
   * -  :ref:`frame_object_diff`
     - Alex Olivas
     - makes diffs of frame object which don't change very often such as Geometery
   * - :ref:`dst`
     - Juan Carlos Diaz Velez
     - Conscise record of every event IceCube observes
   * - :ref:`gcdserver`
     - Jim Braun
     - Imports and exports I3Geometry, I3Calibration, and I3DetectorStatus objects to and from the mongoDB.
   * -  :ref:`production-histograms`
     - Alex Olivas
     - Provides lightweight histograms in production samples.

Processing Scripts
-------------------

.. list-table::
   :widths: 20 20 60                                 
   :header-rows: 1
   :stub-columns: 1		 

   * - Project
     - Maintainer
     - Discription
   * -  :ref:`filterscripts`
     - Erik Blaufuss 
     - Scripts for processing L1 (pole filter) and L2 (northern hemisphere processing)
   * -  :ref:`level3-filter-cascade`
     - Yiqian Xu
     - Scripts for Cascade L3
   * -  :ref:`level3-filter-muon`
     - **NONE**
     - Scripts for Muon L3
   * -  :ref:`level3-filter-lowen`
     - **NONE**
     - Scripts for low energy L3
   * -  :ref:`icetop_Level3_scripts`
     - Javier Gonzalez
     - Scirpts for IceTop and Cosmic Ray L3

