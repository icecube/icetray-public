#! /usr/bin/env python
from __future__ import print_function
import os
import unittest
from I3Tray import I3Tray
from icecube import icetray, dataio, dataclasses, recclasses, simclasses, gulliver, paraboloid
import time

# This is a test for the shovelio module
from icecube.shovelio import I3FrameSequence

e = os.path.expandvars
seekable_file = e("$I3_TESTDATA/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3")
gz_file = e("$I3_TESTDATA/sim/GCD.i3.gz")


class NotEqualError( Exception ):
    def __init__(self, reason):
        super( NotEqualError, self ).__init__(self, reason)

class AllframesModule( icetray.I3Module ):
    '''A module that verifies the output of a shovelio.I3FrameSequence
       against the standard I3Reader output'''

    def __init__(self, context, filenamelist = [] ):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('filenamelist', 'A list of files', [])
        self.AddOutBox("OutBox")
        self.idx = -1
        self.seq = I3FrameSequence()

    def _do_next( self, frame ):
        '''Make a best effort to verify that frame == the next shovelio frame.
           Raise NotEqualError if not'''
        self.idx += 1
        f1 = self.seq[self.idx]
        print("frame", self.idx, frame.Stop)
        print("   f1", self.idx, f1.Stop)

        # Frames have the same number of objects
        # Frames have the same object keys
        s1 = set(f1.keys())
        s2 = set(frame.keys())
        if s1 != s2:
            print("shovelio keys:", len(s1), "I3Reader keys:", len(s2))
            common = s1.intersection(s2)
            print(len(common), "common keys")
            only_s1 = s1.difference(common)
            only_s2 = s2.difference(common)
            if only_s1:
                print(len(only_s1), 'keys only in shovelio:', " ".join(only_s1))
            if only_s2:
                print(len(only_s2), 'keys only in I3Reader:', " ".join(only_s2))
            raise NotEqualError("keys differ" )
        # Frames have the same stop ids
        if f1.Stop != frame.Stop:
            print( "shovelio frame stop {0} != I3Reader frame stop {1}".format(f1.Stop,frame.Stop) )
            raise NotEqualError("stops differ")
        # For any iterable frame objects, the lengths are the same
        for key in frame.keys():
            try:
                k1 = f1[key]
                k2 = frame[key]
                try:
                    lk1 = len(k1)
                    lk2 = len(k2)
                    if( lk1 != lk2 ):
                        print(lk1, lk2)
                        raise NotEqualError( "shovelio object length differs from I3Reader length for key "+key )
                except TypeError as e:
                    # Can't test keys with no __len__
                    pass
            except KeyError as e:
                # Can't test keys that we can't read
                print(e)
                pass

    def Configure(self):
        filenamelist = self.GetParameter('filenamelist')
        print("Configure with filenamelist:", " ".join(filenamelist))
        for f in filenamelist:
            self.seq.add_file( f )
        while self.seq.is_indexing:
            time.sleep(0.1)

    def Process(self):
        self._do_next(self.PopFrame())

    def Finish(self):
        pass


def setUpModule():
    # icetray.I3Logger.global_logger.set_level_for_unit( 'shovelio', icetray.I3LogLevel.LOG_TRACE )
    pass

def tearDownModule():
    pass

class ShovelioI3ReaderComparisonTest( unittest.TestCase ):

    def _run_on( self, files ):
        try:
            tray = I3Tray()
            tray.AddModule( "I3Reader", "reader", filenamelist = files )
            tray.AddModule(AllframesModule, "All", filenamelist = files )
            tray.Execute()
            
        except NotEqualError as e:
            self.fail(e.message)

    def test_one_file( self ):
        f = os.path.expandvars("$I3_TESTDATA/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3")
        self._run_on([ f ])

    def test_repeated_file( self ):
        f = os.path.expandvars("$I3_TESTDATA/event-viewer/Level3aGCD_IC79_EEData_Run00115990_slim.i3")
        self._run_on( [ f, f ] )

    def test_gcd( self ):
        f1 = os.path.expandvars("$I3_TESTDATA/sim/GCD.i3.gz")
        f2 = os.path.expandvars("$I3_TESTDATA/sim/corsika.F2K010001_IC59_slim.i3.gz")
        self._run_on([ f1, f2 ])

if __name__ == "__main__":
    # In Python 2.7 and later, the unittest framework is smart enough to run
    # setUpModule() and tearDownModule() automatically.  For the sake of older
    # pythons, call them manually here.  They'll be invoked twice under 2.7,
    # which is fine.
    setUpModule()
    unittest.main()
    tearDownModule()
