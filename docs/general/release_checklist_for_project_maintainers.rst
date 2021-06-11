.. _icecube-release-checklist:

Release Checklist for Project Maintainers
=========================================

The purpose of this checklist is to give project maintainers some guidance as to what
they should be checking before each release.

Expect Improvement
------------------
**The expectations are that releases should either improve or remain unchanged.** Therefore
strict bitwise backward compatibility is not a requirement. It's OK for algorithms to change,
if the change is an improvement.  This applies to default configurations (i.e. module/segment
parameters).  If a project owner feels a new set of parameters are better and should be
used in production (or in general) the default values of the module or tray segment should
change to reflect that.

.. note::

   These changes should be clearly indicated in the release notes.

Checklist
---------
Project maintainers are responsible for **every** aspect of their project. (See :ref:`icecube-maintainer-responsibilities`)

Below are some things to check :

  * **Release Notes** : Ensure the RELEASE_NOTES are up to date and accurate.  Check the 'git log' and make sure there isn't any commit to your project that you don't understand or agree with. (See :ref:`writing-release-notes` for more information.)
  * **Documentation** 

    - Ensure your project's documentation is *accurate and up to date*. If you see where documentation can easily be improved, please take the time to do it.  
    - Ensure your project's documentation is *complete*.  If something's going to take a little more time, please create a ticket for yourself as a reminder to address it before the next code sprint.

  * **Example Scripts** : Ensure your project's example scripts run.
  * **Tickets** : Are there any critical tickets still open?  If there are non-critical tickets still open, please add a comment to the ticket as to why you're not addressing this now and push the milestone.
  * **Build** : Ensure your project builds without warnings.
  * **Test Coverage** : CI systems give a false sense of security for projects with low test coverage.
    - Strive for greater than 90% line coverage.
    - Strive for close to 100% function coverage.


