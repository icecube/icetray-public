Python bindings to dataclasses
------------------------------

Examples
^^^^^^^^

Get an object out of the frame
""""""""""""""""""""""""""""""

With a frame like this:

.. highlight:: pycon

::

  >>> from icecube import icetray, dataclasses, dataio
  >>> i3f = dataio.I3File("/v/icecube/data/level3.nugen_numu.000768.000111.i3.gz")
  >>> frame = i3f.pop_physics()
  >>> print frame
  [ I3Frame  (Physics):
    'TrackParaboloidFit32' [Physics] ==> I3Particle
    'FirstSubtrackBayesianFit' [Physics] ==> I3Particle
    'TrackLlhFit32_e' [Physics] ==> I3Particle
    'TrackBayesianFitFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'TrackLlhFit32' [Physics] ==> I3Particle
    'ToIParams' [Physics] ==> I3TensorOfInertiaFitParams
    'TrackLlhFitFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'TrackBayesianFit32FitParams' [Physics] ==> I3LogLikelihoodFitParams
    'SecondSubtrackLlhFitFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'InitialPulseSeriesReco' [Physics] ==> I3Map<OMKey, std::vector<I3RecoPulse, std::allocator<I3RecoPulse> > >
    'InIceRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >
    'I3MCTree_MaxInIceTrackIn' [Physics] ==> I3Particle
    'IceTopRawData' [Physics] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >
    'IcLinefit' [Physics] ==> I3Particle
    'I3EventHeader' [Physics] ==> I3EventHeader
    'TrackLlhFit32Umbrella' [Physics] ==> I3Particle
    'I3TriggerHierarchy' [Physics] ==> I3Tree<I3Trigger>
    'IcLinefitParams' [Physics] ==> I3LineFitParams
    'FirstSubtrackLlhFitFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'FilterMask' [Physics] ==> I3Map<std::string, I3FilterResult>
    'SecondSubtrackLlhFit' [Physics] ==> I3Particle
    'CascadeFirst' [Physics] ==> I3Particle
    'TrackLlhFit' [Physics] ==> I3Particle
    'TrackLlhFit32_LogL_dof' [Physics] ==> I3Double
    'I3MCTree_MaxInIceTrackCenter' [Physics] ==> I3Particle
    'TrackParaboloidFit' [Physics] ==> I3Particle
    'FirstSubtrackLlhFit' [Physics] ==> I3Particle
    'MMCTrackList' [Physics] ==> I3Vector<I3MMCTrack>
    'wfllhFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'rloglFinitephpnh_rusage' [Physics] ==> I3RUsage
    'TrackParaboloidFitFitParams' [Physics] ==> I3ParaboloidFitParams
    'CascadeFirstParams' [Physics] ==> I3CFirstFitParams
    'DrivingTime' [Physics] ==> I3Time
    'TrackParaboloidFit32FitParams' [Physics] ==> I3ParaboloidFitParams
    'I3MCWeightDict' [Physics] ==> I3Map<std::string, double>
    'SecondSubtrackBayesianFit' [Physics] ==> I3Particle
    'TrackLlhFit32FitParams' [Physics] ==> I3LogLikelihoodFitParams
    'TrackBayesianFit32' [Physics] ==> I3Particle
    'I3MCTree' [Physics] ==> I3Tree<I3Particle>
    'MCHitSeriesMap' [Physics] ==> I3Map<OMKey, std::vector<I3MCHit, std::allocator<I3MCHit> > >
    'ToI' [Physics] ==> I3Particle
    'FirstSubtrackBayesianFitFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'I3MCTree_MaxInIceTrackOut' [Physics] ==> I3Particle
    'CascadeLinefitParams' [Physics] ==> I3LineFitParams
    'wfllh' [Physics] ==> I3Particle
    'TrackLlhFit32UmbrellaFitParams' [Physics] ==> I3LogLikelihoodFitParams
    'I3MCTree_MaxInIceTrackCoG' [Physics] ==> I3Particle
    'CascadeLinefit' [Physics] ==> I3Particle
    'TrackLlhFit32_LogL' [Physics] ==> I3Double
    'TrackBayesianFit' [Physics] ==> I3Particle
    'SecondSubtrackBayesianFitFitParams' [Physics] ==> I3LogLikelihoodFitParams
  ]
  
Looking at the mc hits,
::

  >>> hits = frame['MCHitSeriesMap']
  >>> hits
  <icecube.dataclasses.I3MCHitSeriesMap object at 0x8511534>

nchannel
""""""""

We can get 'nchannel'::

  >>> len(hits)
  54

