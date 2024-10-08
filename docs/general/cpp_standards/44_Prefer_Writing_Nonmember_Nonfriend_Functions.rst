.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: text

Prefer Writing Non-member Non-friend Functions
----------------------------------------------

Avoid membership fees.  Where possible, prefer making functions non-member,
non-friends.

* Improve encapsulation by minimizing dependencies:  The body of the function
  cannot come to depend on the non--public members of the class.

* Reduce coupling, by breaking apart monolithic classes to liberate separable
  functionality.

* Improve genericity, because it is hard to write templates that don't know
  whether or not an operation is a member for a given type.

Here is an algorithm to determine whether a function should be a member and/or
a friend::

    If the function is one of the operators =, ->, [], or (), which must be members:
    => Make it a member

    Else if:
        a) the function needs a different type as its left-hand argument (as do stream operators, for example);
    or b) it needs type conversions on its leftmost argument;
    or c) it can be implemented using the class's public interface alone:
    => Make it a non-member (and friend if needed in cases a) and b) )

    If it needs to behave virtually:
    => Add a virtual member function to provide the virtual behavior, and implement the non-member in terms of that

    Else:
    => Make it a member
