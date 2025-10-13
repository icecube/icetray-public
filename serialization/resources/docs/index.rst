.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. _serialization:

serialization
=============

This project is a vendored version of `boost::serialization (v1.57) <https://www.boost.org/library/1.57.0/serialization/>`_,
where (for the most part) only the namespace has changed.  The motivation for this freeze was when `boost::serialization
<https://www.boost.org/library/latest/serialization/>`_ broke our archive format (branched long ago from the "standard" Boost
"portable binary archive"), in Boost version 1.58.

.. note::

   While there is no IceTray specific documentation for this project, the
   `boost::serialization (v1.57)
   <https://www.boost.org/library/1.57.0/serialization/>`_, documentation is
   considered as canonical. Do note that we regularly backport significant
   bugfixes from Boost while trying to maintain parity with v1.57.

.. toctree::
   :maxdepth: 3

   release_notes
