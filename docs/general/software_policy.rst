.. _icecube-software-policy:

IceCube 'combo' Software Policy
===============================

Release Policy
--------------

Release Criteria
~~~~~~~~~~~~~~~~
Tests: All tests pass on all supported platforms.
Open Tickets: All blockers must be resolved or downgraded.

With each combo release we should consider a cvmfs and test-data release.

Candidate vs. Release
~~~~~~~~~~~~~~~~~~~~~
  - Candidates are meant for **testing** only.
  - Candidates have short lifetimes and **will be deleted** after the release.
  - Samples produced or processed with candidates **will be deleted**.
  - Samples produced or processed with candidates **should not** be used for analysis.

Release Procedure
~~~~~~~~~~~~~~~~~
0) Make a release candidate.
1) Ensure all tests pass on all supported platforms.
2) Ensure there are no blocking tickets.
3) Push any open non-blocking tickets to the next milestone.
4) Update release notes.
   
New Projects
------------
All new production projects included in combo, must pass a code review. (See :ref:`icecube-code-reviews`)

New Dependencies
----------------
All new third-party dependencies must be discussed on a phone call. (See :ref:`icecube-code-reviews`)
