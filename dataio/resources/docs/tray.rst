IceTray I/O
===========

The dataio modules I3Writer, I3MultiWriter and I3Reader support on-the-fly
compression and exclusion of various frame items via regular expressions.

Reading in .i3 files
--------------------

Make sure you order the files sent to the I3Reader so that the
proper GCD frames are supplied prior to P frames.

Usage
^^^^^

Usage is straightforward.  Available parameters are listed below (from
icetray-inspect dataio)::

   I3Reader
    Filename (string)
      Description :  Filename to read.  Use either this or Filenamelist, not both.
      Default     :  ""

    FilenameList (vector<string>)
      Description :  List of files to read, *IN SORTED ORDER*
      Default     :  []

    SkipKeys (vector<string>)
      Description :  Don't load frame objects with these keys
      Default     :  []


An example python script::

    #!/usr/bin/env python
    import os
    import sys

    from I3Tray import *
    from icecube import dataclasses, dataio

    tray = I3Tray()
    tray.Add("I3Reader", filename="pass1.i3")
    tray.Add("Dump","dump")
    tray.Execute()

Compression
^^^^^^^^^^^
The writers will automatically compress the data using one of the following
compression algorithms: gzip, bzip2, or zstd. The actual to-be-used algorithm
can be specified by giving the file a filename that ends in .gz, .bz2, or .zst,
respectively. This::

    tray.Add("I3Writer", filename="mystuff.i3.gz")

will get you run-of-the-mill gzip compression. With CompressionLevel you can specify:

    ======================  ================
    GZip Compression Level  Meaning
    ======================  ================
    0                       no compression
    1                       fastest
    6                       default
    9                       best compression
    ======================  ================

so this::

    tray.Add("I3Writer", filename="mystuff.i3.gz")

should get you the same result as just writing to disk and then gzipping, and this::

    tray.Add("I3Writer", filename="mystuff.i3.gz", compressionlevel=9)

will compress better at the cost of speed.

In addition, I3Reader will transparently read gzip, bzip2, zstd, or xz-compressed .i3
files, or .i3 files inside of compressed tar archives like the PFFilt files
packaged and transferred over the satellite link by JADE. Any format that
libarchive_ supports can be read.

.. _libarchive: http://www.libarchive.org

General information about the Zstandard compression library can be found in the
zstd_ online documentation, and a more technical documentation in the `zstd manual`_.
The documentation starts with "zstd, short for Zstandard, is a fast lossless
compression algorithm, targeting real-time compression scenarios at zlib-level
and better compression ratios.".

.. _zstd: https://facebook.github.io/zstd
.. _`zstd manual`: https://facebook.github.io/zstd/zstd_manual.html

Other compression strategies can similarly be selected by the filename suffix:

    ====================  ====================
    Compression Strategy  I3Writer File Suffix
    ====================  ====================
    gzip                  .gz
    bzip2                 .bz2
    zstd                  .zst
    ====================  ====================

The meanings of bzip2 compression levels are the same as GZip, i.e. 1-9, with
1 the fastest and 9 the best compression.  ZSTD compression levels are slightly
different:

    ======================  ================
    ZSTD Compression Level  Meaning
    ======================  ================
    0                       no compression
    1                       fastest
    4                       default
    22                      best compression
    ======================  ================

Reading from and writing to remote locations (staging)
------------------------------------------------------

Sometimes the files you want to read are not available on your local
filesystem. For example, if you're running on a random node on the Open Science
Grid, you will not have direct access to the /data/sim and /data/exp
filesystems in Madison. For such situations, dataio has built-in support for
"staging" files in and out of local storage. This support is activated by
replacing the `"I3Reader"` module with the `dataio.I3Reader` tray segment. For
example, to read files via GridFTP, use the following snippet::

	from icecube import icetray, dataio
	tray.Add(dataio.I3Reader, filenamelist=['gsiftp://gridftp.icecube.wisc.edu/data/sim/IceCube/2010/filtered/level3-cscd/CORSIKA-in-ice/9493/92000-92999/Level3_IC79_corsika.009493.092110.i3.bz2 '])

