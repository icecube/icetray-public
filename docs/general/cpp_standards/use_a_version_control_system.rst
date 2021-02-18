Use a version control system
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Keep your stuff in the code repository, and don't be afraid to check in 
every time your software builds, and don't be afraid to tag every time 
it works.  It often happens, when big changes are necessary, that you can 
get ahead of yourself and then forget how to back up to something that
worked.  Code repositories can be a huge help.

When checking code into trunks of projects that are included in production
meta-projects (i.e. offline-software, icerec, and icesim), the bar is a
little higher.  You should still commit early and often, but you should also :

- Ensure that all tests pass (run "ctest" in your build dir.)
- Check the Continuous Integration chain periodically in the next hour or so to make sure
  you didn't inadvertently break something on another platform.
