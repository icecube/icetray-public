.. _writing-release-notes:

Writing Release Notes
=====================
Writing good release notes is becoming a critical exercise on IceCube and one 
that deserves a little investment of each developer's time.  The most difficult
and frustrating question to attempt to answer is **My plot looks different in
this release.  What changed?**  Sometimes finding these changes consumes a 
significant amount of resources (i.e. person-hours) from a group that is 
already stretched pretty thin.

.. important::
  **Take the time to write high-quality release notes.**  If you want someone else to be able to answer the question "My plot looks different.  What changed?" at the eleventh hour the night before you're giving a talk, please take the time to write good release notes.

.. important::
  **Write as you go.**  Get in the habit of updating the release notes with **every** commit.  If updating the release notes is not appropriate you should have at least thought about it.  Maybe mention in the commit message..."a simple refactoring...no need to update release notes on this one."

Write as you go
---------------
This is a recommendation for projects with a lot of development by several people, and it's not required that all maintainers follow it.

Higher Quality Notes
  If you write the note at the time of commit, you're more likely to write a better note.  The change is fresh in your mind.

Quicker Releases
  The most time consuming and tedious part of putting releases together is writing the release notes.  Actually making a release takes about 5-10 minutes.  Writing sub-par, low-quality release notes takes about a day.  For one person managing almost 100 projects, it can easily take a week or more to write high-quality release notes.  This is impractical.

Project Version Number
----------------------
Project maintainers should feel free to recommend the next version number based on a semantic versioning scheme MAJOR-MINOR-PATCH (e.g. V01-03-05):

* MAJOR version when you make incompatible API changes,
* MINOR version when you add functionality in a backwards-compatible manner, and
* PATCH version when you make backwards-compatible bug fixes.

What should go into release notes
---------------------------------
Remember, when using the release notes the question we're often trying to 
answer is **"My plot looks different in this release.  What changed?"**
So if a change has even the slightest chance of producing a change in any
physics distribution it should be documented.

* Any non-zero chance of changing a physics distribution.  You're going to have to use your best judgement here, but you either made the commit yourself, or you're the project maintainer and should have a good feel for the affect each change could have.  When in doubt err on the side of documenting.
* Interface changes - These **must** be documented.
* Bug fixes - These **must** be documented.
* **Include revision numbers** - This will make it much easier to figure out whether the commit was the actual culprit.

What doesn't need to go into release notes
------------------------------------------
You have to be pretty sure that the commit won't have any noticeable impact, but things like :

Whitespace cleanup
  Please, please, please make these separate commits, so when we're debugging we don't have to look through every diff in a monster commit and sift through the whitespace cleanups from the actual changes in functional code.

Code refactor
  You just moved code around, but the functionality is identical. Only simple and truly benign refactors don't need to go into release notes.
