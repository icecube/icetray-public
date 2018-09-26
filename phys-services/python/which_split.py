# $Id: which_split.py 132457 2015-05-14 14:58:44Z blaufuss $
# $Revision: 132457 $
# $LastChangedDate: 2015-05-14 10:58:44 -0400 (Thu, 14 May 2015) $
# $LastChangedBy: blaufuss $

from icecube.icetray import pypick, I3Frame

def which_split(split_name=''):
    """
    Event substream selection utility. It's presented as a "pypick", that is, you can
    do boolean expressions with the function itself (as opposed to its return value):

    If = (which_split('foo') | which_split('bar')) & pypick(lambda f: 'somepulses' in f)

    Note that you should use the symbolic logic operators (&=and, \|=or, ^=xor, ~=invert),
    because the English versions have a slightly different scope and in this case that
    actually matters, using the English versions will sometimes result in undesired
    surprising behavior.
    """
    def f(frame):
        if frame.Stop == I3Frame.Physics:
            eh=frame['I3EventHeader']
            ret = ( eh.sub_event_stream == split_name )
            return ret
        else:
            return False
    return pypick(f)
