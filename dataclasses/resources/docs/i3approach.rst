.. SPDX-FileCopyrightText: 2025 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. _i3Approach:

==========
I3Approach
==========

The I3Approach is a simple wrapper to store an I3Double and an associated I3Position.
It was introduced to store the closest approach of a particle to the IceCube detector,
storing the actual minimum distance between the two
as well as the position the particle is at at that point along its trajectory.

.. code-block:: ipython

  In [1]: from icecube.dataclasses import I3Approach, I3Position

  # The default constructor is full of nans
  In [2]: appr = I3Approach()
  In [3]: print(appr)
  I3Approach(nan,I3Position(nan,nan,nan))

  # You can construct with double and I3Position
  In [4]: appr = I3Approach(1.0, I3Position(2.0,3.0,4.0))
  In [5]: print(appr)
  I3Approach(1.0,I3Position(2.0,3.0,4.0))

  # Or with doubles for the individual position components
  In [6]: appr = I3Approach(1.0, 2.0, 3.0, 4.0)
  In [7]: print(appr)
  I3Approach(1.0,I3Position(2.0,3.0,4.0))

  # You can get the distance and position individually
  In [8]: print(appr.distance)
  1.0
  In [9]: print(appr.position)
  I3Position(2.0,3.0,4.0)

  # And set them as expected
  In [10]: appr.distance = 10.0
  In [11]: appr.position = I3Position(-2.0,-3.0,-4.0)
  In [12]: print(appr)
  I3Approach(10.0,I3Position(-2.0,-3.0,-4.0))

  # There's also many comparators
  In [13]: appr2 = I3Approach(9.0, I3Position(8.0,7.0,6.0))

  # == checks whether both distance and position are equal
  In [14]: print(appr == appr2)
  False

  # != is the negation of ==
  In [15]: print(appr != appr2)
  True

  # <, >, <=, >= all compare the distances only
  In [16]: print(appr < appr2)
  False
  In [17]: print(appr > appr2)
  True
  In [18]: print(appr <= appr2)
  False
  In [19]: print(appr >= appr2)
  True

  # You can also copy another I3Approach
  In [20]: appr = appr2
  In [21]: print(appr)
  I3Approach(9.0,I3Position(8.0,7.0,6.0))

