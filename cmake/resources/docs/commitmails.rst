.mailinglist: Commit Emails
===========================

::

  # 
  # Add your email address here to receive automatic email 
  # notification of commits to this project.
  # 
  troy@resophonic.com
  you@your.institution.edu

If you are involved in development, you may want to make use of svn's
ability to automatically notify you when someone commits to certain
projects.

On every commit to directory R in the icecube repository, subversion
sends an email out to all the email addresses listed in all files
named .mailinglist found in R *and all the parent directories of R*
. These files contain email addresses, one per line. Lines that start
with a hash (#) are ignored.

If you would like to track activity in any project, simply add your
email address to the file :file:`.mailinglist` in the projects' root
directory. If one does not exist, you may create one:

.. code-block:: shell-session

  % echo "me@somewhere.edu" > .mailinglist
  % svn add .mailinglist
  % svn commit .mailinglist -m "Created .mailinglist"


