.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

Python Tests
------------

Below is a simple example of a Python test.  It is strongly recommended that
you use `unittest <https://docs.python.org/3/library/unittest.html>`_ for your
test cases.  It has some nice features.

::

    #!/usr/bin/env python3
    """ I3Time tests """

    import unittest
    from icecube.dataclasses import I3Time

    class I3TimeTest(unittest.TestCase):
        def test_print(self):
            t = I3Time(2012, 116640000117197235)
            print(t)
            print(t.__repr__())

            s = str(t)
            subsecond = int(s.split(" ")[-2].split(".")[-1].replace(",", ""))
            self.assertEqual(subsecond, 117197235)

    if __name__ == "__main__":
    unittest.main()

Common pitfalls:

* Not Executable

  - Before adding to git : ``chmod +x path/to/the/test.py``

* Not registered

  - Edit your project's ``CMakeList.txt`` file and add it to
    ``i3_test_scripts``.
  - Many projects just do something like
    ``i3_test_scripts(resources/test/*.py)`` which automatically
    registers all python scripts in ``resources/test`` as tests.
