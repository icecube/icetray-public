.. _i3ComplexVector:

===============
I3ComplexVector
===============

The I3ComplexVector class shadows most of the :ref:`i3Position` class.
This class describes a 6D vector comprised of a 3D vector with complex amplitudes.
Unlike for :ref:`i3Position`, there is only support for cartesian coordinates.
This class is particularly useful for describing the Fourier Transform of an electromagnetic wave.

The amplitudes are stored as ``std::complex<double>``.
The operators that one would expect are included for this class including, addition, subtraction, dot-products, rotations, and normalizations.

.. code-block:: python

  In [1]: from icecube.dataclasses import I3ComplexVector

  #The default constructor is full of nans
  In [2]: vec = I3ComplexVector()
  In [3]: print(vec)
  I3ComplexVector((nan,0), (nan,0), (nan,0))

  In [4]: vec = I3ComplexVector(1+2j, 2+7j, 13-9j) 
  In [5]: print(vec)
  I3ComplexVector((1,2), (2,7), (13,-9))

  In [6]: vec2 = I3ComplexVector(1+1j, 2+2j, 3-3j)

  # Vector addition
  In [7]: print(vec+vec2)
  I3ComplexVector((2,3), (4,9), (16,-12))

  # Vector subtraction
  In [8]: print(vec-vec2)
  I3ComplexVector((0,1), (0,5), (10,-6))

  #Vector dot product
  In [9]: print(vec*vec2)
  (87+23j)
  #Also defined explicitly
  In [10]: print(vec.Dot(vec2))
  (87+23j)

  #Magnitude of the vector
  In [11]: print(abs(vec))
  17.549928774784245
  In [13]: print(vec.magnitude())
  17.549928774784245

  #Normalize the vector
  In [14]: vec.normalize()
  In [15]: print(vec)
  I3ComplexVector((0.0569803,0.113961), (0.113961,0.398862), (0.740744,-0.512823))

  #Direct assignment of components
  In [16]: vec.x = 1.3j
  In [17]: vec.y = 1-5j
  In [18]: vec.z = 9
  In [19]: print(vec)
  I3ComplexVector((0,1.3), (1,-5), (9,0))

  #Rotations (angle given in rad)
  In [20]: vec.rotate_x(3.141592653)
  In [21]: print(vec)
  I3ComplexVector((0,1.3), (-1,5), (-9,-2.94897e-09))


The class also includes support for scalar math

.. code-block:: python

  In [1]: from icecube.dataclasses import I3ComplexVector

  In [2]: vec = I3ComplexVector(1+2j, 2+7j, 13-9j)

  In [3]: print(vec * 3)
  I3ComplexVector((3,6), (6,21), (39,-27))

  In [4]: print(vec / 3)
  I3ComplexVector((0.333333,0.666667), (0.666667,2.33333), (4.33333,-3))

  #In place operators
  In [5]: vec *= 7
  In [6]: print(vec)
  I3ComplexVector((7,14), (14,49), (91,-63))