The segment dataio.I3Reader is equivalent to::

	tray.context['I3FileStager'] = dataio.get_stagers()
	tray.AddModule('I3Reader', **kwargs)

Behind the scenes the stager will recognize supported URL schemes (currently
file:// http://, ftp://, gsiftp://, and scp://), download the file to a local
directory, and provide the reader with a local filename to read instead. As
soon as the file is no longer needed, it will be automatically deleted. The
inverse operation works for writing as well. If the staging mechanism has been
set up (using either of the snippets above), then I3Writer will also recognize
URL schemes, write the output to a temporary file, and upload it to the
destination when the file is closed. For example, the following snippet will
write an .i3 file to my /data/user directory in Madison from anywhere in the
world::

	tray.Add('I3Writer', filename='gsiftp://gridftp-users.icecube.wisc.edu/data/user/jvansanten/foo.i3.bz2')

Plain POSIX path names, e.g. "/data/user/jvansanten/foo.i3.bz," will not be
staged, but instead read directly. The :py:func:`hdfwriter.I3HDFWriter` and
:py:func:`rootwriter.I3ROOTWriter` segments use the same staging mechanism.
Currently, the following stager classes are implemented:

.. py:module:: icecube.dataio.I3FileStagerFile

.. autoclass:: I3FileStagerFile
   :noindex:

.. autoclass:: GridFTPStager
   :noindex:

.. autoclass:: SCPStager
   :noindex:

Additional Scenarios
--------------------

SkipKeys
^^^^^^^^

You can specify that the reader not read (or the writer not write)
certain keys (that is, the names they're stored under) with SkipKeys,
which now takes, instead of a space-separated list of strings, a
vector of perl-style regular expressions.

so given a frame that looks like this::

 Frame: 5/8
 Key:   1/59               Type                                      Size (bytes)
 DrivingTime               I3Time                                    38
 F2kEventHeader            I3EventHeader                             83
 F2kHitSel_DummyTrig5      I3Vector<int>                             291
 F2kHitSel_DummyTrig6      I3Vector<int>                             291
 F2kHitSel_DummyTrig7      I3Vector<int>                             291
 F2kHitSel_DummyTrig8      I3Vector<int>                             291
 F2kHitSel_FinalHitSel     I3Vector<int>                             171
 F2kHitSel_HitSel0         I3Vector<int>                             283
 F2kHitSel_HitSel1         I3Vector<int>                             199
 F2kHitSel_HitSel2         I3Vector<int>                             171
 F2kMCPrimaryTrack00       I3Particle                                152
 F2kMCTracks               I3Vector<I3Particle>                      9098
 F2kMuonDAQ                I3Map<OMKey, I3AMANDAAnalogReadout>       4242
 F2kMuonDAQ_uncalib        I3Map<OMKey, I3AMANDAAnalogReadout>       4242
 F2kSoftwareTriggerFlags   I3Vector<std::string>                     78
 F2kTrack00                I3Particle                                152
 F2kTrack00HitSel          I3Vector<int>                             411
 F2kTrack00Params          I3Map<std::string, double>                180
 F2kTrack01                I3Particle                                152
 F2kTrack01HitSel          I3Vector<int>                             411
 F2kTrack01Params          I3Map<std::string, double>                180
 F2kTrack02                I3Particle                                152
 F2kTrack02HitSel          I3Vector<int>                             411
 F2kTrack02Params          I3Map<std::string, double>                180
 F2kTrack03                I3Particle                                152
 F2kTrack03HitSel          I3Vector<int>                             411
 F2kTrack03Params          I3Map<std::string, double>                180
 F2kTrack04                I3Particle                                152
 F2kTrack04HitSel          I3Vector<int>                             411
 F2kTrack04Params          I3Map<std::string, double>                180
 F2kTrack05                I3Particle                                152
 F2kTrack05HitSel          I3Vector<int>                             411
 F2kTrack05Params          I3Map<std::string, double>                180
 F2kTrack06                I3Particle                                152
 F2kTrack06HitSel          I3Vector<int>                             411
 F2kTrack06Params          I3Map<std::string, double>                180
 F2kTrack07                I3Particle                                152
 F2kTrack07HitSel          I3Vector<int>                             411
 F2kTrack07Params          I3Map<std::string, double>                180
 F2kTrack08                I3Particle                                152
 F2kTrack08HitSel          I3Vector<int>                             411
 F2kTrack08Params          I3Map<std::string, double>                180
 F2kTrack09                I3Particle                                152
 F2kTrack09HitSel          I3Vector<int>                             411
 F2kTrack09Params          I3Map<std::string, double>                180
 F2kTrack10                I3Particle                                152
 F2kTrack10HitSel          I3Vector<int>                             411
 F2kTrack10Params          I3Map<std::string, double>                180
 F2kTrack11                I3Particle                                152
 F2kTrack11HitSel          I3Vector<int>                             411
 F2kTrack11Params          I3Map<std::string, double>                43
 F2kTrack12                I3Particle                                152
 F2kTrack12HitSel          I3Vector<int>                             411
 F2kTrack12Params          I3Map<std::string, double>                180
 F2kTrack13                I3Particle                                152
 F2kTrack13HitSel          I3Vector<int>                             411
 F2kTrack13Params          I3Map<std::string, double>                180
 F2kTriggers               I3Tree<I3Trigger>                         122

This::

 tray.Add("I3Writer",
          filename="mystuff.i3.gz",
          skipkeys=["F2kHitSel_DummyTrig.*"])

Will skip all the f2k dummy triggers.

This::

    skipkeys = ["F2kTrack.*HitSel", ".*Bryant"]

Will skip all the f2ktrack hit selection thingys, and anything that
ends with "Bryant".  This::

    skipkeys = ["F2kTrack.*HitSel", ".*Bryant"]

But note the dot-star in there, these are perl-style regular
expressions, not the filesystem-globbing stuff that you use in your
shell when doing things like 'ls *.f2k'.  To match anything once,
(like ? in the shell) use a dot.  To match anything any number of
times, use dot-star, like F2k.*

The syntax is a little different, and they can be both absurdly
powerful and, well, simply absurd, if you geek out on them::

    skipkeys = ["F2kTrack\d*(([02468]Params)|([13579]HitSel))"]

This, for instance, removes the Params from even numbered tracks and
HitSels from odd-numbered tracks.  This is the reason for vectors of
regular expressions.  If you just want to type out every single track
name, you certainly can::

     skipkeys =  ["DrivingTime",
                  "F2kEventHeader",
                  "F2kHitSel_DummyTrig5",
                  "F2kHitSel_DummyTrig6",
                  "F2kHitSel_DummyTrig7",
                  "F2kHitSel_DummyTrig8",
                  "F2kHitSel_FinalHitSel",
                  "F2kHitSel_HitSel0",
                  "F2kHitSel_HitSel1",
                  "F2kHitSel_HitSel2",
                  "F2kMCPrimaryTrack00",
                  "F2kMCTracks",
                  "F2kMuonDAQ",
                  "F2k_all_the_others_etc"
                  "F2kMuonDAQ_uncalib",
                  "F2kSoftwareTriggerFlags",
                  "F2kTrack00",
                  "F2kTrack00HitSel",
                  "F2kTrack11Params",
                  "F2kTrack12",
                  "F2kTrack12HitSel",
                  "F2kTrack12Params",
                  "F2kTrack13",
                  "F2kTrack13HitSel",
                  "F2kTrack13Params",
                  "F2kTriggers"]

will work too.

Dropping Orphan Streams
-----------------------

If a filter module operates only on 'P' frames, but an input file contains
both 'Q' and 'P' frames, the output at the end can look like::

    QQQQQPQQQQQQQQQPQQQQQQQPQQQQQQPQQQ

There are a lot of left over 'Q' frames that we should drop to save space.
The easy option to take care of that is `DropOrphanStreams`::


    tray.Add("I3Writer",
             Filename="outfile.i3",
             DropOrphanStreams=[icetray.I3Frame.DAQ])

Writing Multiple Files
----------------------

The module I3MultiWriter will split the output into multiple data
files.  The **filename** argument is actually a printf() type
string, not a plain filename.  This string must contain a %u
formatting character, which will be replaced with the index of the
file in the series written.  For instance::

   tray.Add("I3MultiWriter",
            Filename="foo/myfile-%u.i3.gz",
            SizeLimit=10**6)  # Files of 1MB size: double-star is the exponent operator

will cause the I3MultiWriter to write files foo/myfile-0.i3.gz,
foo/myfile-1.i3.gz, foo/myfile-2.i3.gz, etc.

Probably you will want to specify something like ::

   foo/myfile-%04u.i3.gz

where 04 in ``%04u`` means that the index number of the file will be
left-padded with zeros to a width of 4::

   foo/myfile-0000.i3.gz
   foo/myfile-0001.i3.gz
   foo/myfile-0002.i3.gz

etc.  This is so that the files stay in generated order when listed
with *ls* or passed to the I3Reader via glob().

The other necessary parameter is **SizeLimit** which specifies, in
bytes, a soft limit on the size of each file.  This is not a hard
limit: a file will be closed and the next one opened after a frame
write causes the current file size to exceed this limit.  The files
written will typically exceed this size by the size of one half of one
frame.  One consequence of this behavior is that you can write
one-frame-per-file by specifying a SizeLimit of one byte.

Splitting off the Geometry, Calibration, and DetectorStatus
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is useful in sim production.  You use two writers, an I3Writer
for geometry, calibration and detector status, and an I3MultiWriter
for the physics::

  tray.Add("I3Writer","gcdwriter",
           filename="split.gcd.i3",
           streams=["Geometry", "Calibration", "DetectorStatus"])

  tray.Add("I3MultiWriter","physwriter",
           filename="split.physics.%04u.i3",
           streams=["Physics"],
           sizelimit=10**5)

The 'streams' parameter specifies to each writer which streams they
should react to.  The I3TrayInfo frames get written to all files.  The
names of the streams are case-sensitive.


Reading multiple files with glob
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To read multiple files use the parameter 'FilenameList'.  To generate
the list of files from a directory, you might find the python
<code>glob()</code> function convenient::

 from glob import glob

 file_list = glob("/my/data/\*.i3.gz")
 tray.Add("I3Reader", FilenameList=file_list)

as usual with vector<string> parameters, you can pass an array
literal::

 tray.Add("I3Reader", FilenameList=["file1.i3.gz", "file2.i3.gz", file3.i3.gz"])


The files will be read in order.  When then end of one file is
reached, the next will be opened.

You may mix compressed (.i3.gz) and noncompressed (.i3) files in any order.

If you specify both a 'Filename' and a 'FilenameList' the reader
service will log_fatal() complaining that the configuration is
ambiguous and tell you to use one or the other.

Reading Geometry/Calibration/Status from a separate file
++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Simulation runs have the Geometry, Calibration, and Detector Status
frames in a separate file from the physics.  You want to read this GCD
file first, and then the rest of them in order.

python's glob() function can generate the list of physics files for
you.  Assuming the GCD is in GCD_0340.i3.gz and the associated physics
frames are in files physics_0340.00001.i3.gz through, say,
physics_0340.00999.i3.gz::

  from glob import glob

  physics = glob("physics_0340.*.i3.gz")    # glob() the list of files from the disk

  physics.sort()                            # sort() them (they probably wont glob in alphabetical order)

  tray.Add("I3Reader", FilenameList=["GCD_0340.i3.gz"]+physics)


Examples
--------

There are some example python scripts using dataio in the
`resources/examples` directory.
