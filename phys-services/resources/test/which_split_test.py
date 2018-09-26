#!/usr/bin/env python

# $Id: which_split_test.py 125167 2014-10-29 01:16:40Z jvansanten $
# $Revision: 125167 $
# $LastChangedDate: 2014-10-28 21:16:40 -0400 (Tue, 28 Oct 2014) $
# $LastChangedBy: jvansanten $

from I3Tray import *
from icecube import icetray, dataclasses, dataio
from icecube.icetray import pypick, I3Frame

# this is the thing we want to test in this script:
from icecube.phys_services.which_split import which_split


#
# In this script we are putting together a completely artificial contrived
# situation The only purpose is to test the "which_split" function. Do not take
# this script as an example for anything, except maybe for getting some
# inspiration for how you can use the which_split utility in the "If" parameter
# of an I3ConditionalModule.
#

Nnull=1
Nfoo=3
Nbar=7

class split_test_frame_generator(icetray.I3Module):
    """
    Lo & behold: an utterly ridiculous unphysical event generator!
    It produces a Q frame and a bunch of P frames that can be selected with which_split.
    (As a bonus we add a fake NCh value, which is equal to 12, 17, 22, 27, etc for the
    successive splits in a particular subevent stream, so that boolean expressions with
    which_split can be illustrated.)
    For any other purpose this module is fantastically useless!
    """
    def __init__(self,ctx):
        super(split_test_frame_generator,self).__init__(ctx)
        self.genspecs=[{'name':n,'nP':N} for n,N in zip(['null','foo','bar'],[Nnull,Nfoo,Nbar])]
        self.AddOutBox("OutBox")
    def Configure(self):
        pass
    def Process(self):
        self.make_and_push_Q()
        for pspec in self.genspecs:
            self.make_and_push_P(pspec)
    def make_and_push_Q(self):
        newQframe=I3Frame('Q')
        ehQ=self.make_header(0,'')
        newQframe.Put('I3EventHeader',ehQ)
        self.PushFrame(newQframe)
    def make_and_push_P(self,pspec):
        newPframe=I3Frame('P')
        for i in range(pspec['nP']):
            newPframe=I3Frame('P')
            ehP=self.make_header(i,pspec['name'])
            newPframe.Put('I3EventHeader',ehP)
            newPframe.Put('NCh',icetray.I3Int(12+5*i)) # note: unphysical nonsense! just for this test.
            self.PushFrame(newPframe)
    def make_header(self,i,name):
        newheader=dataclasses.I3EventHeader()
        newheader.sub_event_id = i
        newheader.sub_event_stream = name
        newheader.state = dataclasses.I3EventHeader.OK
        # the rest of the event header we don't care about in this test
        return newheader

class schizzoparanoia(icetray.I3ConditionalModule):
    """
    Module for testing splits.
    Some splits are OK. Others are not.
    """
    def __init__(self,ctx):
        super(schizzoparanoia,self).__init__(ctx)
        self.should_run_on = set()
        self.did_run_on = set()
        self.AddParameter('ShouldRunOn','set of (splitname,subid) pairs that should be seen',self.should_run_on)
        self.AddOutBox("OutBox")
    def Configure(self):
        self.should_run_on = self.GetParameter('ShouldRunOn')
    def DAQ(self,qframe):
        self.ran_on_frame(qframe)
        self.PushFrame(qframe)
    def Physics(self,pframe):
        self.ran_on_frame(pframe)
        self.PushFrame(pframe)
    def ran_on_frame(self,frame):
        sub_name=frame['I3EventHeader'].sub_event_stream
        sub_id=frame['I3EventHeader'].sub_event_id
        self.did_run_on.add((sub_name,sub_id))
    def Finish(self):
        if self.did_run_on == self.should_run_on:
            icetray.logging.log_notice("(%21s) Yay, it worked correctly!" % self.name)
        else:
            forgotten = self.should_run_on - self.did_run_on
            if len(forgotten)>0:
                icetray.logging.log_error( "(%s) FORGOTTEN %s" % (self.name,forgotten))
                for name,i in forgotten:
                    icetray.logging.log_error("(%s) SHOULD have run on sub=%s subid=%d" % (self.name,name,i))
            forbidden = self.did_run_on - self.should_run_on
            if len(forbidden)>0:
                prinr("FORBIDDEN %s" % forbidden)
                for name,i in forbidden:
                    icetray.logging.log_error("(%s) SHOULD NOT have run on sub=%s subid=%d" % (self.name,name,i))

# here is another pypick, just to have something to play with in boolean expressions
def nch_geq(nchmin):
    def f(frame):
        if 'NCh' in frame:
            nch=frame['NCh']        # post-I3PODHolder (SVN commit 1914)
            #nch=frame['NCh'].value # pre-I3PODHolder (SVN commit 1914)
            if nch >= nchmin:
                return True
        return False
    return icetray.pypick(f)

tray=I3Tray()

tray.Add(split_test_frame_generator,"bogogenesis")

#tray.Add("Dump","truck")

tray.Add(schizzoparanoia,"null",
        ShouldRunOn=set([('null',0)]),
        If=which_split('null'),
        )

tray.Add(schizzoparanoia,"foo_bar",
        ShouldRunOn=set([('foo',i) for i in range(Nfoo)]+[('bar',j) for j in range(Nbar)]),
        If=which_split('foo') | which_split('bar'),
        )

tray.Add(schizzoparanoia,"also_foo_bar",
        ShouldRunOn=set([('foo',i) for i in range(Nfoo)]+[('bar',j) for j in range(Nbar)]),
        If=which_split('foo') ^ which_split('bar'),
        )

tray.Add(schizzoparanoia,"foo_bar_and_Q",
        ShouldRunOn=set([('',0)]+[('foo',i) for i in range(Nfoo)]+[('bar',j) for j in range(Nbar)]),
        If=~which_split('null'),
        )

tray.Add(schizzoparanoia,"bar_nch_lt_25",
        ShouldRunOn=set([('bar',i) for i in [0,1,2]]),
        If=which_split('bar') & ~nch_geq(25),
        )

tray.Add(schizzoparanoia,"foo_or_bar_nch_geq_20",
        ShouldRunOn=set([('foo',i) for i in range(2,Nfoo)]+[('bar',j) for j in range(2,Nbar)]),
        If=(which_split('foo') | which_split('bar')) & nch_geq(20),
        )

tray.Add(schizzoparanoia,"foo_nch_lt_13",
        ShouldRunOn=set([('foo',0)]),
        If=which_split('foo') & pypick(lambda f: f.Stop==I3Frame.Physics and f['NCh'] < 13), # post-I3PODHolder
        #If=which_split('foo') & pypick(lambda f: f.Stop==I3Frame.Physics and f['NCh'].value < 13), # pre-I3PODHolder
        )

tray.Execute(1)

