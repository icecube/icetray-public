.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: cpp

Don't write unintentionally nongeneric code
-------------------------------------------

Commit to abstractions, not to details. Use the most generic
and abstract means to implement a piece of functionality.
More generic code is more reusable and more resilient to
changes in its surroundings.

Some examples:

* Use :code:`empty()` instead of :code:`size() == 0`
* Write const-correct code using :code:`const&` parameters
* Prefer iteration to indexed access
* Use the highest class in the hierarchy that offers what you need

Exceptions:

    Sometimes indexing is faster, but check before you blindly do it.
