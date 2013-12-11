from icecube import icetray
class RoundRobinStreams(icetray.I3Module):
    """
    Module creates and pushes frames of different types.
    
    *Parameters*:

    * **Streams** -  list of frame 'stream types' to create and push.

    *Example*::

      tray.AddModule(RoundRobinStreams, 'rrs',
                     Streams = [icetray.I3Frame.Physics,
                                icetray.I3Frame.Geometry])    

    """
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter("Streams",
                          "Which frame types should we issue",
                          [icetray.I3Frame.Physics])
        self.AddOutBox("OutBox")

    def Configure(self):
        self.streams = self.GetParameter("Streams")
        self.index = 0

    def Process(self):
        self.index = (self.index + 1) % len(self.streams)
        frame = icetray.I3Frame(self.streams[self.index])
        self.PushFrame(frame)
