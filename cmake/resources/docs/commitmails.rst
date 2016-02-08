.mailinglist: Commit Emails
===========================

::

  # 
  # Add your email address here to receive automatic email 
  # notification of commits to this project.
  # 
  # Maintainer
  you@your.institution.edu
  # Observer
  someone.else@some.other.institution.edu

The email of the project owner is the first entry in the list.

If you are involved in development, you may want to make use of svn's
ability to automatically notify you when someone commits to certain
projects.  

If you are involved in development, then your email should be in 
the **Maintainer** section.  If you just want to track what's going on,
with no intention of helping with development, then add your email to
the **Observer** section.

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


