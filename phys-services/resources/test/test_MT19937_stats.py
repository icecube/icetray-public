#!/usr/bin/env python
import numpy as np
from scipy import stats
from icecube.phys_services import I3MT19937

N=1000

def kstest(rs,i3name,i3var,spname,spvar):
    sample = [getattr(rs,i3name)(*i3var) for x in range(N)]
    return  stats.kstest(sample, spname, args=spvar)[1]

def chisqtest(rs,i3name,i3var,spname,spvar,nbins):
    sample = [getattr(rs,i3name)(*i3var) for x in range(N)]
    hist = np.bincount(sample,minlength=nbins)
    expected = getattr(stats,spname).pmf(range(nbins),*spvar)*N
    return stats.chisquare(hist,expected)[1]

def test_p(pvals):
    pvals=np.array(pvals)
    assert np.all(pvals>0)
    frac = float((pvals < .05).sum())/pvals.size
    assert(frac<.15)

for rs in [I3MT19937(), I3MT19937(0),I3MT19937([]),
           I3MT19937([0]),I3MT19937([0,0]),I3MT19937([0,0,0])           
           ]:

    test_p([kstest(rs,'exp',(x,),'expon',(0,1./x)) for x in np.arange(.1,10,.1)])
    test_p([kstest(rs,'uniform',(x,),'uniform',(0,x)) for x in np.arange(.1,10,.1)])
    test_p([kstest(rs,'uniform',(x,x+y),'uniform',(x,y)) for x in range(1,10) for y in range(1,10)])
    test_p([kstest(rs,'gaus',(x,y),'norm',(x,y)) for x in range(1,10) for y in range(1,10)])
    test_p([chisqtest(rs,'integer',( x,),'randint',(0,x), x) for x in range(2,100)])
    test_p([chisqtest(rs,'binomial',(x,y),'binom',(x,y),x+1) for x in range(1,10) for y in np.arange(.1,1,.1)])
    test_p([chisqtest(rs,'poisson',(x,),'poisson',(x,),30) for x in np.arange(.1,10,.1)])        
    
