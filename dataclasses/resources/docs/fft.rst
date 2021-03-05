.. _fft:

====================================================
Fourier Transforms and Derived Dataclasses/Functions
====================================================

The Fourier transform and more specifically the discrete Fourier transform (DST), also known as the Fast Fourier Transform (FFT), are critical to the analysis of data that is recorded by antennas.
Many components of the analysis depends on working in Fourier space as the hardware and antennas themselves include frequency-dependent responses.
This section includes a discussion about the intricacies of DFTs, the algorithm that is used under the hood (:ref:`fftw3`), and a note about the calucation of energy in the two domains.

------------------------------
The Discrete Fourier Transform
------------------------------

The Fourier transform is a mapping of continuous functions into the a basis set of sines/cosines.
It is given by:

  .. centered::  :math:`F(\omega) = \int\limits_{-\infty}^\infty f(t) e^{-i2\pi t \omega}\ dt, \qquad f(t) = \int\limits_{-\infty}^\infty F(\omega) e^{i2\pi t \omega}\ d\omega.`

Where :math:`F(\omega)` is the Fourier transform of :math:`f(t)`. In the context of the FFTs that are used for radio analysis, :math:`f(t)` would be a function in the time domain and :math:`F(\omega)` is that function described in the frequency domain.

The discrete version of this instead transforms uniformly sampled values, :math:`{x_n}` into the fourier domain:

  .. centered::  :math:`X_k =\sum \limits^{N-1}_{n=0} x_n e^{-i 2\pi \frac{n k}{N}}, \qquad x_n = \sum \limits^{N-1}_{k=0} X_k e^{i 2\pi \frac{n k}{N}}`

Here the set of :math:`X_k` represent the Fourier transformation of :math:`x_n`, with bins indexed by :math:`k` and :math:`n`, respectively.
Note that this is the most basic definition of what can be considered an DFT.

Due to the Shannon-Nyquist theorem, the output of this formula for an input of N, time-domain amplitudes, sampled at :math:`\Delta t`, is N complex-valued amplitudes.

 .. note::
    Many DFT algorithms, such as the one used here, :ref:`fftw3`, throw out about half of these amplitudes since they represent the "negative frequencies" and are the complex conjugate of the others; thus containing no unique information. So you end up with N/2 + 1 amplitudes. This is known as a *one-sided* DFT as opposed to keeping all N values, a *two-sided* DFT.

These amplitudes describe the frequencies 0 to :math:`f_\text{max} = 1 / (2\, \Delta t)`. So the maximum frequency is dependent on the sampling rate but the resolution of this frequency range is dependent on the number of samples, N.
And the steps in the frequency domain are given by :math:`\Delta f = 1 / (N\,\Delta t)`.

The DFT, as given above does not know anything about the time steps, :math:`\Delta t`, nor the start time of the series.
That is, it does not know if bin 1 corresponds to :math:`t=0` or :math:`t=-1000`.
And as important, the DFT, unlike the continuous version, does not know anything about the units.
Thus it is important to note that if given only a frequency spectrum one cannot uiquely determine the initial time-domain waveform in the same way that performing an integral gives you the solution with an unknown constant.

  .. note:: This is one of the reasons that the class `FFTDataContainer` is important as it keeps the two paired together. This way, when changing between the time domain and the frequency domain, the initial values and time-step is not lost.


For more, see :ref:`fft_data_container`.

.. _FFTNormalizations:

----------------------
Normalizations of FFTs
----------------------

DFTs are tricky due to the non-standardized normalizations that are included in the various packages. For instance, we start with a basic definition of an DFT, given above.
After performing a "round trip", where you convert time-domain values to frequency-domain values and then the inverse back to the time-domain, one will end up with :math:`x_n \longrightarrow N\times x_n` (note that this is what is done for the underlying FFT package, :ref:`fftw3`, see below). So typically FFT packages rescale this equation. The two scalings commonly used are to multiply inverse DFT by :math:`N^{-1}` or to multiply both equations by :math:`N^{-1/2}`.
Thus, care must be taken when working with this abstract space.


  .. warning:: When working with or comparing two waveforms. Normalizations can only be ignored if all of the following are fulfilled:

  - You only care about the relative amplitudes of two spectra
  - Both spectra started in either the time- or Fourier-domain
  - Both have the same length (number of bins)
  - Any FFTs (or :math:`\text{FFT}^{-1}`) were performed using the same algorithm

