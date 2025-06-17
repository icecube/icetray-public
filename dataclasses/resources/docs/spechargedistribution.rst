.. SPDX-FileCopyrightText: 2025 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: pycon

=====================
SPEChargeDistribution
=====================

The SPEChargeDistribution is a PDF class used for sampling the charge response of a PMT to a single incident photon. We also
use the SPEChargeDistribution to calculate the mean expected charge per photon to be used in reconstructions.

The SPEChargeDistribution is the sum of an arbitrary number of Gaussian and Exponential PDFs fit to detector data. Remaining
disagreements between the model and detector data are encoded in ratios given as "residual corrections". Both the PDFs and
residuals may be specified in python or C++.

History
=======

All code used to simulate the detector response requires some method of taking an expected charge and receiving an observed
charge. In IceCube, we refer to this as the "SPE charge distribution", although "single photon response" is sometimes used in
other experiments instead. This distribution models the resulting "charge" (really a voltage) that is read out from the PMT
when a single photon hits the photocathode. In IceCube, we've had two major versions over the years, with the model changing
occurring during the periodic reprocessing of the dataset.

The first, used in what is now referred to as "Pass1" of the data, used a model including a single exponential term and a
single gaussian

.. math::
   f_{pass1}(x) = A e^{-x/\lambda} + B e^{\frac{-1}{2} \left(\frac{x-\mu}{\sigma}\right)^2}

The exponential described various incomplete amplification of signals. The Gaussian here describes the spread in charge
(again, really voltage) for fully amplified signals. The spread here occurs because of uncertainty in the number of electrons
ejected from each dynode during amplification. This "pass1" model used uniform values across the full detector measured at
Chiba University using an average of 118 PMTs. For details see
http://www-ppl.s.chiba-u.jp/research/IceCube/pmt/SPE/CRAnalysis.html#average.

Later, a more detailed model was produced to better model the response of individual PMTs. This model, used for "Pass2"
simulation, added a second exponential distribution to better fit the very low charges in data. For more detailed descriptions
of the model used for Pass2, see our publication at https://arxiv.org/abs/2002.00997.

.. math::
   f_{pass2}(x) = A e^{-x/\lambda_1} + B e^{\frac{-1}{2} \left(\frac{x-\mu}{\sigma}\right)^2} + C e^{-x/\lambda_2}

In this model, each PMT was fit individually using specially selected pulses. The average disagreement between the fit models
and the data was collected for use as a *residual correction* used to better align simulation with the data.

The "Pass2" charge distributions showed significantly improved agreement with data, but also uncovered other minor issues.
Traditionally, the peak of the Gaussian has been defined to represent "1 PE" worth of charge. This calculation is performed
onboard IceCube's DOMs. Differences between the method used online and the pulse extraction methods used offline for analyses
showed a systematic difference. These systematic shifts were fit and the results, known as *SPE corrections*, were used to
rescale the charge values in data for Pass2 data processing.

The updated Pass2 SPE charge distributions also posed a simulation challenge: the new templates included more of the
probability mass below the discriminator threshold, resulting in fewer hits in the detector. In order to preserve the relative
values of the DOM efficiciency measurements, a *compensation factor* was created as a relative rescaling of the PMT
efficiency. This factor, around 1.3 for Pass2 simulation, ensures that the average charge observed for a DOM efficiency of
"1.0" is consistent between Pass1 and Pass2 simulation.

Future calibration methods and upcoming detectors may need more flexibility than the "Pass2" SPE charge distributions allowed,
however. Instead of hardcoding the functional form of SPE distributions, we have instead rewritten the functions to allow for
arbitrary sums of Gaussian and exponential distributions. The *SPE correction* values previously part of the SPE charge
distributions in Pass2 have been moved and now live in the I3DOMCalibration class. Additionally, a parameter previously known
as "SLC Gaussian mean" has been renamed *FADC charge scale* and is used to represent the charge scale of IceCube's two
digitizers, giving the FADC mean single photon peak charge relative to the ATWD's single photon peak charge for use in simulation.

