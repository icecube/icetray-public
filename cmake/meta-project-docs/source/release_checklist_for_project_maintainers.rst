Release Checklist for Project Maintainers
=========================================

The purpose of this checklist is to give project maintainers some guidance as to what
they should be checking before each relases.

Checklist
---------
Project maintainers are responsible for every aspect of their project.  It is no longer
acceptable to claim your project is in good shape and stable because it hasn't crashed
in production.  Just because your project doesn't crash in production, doens't mean
it's not broken.

Below are some things to check :

  * **Release Notes** : Ensure the RELEASE_NOTES are up to date and accurate.  Check the 'svn log' and make sure there isn't any commit to your project to don't understand or agree with.
  * **Documentation** 
    - Ensure your project's documentation is *accurate and up to date*. If you see where documentation can easily be improved, please take the time to do it.  
    - Ensure your project's documentation is *complete*.  If something's going to take a little more time, please create a ticket for yourself as a reminder and get to it when you have time.
  * **Example Scripts** : Ensure your project's example scripts run.
  * **Tickets** : Are there any critical tickets still open?  If there are non-critical tickets still open, please add a comment to the ticket as to why you're not addressing this in the next release and push the milestone.
  * **Build** : Ensure your project builds without warnings.


