from icecube.shovelart import *

from icecube import dataclasses, phys_services
from icecube.simclasses import I3PhotonSeriesMap

import math

class PhotonPaths( PyArtist ):

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings( 
            { 
              "min_residual":20, 
              "max_residual":500, 
              "colormap":I3TimeColorMap(),
              "antares_ref_index":False,
              "linewidth":1,
            } 
        )

    requiredTypes = [ I3PhotonSeriesMap, dataclasses.I3MCTree ]

    def _normalized_residual( self, ref_particle, pos, time ):
        if self.setting("antares_ref_index"):
            n_group = 1.3499+0.0298 # 1.3797
            n_phase = 1.3499        # 1.3499
        else:
            n_group = dataclasses.I3Constants.n_ice_group # 1.35634
            n_phase = dataclasses.I3Constants.n_ice_phase # 1.3195

        timeResidual_min = float(self.setting("min_residual"))
        timeResidual_max = float(self.setting("max_residual"))
        if timeResidual_min==timeResidual_max:
            return 0.
        if timeResidual_min>timeResidual_max: 
            timeResidual_min, timeResidual_max = timeResidual_max, timeResidual_min

        timeResidual = \
        phys_services.I3Calculator.time_residual(
            ref_particle,
            pos,
            time = time,
            n_group = n_group,
            n_phase = n_phase,
            )

        timeResidual_normalized = (timeResidual-timeResidual_min)/(timeResidual_max-timeResidual_min)
        timeResidual_normalized = min(1., max(0., timeResidual_normalized))

        return timeResidual_normalized


    def add_one_photon( self, photon, ref_particle, group ):
        colormap = self.setting("colormap")

        positionList = photon.GetPositionList()
        numEntries = len(positionList)
        speed = photon.GetGroupVelocity()

        to_vtx = positionList[-1]
        to_time = photon.GetTime()
        if to_vtx is None: return # error

        verticesReverse = []
        timesReverse = []
        residualsReverse = []

        verticesReverse.append(to_vtx)
        timesReverse.append(to_time)
        residualsReverse.append( self._normalized_residual(ref_particle, to_vtx, to_time) )

        reachedGap=False
        for i in range(numEntries-2,-1,-1):
            from_vtx = positionList[i]
            if from_vtx is None: 
                reachedGap=True
                continue

            if reachedGap and i>0: continue

            if i==0:
                from_time = photon.GetStartTime()
            else:
                distance = math.sqrt((to_vtx.x-from_vtx.x)**2 + (to_vtx.y-from_vtx.y)**2 + (to_vtx.z-from_vtx.z)**2)
                duration = distance/speed
                from_time = to_time-duration

            # skip if not drawable within time granularity
            if int(from_time) == int(to_time):
                continue

            verticesReverse.append(from_vtx)
            timesReverse.append(from_time)
            residualsReverse.append( self._normalized_residual(ref_particle, from_vtx, from_time) )

            to_vtx = from_vtx
            to_time = from_time

        path = group.addParticlePath()
        for i in range( len(verticesReverse)-1, -1, -1 ):
            path.addPoint(
                timesReverse[i],
                verticesReverse[i],
                colormap.value(residualsReverse[i])
            )
        path.setLineWidth( self.setting("linewidth") )


    def create( self, frame, group ):
        mcTree = frame[list(self.keys())[1]]
        referenceTrack = dataclasses.get_most_energetic_muon(mcTree)
        if referenceTrack is None:
            referenceTrack = dataclasses.get_most_energetic_cascade(mcTree)
        elif mcTree.depth(referenceTrack) > 1:
            referenceTrack = dataclasses.get_most_energetic_cascade(mcTree)
        else:
            # change the shape for tracks so that
            # phys_services.I3Calculator.time_residual
            # does not become confused
            referenceTrack.shape=dataclasses.I3Particle.InfiniteTrack

        photonSeriesMap = frame[list(self.keys())[0]]

        for key, photonSeries in photonSeriesMap:
            for photon in photonSeries:
                self.add_one_photon( photon, referenceTrack, group )

