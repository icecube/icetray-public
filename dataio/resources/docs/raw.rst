Raw I3File Access
=================

For directly accessing an I3 file from outside an I3Tray, the convenience
class :class:`I3File` is available::

    from icecube import icetray, dataio

    file = dataio.I3File('my_file.i3')
    for frame in file:
        # do something with frame

:class:`I3File` supports all the compression formats as the `I3Reader` module does.
Adding an extension should "just work".

Access Methods
--------------

Like a regular file, :class:`I3File` takes a second parameter describing
the "mode" to open in:

'r':
    Read mode

'w':
    Write mode

::

    # we can write to this file
    outfile = dataio.I3File('output.i3','w')
    for frame in file:
        outfile.push(frame)
    outfile.close()

.. important::

   Always close an output file so the buffer
   is written to the file cleanly.

Context Manager
---------------

:class:`I3File` supports the context manager::

    with dataio.I3File('input.i3.gz') as file:
        for frame in file:
            # do things

This handles closing the file if an exception occurs.
