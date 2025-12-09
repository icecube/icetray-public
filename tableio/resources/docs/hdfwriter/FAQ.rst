.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. py:currentmodule:: icecube.tableio

.. _tableio/hdfwriter-faq:

FAQ
===

Can I use tableio/hdfwriter to write from files with only Q frames, or would I have to create P frames on my own with those objects that I want to have in the hdf5 file?
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

The latter. Put an :cpp:class:`I3NullSplitter` in front of :func:`~.I3HDFWriter`. See also :ref:`tableio/hdfwriter-sim-hdfwriter`.

I get errors like FATAL (tableio): trying to get the address of unknown field OFUFilter_14_BadGCD (I3TableRow.h:221 in T \*I3TableRow::GetPointer(const std::string &, size_t) [T = bool])
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

:cpp:class:`I3FilterResultMapConverter` assumes that the set of filter names is fixed for a
run. Either book runs with different filter configurations separately, or
remove the new keys from each ``FilterMask`` if you do not need them.

What happened to tables.Table.readEvent()?
------------------------------------------

Old releases of :ref:`tableio/hdfwriter` included an extension for PyTables that let you
easily read the rows associated with a single event from a ragged table. This
was not terribly well implemented, and conflicted with an API change in the
PyTables 3.3 series, so it was `removed in 2016
<https://code.icecube.wisc.edu/projects/icecube/changeset/151233/IceCube>`_.
Instead of :func:`readEvent`, you can use the much more flexible
and powerful :meth:`~pandas.DataFrame.groupby` operation in `Pandas`_.

.. _Pandas: https://pandas.pydata.org