An easy example to show the pitfalls of comparing two frequency spectrum is to imagine any time series with which you and your friend both
perform a DFT, but using different algorithms/normalizations.
The DC bin (:math:`k=0`) will be simply the sum of the time series bins. But say your friend instead used one with a normalization of :math:`N^{-1/2}` and thus their values would not agree with yours. It would seem that you two disagree on the Fourier amplitudes, however, this is all a normalization.
Further, imagine that you use the same algorithm, but you have two sets of measurements where you sampled at two different frequency rates, but over the same time interval (i.e. one measurement has :math:`N` bins and the other does not).
Our basic definition of the DFT does not know at which frequency you sampled, thus the frequency spectra will be offset since in one case, your DC bin will be the sum of :math:`N` time-domain bins and the other will not.

For this reason, it is extremely important that one **does not use FFT algorithms from other packages** such as numpy, scipy, etc. Doing this can yield unexpected results which are hard to track down. Instead it is recommended only to use the DFT algorithm provided by the :ref:`fft_data_container` class.


.. _fftw3:

----------------------
FFTW
----------------------

The Fourier package that actually handles the computation of FFTs is the Fastest Fourier Transform in the West version 3.3.4 
(`external documentation <http://www.fftw.org/#documentation>`_). This package is an open source code that which includes many algorithms to perform FFTs and is able to decide which one will yield the best performance for the job.

The portion of the FFTW code that is most relevant for this project is the real discrete 1D Fourier transform, the FFTW function ``fftw_plan_dft_r2c_1d``. This performs a one-sided FFT, returning :math:`N/2+1` amplitudes (where :math:`N` is the number of time-series measurements), with **no scaling/normalization**.

FFTW has a number of algorithms to actually carry out the FFT calculations and the best method depends on the length of the time series as well as the compiler. 
There are a number of flags that can be passed to FFTW to determine how much time it is allowed to spend on finding the optimal method for the given transform size, :math:`N`. 
This is done internally, by trying many different FFT algorithms on the passed in series and recording how long it takes to perform. 
This is reported to be compiler dependent and thus must be done for each machine. 
However, once a method has been found, further FFTs of the same length will automatically use this result and the calculation will not need to be redone. 
There is even a method to save the optimization data and simply load it, removing the need to optimize again on the next execution of the program. 
However, benchmark tests on ``Cobalt`` seem to show this optimization takes only fractions of a second (for order :math:`N <= 10^{5}`) and is thus not worth storing, even when using flags that require a high level of optimization.

The interface with FFTW is handled by the ``FFTWPlan`` class in and should never need to be directly interacted with directly by users nor developers. 
Instead users should rely exclusively on the :ref:`fft_data_container` class to perform Fourier transforms. 
This avoids losing important information such as the start- and step-time of the time-series as well as mixing FFT normalizations across the project which would be dangerous!



.. _fft_data_container:

----------------------
FFTDataContainer
----------------------

The :class:`FFTDataContainer` class is essential for a few main reasons:

1. Provides a wrapper for :ref:`fftw3` such that one never needs to explicitly "ask" for a DFT to be performed
2. Provides a consistent normalization for the transform
3. Holds the :math:`\Delta f`, :math:`\Delta t`, and :math:`T_0` which can otherwise be lost between analysis steps
4. Reduces the total amount of computationally expensive DFTs that need to be done
5. Ensures that both domains are up to date with respect to each other

This class holds the time-series and the fourier transform (amplitudes, start times, binning, etc).
It keeps track of the domain which was most recently updated so that when you request the time-series/spectrum that the class performs any DFT, as necessary, before giving you the waveform.

^^^^^^^^^^^^
Example
^^^^^^^^^^^^

