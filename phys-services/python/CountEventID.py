"""
author mzoll <marcel.zoll@fysik.su.se>
"""

import string
from icecube import icetray, dataclasses

class CountEventID(icetray.I3Module):
  """                                                                                                                                       
  Convenience module, similar 'Dump', which automatically prints out comprehensive info of passed-by frames
  """
  def __init__(self, ctx):
    icetray.I3Module.__init__(self, ctx)
    self.AddOutBox("OutBox")
    self.to_logging_ = False
    self.frameCount_= 0
    self.frameCount_= 0
    self.q_frameCount_= 0
    self.p_frameCount_= 0
    self.g_seen_= False
    self.c_seen_= False
    self.d_seen_= False
    self.q_seen_= False
    self.AddParameter("ToLogging", "use the Log_info-stream for user output", self.to_logging_)
  def Configure(self):
    icetray.I3Module.Configure(self)      
    self.to_logging_ = self.GetParameter("ToLogging")
  
  def Process(self):
    frame = self.PopFrame()
    
    if (not frame):
      icetray.logging.log_fatal("FrameID is not a driving module!")
    
    outstream = []
    
    outstream.append("[%08d] "%( self.frameCount_ ))
    
    stop = frame.Stop
    
    
    if stop == icetray.I3Frame.TrayInfo :
        outstream.append( "Info-frame" )
    elif stop==icetray.I3Frame.Geometry :
        outstream.append("Geometry frame")
        if (self.g_seen_):
          outstream.append("[changed]")
          self.q_seen_ = False
        else:
          self.g_seen_ = True
    elif stop==icetray.I3Frame.Calibration :
        outstream.append( "Calibration frame")
        if (self.c_seen_):
          outstream.append("[changed]")
          self.q_seen_ = False
        else:
          self.c_seen_ = True
    elif stop==icetray.I3Frame.DetectorStatus : 
        outstream.append("DetectorStatus frame")
        if (self.d_seen_):
          outstream.append("[changed]")
          self.q_seen_ = False
        else:
          self.d_seen_ = True
    elif stop==icetray.I3Frame.DAQ :
        outstream.append("DAQ frame")
        seen = "+["
        if (self.g_seen_):
          seen+="G"
        if (self.c_seen_):
          seen+="C"
        if (self.d_seen_):
          seen+="D"
        seen += "]"
        outstream.append(seen)
        outstream.append("::")
          
        if (not frame.Has("I3EventHeader")):
          icetray.logging.log_error("This frames misses the I3EventHeader!")
          icetray.logging.log_error(str(frame))
        else:
          eh = frame["I3EventHeader"]
          outstream.append("Count: %08d Run: %06d Event: %08d"%(self.q_frameCount_,eh.run_id,eh.event_id))
    
        self.q_frameCount_+=1
        if (self.g_seen_ and self.c_seen_ and self.d_seen_):
          self.q_seen_ = True
       
    elif stop==icetray.I3Frame.Physics : 
        outstream.append("Physics frame")
        seen = "+["
        if (self.g_seen_):
          seen+="G"
        if (self.c_seen_):
          seen+="C"
        if (self.d_seen_):
          seen+="D"
        if (self.q_seen_):
          seen+="Q"
        seen += "]"
        outstream.append(seen)
        outstream.append("::")
          
        if ( not frame.Has("I3EventHeader")):
          icetray.logging.log_error("This frames misses the I3EventHeader!")
          icetray.logging.log_error(str(frame))
        else:
          eh = frame["I3EventHeader"]
          outstream.append("Count: %08d Run: %06d Event: %08d SubEventStream: %s SubEventID: %02d"%(self.p_frameCount_,eh.run_id,eh.event_id,eh.sub_event_stream,eh.sub_event_id))
          
        self.p_frameCount_+=1

    else:
        outstream.append("%s-frame "%(frame.Stop))
        if (self.g_seen_ and self.c_seen_ and self.d_seen_):
          outstream.append("[+GCD]")
        if (q_frameCount_):
          outstream.append("[+Q]")

    self.frameCount_+=1
    
    if (self.to_logging_):
      icetray.logging.log_info(string.join(outstream, " "), "CountEventID")
    else:
      print (string.join(outstream, " "))

    self.PushFrame(frame,"OutBox")
    
  