:mod:`icecube.dataio` -- python bindings to Data I/O utilities
===============================================================

.. module:: icecube.dataio
   :synopsis: python bindings to Data I/O utilities

.. class:: I3File
   :noindex:

   Simple sequential reader/writer of ``.i3`` files.

   .. data:: Writing
      :noindex:
	     
   .. data:: Reading
      :noindex:

      Used to indicate the mode with which to open a file. Possible
      values are ``Reading`` and ``Writing``.  Obviously you can only
      push frames onto a ``Writing`` file and pop frames from a
      ``Reading`` file.

   .. method:: I3File()
      :noindex:

      Create an I3File object w/o an actual associated file::

        # not very useful
        f = I3File()      	     

   .. method:: open_file(path, mode = I3File.Reading)
      :noindex:

      Open file at *path*.  Default mode is for reading::

        # open for reading
        i3file = I3File()
        i3file.open_file("mydata.i3")

        # open for writing
        i3file2 = I3File()
        i3file2.open_file("mydata.i3", I3File.Writing)

      *path* may end in ``.i3`` or ``.i3.gz`` for uncompressed or
       gzip-compresed files (applies for both **Reading** and **Writing**)

   .. method:: I3File(path, mode = I3File.Reading)
      :noindex:

      Create an I3File, then call::

        self.open_file(path, mode)

   .. method:: pop_frame()
      :noindex:

      Return the next frame of any type from the file::

        frame = i3file.pop_frame()
        print frame	

   .. method:: pop_frame(stream)
      :noindex:

      Return the next frame of with stream type *stream* from the file::

        frame = i3file.pop_frame(icetray.I3Frame.Geometry)
        print frame

   .. method:: pop_physics()
      :noindex:

      Shorthand for::
      
         pop_frame(icetray.I3Frame.Physics)

   .. method:: push(frame)
      :noindex:

      Push frame onto file (file must be open for writing)::

        frame = icetray.I3Frame(icetray.I3Frame.Physics)
	i3file = I3File("generated.i3.gz", I3File.Writing)
	i3file.push(frame)

   .. method:: close()
      :noindex:

      Close the file.
      
   .. method:: rewind()
      :noindex:

      Close and reopen the file to the beginning.   
      
   .. method:: more()
      :noindex:

      Returns true if there are more frames available.  This prints
      all the event ids in a file::

        i3f = I3File("mydata.i3")
        while i3f.more():
          phys = i3f.pop_frame()
          print phys['I3EventHeader'].EventID
      	      
      You can also use the iterator interface rather than writing an
      explicit loop.

   .. method:: next()
      :noindex:

      Returns the next frame, if available, else throws StopIteration.
      This is part of the python 'iterator protocol'; this function
      combined with ``__iter__()``, gets you iteration in loops and
      list comprehensions (see __iter__() below):

   .. method:: __iter__()
      :noindex:

      Return an iterator to the I3File (just a freshly-opened copy of
      the I3File object itself, since I3File implements the iterator
      protocol).  The I3File class supports standard python iteration.
      This means you can use the I3File in looping contexts::

         i3f = I3File("mydata.i3")
         for frame in i3f:
             print frame

      or minus the intermediate variable ``i3f``::

         for frame in I3File('mydata.i3'):
             print frame  	       

      and list comprehensions.  For instance this gets the EventID of
      all physics frames in the file ``mydata.i3``::

         eventids = [frame['I3EventHeader'].EventId 
	             for frame in I3File('mydata.i3')
	             if frame.GetStop() == icetray.I3Frame.Physics]
 
	  


      
   

