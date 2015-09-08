Submitting Tickets
==================
Submitting tickets via Trac (`<http://code.icecube.wisc.edu/projects/icecube/newticket>`_) is 
pretty self-explanatory, but there are a few things to keep in mind.  The most important
of which is **DON'T SUBMIT TICKETS AS USER 'ICECUBE'!!!**  We often need to follow up 
with the reporter and there's no way to do that if you don't submit under your own name.

Please keep in mind, the more information you include, the easier you make it for
the ticket owners (whomever they might be).  This increases the chance of getting your
issue resolved sooner.

Required Fields
---------------
If you're submitting a ticket, please, please, please set the following five fields accordingly.

* **Summary** - Please begin with '[<project-name>] ...' (e.g. '[icetray] Test is failing on RHEL666.'). Please keep the description succinct.
* **Description** - This is where you should include a complete description of the problem, with enough information for the owner to reproduce the problem.
* **Type** - What kind of issue are you reporting?

  - **defect** : Something is broken.
  - **enhancement** : Wishlist feature that would be nice, but isn't urgent.
  - **task** : Something to do that's not a bugfix, enhancement, or cleanup.
  - **rumor/allegation** :  You're not really sure this is a problem, but you think it might be.
  - **cleanup** : The code "works" just fine, but you're reporting something that should be cleaned up.

* **Component** : These fields should be pretty self-explanatory, but here's a little help just in case :

  - **cmake** : Issues with build system.
  - **cvmfs** : Issues with cvmfs.
  - **icecube offline** : Any project that exists in the offline-software meta-project.  Look at the externals `here <http://code.icecube.wisc.edu/projects/icecube/browser/IceCube/meta-projects/offline-software/trunk>`__ .
  - **iceprod** : Issues with the production software (IceProd 1 or IceProd 2).
  - **icerec** : Any project that exists in the icerec meta-project that's not part of offline.  Look at the externals `here <http://code.icecube.wisc.edu/projects/icecube/browser/IceCube/meta-projects/icerec/trunk>`__ .
  - **infrastructure** : You know...roads, bridges, plumbing, etc...
  - **jeb + pnf** : Issues with online (pole) processing.  Projects that belong to 
  - **other** : Doesn't fit in any other component.
  - **RASTA** : Not used.
  - **simulation** : Any project that exists in the simulation meta-project that's not part of offline.  Look at the externals `here <http://code.icecube.wisc.edu/projects/icecube/browser/IceCube/meta-projects/simulation/trunk>`__ .
  - **tools/ports** : Issues with either ports or builds against system packages.

* **Priority** - How urgent is this?

  - **blocker** : You're stuck and need this fixed to get unstuck.
  - **critical** : It's pretty serious.
  - **major** : Serious, but not critical.
  - **normal** : It'll get resolved on a natural timescale.
  - **minor** : Maybe it'll get resolved in a few years.
  - **trivial** : Why are you submitting this ticket if it's trivial?
  - **README** : Don't use this.

Optional Fields
---------------
The following fields can typically be left to meta-project maintainers, since the general user won't 
know what to set these to.

* **Milestone** - You typically won't know when the issue can be expected to be addressed.  This depends on the ticket owner's schedule, the severity, and difficulty.  This will be set later by release managers. 
* **Version** - Not used.
* **Keywords** - Useful for searching.  Think about adding some keywords here.
* **Cc** - If you think someone else should be following the status of this issue, feel free to add their svn username here.
* **Owner** - This is the person who's going to resolve the ticket.  Most people don't know who this is, so meta-project maintainers can set this later.
* **Attaching files** - This can be incredibly helpful in reproducing the error for the ticket owner.  Strongly consider attaching a simple script that reproduces the error.
