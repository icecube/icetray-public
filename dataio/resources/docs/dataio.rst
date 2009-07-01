IceTray I/O
==============

The dataio modules I3Writer, I3MultiWriter and I3ReaderService support on-the-fly compression and exclusion of various frame items via regular expressions.  The .i3 file browser 'dataio-shovel' is a handy utility.

Reading in .i3 files from the data warehouse
---------------------------------------------

If you are reading an .i3 file(s) that contain all relevant information 
for your events, using the I3Muxer/I3ReaderService is not required.  If 
all Geometry, Calibration, Detector status information is included in 
the .i3 file or files supplied with the Physics event data, then using 
the Muxer is not required.  You can easily use
the I3Reader module to read your .i3 files.  This is 
generally true of simulation and level0, level1,... files
that are available in the data warehouse.

**Note:** Just make sure you order the files sent to the I3Reader so that the proper GCD frames are supplied prior to P frames.

When do I need to use the Muxer (**TODO link to Muxer) and 
appropriate source services?

* Using the database as a source of Geometry, Calibration, DetectorStatus
 * This includes decoding RAW data that does not contains GCD frame.
* Using a database service to replace the G, C and/or D frames in a existing .i3 file.
* Using the EmptyStream services to fill dummy G, C, D, and/or P frames.

Basically, anytime you are getting information from other sources that a single stream of i3 files....

Usage
^^^^^^

Usage is straightforward.   Available parameters are listed below (from icetray-inspect dataio)::
 
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
 from I3Tray import *
 
 from os.path import expandvars
 
 import os
 import sys
 
 load("libdataclasses")
 load("libdataio")
 
 tray = I3Tray()
 tray.AddModule("I3Reader","reader")(
    ("Filename", "pass1.i3")
    )
 tray.AddModule("Dump","dump")
 tray.AddModule("TrashCan", "the can");
 
 tray.Execute()
 tray.Finish()


Compression
^^^^^^^^^^^^

The writers will automatically compress if you specify a filename that ends in .gz. This::

  tray.AddModule("I3Writer", "write")(                                              
      ("filename", "mystuff.i3.gz")                                                 
      )                                                                             

