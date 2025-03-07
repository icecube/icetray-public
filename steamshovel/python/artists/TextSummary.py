# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import re
import math
from icecube.shovelart import *
from icecube import dataclasses
try: from icecube import simclasses
except: pass # optional loading of simclasses
try: from icecube import recclasses
except: pass # optional loading of recclasses
try: from icecube import toprec
except: pass # optional loading of toprec
try: from icecube import ddddr
except: pass
from icecube.dataclasses import I3Constants, I3Particle, \
    get_most_energetic_muon, get_most_energetic_cascade, get_most_energetic_primary
from icecube.icetray import I3Frame, I3Units
from .util import format_engineering


class TextSummary( PyArtist ):

    numRequiredKeys = 1

    def isValidKey( self, frame, key_idx, key ):
        'Accept every frame object'
        return True

    def __init__( self ):
        PyArtist.__init__(self)
        font = PyQFont()
        self.defineSettings(( "short", True,
                              "font", font,
                              "fontsize", font.pointSize ))

    def create( self, frame, output ):
        short = self.setting( "short" )
        font = self.setting( "font" )
        font.pointSize = self.setting( "fontsize" )

        key = self.keys()[0]
        tname = frame.type_name(key)
        obj = None
        try:
            obj = frame[key]
        except:
            pass # unregistered type

        # delegate string formatting based on object type
        if obj is None:
            tx = (f"{key}: could not be loaded\n \n"
                  f"{tname} seems to be an unregistered type.\n"
                  "You may be running steamshovel from the wrong meta-project,\n"
                  "or this is a custom type whose Python module has not been loaded.")

        elif isinstance(obj, dataclasses.I3Time):
            tx = handle_I3Time( key, obj, short )

        elif isinstance(obj, dataclasses.I3MCTree):
            tx = handle_I3MCTree( obj )

        elif isinstance(obj, dataclasses.I3Particle):
            tx = handle_I3Particle( key, obj, short )

        elif isinstance(obj, dataclasses.I3EventHeader):
            tx = handle_I3EventHeader( obj, short, frame.Stop )

        elif tname.startswith("I3PODHolder") or \
             tname in ("I3Bool", "I3Int", "I3Double", "I3String"):
            tx = handle_POD( key, obj, short )

        elif isinstance(obj, dataclasses.I3FilterResultMap):
            tx = handle_Filter( key, obj, short )

        elif isinstance(obj, dataclasses.I3VectorOMKey):
            tx = handle_VectorOMKey( key, obj, short )

        elif tname.startswith("I3Map<OMKey"):
            tx = handle_general_MapOMKey( key, tname, obj, short )

        elif tname.startswith("I3MuonEnergyParams"):
            tx = handle_I3MuonEnergyParams(key, obj, short)
        elif tname.startswith("I3MuonEnergyCascadeParams"):
            tx = handle_I3MuonEnergyCascadeParams(key, obj)
        else:
            # call generic str representation and hope for the best
            tx = str(obj)
            # give out a warning if str representation is missing
            if re.match("<?(.+) 0x[0-9a-f]+>", tx):
                tx = (f"{key}: no text representation\n \n"
                      "You could add a handler for type {tname}\n"
                      "to TextSummary, or add an ostream operator in C++.")

        to = output.addTextOverlay( tx, font )


def handle_I3Time( key, obj, short ):
    i3time = obj
    if short:
      ns10_str = str(i3time.utc_daq_time)
      t_text = f"Y{str(i3time.utc_year)}+{ns10_str[:-1]}.{ns10_str[-1:]} ns"
    else:
      t_text = str(i3time)
    return t_text


def handle_I3MCTree(mctree):
    # Interaction
    beam_particle = get_most_energetic_primary2(mctree)
    if beam_particle:
        if beam_particle.is_neutrino:
            ptypes = [p.type for p in mctree.primaries]
            if len(ptypes) == 2:
                interaction = f"{ptypes[0]} + {ptypes[1]}"
            elif len(ptypes) == 1:
                interaction = f"{ptypes[0]}"
            else:
                interaction = f"{len(ptypes)} primaries"
            daughters = mctree.get_daughters(beam_particle)
            if daughters:
                interaction += " -> "
                dtypes = [str(d.type) for d in daughters]
            interaction += " + ".join(dtypes)
        elif beam_particle.type == I3Particle.Gamma:
            interaction = "Gamma event"
        else:
            interaction = "Cosmic ray event"
        out = f'Interaction:\n  {interaction:s}\n'
    else:
        out = 'No beam particle\n'

    # Primaries
    for p in mctree.primaries:
        out += ('Primary\n'+\
                f'  Type  : {p.type!s}\n'+\
                f'  Energy: {p.energy:.2e}GeV\n')
    # Muons
    p = get_most_energetic_muon(mctree)
    if p:
        out += ('Muon\n'+\
                f'  Type  : {p.type!s}\n'+\
                f'  Energy: {p.energy:.2e}GeV\n')
    # Cascades
    p = get_most_energetic_cascade(mctree)
    if p:
        out += ('Cascade\n'+\
                f'  Type  : {p.type!s}\n'+\
                f'  Energy: {p.energy:.2e}GeV\n')
    return out