Python Interface
================

SPEChargeDistributions may be constructed from python. For example, let's assume we want to define a model ``f(q)`` with three
components and a simple residual ``R(q)``.

.. math::
  f(q) = 0.2*Exp(\lambda=3) + 0.5*Exp(\lambda=0.7) + 0.3*Gaus(\mu=1, \sigma=0.2)

.. math::   
  R(q) =
   \begin{cases}
     0.75 & 0.0 <= q < 0.5 \\
     1.0  & 0.5 <= q < 2 \\
   \end{cases}

To construct this distribution in python, we do this:

   >>> import numpy as np
   >>> from icecube.dataclasses import SPEChargeDistribution
   >>>
   >>> # Define some aliases for cleanliness
   >>> exp = SPEChargeDistribution.Exponential
   >>> gaus = SPEChargeDistribution.Gaussian
   >>>
   >>> # Construct the object. It's empty at this point.
   >>> spe = SPEChargeDistribution()
   >>>
   >>> # Build the PDFs
   >>> spe.pdfs.append(exp(amplitude=0.2, width=3))
   >>> spe.pdfs.append(exp(amplitude=0.5, width=0.7))
   >>> spe.pdfs.append(gaus(amplitude=0.3, mean=1, sigma=0.2))
   >>>
   >>> # Set the residuals
   >>> x = np.array([0.00, 0.50, 2.00])
   >>> y = np.array([0.75, 1.00, 1.00])
   >>> spe.residuals.Set(x.tolist(), y.tolist())

The compensation factors and FADC charge scale (described in the History section above) can also be set from python.

   >>> spe.compensation_factor = 1.2
   >>> spe.fadc_charge_scale = 1.30

At this point, we have a fully constructed SPEChargeDistribution. We can now use it to calculate things like the expected mean
SPE charge and standard deviation. These are given in units of PE and PE**2 respectively.

   >>> mean_charge = spe.Mean()
   >>> stddev = spe.StdDev()

If you want to limit GCD file sizes, you'll want to create a single ``SPEChargeDistribution.Residuals`` object and use it for
multiple DOMs.

   >>> residuals = SPEChargeDistribution.Residuals()
   >>> residuals.Set([0, 0.5, 2], [0.75, 1.0, 1.0])
   >>>
   >>> cal = frame["I3Calibration"]
   >>> for omkey in cal.dom_cal:
   >>>     cal.dom_cal[omkey].combined_spe_charge_distribution.residuals = residuals
   >>>
   >>> del frame["I3Calibration"]
   >>> frame["I3Calibration"] = cal

This can be used to slightly reduce the GCD file size. For example, the pass2 GCD file using the new format sheds about 1% of
it's total size when sharing the ``Residuals`` objects.

   >>> /scratch/icetray/testing/ $ ls -l gcd_*
   >>>
   >>> -rw-rw-r-- 1 mlarson mlarson 13705364 Apr 30 10:23 gcd_no_shared_residuals.i3.zst
   >>> -rw-rw-r-- 1 mlarson mlarson 13493370 Apr 30 10:23 gcd_shared_residuals.i3.zst
   >>>

We can also sample from the distribution, although this requires DOMLauncher at the moment due to technical reasons explained
below.

   >>> from icecube.phys_services import I3MTRandomService
   >>> from icecube.DOMLauncher import HamamatsuR7081_02PMT
   >>>
   >>> rng = I3MTRandomService(seed=0)
   >>> pmt = HamamatsuR7081_02PMT(prePulseProbability=0,
   >>>                            latePulseProbability=0,
   >>>                            afterPulseProbability=0,
   >>>                            randomService=rng)
   >>> pmt.setSPEChargeDistribution(spe)
   >>>
   >>> # Sample a single charge
   >>> q = pmt.sampleNormalPulseCharge(npe=1)
   >>>
   >>> # Sample many charges
   >>> q = [pmt.sampleNormalPulseCharge(npe=1) for _ in range(100000)]


