.. _hdfwriter-faq:

FAQ
===

Can I use hdfwriter to write from files with only Q frames, or would I have to create P frames on my own with those objects that I want to have in the hdf5 file?
-----------------------------------------------------------------------------------------------------------------------------------------------------------------

The latter. Put an I3NullSplitter in front of I3HDFWriter.

I get errors like FATAL (tableio): trying to get the address of unknown field OFUFilter_14_BadGCD (I3TableRow.h:221 in T \*I3TableRow::GetPointer(const std::string &, size_t) [T = bool])
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

I3FilterResultMapConverter assumes that the set of filter names is fixed for a
run. Either book runs with different filter configurations separately, or
remove the new keys from each FilterMask if you do not need them.

What happened to tables.Table.readEvent()?
------------------------------------------

Old releases of hdfwriter included an extension for PyTables that let you
easily read the rows associated with a single event from a ragged table. This
was not terribly well implemented, and conflicted with an API change in the
PyTables 3.3 series. Instead of readEvent, you can use the much more flexible
and powerful `groupby` operation in `Pandas`_.

.. _Pandas: http://pandas.pydata.org
