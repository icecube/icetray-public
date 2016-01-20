I3Tray Quick Reference
======================

Modules
-------

First Module
^^^^^^^^^^^^

The driving module.  No inbox, only an outbox.

Options:

- I3Reader
- I3InfiniteSource
- I3CorsikaReader
- (some more obscure things)

Intermediate Modules
^^^^^^^^^^^^^^^^^^^^

Regular modules.  Has both an inbox and an outbox.

Last Module
^^^^^^^^^^^

The trashcan.  No outbox, only an inbox.

Outboxes
--------

Default is "OutBox"

If you have multiple outboxes, they cannot be recombined and must be
written to separate files (with separate trashcans).

The outbox is a queue of frames.  The module pushes frames onto this
queue, which is consumed by the next module's inbox.

Process
^^^^^^^

The Process function of a module handles internal things

I3Module.Do -> I3Module.Process -> I3Module.DAQ or I3Module.Physics

RequestSuspension
^^^^^^^^^^^^^^^^^

Stop calling Process.  Once all queues are cleared, I3Tray.Finish is called.

Packet Modules
--------------

When you have QPPQPP and want to operate on a single packet of QPP.
Or maybe you want to reunify QP frames for icetop-inice coincidence.

The packet module buffers frames until we have an entire packet, then
processes it.

FramePacket
^^^^^^^^^^^

Function that takes a vector of frames (QPP).  Otherwise just like
Physics function.

Splitter Module
^^^^^^^^^^^^^^^

Does the reverse, pushing multiple frames per single input frame.

Pointers
^^^^^^^^

Note that if you reuse a pointer between frames in the same packet,
the previous frame contents can be overwritten.  Please do not reuse
pointers between frames!

Source
^^^^^^

The I3PacketModule definition is here:
http://code.icecube.wisc.edu/icetray/projects/icetray/trunk/public/icetray/I3PacketModule.h

Frame Mixing
------------

I3Frame objects have names, streams, and data (key, metadata, value).

Every time you see a frame name, all values get merged into subsequent
frames until you see that frame name again.

Details for advanced frame mixing can be found :ref:`here <frame-mixing-details>`.

I3Frame::merge
^^^^^^^^^^^^^^

Merge things into the active frame in memory.

I3Frame::purge
^^^^^^^^^^^^^^

Purge things from the active frame in memory.

PushFrame
^^^^^^^^^

Every time this is called, the frame is purged and then merged with
a cached copy of previous frames.

Note that mixing breaks if you try to add things to a frame after
pushing it.  (break means strange behavior in this case)

Exceptions
^^^^^^^^^^

The two exceptions for mixing are TrayInfo or Physics frames.
These do not mix into other frames.

Examples
--------

I3CorsikaReader::   

    void I3CORSIKAReader::Process()
    {
        // Make a new frame.
        I3FramePtr frame(new I3Frame(I3Frame::DAQ));
        
        // i3_ifs_ is a frame stream.
        // Write frames from file before starting to write
        // our own frames.
        if (!i3_ifs_.empty()) {
            // Get frame from prefix file
            frame->load(i3_ifs_);
            if (i3_ifs_.peek() == EOF)
                i3_ifs_.reset();
            PushFrame(frame);
            return;
        }

        // Fill the frame with CORSIKA info.
        if (FillFrameFromCORSIKA(frame) != 0)
            return; // skip if no CORSIKA info

        // Push the frame to the outbox.
        PushFrame(frame);
    }
    
    
    
    
    