that will get you run-of-the-mill gzip compression. With CompressionLevel you can specify::

 Compression Level  -  Meaning

            0       -  no compression. Or just specify a filename that doesn't end in .gz

            1       -  fastest
            6       -  default (best combination of compression and speed
            9       -  best compression

 
so this:: 

 tray.AddModule("I3Writer", "write")(                                               
     ("filename", "mystuff.i3.gz")                                                  
     )                                                                             

should get you the same result as just writing to disk and then gzipping, and this::

 tray.AddModule("I3Writer", "write")(                                               
     ("filename", "mystuff.i3.gz"),
     ("compressionlevel", 9)
     )                                                                             

will compress better at the cost of speed. The I3Reader will recognize if the file ends in .gz and turn on decompression if necessary::

 tray.AddModule("I3ReaderServiceFactory", "reader")(                                
     ("filename", "mystuff.i3.gz")                                                  
     )                                                                             

It does not need to know what the compression level of the input file is.

The I3Writer will automatically compress if you specify a filename
that ends in .gz::

  tray.AddModule("I3Writer", "write")(
     ("filename", "mystuff.i3.gz")
     )

that will get you run-of-the-mill gzip compression.  With
'CompressionLevel' you can specify the usual::

 0 == no compression
 1 == fastest
 6 == default
 9 == best compression

so this::

 tray.AddModule("I3Writer", "write")(
    ("filename", "mystuff.i3.gz")
    )

should get you the same result as just writing to disk and then
gzipping.

The I3Reader will just recognize if the file ends in .gz and turn on
decompression if necessary::

 tray.AddService("I3ReaderServiceFactory", "reader")(
    ("filename", "mystuff.i3.gz")
    )

not much to explain there.

SkipKeys
^^^^^^^^^^^

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

 tray.AddModule("I3Writer", "writer")(
    ("filename", "mystuff.i3.gz"),
    ("skipkeys", ["F2kHitSel_DummyTrig.*"])
    )

Will skip all the f2k dummy triggers.

This::

    ("skipkeys", ["F2kTrack.*HitSel", ".*Bryant"])

Will skip all the f2ktrack hit selection thingys, and anything that
ends with "Bryant".  This::

    ("skipkeys", ["F2kTrack.*HitSel", ".*Bryant"])

But note the dot-star in there, these are perl-style regular
expressions, not the filesystem-globbing stuff that you use in your
shell when doing things like 'ls *.f2k'.  To match anything once,
(like ? in the shell) use a dot.  To match anything any number of
times, use dot-star, like F2k.* 

The syntax is a little different, and they can be both absurdly
powerful and, well, simply absurd, if you geek out on them::

    ("skipkeys", ["F2kTrack\d*(([02468]Params)|([13579]HitSel))"])

This, for instance, removes the Params from even numbered tracks and
HitSels from odd-numbered tracks.  This is the reason for vectors of
regular expressions.  If you just want to type out every single track
name, you certainly can::

    ("skipkeys", ["DrivingTime",
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
                  "F2kTriggers"])

will work too.

Splitting off the Geometry, Calibration, and DetectorStatus 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is useful in sim production.  You use two writers, an I3Writer for geometry, calibration and detector status, and an I3MultiWriter for the physics::

  tray.AddModule("I3Writer","gcdwriter")(
    ("filename", "split.gcd.i3"),
    ("streams", ["Geometry", "Calibration", "DetectorStatus"])
    )
 
  tray.AddModule("I3MultiWriter","physwriter")(
    ("filename", "split.physics.%04u.i3"),
    ("streams", ["Physics"]),
    ("sizelimit", 10**5)
    )

The 'streams' parameter specifies to each writer which streams they should react to.  The I3TrayInfo frames get written to all files.  The names of the streams are case-sensitive.


The dataio-shovel 
----------------------

The dataio-shovel is a .i3 file browser utility. It has interactive help and can be handy to identify what is inside a .i3 file. Try it out. Here comes the screenshot:

.. image:: Dataio_shovel.gif

The bottom part of the display shows a tape-like representation of the icecube data stream. The G, C, D, and P represent Geometry, Calibration, DetectorStatus and Physics, respectively. The bar in the middle is the location of the tape read head. On the top half are what is currently under the read head: a 'frame'. Each row is one data item. On the left is the 'key' the object is stored under, in the middle is the type of the object (with typedefs stripped away), on the right is the on-disk size of the object in bytes.

The up-down arrows select frame items and the 'x' key will display that item in XML. The 'c' key will show the configuration of the tray that generated the datafile, including all module parameters, svn information, time of run, hostname, compiler and root versions, and operating system type. Full help is available in the shovel itself.



Reading multiple files
-----------------------

To read multiple files use the parameter 'FilenameList'.   To generate the list of files from a directory, you might find the python <code>glob()</code> function convenient::

 from glob import glob
 
 file_list = glob("/my/data/*.i3.gz")
 tray.AddService("I3ReaderServiceFactory", "readerfactory")(
   ("FilenameList", file_list)
   )

as usual with vector<string> parameters, you can pass an array literal::

 tray.AddService("I3ReaderServiceFactory", "readerfactory")(
   ("FilenameList", ["file1.i3.gz", "file2.i3.gz", file3.i3.gz"])
   )


The files will be read in order.   When then end of one file is reached, the next will be opened.

You may mix compressed (.i3.gz) and noncompressed (.i3) files in any order.

If you specify both a 'Filename' and a 'FilenameList' the reader service will log_fatal() complaining that the configuration is ambiguous and tell you to use one or the other.

Reading Geometry/Calibration/Status from a separate file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Simulation runs have the Geometry, Calibration, and Detector Status frames in a separate file from the physics.  You want to read this GCD file first, and then the rest of them in order.

python's glob() function can generate the list of physics files for you.  Assuming the GCD is in GCD_0340.i3.gz and the associated physics frames are in files physics_0340.00001.i3.gz through, say, physics_0340.00999.i3.gz::


  from glob import glob 

  physics = glob("physics_0340.*.i3.gz")    # glob() the list of files from the disk

  physics.sort()                            # sort() them (they probably wont glob in alphabetical order)  

  tray.AddService("I3ReaderServiceFactory", "readerfactory")(
    ("FilenameList", ["GCD_0340.i3.gz"] + physics)  # careful with your angle-brackets here
  )


Writing Multiple Files
-----------------------

The module I3MultiWriter will split the output into multiple data files.  
The <b>filename</b> argument is actually a printf() type string, not a plain filename.  This string must contain a %u formatting character, which will be replaced with the index of the file in the series written.  For instance::

   tray.AddModule("I3MultiWriter", "writer")(
      ("Filename", "foo/myfile-%u.i3.gz"),
      ("SizeLimit", 10**6)  # Files of 1MB size: double-star is the exponent operator
      )

will cause the I3MultiWriter to write files foo/myfile-0.i3.gz, foo/myfile-1.i3.gz, foo/myfile-2.i3.gz, etc.

Probably you will want to specify something like ::

   foo/myfile-%04u.i3.gz

where 04 in %04u means that the index number of the file will be left-padded with zeros to a width of 4::

   foo/myfile-0000.i3.gz
   foo/myfile-0001.i3.gz
   foo/myfile-0002.i3.gz

etc.  This is so that the files stay in generated order when listed with <i>ls</i> or passed to the I3ReaderService via glob().

The other necessary parameter is <b>SizeLimit</b> which specifies, in bytes, a soft limit on the size of each file.  This is not a hard limit:  a file will be closed and the next one opened after a frame write causes the current file size to exceed this limit.  The files written will typically exceed this size by the size of one half of one frame.  One consequence of this behavior is that you can write one-frame-per-file by specifying a SizeLimit of one byte.