C++ Implementation Details
==========================

The C++ interface for the SPEChargeDistribution mirrors the python implementation, with similar constructors available for the
SPEChargeDistrbution class itself as well as the ``PDF`` classes.

PDFs Implementation
-------------------

Under the hood, the SPEChargeDistribution class consists of two pieces:

1. ``std::vector<boost::shared_ptr<SPEChargeDistribution::PDF>> pdfs`` where ``PDF`` is the base class of the
   ``SPEChargeDistribution::Exponential`` and ``SPEChargeDistribution::Gaussian`` derived classes.
2. ``boost::shared_ptr<SPEChargeDistribution::Residuals>> residuals`` holding the residual corrections to be applied to the
   model.

The ``PDF`` subclasses directly contain functions to calculate probabilities and sample charge, although sampling is
implemented via a template in order to break a dependency loop.

   >>> template<class rng> double Sample(boost::shared_ptr<rng> random)

Because ``boost::python`` requires a defined template parameter (and the ``I3RandomService`` isn't defined until we build
``phys-services``, which in turn depends on ``dataclasses``), this method cannot be directly exposed to python. This may be
fixable with ``concepts`` in C++20 or ``boost::type_erasure`` in the future, although this is not attempted at this time.

Residual values
---------------

Like ``PDF``, ``Residuals`` is its own subclass within ``SPEChargeDistribution``. This is intended to allow for shared
residual objects across multiple ``SPEChargeDistribution`` objects.

Residuals are specified as lists of doubles corresponding to ``x`` (ie, charge) and ``y`` (ie, data/model ratio). These are
points, not histogram bins: we require ``len(x)==len(y)`` since the actual residuals used in calculations are interpolations
from these points. If you generate these from histograms with bins ``len(xbins) = len(ratios)+1``, be sure to append a padding
to the right side of your ratios before passing them in.

``SPEChargeDistribution::Residuals`` have private ``x`` and ``y`` data members. These can be set using the ``Residuals::Set``
method. Both must be set simultaneously to ensure that the length of both lists match. The values can be accessed as ``const
std::vector<double>`` views via the ``Residuals::GetX()`` and ``Residuals::GetY()`` methods. In python, these values can be
passed to ``Residuals.Set`` as standard python lists. These can be generated from numpy with the ``array.tolist()`` methods.

.. note::
    The input arrays provided to the ``Residuals`` code are required to be sorted by `x` value.

There are three cases for these residuals:

 1. **len(x)=len(y)=0**: Empty lists result in no residual being applied (ie, assuming ``data==model`` is True)
 2. **len(x)=len(y)=1**: Both the x and y arrays have one element. We assume that the residual is a constant for all charges.
 3. **len(x)=len(y)>1**: Both lists have multiple elements. Charges below ``x[0]`` receive residual value ``y[0]``. All other
    values are linearly interpolated (``charge < x[-1]``) or linearly extrapolated (``charge >= x[-1]``).

Due to previous implementation choices, the pass2 residuals are hardcoded as private static variables in the class
(``Residuals::x_pass2`` and ``Residuals::y_pass2``). These are used when we construct pass2-like SPEChargeDistributions,
either from GCD files (``version==2`` in serialization), when explicitly requested via ``Residuals::SetPass2()``, or when
using the old constructor

    >>> SPEChargeDistribution(double amp_exp1,
    >>>                       double width_exp1,
    >>>                       double amp_exp2,
    >>>                       double width_exp2,
    >>>                       double amp_gaus,
    >>>                       double mean_gaus,
    >>>                       double width_gaus,
    >>>                       double factor_compensation,
    >>>                       double gaus_mean_SLC)

which is maintained for use as defaults inside ``DOMLauncher``. This constructor is not exposed to python.