def handle_I3Particle( key, particle, short ):
    if short:
        def valid(x):
            return not math.isnan(x)
        part_text = (f"{key}:\n"
                     f"Type/Status: {particle.type_string}.{particle.fit_status_string}\n")

        pdir = particle.dir
        if valid(pdir.zenith) or valid(pdir.azimuth):
            part_text += (f"Zenith/Azimuth: ({pdir.zenith / I3Units.degree:.1f}, "
                          f"{pdir.azimuth / I3Units.degree:.1f}) deg\n")

        ppos = particle.pos
        if valid(ppos.x) or valid(ppos.y) or valid(ppos.z):
            part_text += f"Vertex: ({ppos.x:.1f}, {ppos.y:.1f}, {ppos.z:.1f}) m\n"

        if valid(particle.time):
            part_text += f"Time: {particle.time / I3Units.ns:.3f} ns\n"

        if valid(particle.energy):
            part_text += f"Energy: {format_engineering(particle.energy * 1000000000.0, 'eV')}\n"

        if valid(particle.speed):
            part_text += f"Speed: {particle.speed:.6f} ({particle.speed / I3Constants.c:.3f}c)"
    else:
        part_text = str(particle)
    return part_text


def handle_I3EventHeader(eh, short, fstop):
    if short:
        eh_text = f"Event {eh.run_id}/{eh.event_id}"
        if fstop == I3Frame.Physics:
            eh_text += f"-{eh.sub_event_id}"
        eh_text += f"\nTime {str(eh.start_time)[:19]} UTC"
        eh_text += f"\nDuration {eh.end_time - eh.start_time:.1f} ns"
    else:
        eh_text = str(eh)
    return eh_text


def handle_POD(key, pod, short):
    # use improved format_engineering here
    s = str( pod.value )
    if not short:
        s = f"{key}: {s}"
    return s


def handle_Filter(key, fm, short):
    fm_text=""
    if short:
        fm_text = f"{key} : \n"
        for filterkey in fm.items():
            if filterkey[1].condition_passed and filterkey[1].prescale_passed:
                fm_text += f"{filterkey[0]}\n"
    else:
        fm_text = f"{key} :: (condition/prescale)\n"
        for filterkey in fm.items():
            fm_text += f"{filterkey[0]} : "
            fm_text += f"{str(filterkey[1].condition_passed)}/"
            fm_text += f"{str(filterkey[1].prescale_passed)}\n"
    return fm_text


def handle_VectorOMKey(key, omvec, short):
    nom = len(omvec)
    if short:
        return f"{key}: {nom} entries"
    else:
        ls = []
        for omkey in omvec:
            ls.append(f" {omkey.string:02},{omkey.om:02},{omkey.pmt:02} ")
        ls.sort()

        nchunk = 10
        ls2 = []
        for i in range(int(len(ls) / nchunk) + int(len(ls) % nchunk != 0)):
            ls2.append(",".join(ls[i * nchunk: (i + 1) * nchunk]))
        joined_ls2 = "\n".join(ls2)
        return f"{key}:\n{joined_ls2}"


def handle_general_MapOMKey(key, tname, ommap, short):
    nom = len(ommap)
    if short:
        return f"{key}: {nom} entries"
    else:
        return (f"{key} is a map of DOMs with {nom} entries.\n \n"
                f"No specialized handler is implemented for {tname},\n"
                "you may add one to TextSummary.")

def handle_I3MuonEnergyCascadeParams(key, params):
        return f"{key}:\n{str(params)}"

def handle_I3MuonEnergyParams(key, params, short):
    if short:
        s = f"{key}:\n"
        s += f"peak_energy: {params.peak_energy:.2f} GeV\n"
        s += f"mean: {params.mean:.2f} GeV\n"
        s += f"median: {params.median:.2f} GeV\n"
        s += f"peak/median: {params.peak_energy / params.median:.2f}\n"
        return s
    else:
        return f"{key}:\n{str(params)}"


def get_most_energetic_primary2(mctree):
    # temporary fix for get_most_energetic_primary, see bug 958
    rval = get_most_energetic_primary(mctree)
    if not rval and mctree.primaries:
        highest_energy = 0.0
        for x in mctree.primaries:
            if x.energy > highest_energy:
                highest_energy = x.energy
                rval = x
    return rval
