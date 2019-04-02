:mod:`icecube.dataio` -- python bindings to Data I/O utilities
===============================================================

.. module:: icecube.dataio
   :synopsis: python bindings to Data I/O utilities

.. class:: I3File
   :noindex:

   Simple sequential reader/writer of ``.i3`` files.

   .. data:: frameno
      :noindex:

      The current frame number. Read-only memeber.

   .. data:: mode
      :noindex:

      Used to indicate the mode with which to open a file. Possible
      values are ``Write/Append``, ``Write/Create``, ``Write/Truncate``,
      and ``Read``. Read-only parameter.

   .. data:: path
      :noindex:

      Path to the file. Read-only member.

   .. data:: size
      :noindex:

      Size of the file in frames. Does not include unread frames. Read-only member.

   .. data:: stream
      :noindex:

      Indicates the ``Stream`` type, also called a Stop. Possible values include
      ``I3Frame::None``, ``I3Frame::Geometry``, ``I3Frame::Calibration``,
      ``I3Frame::DetectorStatus``, ``I3Frame::Simulation``, ``I3Frame::DAQ``,
      ``I3Frame::Physics``, and ``I3Frame::TrayInfo``. Read-only parameter.

   .. data:: type
      :noindex:

      Used to indicate the type of file. Possible values are ``Closed``,
      ``Stream``, ``Empty``, and ``File``. Read-only parameter.

   .. method:: close()
      :noindex:

      Close the file.

   .. method:: get_current_frame_and_deps()
      :noindex:

      Gets the current frame, and the frames it depends on. Returns a list of I3Frames [parent ... current].

   .. method:: get_mixed_frames()
      :noindex:

      Gets the frames that the current frame depends on. Returns a list of I3Frames.

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

   .. method:: pop_daq()
      :noindex:

      Shorthand for::

         pop_frame(icetray.I3Frame.DAQ)

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

   .. method:: rewind()
      :noindex:

      Close and reopen the file to the beginning.

   .. method:: seek(frame_number)
      :noindex:

      Seek to  specific frame number.

   .. method:: I3File()
      :noindex:

      Create an I3File object w/o an actual associated file::

        # not very useful
        f = I3File()

   .. method:: I3File(path, mode = I3File.Reading)
      :noindex:

      Create an I3File, then call::

        self.open_file(path, mode)

   .. method:: __iter__()
      :noindex:

      Return an iterator to the I3File of type I3FileIterator,
      which supports standard python iteration.
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

   .. method:: __len__()
      :noindex:

      Returns the size of the I3File in number of frames.

   .. method:: __exit__()
      :noindex:

      Exit context and close I3File

   .. method:: __enter__()
      :noindex:

      Enter context and open I3File

.. class:: I3FileIterator
   :noindex:

   Simple iterator for the frames contained in an I3File

   .. method:: __next__()
      :noindex:

      Same as next()

   .. method:: next()
      :noindex:

      Returns the next frame, if available, else throws StopIteration.
      This is part of the python 'iterator protocol'; this function
      gets you iteration in loops and list comprehensions (see __iter__() above):


   .. method:: __iter__()
      :noindex:

      Return an iterator to the I3File of type I3FileIterator,
      which supports standard python iteration.
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

.. class:: I3FileStager
   :noindex:

   A base class interface for staging files
   to local storage from remote locations

   This class is supposed to handle copying files
   to local storage for reading, and copying local
   files to remote storage for writing.

   .. method:: ReadSchemes()
      :noindex:

      Returns the URI scheme this stager can read.
      (i.e. the in a URL before the colon, e.g. "http",
      "ftp", "gsiftp", ...)

   .. method:: WriteSchemes()
      :noindex:

      Returns the URI scheme this stager can write

   .. method:: CanStageIn(url)
      :noindex:

      Returns True if url matches any of the URI
      schemes this stager can read

   .. method:: CanStageOut()
      :noindex:

      Returns True if url matches any of the URI
      schemes this stager can write

   .. method:: GetReadablePath()
      :noindex:

      Returns a readable local file handle for a remote URI

   .. method:: GetWriteablePath()
      :noindex:

      Returns a writeable local file handle for a remote URI

.. class:: I3FileStagerCollection
   :noindex:

   A subclass of I3FileStager for staging multiple files

   .. method:: __init__(pointer)
      :noindex:

      Initializes an instance of I3FileStagerCollection
      from an I3FileStagerCollectionPointer

.. class:: I3TrivialFileStager
   :noindex:

   A subclass of I3FileStager with no remote capability

   .. method:: __init__(pointer)
      :noindex:

      Initializes an instance of I3TrivialFileStager
      from an I3TrivialFileStagerPointer

.. class:: I3FrameSequence
   :noindex:

   A class to easily access multiple I3Files as if they were
   one large sequence of frames. Only supports read access.

   .. method:: __init__()

   .. method:: __init__(frame_sequence)
      :noindex:

      Copy constructor

   .. method:: __init__(file_names, cache_size)
      :noindex:

      Create and open an I3FrameSequence

   .. method:: close()
      :noindex:

      Close the files

   .. method:: add_file(file_name)
      :noindex:

      Add a file

   .. method:: close_last_file()
      :noindex:

      Close the last file

   .. method:: more()
      :noindex:

      Test if there is another frame

   .. method:: rewind()
      :nonindex:

      Return to the beginning of the sequence of files/frames.

   .. method:: pop_daq()
      :noindex:

      Return the next DAQ frame from the file, skipping frames on other streams.

   .. method:: pop_physics()
      :noindex:

      Return the next physics frame from the file, skipping frames on other streams.

   .. method:: seek(number)
      :noindex:

      Seek to a specific frame number

   .. method:: get_mixed_frames()
      :noindex:

      Returns the frames that are mixed into the current frame.

   .. data:: paths
      :noindex:

      Contains the paths to the files in the frame sequence

   .. data:: frameno
      :noindex:

      Contains the next frame number

   .. data:: size
      :noindex:

      Contains the total number of frames across all files in the I3FrameSequence

   .. data:: cur_size
      :noindex:

      Contains the current size of the frame sequence

