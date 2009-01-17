:mod:`dataio` -- python bindings to Data I/O utilities
======================================================

.. module:: dataio
   :synopsis: python bindings to Data I/O utilities

.. class:: icecube.dataio.I3File

   Simple sequential reader/writer of ``.i3`` files.

   .. data:: Writing
   .. data:: Reading

      Used to indicate the mode with which to open a file. Possible
      values are ``Reading`` and ``Writing``.  Obviously you can only
      push frames onto a ``Writing`` file and pop frames from a
      ``Reading`` file.

   .. method:: I3File()

      Create an I3File object w/o an actual associated file::

        # not very useful
        f = dataio.I3File()      	     

   .. method:: open_file(path, mode = dataio.I3File.Reading)

      Open file at *path*.  Default mode is for reading::

        # open for reading
        i3file = dataio.I3File()
        i3file.open_file("mydata.i3")

        # open for writing
        i3file2 = dataio.I3File()
        i3file2.open_file("mydata.i3", dataio.I3File.Writing)

      *path* may end in ``.i3`` or ``.i3.gz`` for uncompressed or
       gzip-compresed files (applies for both **Reading** and **Writing**)

   .. method:: I3File(path, mode = dataio.I3File.Reading)

      Create an I3File, then call::

        self.open_file(path, mode)

   .. method:: pop_frame()

      Return the next frame of any type from the file::

        frame = i3file.pop_frame()
        print frame	

   .. method:: pop_frame(stream)

      Return the next frame of with stream type *stream* from the file::

        frame = i3file.pop_frame(icetray.I3Frame.Geometry)
        print frame

   .. method:: pop_physics()

      Shorthand for::
      
         pop_frame(icetray.I3Frame.Physics)

   .. method:: push(frame)

      Push frame onto file (file must be open for writing)::

        frame = icetray.I3Frame(icetray.I3Frame.Physics)
	i3file = dataio.I3File("generated.i3.gz", dataio.I3File.Writing)
	i3file.push(frame)

   .. method:: close()

      Close the file.
      
   .. method:: more()

      Returns true if there are more frames available.  This prints
      all the event ids in a file::

        i3f = dataio.I3File("mydata.i3")
        while i3f.more():
          phys = i3f.pop_frame()
          print phys['I3EventHeader'].EventID
      	      
      You can also use the iterator interface rather than writing an
      explicit loop.

   .. method:: __iter__()

      Return an iterator to the I3File.  The I3File class supports
      standard python iteration.  This means you can use the I3File
      in looping contexts::

         i3f = dataio.I3File("mydata.i3")
         for frame in i3f:
             print frame

      or minus the intermediate variable ``i3f``::

         for frame in dataio.I3File('mydata.i3'):
             print frame  	       

      and list comprehensions.  For instance this gets the EventID of
      all physics frames in the file ``mydata.i3``::

         eventids = [frame['I3EventHeader'].EventId 
	             for frame in dataio.I3File('mydata.i3')
	             if frame.GetStop() == icetray.I3Frame.Physics]
 
      .. warning::

          I3File iterators clobber each other.  The I3File has only
          (internal) current position: you can't, for instance, loop
          twice over the same file (for this, just reopen the file) or
          use two iterators into the same file concurrently.



      
   