.. code-block:: python

  from icecube import dataclasses

  #Make a time series and fill it with random data
  series = dataclasses.AntennaTimeSeries()
  for i in range(100):
    series.PushBack(i)

  #Make an FFTDataContainer and insert the time series into it
  fft = dataclasses.FFTData()
  fft.LoadTimeSeries()

  #When you request the frequency spectrum, the FFTDataContainer
  #knows that the time series has most recently been updated so the
  #DFT is performed to update the spectrum which is then returned
  spectrum = fft.GetFrequencySpectrum()

  #Pass this spectrum into some function which does some
  #augmentation to it
  MangleTheSpectrum(spectrum)
  
  #Now when you request the time series, it knows that the DFT
  #needs to be done again because the spectrum is more up-to-date
  series2 = fft.GetTimeSeries()

  #Assuming that MangleTheSpectrum actually changed the spectrum
  #This would return true
  assert(series2 != series1)

Note that there is no actual "monitoring" of the locations in memory to know **if** the waveform in one of the domains was augmented.
Rather, the class assumes that when a particular domain was requested, it also was augmented.
So in the example code above, event if ``MangleTheSpectrum`` actually did not change the passed in spectrum, the DFT would still be performed when requesting ``series2``.
Thus, in python, this class specifically saves on computation if the same series is requested twice in a row.
However, for C++, you can also request a ``const`` waveform (``GetConstTimeSeries`` and ``GetConstFrequencySpectrum``) in which the FFT will be performed and the :ref:`fft_data_container` will know that no updates have taken place since.

^^^^^^^^^^^^^
Normalization
^^^^^^^^^^^^^

As described in :ref:`FFTNormalizations`, the normalization used in the DFT algorithm is very important when one needs to calculate physics constants, particularly in the Fourier domain.
The underlying FFT algorithm, :ref:`fftw3` does not include any normalization, and thus the values would be scaled by N after one "round trip" to/from the Fourier domain.
Instead, the FFTs performed when the times-series or frequency spectrum is requested, includes its own normalization.
Specifically:

  .. centered::  :math:`X_k = \Delta t \sum \limits^{N-1}_{n=0} x_n e^{-i 2\pi \frac{n k}{N}}, \qquad x_n = \Delta f \sum \limits^{N-1}_{k=0} X_k e^{i 2\pi \frac{n k}{N}}`

Note that since :math:`\Delta t \ \Delta f = 1/N`, that this normalization leaves the values unchanged after one round trip.
Also, this normalization implicty includes units, which is uncommon for most FFT algorithms (such as `numpy <https://numpy.org/devdocs/reference/generated/numpy.fft.rfft.html>`_).
In the frequency domain, the units associated with the amplitudes are thus [:math:`x_n`][Time].
Typically this would be Volt/Hz or Volt/(meter Hz) for voltages and electric fields, respectively.


^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Calculation of Energy in the Two Domains
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The calculation of the energy, particularly in the Fourier domain is not a trivial task as it is directly related to normalization used by a given DFT algorithm.
For the normalization used by the :ref:`fft_data_container`, there is a relationship between the two domains given by:

  .. centered::  :math:`\Delta t \sum \limits^{N-1}_{n=0} |x_n|^2 = \Delta f \left[ |X_0|^2 + |X_{N/2}|^2 +  2 \times \sum \limits^{N/2-1}_{k=1} |X_k|^2 \right]`

This equation is a the formulation of Parseval's Theorem which essentially notes this relationship of the sum of the amplitudes squared in the two domains.
Note that the different treatment of the :math:`k=0,N/2` term and the factor of 2 on the right hand side comes from the fact that the negative frequency values have been removed and so the remaining values contain half the power.

The calculation of the energy then depends on the units of :math:`x_n`.
For the common application of measured voltages, dividing both sides by the relevant impedance will yield energy on both sides.
If the measured quantity is an electric field, then the division by the impedance will yeild the *energy fluence* (the integral of the Poynting vector).

 .. note::
    This formulation of Parseval's Theorem is only valid for combination of DFT algorithm and normalization used by the :ref:`fft_data_container`.


.. _hilbert_envelope:

---------------------
Hilbert Envelope
---------------------

The Hilbert Envelope describes the `analytic signal <https://en.wikipedia.org/wiki/Analytic_signal>`_ associated with an oscillating function.
The DFT of a real-valued time-series includes, to some extent the same information twice, where the amplitudes associated with the negative frequencies are simply the Hermitian conjugate of the amplitudes of the positive frequencies.
Making use of this symmetry, the analytic signal uses the Hilbert Transform:

  .. centered::  :math:`F(H(u))(\omega) = -i\, \text{sign}(\omega) \, F(u)(\omega)`

