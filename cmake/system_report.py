#!/usr/bin/env python
import sys
import urllib
import urllib2
try:
    url = 'http://code.icecube.wisc.edu/report/'
    data = dict([v.split(':',1) for v in sys.argv[1].split(',') if ':' in v])
    urllib2.urlopen(url+'?'+urllib.urlencode(data))
except:
    pass
