..
.. Copyright  (C) 2010 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. @author Fabian Kislat <fabian.kislat@desy.de>, $LastChangedBy$

====================
 I3ROOTTableService
====================

.. cpp:class:: I3ROOTTableService

    encapsulates a root file and provides methods of writing data to that ROOT
    file in the tableio framework.

    .. cpp:function:: I3ROOTTableService(const std::string &filename, const std::string &master = "MasterTree", int compress = 1, long long maxSize, const std::string &mode = "RECREATE")

        creates a new I3ROOTTableService with the given parameters:

	* ``filename`` - Name of the ROOT files to be written;
	* ``master`` -  Name of the master tree in the ROOT file. All other trees are friends of this tree.
   	* ``compress`` - Compression parameter. See the documentation of ``TFile`` in the ROOT documentation for the
	  meaning of this number. The default should be okay for most people.
	* ``maxSize`` - Maximum file size. When reaching this size, ROOT will automatically split the files. You
	  should be aware that split files have to be read in a chain. Otherwise they might not be aligned correctly.
	* ``mode`` - The file mode of the ROOT file. See the documentation of ``TFile`` in the ROOT documentation.
	  Only change this if you really know what you are doing!

    All other functions are private/protected and do not need to be accessed by the user.
