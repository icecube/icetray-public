Use vector by default
---------------------

If you have a good reason for using a specific container, then do so.
Otherwise, `vector` is the best choice for most situations.

The cpp standard actually has it written in:

    vector is the type of sequence that should be used by default.

`vector` is guaranteed to have the following properties:

* lowest space overhead of any container

* fastest access speed to contained elements

* inherent locality of reference (objects near each other in the
  container are near each other in memory)

* layout-compatible with C

* fastest iterators

Examples:

* use vector for small lists

* or really, use vector for anything small
