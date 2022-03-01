.. index:: photospline
   pair:   tool; photospline

.. highlight:: sh

Photospline
===========

Detector response to a high-energy physics process is often estimated by
Monte Carlo simulation. For purposes of data analysis, the results of this
simulation are typically stored in large multi-dimensional histograms,
which can quickly become unwieldy in terms of size or numerically
problematic due to unfilled bins or interpolation
artifacts. `Photospline <https://github.com/icecube/photospline>`_
is a library that uses the penalized spline technique to efficiently
compute, store, and evaluate B-spline representations of such tables.