or the list of hit OMs::

  >>> [e.key() for e in hits] 
  [OMKey(21,24), OMKey(21,31),
  OMKey(29,41), OMKey(29,60), OMKey(30,47), OMKey(38,41),
  OMKey(38,58), OMKey(39,13), OMKey(39,46), OMKey(40,42),
  OMKey(40,43), OMKey(40,44), OMKey(40,45), OMKey(40,54),
  OMKey(46,58), OMKey(47,42), OMKey(48,3), OMKey(48,38), OMKey(48,41),
  OMKey(48,44), OMKey(49,15), OMKey(49,36), OMKey(49,43),
  OMKey(49,44), OMKey(49,45), OMKey(49,46), OMKey(50,46),
  OMKey(50,57), OMKey(56,32), OMKey(56,38), OMKey(57,5), OMKey(57,41),
  OMKey(57,42), OMKey(57,43), OMKey(57,44), OMKey(58,10),
  OMKey(58,42), OMKey(58,47), OMKey(59,39), OMKey(59,40), OMKey(65,2),
  OMKey(65,5), OMKey(65,40), OMKey(65,44), OMKey(65,50), OMKey(66,18),
  OMKey(66,52), OMKey(67,40), OMKey(72,37), OMKey(72,41),
  OMKey(72,44), OMKey(73,12), OMKey(73,50), OMKey(74,10)]

