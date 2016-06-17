Memory Tracking
===============

How to use memory tracking
--------------------------

Snapshot
""""""""

A snapshot is the contents of memory at a specific point in time.

An example::

    def mem(fr):
        snap = icetray.memory.get_extents()
        icetray.memory_util.print_snapshot(snap)
        
    tray.Add(mem)

This gets a snapshot for every frame that passes.

.. note::

   This is only of limited use, as it mostly shows the memory usage
   while modules are not actively processing a frame. But it is good
   for seeing the background level, or how much a module is carrying
   over from frame to frame.

Timeline
""""""""

A timeline is the view of memory over multiple snapshots.

A basic example wraps the `execute()` function with memory tracking::

    t = icetray.memory.MemoryTimeline()
    t.start(10000)

    tray.Execute(5)

    t.stop()
    icetray.memory_util.graph_timeline(t.get_timeline(),filename='mem.png')

This will generate a graph of the top 10 memory consumers, saved to
`mem.png`.

API Documentation
-----------------

.. autoclass:: icecube.icetray.memory
   :members:

.. automodule:: icecube.icetray.memory_util
   :members: