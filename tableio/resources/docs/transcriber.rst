.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$


The table transcriber
======================

Suppose that you, Grad Student A, would like Grad Student B to have a look at
your data. You are a python/HDF5 user, while your colleague swears allegiance
to ROOT. You've already dumped your data to HDF5, performing a few nontrivial
calculations along the way. In the past, you would have needed to reprocess
all of your data and dump it to ROOT files, eliminating most of the benefit
you enjoyed by working in HDF5 in the first place.

For this case, tableio provides I3TableTranscriber, which communicates with a
table writer service using the same interface as I3TableWriter. Instead
of I3Frames from IceTray, however, the transcriber uses another table service
that supports reading (of which I3HDFTableService is currently the only one)
for input.

This allows you to transcribe HDF tables to any other format that has an
I3TableWriterService::

    from icecube import icetray,tableio
    from icecube.hdfwriter import I3HDFTableService
    from icecube.rootwriter import I3ROOTTableService
    
    from icecube.tableio import I3TableTranscriber

    inservice = I3HDFTableService('foo.hd5','r')
    outservice = I3ROOTTableService('foo.root')

    scribe = I3TableTranscriber(inservice,outservice)

    scribe.Execute()