nhit
""""

or the total number of hits in all oms;  Create a list of hits per
OM::

  >>> [len(e.data()) for e in hits] 
  [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 5, 2, 1, 2, 1, 1, 1, 1, 1, 1, 5, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 2, 1, 1, 1, 1]

and take the sum::

  >>> sum([len(e.data()) for e in hits])
  70

one can get a list of lists of hit times::

  >>> [[hit.Time for hit in e.data()] for e in hits]
  [[204604.9742586276], [211559.12329093015], [217228.25888738033],
  [202762.55772166929], [203625.54720526567], [223173.56777643564],
  [203903.865336891], [196707.33572210337], [206972.02583769718],
  [206583.63700918012], [205939.05947181155, 207110.31857003138],
  [205962.32537366869, 206270.57986411842], [206487.34792608922,
  214417.52757325419], [195490.18964865553], [195203.18451091682],
  [211080.56898628554], [199945.66162354519], [202687.33103472061],
  [205426.34651168939], [212989.69564489473], [218339.10920387553],
  [223291.45251082265], [205808.24168743056], [205496.01178302418,
  205496.71124786529, 205525.32546579896, 205562.98169248717,
  205609.40689863981], [205537.32113750148, 207372.70384354892],
  [205891.7556336984], [199383.30950566352, 211678.89014103252],
  [195923.0251990071], [194945.97831539251], [196519.39973339965],
  [206981.80230232462], [207692.90224580758], [205368.31951649411],
  [204809.47011011466, 204848.77878282889, 204885.80844991628,
  205360.19684061655, 210251.45781586654], [204799.03252794495,
  205630.88520065596], [198762.71528484017], [205454.05597575437],
  [206051.72036208294, 214004.1349712183], [198149.68830273568],
  [213105.61270986524], [216064.29977506239], [200573.88347293844],
  [205112.0535136415], [204456.76376554792], [205180.55285840927],
  [202969.06534386508], [197885.66878645719], [213020.2059352111],
  [221589.93938874462], [204374.1205059239, 204678.91635859985],
  [204925.51566635544], [194694.86695246131], [197903.13150816062],
  [222911.53660383544]]

or with a nested list comprehension get one flat list::

  >>> [hit.Time for entry in hits for hit in entry.data()]
  [204604.9742586276, 211559.12329093015, 217228.25888738033,
  202762.55772166929, 203625.54720526567, 223173.56777643564,
  203903.865336891, 196707.33572210337, 206972.02583769718,
  206583.63700918012, 205939.05947181155, 207110.31857003138,
  205962.32537366869, 206270.57986411842, 206487.34792608922,
  214417.52757325419, 195490.18964865553, 195203.18451091682,
  211080.56898628554, 199945.66162354519, 202687.33103472061,
  205426.34651168939, 212989.69564489473, 218339.10920387553,
  223291.45251082265, 205808.24168743056, 205496.01178302418,
  205496.71124786529, 205525.32546579896, 205562.98169248717,
  205609.40689863981, 205537.32113750148, 207372.70384354892,
  205891.7556336984, 199383.30950566352, 211678.89014103252,
  195923.0251990071, 194945.97831539251, 196519.39973339965,
  206981.80230232462, 207692.90224580758, 205368.31951649411,
  204809.47011011466, 204848.77878282889, 204885.80844991628,
  205360.19684061655, 210251.45781586654, 204799.03252794495,
  205630.88520065596, 198762.71528484017, 205454.05597575437,
  206051.72036208294, 214004.1349712183, 198149.68830273568,
  213105.61270986524, 216064.29977506239, 200573.88347293844,
  205112.0535136415, 204456.76376554792, 205180.55285840927,
  202969.06534386508, 197885.66878645719, 213020.2059352111,
  221589.93938874462, 204374.1205059239, 204678.91635859985,
  204925.51566635544, 194694.86695246131, 197903.13150816062,
  222911.53660383544]





Getting the energy of the most energetic primary
""""""""""""""""""""""""""""""""""""""""""""""""

.. code-block:: pycon

  In [1]: from icecube import icetray, dataclasses, dataio

  In [2]: f = dataio.I3File("/v/icecube/data/combinedMC600.i3")
  
  In [3]: frame = f.pop_physics()
  
  In [4]: print frame
  [ I3Frame  (P):
    'BadOMSelection' [P] ==> I3Vector<OMKey>
    'CalibratedATWD' [P] ==> I3Map<OMKey, std::vector<I3Waveform, std::allocator<I3Waveform> > >
    'CalibratedFADC' [P] ==> I3Map<OMKey, std::vector<I3Waveform, std::allocator<I3Waveform> > >
    'DOMLaunch' [P] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >
    'DOMLaunch_rc' [P] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >
    'DrivingTime' [P] ==> I3Time
    'DrivingTimeUCR' [P] ==> I3Time
    'GlobalTriggerHierarchy' [P] ==> I3Tree<I3Trigger>
    'I3-mult' [P] ==> I3Tree<I3Trigger>
    'I3EventHeader' [P] ==> I3EventHeader
    'I3MCTree' [P] ==> I3Tree<I3Particle>
    'IIEventHead' [P] ==> I3EventHeader
    'IceTopRawData' [P] ==> I3Map<OMKey, std::vector<I3DOMLaunch, std::allocator<I3DOMLaunch> > >
    'MCHitSeriesMap' [P] ==> I3Map<OMKey, std::vector<I3MCHit, std::allocator<I3MCHit> > >
    'PrimaryTree' [P] ==> I3Tree<I3Particle>
    'TWRLaunchSeriesMap' [P] ==> I3Map<OMKey, std::vector<I3TWRLaunch, std::allocator<I3TWRLaunch> > >
    'icetop_trig' [P] ==> I3Tree<I3Trigger>
    'noisyMCMap' [P] ==> I3Map<OMKey, std::vector<I3MCHit, std::allocator<I3MCHit> > >
    'twrSelected' [P] ==> I3Map<OMKey, std::vector<I3TWRLaunch, std::allocator<I3TWRLaunch> > >
    'twrSelectedCleanedKeys' [P] ==> I3Vector<OMKey>
  ]
  
  
  In [5]: mctree = frame['I3MCTree']
  
  In [6]: prim = mctree.GetMostEnergeticPrimary()
  
  In [7]: prim
  Out[7]: <icecube.dataclasses.I3Particle object at 0x8372f7c>
  
  In [8]: prim.GetEnergy()
  Out[8]: 497.685




Getting an I3RecoPulseSeriesMap from an I3RecoPulseSeriesMapMask
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

.. code-block:: pycon

  In [1]: frame.keys()
  Out[1]: 
  ['CleanTriggerHierarchy_IT',
   'SPEFitSingleFitParams',
   'FilterMask',
   'TWOfflinePulsesFR',
   'SPEFitSingle',
   'SPEFit2CramerRaoParams',
   'MaskedOfflinePulses',
   'I3EventHeader',
   'SPEFit2MuE',
   'OfflinePulsesHLC',
   'I3DST11',
   'I3SuperDST',
   'TWOfflinePulsesHLC',
   'SRTOfflinePulses',
   'TWOfflinePulsesDC',
   'RTTWOfflinePulsesFR',
   'I3TriggerHierarchy',
   'LineFit',
   'OfflinePulses',
   'TWSRTOfflinePulses',
   'SPEFit2',
   'LineFitParams',
   'SPEFit2FitParams']
  
  In [2]: pulse_map = dataclasses.I3RecoPulseSeriesMap.from_frame(frame,'TWSRTOfflinePulses') 
  
  In [3]: pulse_map.keys()
  Out[3]: 
  [OMKey(4,8,0),
   OMKey(4,10,0),
   OMKey(10,10,0),
   OMKey(11,14,0),
   OMKey(11,19,0),
   OMKey(11,20,0),
   OMKey(11,21,0),
   OMKey(11,22,0),
   OMKey(11,23,0),
   OMKey(18,11,0)]

