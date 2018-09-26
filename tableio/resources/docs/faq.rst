.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$


Frequently asked questions and common pitfalls
===============================================

1. I get weird errors like "error: macro "BOOST_PP_SEQ_ELEM_III" requires 2
arguments, but only 1 given" when compiling tableio.

    This happens when you use versions of dataclasses older than tableio itself. Use
    at least version 10-06-00 (part of offline-software 10-06-00). As a general rule,
    you should use tableio with the offline-software release it's part of.