where :math:`F(x)` is the Fourier transform, and :math:`H(u)` is the Hilbert Envelope encompassing a function u.
Intuititvely, the Hilbert Envelope is the envelope which modulates an oscillating signal.
For instance, this would be the exponential decay which modulates a damped oscillator or the line :math:`y=1` for a sine curve.

This Hilbert Envelope is often used in radio analysis as its formulation directly shadows the electromagntic wave.
Note that the Hilbert Transform essentially is the combination of the E-field and B-field together, since one lags the other.
Thus the envelope gives the total amplitude of the electromagnetic wave, even if you are only measuring the E-field component.

^^^^^^^^^^^^
Example
^^^^^^^^^^^^

As part of the FFT tools in this project, there are functions to calculate the Hilbert Envelope for you.
Below is an example of how to use these functions

.. code-block:: python

  from icecube import dataclasses
  from icecube.dataclasses import fft #for GetHilbert____

  #We start with an FFTDataContainer
  fftData = frame["FFTDataContainerInFrame"]
  
  #This gets the amplitude of the maximum of the Hilbert Envelope
  hilbertPeak = fft.GetHilbertPeak(fftData)
  #Likewise, this gives the time at which this maximum occurs
  peakTime = fft.GetHilbertPeakTime(fftData)

  #You can instead get a waveform that directly gives you the envelope
  hilbertEnvelope = fft.GetHilbertEnvelope(fftData)

  ##############
  #One can instead perform these calculations on a time-series directly
  timeSeries = fftData.GetTimeSeries()
  hilbertPeak = fft.GetHilbertPeak(timeSeries)
  peakTime = fft.GetHilbertPeakTime(timeSeries)
  hilbertEnvelope = fft.GetHilbertEnvelope(timeSeries)

Note that these functions work on 1D (`AntennaTimeSeries`) and 3D (`EFieldTimeSeries`) waveforms.
If the Hilbert Envelope is requested, it will return a waveform of the same dimension as the object passed in.
Also note that there is no functionality to get the Hilbert Envelope from the frequency spectrum since you cannot determine the start time of the waveform and as such the peak time of the Hilbert Envelope is also ill defined.


.. _waveform_resampling:

---------------------
Waveform Resampling
---------------------

In the waveform analysis, the waveform is discribed by oscillating functions.
We can take advantage of this to artificially increase/decrease the resolution of the data sampling.
The idea is that since the waveform has been decomposed into sines/cosines in the DFT process, you know exactly what you would have measured at any point along these functions had the sampling resolution been higher/lower.
In practice this boils down to two steps: upsampling and downsampling.

The upsampling process works by taking some frequency spectrum of length (N/2 + 1) with maximum frequency :math:`f_s`.
To upsample, one adds additional bins to the frequency spectrum with an amplitude of zero.
This adds no additional power to the waveform, leaving it essentially untouched in amplitude and length.
Except now, the binning is more closely spaced.
This process is commonly referred to as `zero-padding`.

Downsampling works by simply throwing out data points in the time domain.
For instance if you want to downsample by a factor of 2, then the process simply consists of removing every-other entry in the time series.
In such a case, the maximum sampling frequency would change from :math:`f_s \longrightarrow f_s/2`.

These two processes can be used in conjunction to change the sampling frequency from the one used at the time of measurement to some other rate.
Because the throwing out of data actually removes information, it is best to upsample and *then* downsample instead of the other way around.
This process has been implemented into a single function which does this for you.

.. code-block:: python

  from icecube import dataclasses
  from icecube.dataclasses import fft #for ResampleFFTData

  #We start with an FFTDataContainer
  fftData = frame["FFTDataContainerInFrame"]

  #Specify the sampling rate that you want the waveform to be resampled to
  targetSamplingRate = 0.5 * I3Units.megahertz

  #Perform the resampling here
  fft.ResampleFFTData(fftData, targetSamplingRate)

  assert(fftData.GetTimeSeries().binning == targetSamplingRate)

