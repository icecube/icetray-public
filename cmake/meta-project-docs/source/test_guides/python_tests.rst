Python Tests
************
Below is a simple example of a python test.  It is strongly recommended that
you use unittest for your test cases.  It has some nice features.

::

  #!/usr/bin/env python
  import unittest

  from icecube.dataclasses import I3Particle

  class I3ParticleTest(unittest.TestCase):
      def test_default_equality(self):
          p1 = I3Particle()
          p2 = I3Particle()
          self.assertEqual(p1, p2, "Ooops.  Something broke.")

  unittest.main()

Common pitfalls:

* Not Executable

  - Before adding to svn : ``chmod +x path/to/the/test.py``
  - After adding to svn : ``svn ps svn:executable "*"
    path/to/the/test.py`` (don't forget to commit this change).

* Not registered

  - Edit your project's CMakeList.txt file and add it to
    ``i3_test_scripts``.
  - Many projects just do something like
    ``i3_test_scripts(resources/test/*.py)`` which automatically
    registers all python scripts in ``resources/test`` as tests.

